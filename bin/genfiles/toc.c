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
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 118
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 160
int Cyc_Tcutil_unify(void*,void*);
# 165
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 255 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 263
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 265
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 275
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 333
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 362
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 366
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 369
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn);struct _tuple12{unsigned int f1;int f2;};
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
{const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,Cyc_fprintf(Cyc_stderr,((_tmp9C7="\n",_tag_dyneither(_tmp9C7,sizeof(char),2))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9CA;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9C9;(int)_throw((void*)((_tmp9C9=_cycalloc_atomic(sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=Cyc_Toc_Toc_Unimplemented,_tmp9CA)),_tmp9C9)))));};}
# 145
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 148
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9CD;void*_tmp9CC;(_tmp9CC=0,Cyc_fprintf(Cyc_stderr,((_tmp9CD="\n",_tag_dyneither(_tmp9CD,sizeof(char),2))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9D0;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9CF;(int)_throw((void*)((_tmp9CF=_cycalloc_atomic(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=Cyc_Toc_Toc_Impossible,_tmp9D0)),_tmp9CF)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmp9D1;skip_stmt_opt=((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1[0]=Cyc_Absyn_skip_stmt(0),_tmp9D1))));}
return*skip_stmt_opt;}
# 274
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 277
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9D4;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9D3;return(void*)((_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=13,((_tmp9D4.f1=t,((_tmp9D4.f2=e,((_tmp9D4.f3=0,((_tmp9D4.f4=Cyc_Absyn_No_coercion,_tmp9D4)))))))))),_tmp9D3))));}
# 280
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9D7;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9D6;return(void*)((_tmp9D6=_cycalloc(sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=19,((_tmp9D7.f1=e,_tmp9D7)))),_tmp9D6))));}
# 283
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9DA;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9D9;return(void*)((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=22,((_tmp9DA.f1=e1,((_tmp9DA.f2=e2,_tmp9DA)))))),_tmp9D9))));}
# 286
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9DD;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9DC;return(void*)((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=36,((_tmp9DD.f1=s,_tmp9DD)))),_tmp9DC))));}
# 289
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9E0;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9DF;return(void*)((_tmp9DF=_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF[0]=((_tmp9E0.tag=16,((_tmp9E0.f1=t,_tmp9E0)))),_tmp9DF))));}
# 292
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9E3;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9E2;return(void*)((_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=9,((_tmp9E3.f1=e,((_tmp9E3.f2=es,((_tmp9E3.f3=0,((_tmp9E3.f4=1,_tmp9E3)))))))))),_tmp9E2))));}
# 295
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9E6;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9E5;return(void*)((_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E6.tag=1,((_tmp9E6.f1=e,_tmp9E6)))),_tmp9E5))));}
# 298
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9E9;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9E8;return(void*)((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=2,((_tmp9E9.f1=s1,((_tmp9E9.f2=s2,_tmp9E9)))))),_tmp9E8))));}
# 301
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9EC;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9EB;return(void*)((_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB[0]=((_tmp9EC.tag=5,((_tmp9EC.f1=e1,((_tmp9EC.f2=e2,((_tmp9EC.f3=e3,_tmp9EC)))))))),_tmp9EB))));}
# 304
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9EF;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9EE;return(void*)((_tmp9EE=_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=20,((_tmp9EF.f1=e,((_tmp9EF.f2=n,((_tmp9EF.f3=0,((_tmp9EF.f4=0,_tmp9EF)))))))))),_tmp9EE))));}
# 307
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9F2;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9F1;return(void*)((_tmp9F1=_cycalloc(sizeof(*_tmp9F1)),((_tmp9F1[0]=((_tmp9F2.tag=21,((_tmp9F2.f1=e,((_tmp9F2.f2=n,((_tmp9F2.f3=0,((_tmp9F2.f4=0,_tmp9F2)))))))))),_tmp9F1))));}
# 310
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 313
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9F5;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9F4;return(void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=35,((_tmp9F5.f1=tdopt,((_tmp9F5.f2=ds,_tmp9F5)))))),_tmp9F4))));}
# 315
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9F8;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9F7;return(void*)((_tmp9F7=_cycalloc(sizeof(*_tmp9F7)),((_tmp9F7[0]=((_tmp9F8.tag=8,((_tmp9F8.f1=v,((_tmp9F8.f2=s,_tmp9F8)))))),_tmp9F7))));}
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
struct Cyc_Core_Impossible_exn_struct _tmp9FE;const char*_tmp9FD;struct Cyc_Core_Impossible_exn_struct*_tmp9FC;(int)_throw((void*)((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FE.tag=Cyc_Core_Impossible,((_tmp9FE.f1=((_tmp9FD="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9FD,sizeof(char),44))),_tmp9FE)))),_tmp9FC)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA0B;const char*_tmpA0A;void*_tmpA09[1];struct Cyc_String_pa_PrintArg_struct _tmpA08;struct Cyc_Core_Impossible_exn_struct*_tmpA07;(int)_throw((void*)((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07[0]=((_tmpA0B.tag=Cyc_Core_Impossible,((_tmpA0B.f1=(struct _dyneither_ptr)((_tmpA08.tag=0,((_tmpA08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA09[0]=& _tmpA08,Cyc_aprintf(((_tmpA0A="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA0A,sizeof(char),67))),_tag_dyneither(_tmpA09,sizeof(void*),1)))))))),_tmpA0B)))),_tmpA07)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA11;const char*_tmpA10;struct Cyc_Core_Impossible_exn_struct*_tmpA0F;(int)_throw((void*)((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F[0]=((_tmpA11.tag=Cyc_Core_Impossible,((_tmpA11.f1=((_tmpA10="impossible type (not pointer)",_tag_dyneither(_tmpA10,sizeof(char),30))),_tmpA11)))),_tmpA0F)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmpA14;void*_tmpA13;(_tmpA13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA14="is_nullable",_tag_dyneither(_tmpA14,sizeof(char),12))),_tag_dyneither(_tmpA13,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA16;unsigned int _tmpA15;struct _dyneither_ptr buf=(_tmpA15=len,((_tmpA16=_cyccalloc_atomic(sizeof(char),_tmpA15),_tag_dyneither(_tmpA16,sizeof(char),_tmpA15))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_stmttmp9=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=_stmttmp9;_tmp104=*_tmp103;
{char _tmpA19;char _tmpA18;struct _dyneither_ptr _tmpA17;(_tmpA17=p,((_tmpA18=*((char*)_check_dyneither_subscript(_tmpA17,sizeof(char),0)),((_tmpA19='_',((_get_dyneither_size(_tmpA17,sizeof(char))== 1  && (_tmpA18 == '\000'  && _tmpA19 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA17.curr)=_tmpA19)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 485
{char _tmpA1C;char _tmpA1B;struct _dyneither_ptr _tmpA1A;(_tmpA1A=p,((_tmpA1B=*((char*)_check_dyneither_subscript(_tmpA1A,sizeof(char),0)),((_tmpA1C='_',((_get_dyneither_size(_tmpA1A,sizeof(char))== 1  && (_tmpA1B == '\000'  && _tmpA1C != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA1A.curr)=_tmpA1C)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA1D;Cyc_strcpy(p,((_tmpA1D="_struct",_tag_dyneither(_tmpA1D,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 497
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct Cyc_Toc_TocState _stmttmpA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=_stmttmpA;_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmpA1E;pair=((_tmpA1E.f1=fieldname,((_tmpA1E.f2=dtname,_tmpA1E))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple1**_tmp113=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple1*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmpA1F;struct _tuple14*_tmp115=(_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->f1=fieldname,((_tmpA1F->f2=dtname,_tmpA1F)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple1*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA22;struct _tuple1*_tmpA21;struct _tuple1*res=(_tmpA21=_cycalloc(sizeof(*_tmpA21)),((_tmpA21->f1=_tmp117,((_tmpA21->f2=((_tmpA22=_cycalloc(sizeof(*_tmpA22)),((_tmpA22[0]=newvar,_tmpA22)))),_tmpA21)))));
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
struct Cyc_Int_pa_PrintArg_struct _tmpA2A;void*_tmpA29[1];const char*_tmpA28;struct _dyneither_ptr*_tmpA27;struct _dyneither_ptr*xname=(_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=(struct _dyneither_ptr)((_tmpA2A.tag=1,((_tmpA2A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA29[0]=& _tmpA2A,Cyc_aprintf(((_tmpA28="_tuple%d",_tag_dyneither(_tmpA28,sizeof(char),9))),_tag_dyneither(_tmpA29,sizeof(void*),1)))))))),_tmpA27)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA2D;struct Cyc_List_List*_tmpA2C;_tmp128=((_tmpA2C=_cycalloc(sizeof(*_tmpA2C)),((_tmpA2C->hd=((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D->name=Cyc_Absyn_fieldname(i),((_tmpA2D->tq=Cyc_Toc_mt_tq,((_tmpA2D->type=(void*)ts2->hd,((_tmpA2D->width=0,((_tmpA2D->attributes=0,((_tmpA2D->requires_clause=0,_tmpA2D)))))))))))))),((_tmpA2C->tl=_tmp128,_tmpA2C))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA32;struct _tuple1*_tmpA31;struct Cyc_Absyn_Aggrdecl*_tmpA30;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30->kind=Cyc_Absyn_StructA,((_tmpA30->sc=Cyc_Absyn_Public,((_tmpA30->name=(
(_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->f1=Cyc_Absyn_Rel_n(0),((_tmpA31->f2=xname,_tmpA31)))))),((_tmpA30->tvs=0,((_tmpA30->impl=(
(_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32->exist_vars=0,((_tmpA32->rgn_po=0,((_tmpA32->fields=_tmp128,((_tmpA32->tagged=0,_tmpA32)))))))))),((_tmpA30->attributes=0,_tmpA30)))))))))))));
# 548
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA38;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA37;struct Cyc_List_List*_tmpA36;Cyc_Toc_result_decls=((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=Cyc_Absyn_new_decl((void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA37.tag=6,((_tmpA37.f1=_tmp12B,_tmpA37)))),_tmpA38)))),0),((_tmpA36->tl=Cyc_Toc_result_decls,_tmpA36))))));}
{struct _tuple17*_tmpA3B;struct Cyc_List_List*_tmpA3A;*_tmp120=((_tmpA3A=_region_malloc(d,sizeof(*_tmpA3A)),((_tmpA3A->hd=((_tmpA3B=_region_malloc(d,sizeof(*_tmpA3B)),((_tmpA3B->f1=x,((_tmpA3B->f2=ts,_tmpA3B)))))),((_tmpA3A->tl=*_tmp120,_tmpA3A))))));}{
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
struct Cyc_Int_pa_PrintArg_struct _tmpA43;void*_tmpA42[1];const char*_tmpA41;struct _dyneither_ptr*_tmpA40;struct _dyneither_ptr*xname=(_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=(struct _dyneither_ptr)((_tmpA43.tag=1,((_tmpA43.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA42[0]=& _tmpA43,Cyc_aprintf(((_tmpA41="_tuple%d",_tag_dyneither(_tmpA41,sizeof(char),9))),_tag_dyneither(_tmpA42,sizeof(void*),1)))))))),_tmpA40)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 612
{struct _tuple18*_tmpA46;struct Cyc_List_List*_tmpA45;*_tmp13B=((_tmpA45=_region_malloc(d,sizeof(*_tmpA45)),((_tmpA45->hd=((_tmpA46=_region_malloc(d,sizeof(*_tmpA46)),((_tmpA46->f1=struct_name,((_tmpA46->f2=type_args,((_tmpA46->f3=x,_tmpA46)))))))),((_tmpA45->tl=*_tmp13B,_tmpA45))))));}{
# 615
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 622
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA49;struct Cyc_List_List*_tmpA48;_tmp147=((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->hd=((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->name=_tmp14C->name,((_tmpA49->tq=Cyc_Toc_mt_tq,((_tmpA49->type=t,((_tmpA49->width=_tmp14C->width,((_tmpA49->attributes=_tmp14C->attributes,((_tmpA49->requires_clause=0,_tmpA49)))))))))))))),((_tmpA48->tl=_tmp147,_tmpA48))))));};}
# 626
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA4E;struct _tuple1*_tmpA4D;struct Cyc_Absyn_Aggrdecl*_tmpA4C;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->kind=Cyc_Absyn_StructA,((_tmpA4C->sc=Cyc_Absyn_Public,((_tmpA4C->name=(
(_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->f1=Cyc_Absyn_Rel_n(0),((_tmpA4D->f2=xname,_tmpA4D)))))),((_tmpA4C->tvs=0,((_tmpA4C->impl=(
(_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->exist_vars=0,((_tmpA4E->rgn_po=0,((_tmpA4E->fields=_tmp147,((_tmpA4E->tagged=0,_tmpA4E)))))))))),((_tmpA4C->attributes=0,_tmpA4C)))))))))))));
# 634
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA54;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA53;struct Cyc_List_List*_tmpA52;Cyc_Toc_result_decls=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->hd=Cyc_Absyn_new_decl((void*)((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA53.tag=6,((_tmpA53.f1=_tmp14F,_tmpA53)))),_tmpA54)))),0),((_tmpA52->tl=Cyc_Toc_result_decls,_tmpA52))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 616
;_pop_region(r);};};
# 570
;_pop_dynregion(d);};}
# 643
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA61;const char*_tmpA60;void*_tmpA5F[1];struct Cyc_Int_pa_PrintArg_struct _tmpA5E;struct _tuple1*_tmpA5D;struct _tuple1*res=(_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->f1=Cyc_Absyn_Loc_n,((_tmpA5D->f2=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=(struct _dyneither_ptr)((_tmpA5E.tag=1,((_tmpA5E.f1=(unsigned int)_tmp15B,((_tmpA5F[0]=& _tmpA5E,Cyc_aprintf(((_tmpA60="_tmp%X",_tag_dyneither(_tmpA60,sizeof(char),7))),_tag_dyneither(_tmpA5F,sizeof(void*),1)))))))),_tmpA61)))),_tmpA5D)))));
return res;}
# 651
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct Cyc_Toc_TocState _stmttmp10=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=_stmttmp10;_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA69;void*_tmpA68[1];const char*_tmpA67;struct _dyneither_ptr*_tmpA66;struct _dyneither_ptr*res=(_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=(struct _dyneither_ptr)((_tmpA69.tag=1,((_tmpA69.f1=(unsigned int)_tmp165,((_tmpA68[0]=& _tmpA69,Cyc_aprintf(((_tmpA67="_LL%X",_tag_dyneither(_tmpA67,sizeof(char),6))),_tag_dyneither(_tmpA68,sizeof(void*),1)))))))),_tmpA66)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA6C;void*_tmpA6B;(_tmpA6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6C="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA6C,sizeof(char),43))),_tag_dyneither(_tmpA6B,sizeof(void*),0)));}{
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
struct _tuple9*_tmpA6D;return(_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->f1=_tmp170,((_tmpA6D->f2=_tmp171,((_tmpA6D->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA6D)))))));};}
# 688
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _stmttmp12=*x;struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple11 _tmp174=_stmttmp12;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple11*_tmpA6E;return(_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E->f1=_tmp175,((_tmpA6E->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA6E)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpA6F;return(_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->name=f->name,((_tmpA6F->tq=Cyc_Toc_mt_tq,((_tmpA6F->type=
# 720
Cyc_Toc_typ_to_c(f->type),((_tmpA6F->width=f->width,((_tmpA6F->attributes=f->attributes,((_tmpA6F->requires_clause=0,_tmpA6F)))))))))))));}
# 725
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 727
return;}
# 730
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA70;cs=((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA70))));}
# 735
return*cs;}
# 737
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA71;r=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA71))));}
# 742
return*r;}
# 744
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA72;r=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA72))));}
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
const char*_tmpA75;void*_tmpA74;(_tmpA74=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA75="unresolved DatatypeFieldType",_tag_dyneither(_tmpA75,sizeof(char),29))),_tag_dyneither(_tmpA74,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
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
{struct Cyc_List_List*_tmpA76;_tmp1D1=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=(void*)_tmp1A5->hd,((_tmpA76->tl=_tmp1D1,_tmpA76))))));}goto _LL9F;_LL9F:;}{
# 814
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 817
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpA77;struct _tuple9*_tmp1DC=(_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->f1=_tmp1A4->name,((_tmpA77->f2=_tmp1A4->tq,((_tmpA77->f3=_tmp1DB,_tmpA77)))))));
struct Cyc_List_List*_tmpA78;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->hd=_tmp1DC,((_tmpA78->tl=0,_tmpA78)))))));}{
# 821
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA7E;struct Cyc_Absyn_FnInfo _tmpA7D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA7C;return(void*)((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7E.tag=9,((_tmpA7E.f1=((_tmpA7D.tvars=0,((_tmpA7D.effect=0,((_tmpA7D.ret_tqual=_tmp1A0,((_tmpA7D.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA7D.args=_tmp1DA,((_tmpA7D.c_varargs=_tmp1A3,((_tmpA7D.cyc_varargs=0,((_tmpA7D.rgn_po=0,((_tmpA7D.attributes=_tmp1D1,_tmpA7D)))))))))))))))))),_tmpA7E)))),_tmpA7C))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 826
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 831
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA81;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA80;return(void*)((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA81.tag=12,((_tmpA81.f1=_tmp1A9,((_tmpA81.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA81)))))),_tmpA80))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA84;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA83;return(void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA84.tag=17,((_tmpA84.f1=_tmp1B3,((_tmpA84.f2=0,((_tmpA84.f3=_tmp1B5,((_tmpA84.f4=0,_tmpA84)))))))))),_tmpA83))));}else{
return t;}}else{
# 868
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA87;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA86;return(void*)((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA87.tag=17,((_tmpA87.f1=_tmp1B3,((_tmpA87.f2=0,((_tmpA87.f3=_tmp1B5,((_tmpA87.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA87)))))))))),_tmpA86))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
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
const char*_tmpA8B;void*_tmpA8A[1];struct Cyc_String_pa_PrintArg_struct _tmpA89;(_tmpA89.tag=0,((_tmpA89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA8A[0]=& _tmpA89,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8B="atomic_typ:  bad type %s",_tag_dyneither(_tmpA8B,sizeof(char),25))),_tag_dyneither(_tmpA8A,sizeof(void*),1)))))));}_LLB8:;}
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
const char*_tmpA8E;void*_tmpA8D;(_tmpA8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8E="is_poly_field: type missing fields",_tag_dyneither(_tmpA8E,sizeof(char),35))),_tag_dyneither(_tmpA8D,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 984
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA92;void*_tmpA91[1];struct Cyc_String_pa_PrintArg_struct _tmpA90;(_tmpA90.tag=0,((_tmpA90.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA91[0]=& _tmpA90,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA92="is_poly_field: bad field %s",_tag_dyneither(_tmpA92,sizeof(char),28))),_tag_dyneither(_tmpA91,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA96;void*_tmpA95[1];struct Cyc_String_pa_PrintArg_struct _tmpA94;(_tmpA94.tag=0,((_tmpA94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA95[0]=& _tmpA94,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA96="is_poly_field: bad type %s",_tag_dyneither(_tmpA96,sizeof(char),27))),_tag_dyneither(_tmpA95,sizeof(void*),1)))))));}_LLEC:;}
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
const char*_tmpA9A;void*_tmpA99[1];struct Cyc_String_pa_PrintArg_struct _tmpA98;(_tmpA98.tag=0,((_tmpA98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA99[0]=& _tmpA98,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9A="is_poly_project: bad type %s",_tag_dyneither(_tmpA9A,sizeof(char),29))),_tag_dyneither(_tmpA99,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1006
 return 0;_LLF3:;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA9B;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->hd=s,((_tmpA9B->tl=0,_tmpA9B)))))),0);}
# 1015
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA9C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->hd=s,((_tmpA9C->tl=0,_tmpA9C)))))),0);}
# 1019
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1025
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA9D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA9D[1]=s,((_tmpA9D[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1029
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA9E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA9E[1]=n,((_tmpA9E[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1033
struct Cyc_Absyn_Exp*_tmpA9F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA9F[1]=n,((_tmpA9F[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpAA0[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpAA0[2]=n,((_tmpAA0[1]=s,((_tmpAA0[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1041
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpAA1;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->hd=e,((_tmpAA1->tl=0,_tmpAA1)))))),0);}
# 1045
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpAA2[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA2,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAA8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA7;struct Cyc_List_List*_tmpAA6;Cyc_Toc_result_decls=((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=Cyc_Absyn_new_decl((void*)((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA7.tag=0,((_tmpAA7.f1=vd,_tmpAA7)))),_tmpAA8)))),0),((_tmpAA6->tl=Cyc_Toc_result_decls,_tmpAA6))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1075
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1077
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1079
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpAAF;struct _tuple16*_tmpAAE;struct _tuple16*_tmpAAD;struct _tuple16*_tmpAAC[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpAAC[2]=(
# 1082
(_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->f1=0,((_tmpAAD->f2=xplussz,_tmpAAD)))))),((_tmpAAC[1]=(
# 1081
(_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->f1=0,((_tmpAAE->f2=xexp,_tmpAAE)))))),((_tmpAAC[0]=(
# 1080
(_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF->f1=0,((_tmpAAF->f2=xexp,_tmpAAF)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAAC,sizeof(struct _tuple16*),3)))))))),0);}
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
int*_tmpAB7;struct Cyc_Absyn_Exp**_tmpAB6;int*_tmpAB5;struct Cyc_Toc_Env*_tmpAB4;return(_tmpAB4=_region_malloc(r,sizeof(*_tmpAB4)),((_tmpAB4->break_lab=(struct _dyneither_ptr**)0,((_tmpAB4->continue_lab=(struct _dyneither_ptr**)0,((_tmpAB4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAB4->varmap=(struct Cyc_Dict_Dict)
# 1147
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpAB4->toplevel=(int)1,((_tmpAB4->in_lhs=(int*)(
# 1149
(_tmpAB5=_region_malloc(r,sizeof(*_tmpAB5)),((_tmpAB5[0]=0,_tmpAB5)))),((_tmpAB4->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpAB4->struct_info).varsizeexp=((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=0,_tmpAB6)))),(((_tmpAB4->struct_info).lhs_exp=0,_tmpAB4->struct_info)))),((_tmpAB4->in_sizeof=(int*)(
(_tmpAB7=_region_malloc(r,sizeof(*_tmpAB7)),((_tmpAB7[0]=0,_tmpAB7)))),((_tmpAB4->rgn=(struct _RegionHandle*)r,_tmpAB4)))))))))))))))))));}
# 1155
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB8;return(_tmpAB8=_region_malloc(r,sizeof(*_tmpAB8)),((_tmpAB8->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpAB8->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpAB8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpAB8->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpAB8->toplevel=(int)_tmp255,((_tmpAB8->in_lhs=(int*)_tmp256,((_tmpAB8->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpAB8->in_sizeof=(int*)_tmp258,((_tmpAB8->rgn=(struct _RegionHandle*)r,_tmpAB8)))))))))))))))))));};}
# 1160
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB9;return(_tmpAB9=_region_malloc(r,sizeof(*_tmpAB9)),((_tmpAB9->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpAB9->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpAB9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpAB9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpAB9->toplevel=(int)0,((_tmpAB9->in_lhs=(int*)_tmp260,((_tmpAB9->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpAB9->in_sizeof=(int*)_tmp262,((_tmpAB9->rgn=(struct _RegionHandle*)r,_tmpAB9)))))))))))))))))));};}
# 1164
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpABA;return(_tmpABA=_region_malloc(r,sizeof(*_tmpABA)),((_tmpABA->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpABA->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpABA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpABA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpABA->toplevel=(int)1,((_tmpABA->in_lhs=(int*)_tmp26A,((_tmpABA->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpABA->in_sizeof=(int*)_tmp26C,((_tmpABA->rgn=(struct _RegionHandle*)r,_tmpABA)))))))))))))))))));};}
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
struct Cyc_Toc_Env*_tmpABB;return(_tmpABB=_region_malloc(r,sizeof(*_tmpABB)),((_tmpABB->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpABB->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpABB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpABB->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpABB->toplevel=(int)_tmp27A,((_tmpABB->in_lhs=(int*)_tmp27B,((_tmpABB->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpABB->struct_info).varsizeexp=_tmp27C,(((_tmpABB->struct_info).lhs_exp=exp,_tmpABB->struct_info)))),((_tmpABB->in_sizeof=(int*)_tmp27D,((_tmpABB->rgn=(struct _RegionHandle*)r,_tmpABB)))))))))))))))))));};}
# 1191
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _stmttmp1F=(*x).f1;union Cyc_Absyn_Nmspace _tmp27F=_stmttmp1F;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1194
const char*_tmpABF;void*_tmpABE[1];struct Cyc_String_pa_PrintArg_struct _tmpABD;(_tmpABD.tag=0,((_tmpABD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpABE[0]=& _tmpABD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABF="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpABF,sizeof(char),30))),_tag_dyneither(_tmpABE,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1197
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpAC0;return(_tmpAC0=_region_malloc(r,sizeof(*_tmpAC0)),((_tmpAC0->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpAC0->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpAC0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpAC0->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpAC0->toplevel=(int)_tmp288,((_tmpAC0->in_lhs=(int*)_tmp289,((_tmpAC0->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpAC0->in_sizeof=(int*)_tmp28B,((_tmpAC0->rgn=(struct _RegionHandle*)r,_tmpAC0)))))))))))))))))));};};}
# 1204
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpAC1;return(_tmpAC1=_region_malloc(r,sizeof(*_tmpAC1)),((_tmpAC1->break_lab=(struct _dyneither_ptr**)0,((_tmpAC1->continue_lab=(struct _dyneither_ptr**)0,((_tmpAC1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpAC1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpAC1->toplevel=(int)_tmp293,((_tmpAC1->in_lhs=(int*)_tmp294,((_tmpAC1->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpAC1->in_sizeof=(int*)_tmp296,((_tmpAC1->rgn=(struct _RegionHandle*)r,_tmpAC1)))))))))))))))))));};}
# 1210
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1215
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpAC2;fallthru_vars=((_tmpAC2=_region_malloc(r,sizeof(*_tmpAC2)),((_tmpAC2->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpAC2->tl=fallthru_vars,_tmpAC2))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpAC3;struct Cyc_Toc_FallthruInfo*fi=
(_tmpAC3=_region_malloc(r,sizeof(*_tmpAC3)),((_tmpAC3->label=fallthru_l,((_tmpAC3->binders=fallthru_vars,((_tmpAC3->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpAC3)))))));
struct _dyneither_ptr**_tmpAC6;struct Cyc_Toc_Env*_tmpAC5;return(_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5->break_lab=(struct _dyneither_ptr**)((_tmpAC6=_region_malloc(r,sizeof(*_tmpAC6)),((_tmpAC6[0]=break_l,_tmpAC6)))),((_tmpAC5->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpAC5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAC5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpAC5->toplevel=(int)_tmp29E,((_tmpAC5->in_lhs=(int*)_tmp29F,((_tmpAC5->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpAC5->in_sizeof=(int*)_tmp2A1,((_tmpAC5->rgn=(struct _RegionHandle*)r,_tmpAC5)))))))))))))))))));};};};}
# 1227
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1230
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpAC9;struct Cyc_Toc_Env*_tmpAC8;return(_tmpAC8=_region_malloc(r,sizeof(*_tmpAC8)),((_tmpAC8->break_lab=(struct _dyneither_ptr**)((_tmpAC9=_region_malloc(r,sizeof(*_tmpAC9)),((_tmpAC9[0]=break_l,_tmpAC9)))),((_tmpAC8->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpAC8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAC8->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpAC8->toplevel=(int)_tmp2AC,((_tmpAC8->in_lhs=(int*)_tmp2AD,((_tmpAC8->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpAC8->in_sizeof=(int*)_tmp2AF,((_tmpAC8->rgn=(struct _RegionHandle*)r,_tmpAC8)))))))))))))))))));};}
# 1237
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1240
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpACC;struct Cyc_Toc_Env*_tmpACB;return(_tmpACB=_region_malloc(r,sizeof(*_tmpACB)),((_tmpACB->break_lab=(struct _dyneither_ptr**)0,((_tmpACB->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpACB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpACC=_region_malloc(r,sizeof(*_tmpACC)),((_tmpACC->label=next_l,((_tmpACC->binders=0,((_tmpACC->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpACC)))))))),((_tmpACB->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpACB->toplevel=(int)_tmp2B7,((_tmpACB->in_lhs=(int*)_tmp2B8,((_tmpACB->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpACB->in_sizeof=(int*)_tmp2BA,((_tmpACB->rgn=(struct _RegionHandle*)r,_tmpACB)))))))))))))))))));};}
# 1255 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1258
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_stmttmp20=e->annot;void*_tmp2BD=_stmttmp20;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1263
{const char*_tmpACF;void*_tmpACE;(_tmpACE=0,Cyc_Tcutil_terr(e->loc,((_tmpACF="dereference of NULL pointer",_tag_dyneither(_tmpACF,sizeof(char),28))),_tag_dyneither(_tmpACE,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1268
 return 1;_LL114:;_LL115: {
const char*_tmpAD2;void*_tmpAD1;(_tmpAD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD2="need_null_check",_tag_dyneither(_tmpAD2,sizeof(char),16))),_tag_dyneither(_tmpAD1,sizeof(void*),0)));}_LL10B:;}
# 1273
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_stmttmp21=e->annot;void*_tmp2C6=_stmttmp21;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1278
{const char*_tmpAD5;void*_tmpAD4;(_tmpAD4=0,Cyc_Tcutil_terr(e->loc,((_tmpAD5="dereference of NULL pointer",_tag_dyneither(_tmpAD5,sizeof(char),28))),_tag_dyneither(_tmpAD4,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAD8;void*_tmpAD7;(_tmpAD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD8="get_relns",_tag_dyneither(_tmpAD8,sizeof(char),10))),_tag_dyneither(_tmpAD7,sizeof(void*),0)));}_LL116:;}static char _tmp2D5[8]="*bogus*";
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
if(e->topt == 0){const char*_tmpADB;void*_tmpADA;(_tmpADA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADB="Missing type in primop ",_tag_dyneither(_tmpADB,sizeof(char),24))),_tag_dyneither(_tmpADA,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1370
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpADE;void*_tmpADD;(_tmpADD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADE="Missing type in primop ",_tag_dyneither(_tmpADE,sizeof(char),24))),_tag_dyneither(_tmpADD,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1374
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpADF;return(_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->f1=Cyc_Toc_mt_tq,((_tmpADF->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpADF)))));}
# 1377
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpAE0;return(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->f1=0,((_tmpAE0->f2=e,_tmpAE0)))));};}
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
const char*_tmpAE3;void*_tmpAE2;(_tmpAE2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE3="bogus x to make_struct",_tag_dyneither(_tmpAE3,sizeof(char),23))),_tag_dyneither(_tmpAE2,sizeof(void*),0)));}_LL12D:;}else{
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
const char*_tmpAE7;void*_tmpAE6[1];struct Cyc_String_pa_PrintArg_struct _tmpAE5;(_tmpAE5.tag=0,((_tmpAE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE6[0]=& _tmpAE5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE7="array_elt_type called on %s",_tag_dyneither(_tmpAE7,sizeof(char),28))),_tag_dyneither(_tmpAE6,sizeof(void*),1)))))));}_LL139:;}
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
if(_tmp2FA->tl != 0){const char*_tmpAEA;void*_tmpAE9;(_tmpAE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAEA="multiple designators in array",_tag_dyneither(_tmpAEA,sizeof(char),30))),_tag_dyneither(_tmpAE9,sizeof(void*),0)));}{
void*_tmp2FE=(void*)_tmp2FA->hd;
void*_tmp2FF=_tmp2FE;struct Cyc_Absyn_Exp*_tmp301;_LL13F: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp300=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp2FF;if(_tmp300->tag != 0)goto _LL141;else{_tmp301=_tmp300->f1;}}_LL140:
# 1480
 Cyc_Toc_exp_to_c(nv,_tmp301);
e_index=_tmp301;
goto _LL13E;_LL141: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp302=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp2FF;if(_tmp302->tag != 1)goto _LL13E;}_LL142: {
const char*_tmpAED;void*_tmpAEC;(_tmpAEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAED="field name designators in array",_tag_dyneither(_tmpAED,sizeof(char),32))),_tag_dyneither(_tmpAEC,sizeof(void*),0)));}_LL13E:;};}{
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
const char*_tmpAF0;void*_tmpAEF;(_tmpAEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF0="init_comprehension passed a bad type!",_tag_dyneither(_tmpAF0,sizeof(char),38))),_tag_dyneither(_tmpAEF,sizeof(void*),0)));}_LL14E:;}{
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
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF2;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp311,Cyc_Absyn_varb_exp(_tmp311,(void*)((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=((_tmpAF3.tag=4,((_tmpAF3.f1=vd,_tmpAF3)))),_tmpAF2)))),0));
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
const char*_tmpAF6;void*_tmpAF5;(_tmpAF5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF6="empty designator list",_tag_dyneither(_tmpAF6,sizeof(char),22))),_tag_dyneither(_tmpAF5,sizeof(void*),0)));}
if(_tmp32D->tl != 0){
const char*_tmpAF9;void*_tmpAF8;(_tmpAF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF9="too many designators in anonymous struct",_tag_dyneither(_tmpAF9,sizeof(char),41))),_tag_dyneither(_tmpAF8,sizeof(void*),0)));}{
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
const char*_tmpAFC;void*_tmpAFB;(_tmpAFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFC="array designator in struct",_tag_dyneither(_tmpAFC,sizeof(char),27))),_tag_dyneither(_tmpAFB,sizeof(void*),0)));}_LL163:;};}}
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
struct Cyc_String_pa_PrintArg_struct _tmpB04;void*_tmpB03[1];const char*_tmpB02;void*_tmpB01;(_tmpB01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB04.tag=0,((_tmpB04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB03[0]=& _tmpB04,Cyc_aprintf(((_tmpB02="get_member_offset %s failed",_tag_dyneither(_tmpB02,sizeof(char),28))),_tag_dyneither(_tmpB03,sizeof(void*),1)))))))),_tag_dyneither(_tmpB01,sizeof(void*),0)));};}
# 1671
static int Cyc_Toc_init_variable_array(void**typ){
{void*_stmttmp30=Cyc_Tcutil_compress(*typ);void*_tmp35C=_stmttmp30;void*_tmp35E;struct Cyc_Absyn_Tqual _tmp35F;struct Cyc_Absyn_Exp*_tmp360;union Cyc_Absyn_Constraint*_tmp361;unsigned int _tmp362;_LL17D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35C;if(_tmp35D->tag != 8)goto _LL17F;else{_tmp35E=(_tmp35D->f1).elt_type;_tmp35F=(_tmp35D->f1).tq;_tmp360=(_tmp35D->f1).num_elts;_tmp361=(_tmp35D->f1).zero_term;_tmp362=(_tmp35D->f1).zt_loc;}}_LL17E:
# 1676
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp360))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB0A;struct Cyc_Absyn_ArrayInfo _tmpB09;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB08;*typ=(void*)((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB0A.tag=8,((_tmpB0A.f1=((_tmpB09.elt_type=_tmp35E,((_tmpB09.tq=_tmp35F,((_tmpB09.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB09.zero_term=_tmp361,((_tmpB09.zt_loc=_tmp362,_tmpB09)))))))))),_tmpB0A)))),_tmpB08))));}
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
struct Cyc_Absyn_Aggrfield*_tmpB0B;struct Cyc_Absyn_Aggrfield*_tmp367=(_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->name=_tmp366->name,((_tmpB0B->tq=Cyc_Toc_mt_tq,((_tmpB0B->type=new_field_type,((_tmpB0B->width=_tmp366->width,((_tmpB0B->attributes=_tmp366->attributes,((_tmpB0B->requires_clause=0,_tmpB0B)))))))))))));
# 1700
struct Cyc_List_List*_tmpB0C;new_fields=((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->hd=_tmp367,((_tmpB0C->tl=new_fields,_tmpB0C))))));}else{
# 1702
struct Cyc_List_List*_tmpB0D;new_fields=((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB0D->tl=new_fields,_tmpB0D))))));}}{
# 1704
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB15;void*_tmpB14[1];const char*_tmpB13;struct _dyneither_ptr*_tmpB12;struct _dyneither_ptr*name=
(_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12[0]=(struct _dyneither_ptr)((_tmpB15.tag=1,((_tmpB15.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB14[0]=& _tmpB15,Cyc_aprintf(((_tmpB13="_genStruct%d",_tag_dyneither(_tmpB13,sizeof(char),13))),_tag_dyneither(_tmpB14,sizeof(void*),1)))))))),_tmpB12)));
struct _tuple1*_tmpB16;struct _tuple1*qv=(_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB16->f2=name,_tmpB16)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB17;struct Cyc_Absyn_AggrdeclImpl*_tmp36C=(_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpB17->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpB17->fields=_tmp36B,((_tmpB17->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpB17)))))))));
# 1712
struct Cyc_Absyn_Aggrdecl*_tmpB18;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18->kind=ad->kind,((_tmpB18->sc=ad->sc,((_tmpB18->name=qv,((_tmpB18->tvs=ad->tvs,((_tmpB18->impl=_tmp36C,((_tmpB18->attributes=ad->attributes,_tmpB18)))))))))))));
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
const char*_tmpB1B;void*_tmpB1A;(_tmpB1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1B="stmt not an expression!",_tag_dyneither(_tmpB1B,sizeof(char),24))),_tag_dyneither(_tmpB1A,sizeof(void*),0)));}_LL181:;}{
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
const char*_tmpB1E;void*_tmpB1D;(_tmpB1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1E="init_struct: bad struct type",_tag_dyneither(_tmpB1E,sizeof(char),29))),_tag_dyneither(_tmpB1D,sizeof(void*),0)));}_LL186:;}
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
struct Cyc_Absyn_Aggrfield*_tmpB1F;struct Cyc_Absyn_Aggrfield*_tmp389=(_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->name=_tmp388->name,((_tmpB1F->tq=Cyc_Toc_mt_tq,((_tmpB1F->type=
# 1785
Cyc_Tcutil_rsubstitute(temp,_tmp387,_tmp388->type),((_tmpB1F->width=_tmp388->width,((_tmpB1F->attributes=_tmp388->attributes,((_tmpB1F->requires_clause=0,_tmpB1F)))))))))))));
# 1793
if(_tmp382->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp389->type)){
struct _dyneither_ptr**_tmpB20;index=((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20[0]=_tmp389->name,_tmpB20))));}}{
# 1800
struct Cyc_List_List*_tmpB21;new_fields=((_tmpB21=_cycalloc(sizeof(*_tmpB21)),((_tmpB21->hd=_tmp389,((_tmpB21->tl=new_fields,_tmpB21))))));};}
# 1802
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB29;void*_tmpB28[1];const char*_tmpB27;struct _dyneither_ptr*_tmpB26;struct _dyneither_ptr*name=
(_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26[0]=(struct _dyneither_ptr)((_tmpB29.tag=1,((_tmpB29.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB28[0]=& _tmpB29,Cyc_aprintf(((_tmpB27="_genStruct%d",_tag_dyneither(_tmpB27,sizeof(char),13))),_tag_dyneither(_tmpB28,sizeof(void*),1)))))))),_tmpB26)));
struct _tuple1*_tmpB2A;struct _tuple1*qv=(_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB2A->f2=name,_tmpB2A)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB2B;struct Cyc_Absyn_AggrdeclImpl*_tmp38D=(_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B->exist_vars=0,((_tmpB2B->rgn_po=0,((_tmpB2B->fields=aggrfields,((_tmpB2B->tagged=0,_tmpB2B)))))))));
# 1811
struct Cyc_Absyn_Aggrdecl*_tmpB2C;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C->kind=Cyc_Absyn_StructA,((_tmpB2C->sc=Cyc_Absyn_Public,((_tmpB2C->name=qv,((_tmpB2C->tvs=0,((_tmpB2C->impl=_tmp38D,((_tmpB2C->attributes=0,_tmpB2C)))))))))))));
# 1817
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_Aggrdecl**_tmpB35;struct Cyc_Absyn_AggrInfo _tmpB34;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB33;struct_type=(void*)((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33[0]=((_tmpB36.tag=11,((_tmpB36.f1=((_tmpB34.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=new_ad,_tmpB35))))),((_tmpB34.targs=0,_tmpB34)))),_tmpB36)))),_tmpB33))));}
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
const char*_tmpB39;void*_tmpB38;(_tmpB38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB39="empty designator list",_tag_dyneither(_tmpB39,sizeof(char),22))),_tag_dyneither(_tmpB38,sizeof(void*),0)));}
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
const char*_tmpB3C;void*_tmpB3B;(_tmpB3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3C="array designator in struct",_tag_dyneither(_tmpB3C,sizeof(char),27))),_tag_dyneither(_tmpB3B,sizeof(void*),0)));}_LL18B:;}
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
struct _tuple6 _tmpB40;union Cyc_Absyn_Cnst _tmpB3F;struct Cyc_Absyn_Exp*_tmpB3D[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB3D[1]=Cyc_Absyn_sizeoftyp_exp(_tmp3B8,0),((_tmpB3D[0]=_tmp3B3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpB3F.Int_c).val=((_tmpB40.f1=Cyc_Absyn_Unsigned,((_tmpB40.f2=(int)sizeof(double),_tmpB40)))),(((_tmpB3F.Int_c).tag=5,_tmpB3F)))),0),0);};}
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
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB4A;struct Cyc_Absyn_Aggrdecl**_tmpB49;struct Cyc_Absyn_AggrInfo _tmpB48;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB47;struct_type=(void*)(
(_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=((_tmpB4A.tag=11,((_tmpB4A.f1=((_tmpB48.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=ad,_tmpB49))))),((_tmpB48.targs=0,_tmpB48)))),_tmpB4A)))),_tmpB47))));}
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
const char*_tmpB4D;void*_tmpB4C;(_tmpB4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4D="array designator in struct",_tag_dyneither(_tmpB4D,sizeof(char),27))),_tag_dyneither(_tmpB4C,sizeof(void*),0)));}_LL190:;}}}
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
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB50;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB4F;return Cyc_Absyn_new_exp((void*)((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F[0]=((_tmpB50.tag=4,((_tmpB50.f1=e1,((_tmpB50.f2=incr,_tmpB50)))))),_tmpB4F)))),0);}
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
{struct Cyc_List_List*_tmpB51;Cyc_Toc_lvalue_assign(e1,((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51->hd=_tmp3D1,((_tmpB51->tl=fs,_tmpB51)))))),f,f_env);}
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
const char*_tmpB55;void*_tmpB54[1];struct Cyc_String_pa_PrintArg_struct _tmpB53;(_tmpB53.tag=0,((_tmpB53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB54[0]=& _tmpB53,Cyc_Toc_toc_impos(((_tmpB55="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB55,sizeof(char),23))),_tag_dyneither(_tmpB54,sizeof(void*),1)))))));}_LL1AE:;}
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
const char*_tmpB59;void*_tmpB58[1];struct Cyc_String_pa_PrintArg_struct _tmpB57;(_tmpB57.tag=0,((_tmpB57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB58[0]=& _tmpB57,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB59="can't take & of exp %s",_tag_dyneither(_tmpB59,sizeof(char),23))),_tag_dyneither(_tmpB58,sizeof(void*),1)))))));};_LL1B7:;}
# 2018
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_stmttmp3A=s->r;void*_tmp3EB=_stmttmp3A;struct Cyc_Absyn_Stmt*_tmp3ED;struct Cyc_Absyn_Stmt*_tmp3EF;struct Cyc_Absyn_Exp**_tmp3F1;_LL1C1: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3EC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3EC->tag != 2)goto _LL1C3;else{_tmp3ED=_tmp3EC->f2;}}_LL1C2:
 _tmp3EF=_tmp3ED;goto _LL1C4;_LL1C3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3EE=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3EE->tag != 12)goto _LL1C5;else{_tmp3EF=_tmp3EE->f2;}}_LL1C4:
 Cyc_Toc_push_address_stmt(_tmp3EF);goto _LL1C0;_LL1C5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3F0=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3EB;if(_tmp3F0->tag != 1)goto _LL1C7;else{_tmp3F1=(struct Cyc_Absyn_Exp**)& _tmp3F0->f1;}}_LL1C6:
*_tmp3F1=Cyc_Toc_push_address_exp(*_tmp3F1);goto _LL1C0;_LL1C7:;_LL1C8: {
# 2024
const char*_tmpB5D;void*_tmpB5C[1];struct Cyc_String_pa_PrintArg_struct _tmpB5B;(_tmpB5B.tag=0,((_tmpB5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB5C[0]=& _tmpB5B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5D="can't take & of stmt %s",_tag_dyneither(_tmpB5D,sizeof(char),24))),_tag_dyneither(_tmpB5C,sizeof(void*),1)))))));}_LL1C0:;}
# 2029
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2031
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2033
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB5E;result=((_tmpB5E=_region_malloc(r2,sizeof(*_tmpB5E)),((_tmpB5E->hd=f(x->hd,env),((_tmpB5E->tl=0,_tmpB5E))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB5F;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB5F=_region_malloc(r2,sizeof(*_tmpB5F)),((_tmpB5F->hd=f(x->hd,env),((_tmpB5F->tl=0,_tmpB5F))))));}
prev=prev->tl;}
# 2040
return result;}
# 2042
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2046
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpB60;return(_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60->f1=0,((_tmpB60->f2=e,_tmpB60)))));}
# 2050
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_stmttmp3B=Cyc_Tcutil_compress(t);void*_tmp3F8=_stmttmp3B;struct Cyc_Absyn_PtrInfo _tmp3FA;_LL1CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F8;if(_tmp3F9->tag != 5)goto _LL1CC;else{_tmp3FA=_tmp3F9->f1;}}_LL1CB:
 return _tmp3FA;_LL1CC:;_LL1CD: {
const char*_tmpB63;void*_tmpB62;(_tmpB62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB63="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB63,sizeof(char),28))),_tag_dyneither(_tmpB62,sizeof(void*),0)));}_LL1C9:;}
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
{const char*_tmpB64;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB64="0.0F",_tag_dyneither(_tmpB64,sizeof(char),5))),0),0);}goto _LL1CE;_LL1E1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FD;if(_tmp40C->tag != 7)goto _LL1E3;else{if(_tmp40C->f1 != 1)goto _LL1E3;}}_LL1E2:
{const char*_tmpB65;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB65="0.0",_tag_dyneither(_tmpB65,sizeof(char),4))),1),0);}goto _LL1CE;_LL1E3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FD;if(_tmp40D->tag != 7)goto _LL1E5;else{_tmp40E=_tmp40D->f1;}}_LL1E4:
{const char*_tmpB66;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB66="0.0L",_tag_dyneither(_tmpB66,sizeof(char),5))),_tmp40E),0);}goto _LL1CE;_LL1E5:;_LL1E6: {
# 2077
const char*_tmpB6A;void*_tmpB69[1];struct Cyc_String_pa_PrintArg_struct _tmpB68;(_tmpB68.tag=0,((_tmpB68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB69[0]=& _tmpB68,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6A="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB6A,sizeof(char),40))),_tag_dyneither(_tmpB69,sizeof(void*),1)))))));}_LL1CE:;}
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
struct Cyc_Core_Opt*_tmpB6B;struct Cyc_Core_Opt*_tmp418=(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((_tmpB6B->v=_tmp417,_tmpB6B)));
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
{struct Cyc_Absyn_Exp*_tmpB6C[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB6C[2]=_tmp41E,((_tmpB6C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB6C[0]=_tmp41D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E7;_LL1EC:;_LL1ED: {
const char*_tmpB6F;void*_tmpB6E;(_tmpB6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6F="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB6F,sizeof(char),53))),_tag_dyneither(_tmpB6E,sizeof(void*),0)));}_LL1E7:;}{
# 2123
struct _tuple1*_tmp422=Cyc_Toc_temp_var();
struct _RegionHandle _tmp423=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp423;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp424=Cyc_Toc_add_varmap(rgn2,nv,_tmp422,Cyc_Absyn_var_exp(_tmp422,0));
struct Cyc_Absyn_Vardecl*_tmpB70;struct Cyc_Absyn_Vardecl*_tmp425=(_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->sc=Cyc_Absyn_Public,((_tmpB70->name=_tmp422,((_tmpB70->tq=Cyc_Toc_mt_tq,((_tmpB70->type=_tmp416,((_tmpB70->initializer=xinit,((_tmpB70->rgn=0,((_tmpB70->attributes=0,((_tmpB70->escapes=0,_tmpB70)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB73;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB72;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp426=(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=4,((_tmpB73.f1=_tmp425,_tmpB73)))),_tmpB72)));
struct Cyc_Absyn_Exp*_tmp427=Cyc_Absyn_varb_exp(_tmp422,(void*)_tmp426,0);
_tmp427->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_deref_exp(_tmp427,0);
_tmp428->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp424,_tmp428);{
struct _tuple1*_tmp429=Cyc_Toc_temp_var();
_tmp424=Cyc_Toc_add_varmap(rgn2,_tmp424,_tmp429,Cyc_Absyn_var_exp(_tmp429,0));{
struct Cyc_Absyn_Vardecl*_tmpB74;struct Cyc_Absyn_Vardecl*_tmp42A=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->sc=Cyc_Absyn_Public,((_tmpB74->name=_tmp429,((_tmpB74->tq=Cyc_Toc_mt_tq,((_tmpB74->type=_tmp415,((_tmpB74->initializer=_tmp428,((_tmpB74->rgn=0,((_tmpB74->attributes=0,((_tmpB74->escapes=0,_tmpB74)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB77;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB76;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42B=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=4,((_tmpB77.f1=_tmp42A,_tmpB77)))),_tmpB76)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_varb_exp(_tmp429,(void*)_tmp42B,0);
_tmp42C->topt=_tmp428->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp42C,e2,0);
z_init->topt=_tmp42C->topt;}
# 2144
Cyc_Toc_exp_to_c(_tmp424,z_init);{
struct _tuple1*_tmp42D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB78;struct Cyc_Absyn_Vardecl*_tmp42E=(_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->sc=Cyc_Absyn_Public,((_tmpB78->name=_tmp42D,((_tmpB78->tq=Cyc_Toc_mt_tq,((_tmpB78->type=_tmp415,((_tmpB78->initializer=z_init,((_tmpB78->rgn=0,((_tmpB78->attributes=0,((_tmpB78->escapes=0,_tmpB78)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB7B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB7A;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42F=(_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=((_tmpB7B.tag=4,((_tmpB7B.f1=_tmp42E,_tmpB7B)))),_tmpB7A)));
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
struct Cyc_Absyn_Exp*_tmpB7C[2];struct Cyc_List_List*_tmp436=(_tmpB7C[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB7C[0]=
# 2163
Cyc_Absyn_varb_exp(_tmp422,(void*)_tmp426,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB7C,sizeof(struct Cyc_Absyn_Exp*),2)))));
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB82;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB81;struct Cyc_Absyn_Decl*_tmpB80;_tmp43D=Cyc_Absyn_decl_stmt(((_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80->r=(void*)((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82[0]=((_tmpB81.tag=0,((_tmpB81.f1=_tmp42E,_tmpB81)))),_tmpB82)))),((_tmpB80->loc=0,_tmpB80)))))),_tmp43D,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB88;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB87;struct Cyc_Absyn_Decl*_tmpB86;_tmp43D=Cyc_Absyn_decl_stmt(((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->r=(void*)((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88[0]=((_tmpB87.tag=0,((_tmpB87.f1=_tmp42A,_tmpB87)))),_tmpB88)))),((_tmpB86->loc=0,_tmpB86)))))),_tmp43D,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB8E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB8D;struct Cyc_Absyn_Decl*_tmpB8C;_tmp43D=Cyc_Absyn_decl_stmt(((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->r=(void*)((_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E[0]=((_tmpB8D.tag=0,((_tmpB8D.f1=_tmp425,_tmpB8D)))),_tmpB8E)))),((_tmpB8C->loc=0,_tmpB8C)))))),_tmp43D,0);}
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
struct Cyc_String_pa_PrintArg_struct _tmpB96;void*_tmpB95[1];const char*_tmpB94;void*_tmpB93;(_tmpB93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB96.tag=0,((_tmpB96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB95[0]=& _tmpB96,Cyc_aprintf(((_tmpB94="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB94,sizeof(char),51))),_tag_dyneither(_tmpB95,sizeof(void*),1)))))))),_tag_dyneither(_tmpB93,sizeof(void*),0)));}_LL1EE:;}{
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
# 2234
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2237
void*_stmttmp3F=e->r;void*_tmp46A=_stmttmp3F;struct Cyc_Absyn_Exp*_tmp46C;struct Cyc_Absyn_Exp*_tmp46E;struct _dyneither_ptr*_tmp46F;int*_tmp470;struct Cyc_Absyn_Exp*_tmp472;struct _dyneither_ptr*_tmp473;int*_tmp474;_LL1F4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp46B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46A;if(_tmp46B->tag != 13)goto _LL1F6;else{_tmp46C=_tmp46B->f2;}}_LL1F5: {
const char*_tmpB99;void*_tmpB98;(_tmpB98=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB99="cast on lhs!",_tag_dyneither(_tmpB99,sizeof(char),13))),_tag_dyneither(_tmpB98,sizeof(void*),0)));}_LL1F6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp46D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46A;if(_tmp46D->tag != 20)goto _LL1F8;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46D->f2;_tmp470=(int*)& _tmp46D->f4;}}_LL1F7: {
# 2240
void*_stmttmp42=Cyc_Tcutil_compress((void*)_check_null(_tmp46E->topt));void*_tmp477=_stmttmp42;union Cyc_Absyn_AggrInfoU _tmp479;_LL1FD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp477;if(_tmp478->tag != 11)goto _LL1FF;else{_tmp479=(_tmp478->f1).aggr_info;}}_LL1FE: {
# 2242
struct Cyc_Absyn_Aggrdecl*_tmp47A=Cyc_Absyn_get_known_aggrdecl(_tmp479);
*f_tag=Cyc_Toc_get_member_offset(_tmp47A,_tmp46F);{
# 2245
const char*_tmpB9E;void*_tmpB9D[2];struct Cyc_String_pa_PrintArg_struct _tmpB9C;struct Cyc_String_pa_PrintArg_struct _tmpB9B;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB9B.tag=0,((_tmpB9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp46F),((_tmpB9C.tag=0,((_tmpB9C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47A->name).f2),((_tmpB9D[0]=& _tmpB9C,((_tmpB9D[1]=& _tmpB9B,Cyc_aprintf(((_tmpB9E="_union_%s_%s",_tag_dyneither(_tmpB9E,sizeof(char),13))),_tag_dyneither(_tmpB9D,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB9F;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F[0]=str,_tmpB9F)))));}
if(clear_read)*_tmp470=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47A->impl))->tagged;};}_LL1FF:;_LL200:
 return 0;_LL1FC:;}_LL1F8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp471=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46A;if(_tmp471->tag != 21)goto _LL1FA;else{_tmp472=_tmp471->f1;_tmp473=_tmp471->f2;_tmp474=(int*)& _tmp471->f4;}}_LL1F9: {
# 2252
void*_stmttmp40=Cyc_Tcutil_compress((void*)_check_null(_tmp472->topt));void*_tmp480=_stmttmp40;void*_tmp482;_LL202: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp480;if(_tmp481->tag != 5)goto _LL204;else{_tmp482=(_tmp481->f1).elt_typ;}}_LL203: {
# 2254
void*_stmttmp41=Cyc_Tcutil_compress(_tmp482);void*_tmp483=_stmttmp41;union Cyc_Absyn_AggrInfoU _tmp485;_LL207: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp484=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp483;if(_tmp484->tag != 11)goto _LL209;else{_tmp485=(_tmp484->f1).aggr_info;}}_LL208: {
# 2256
struct Cyc_Absyn_Aggrdecl*_tmp486=Cyc_Absyn_get_known_aggrdecl(_tmp485);
*f_tag=Cyc_Toc_get_member_offset(_tmp486,_tmp473);{
# 2259
const char*_tmpBA4;void*_tmpBA3[2];struct Cyc_String_pa_PrintArg_struct _tmpBA2;struct Cyc_String_pa_PrintArg_struct _tmpBA1;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpBA1.tag=0,((_tmpBA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp473),((_tmpBA2.tag=0,((_tmpBA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp486->name).f2),((_tmpBA3[0]=& _tmpBA2,((_tmpBA3[1]=& _tmpBA1,Cyc_aprintf(((_tmpBA4="_union_%s_%s",_tag_dyneither(_tmpBA4,sizeof(char),13))),_tag_dyneither(_tmpBA3,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpBA5;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5[0]=str,_tmpBA5)))));}
if(clear_read)*_tmp474=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp486->impl))->tagged;};}_LL209:;_LL20A:
 return 0;_LL206:;}_LL204:;_LL205:
# 2265
 return 0;_LL201:;}_LL1FA:;_LL1FB:
# 2267
 return 0;_LL1F3:;}
# 2279 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp48C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48C,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp48D,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48C,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2303 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2307
struct _tuple1*_tmp48E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48E,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2317
struct Cyc_Absyn_Exp*_tmp48F=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp48F,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2320
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48E,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple20{void*f1;void*f2;};struct _tuple21{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBF4(unsigned int*_tmpBF3,unsigned int*_tmpBF2,struct _tuple1***_tmpBF0){for(*_tmpBF3=0;*_tmpBF3 < *_tmpBF2;(*_tmpBF3)++){(*_tmpBF0)[*_tmpBF3]=(struct _tuple1*)
# 2697 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2326 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp490=e->r;
if(e->topt == 0){
const char*_tmpBA9;void*_tmpBA8[1];struct Cyc_String_pa_PrintArg_struct _tmpBA7;(_tmpBA7.tag=0,((_tmpBA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBA8[0]=& _tmpBA7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA9="exp_to_c: no type for %s",_tag_dyneither(_tmpBA9,sizeof(char),25))),_tag_dyneither(_tmpBA8,sizeof(void*),1)))))));}
# 2332
if((nv->struct_info).lhs_exp != 0){
void*_tmp494=_tmp490;_LL20C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp495=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp494;if(_tmp495->tag != 28)goto _LL20E;}_LL20D:
 goto _LL20B;_LL20E:;_LL20F:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL20B:;}{
# 2340
void*old_typ=(void*)_check_null(e->topt);
void*_tmp496=_tmp490;struct _tuple1*_tmp49A;void*_tmp49B;enum Cyc_Absyn_Primop _tmp49D;struct Cyc_List_List*_tmp49E;struct Cyc_Absyn_Exp*_tmp4A0;enum Cyc_Absyn_Incrementor _tmp4A1;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Core_Opt*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_List_List*_tmp4B5;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_List_List*_tmp4B8;int _tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_VarargInfo*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_List_List*_tmp4C2;void**_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;int _tmp4C6;enum Cyc_Absyn_Coercion _tmp4C7;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CE;void*_tmp4D0;void*_tmp4D2;struct Cyc_List_List*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D7;struct _dyneither_ptr*_tmp4D8;int _tmp4D9;int _tmp4DA;struct Cyc_Absyn_Exp*_tmp4DC;struct _dyneither_ptr*_tmp4DD;int _tmp4DE;int _tmp4DF;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_List_List*_tmp4E4;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;int _tmp4EB;struct Cyc_Absyn_Exp*_tmp4ED;void*_tmp4EE;int _tmp4EF;struct _tuple1*_tmp4F1;struct Cyc_List_List*_tmp4F2;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Aggrdecl*_tmp4F4;void*_tmp4F6;struct Cyc_List_List*_tmp4F7;struct Cyc_List_List*_tmp4F9;struct Cyc_Absyn_Datatypedecl*_tmp4FA;struct Cyc_Absyn_Datatypefield*_tmp4FB;int _tmp4FF;struct Cyc_Absyn_Exp*_tmp500;void**_tmp501;struct Cyc_Absyn_Exp*_tmp502;int _tmp503;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp508;struct _dyneither_ptr*_tmp509;struct Cyc_Absyn_Stmt*_tmp50B;_LL211: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp497=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp497->tag != 0)goto _LL213;else{if(((_tmp497->f1).Null_c).tag != 1)goto _LL213;}}_LL212: {
# 2347
struct Cyc_Absyn_Exp*_tmp510=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp510,_tmp510))->r;else{
# 2352
struct Cyc_Absyn_Exp*_tmpBAA[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpBAA[2]=_tmp510,((_tmpBAA[1]=_tmp510,((_tmpBAA[0]=_tmp510,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAA,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2354
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2356
goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp498=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp498->tag != 0)goto _LL215;}_LL214:
 goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp499=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp499->tag != 1)goto _LL217;else{_tmp49A=_tmp499->f1;_tmp49B=(void*)_tmp499->f2;}}_LL216:
# 2359
{struct _handler_cons _tmp512;_push_handler(& _tmp512);{int _tmp514=0;if(setjmp(_tmp512.handler))_tmp514=1;if(!_tmp514){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp49A))->r;;_pop_handler();}else{void*_tmp513=(void*)_exn_thrown;void*_tmp516=_tmp513;void*_tmp518;_LL266: {struct Cyc_Dict_Absent_exn_struct*_tmp517=(struct Cyc_Dict_Absent_exn_struct*)_tmp516;if(_tmp517->tag != Cyc_Dict_Absent)goto _LL268;}_LL267: {
# 2361
const char*_tmpBAE;void*_tmpBAD[1];struct Cyc_String_pa_PrintArg_struct _tmpBAC;(_tmpBAC.tag=0,((_tmpBAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp49A)),((_tmpBAD[0]=& _tmpBAC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAE="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBAE,sizeof(char),32))),_tag_dyneither(_tmpBAD,sizeof(void*),1)))))));}_LL268: _tmp518=_tmp516;_LL269:(void)_throw(_tmp518);_LL265:;}};}
# 2363
goto _LL210;_LL217: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp49C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp49C->tag != 2)goto _LL219;else{_tmp49D=_tmp49C->f1;_tmp49E=_tmp49C->f2;}}_LL218: {
# 2366
struct Cyc_List_List*_tmp51C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp49E);
# 2368
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp49E);
switch(_tmp49D){case Cyc_Absyn_Numelts: _LL26A: {
# 2371
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E))->hd;
{void*_stmttmp68=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp51D=_stmttmp68;struct Cyc_Absyn_Exp*_tmp51F;void*_tmp521;union Cyc_Absyn_Constraint*_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;_LL26D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51D;if(_tmp51E->tag != 8)goto _LL26F;else{_tmp51F=(_tmp51E->f1).num_elts;}}_LL26E:
# 2375
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp51F))){
const char*_tmpBB1;void*_tmpBB0;(_tmpBB0=0,Cyc_Tcutil_terr(e->loc,((_tmpBB1="can't calculate numelts",_tag_dyneither(_tmpBB1,sizeof(char),24))),_tag_dyneither(_tmpBB0,sizeof(void*),0)));}
e->r=_tmp51F->r;goto _LL26C;_LL26F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp520=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51D;if(_tmp520->tag != 5)goto _LL271;else{_tmp521=(_tmp520->f1).elt_typ;_tmp522=((_tmp520->f1).ptr_atts).nullable;_tmp523=((_tmp520->f1).ptr_atts).bounds;_tmp524=((_tmp520->f1).ptr_atts).zero_term;}}_LL270:
# 2379
{void*_stmttmp69=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp523);void*_tmp527=_stmttmp69;struct Cyc_Absyn_Exp*_tmp52A;_LL274: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp528=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp527;if(_tmp528->tag != 0)goto _LL276;}_LL275:
# 2381
{struct Cyc_Absyn_Exp*_tmpBB2[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpBB2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp521),0),((_tmpBB2[0]=(struct Cyc_Absyn_Exp*)_tmp49E->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB2,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL273;_LL276: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp529=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp527;if(_tmp529->tag != 1)goto _LL273;else{_tmp52A=_tmp529->f1;}}_LL277:
# 2387
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp524)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp49E->hd);
# 2391
struct Cyc_Absyn_Exp*_tmpBB3[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpBB3[1]=_tmp52A,((_tmpBB3[0]=(struct Cyc_Absyn_Exp*)_tmp49E->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB3,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp522)){
if(!Cyc_Evexp_c_can_eval(_tmp52A)){
const char*_tmpBB6;void*_tmpBB5;(_tmpBB5=0,Cyc_Tcutil_terr(e->loc,((_tmpBB6="can't calculate numelts",_tag_dyneither(_tmpBB6,sizeof(char),24))),_tag_dyneither(_tmpBB5,sizeof(void*),0)));}
# 2396
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp52A,Cyc_Absyn_uint_exp(0,0));}else{
# 2398
e->r=_tmp52A->r;goto _LL273;}}
# 2400
goto _LL273;_LL273:;}
# 2402
goto _LL26C;_LL271:;_LL272: {
# 2404
const char*_tmpBBB;void*_tmpBBA[2];struct Cyc_String_pa_PrintArg_struct _tmpBB9;struct Cyc_String_pa_PrintArg_struct _tmpBB8;(_tmpBB8.tag=0,((_tmpBB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB9.tag=0,((_tmpBB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBBA[0]=& _tmpBB9,((_tmpBBA[1]=& _tmpBB8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBB="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBBB,sizeof(char),41))),_tag_dyneither(_tmpBBA,sizeof(void*),2)))))))))))));}_LL26C:;}
# 2407
break;}case Cyc_Absyn_Plus: _LL26B:
# 2412
{void*_stmttmp66=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd);void*_tmp533=_stmttmp66;void*_tmp535;union Cyc_Absyn_Constraint*_tmp536;union Cyc_Absyn_Constraint*_tmp537;_LL27A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp533;if(_tmp534->tag != 5)goto _LL27C;else{_tmp535=(_tmp534->f1).elt_typ;_tmp536=((_tmp534->f1).ptr_atts).bounds;_tmp537=((_tmp534->f1).ptr_atts).zero_term;}}_LL27B:
# 2414
{void*_stmttmp67=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp536);void*_tmp538=_stmttmp67;struct Cyc_Absyn_Exp*_tmp53B;_LL27F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp539=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp538;if(_tmp539->tag != 0)goto _LL281;}_LL280: {
# 2416
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpBBC[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBBC[2]=e2,((_tmpBBC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp535),0),((_tmpBBC[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL27E;}_LL281: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp53A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp538;if(_tmp53A->tag != 1)goto _LL27E;else{_tmp53B=_tmp53A->f1;}}_LL282:
# 2422
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp537)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E->tl))->hd;
struct Cyc_Absyn_Exp*_tmpBBD[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpBBD[2]=e2,((_tmpBBD[1]=_tmp53B,((_tmpBBD[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
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
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2448
struct Cyc_Absyn_Exp*_tmpBBE[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBBE[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpBBE[1]=
# 2449
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBBE[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2453
break;}case Cyc_Absyn_Eq: _LL283:
 goto _LL284;case Cyc_Absyn_Neq: _LL284:
 goto _LL285;case Cyc_Absyn_Gt: _LL285:
 goto _LL286;case Cyc_Absyn_Gte: _LL286:
 goto _LL287;case Cyc_Absyn_Lt: _LL287:
 goto _LL288;case Cyc_Absyn_Lte: _LL288: {
# 2461
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49E->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL289:
 break;}
# 2473
goto _LL210;}_LL219: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp49F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp49F->tag != 4)goto _LL21B;else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}}_LL21A: {
# 2475
void*e2_cyc_typ=(void*)_check_null(_tmp4A0->topt);
# 2484 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBBF;struct _dyneither_ptr incr_str=(_tmpBBF="increment",_tag_dyneither(_tmpBBF,sizeof(char),10));
if(_tmp4A1 == Cyc_Absyn_PreDec  || _tmp4A1 == Cyc_Absyn_PostDec){const char*_tmpBC0;incr_str=((_tmpBC0="decrement",_tag_dyneither(_tmpBC0,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4A0,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBC4;void*_tmpBC3[1];struct Cyc_String_pa_PrintArg_struct _tmpBC2;(_tmpBC2.tag=0,((_tmpBC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBC3[0]=& _tmpBC2,Cyc_Tcutil_terr(e->loc,((_tmpBC4="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBC4,sizeof(char),74))),_tag_dyneither(_tmpBC3,sizeof(void*),1)))))));}{
const char*_tmpBC7;void*_tmpBC6;(_tmpBC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC7="in-place inc/dec on zero-term",_tag_dyneither(_tmpBC7,sizeof(char),30))),_tag_dyneither(_tmpBC6,sizeof(void*),0)));};}{
# 2493
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4A0,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp545=Cyc_Absyn_signed_int_exp(1,0);
_tmp545->topt=Cyc_Absyn_sint_typ;
switch(_tmp4A1){case Cyc_Absyn_PreInc: _LL28B:
# 2500
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBCD;struct Cyc_Core_Opt*_tmpBCC;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBCB;e->r=(void*)((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBCD.tag=3,((_tmpBCD.f1=_tmp4A0,((_tmpBCD.f2=((_tmpBCC=_cycalloc_atomic(sizeof(*_tmpBCC)),((_tmpBCC->v=(void*)Cyc_Absyn_Plus,_tmpBCC)))),((_tmpBCD.f3=_tmp545,_tmpBCD)))))))),_tmpBCB))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL28C:
# 2504
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBD3;struct Cyc_Core_Opt*_tmpBD2;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBD1;e->r=(void*)((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1[0]=((_tmpBD3.tag=3,((_tmpBD3.f1=_tmp4A0,((_tmpBD3.f2=((_tmpBD2=_cycalloc_atomic(sizeof(*_tmpBD2)),((_tmpBD2->v=(void*)Cyc_Absyn_Minus,_tmpBD2)))),((_tmpBD3.f3=_tmp545,_tmpBD3)))))))),_tmpBD1))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL28D:
# 2508
{const char*_tmpBD7;void*_tmpBD6[1];struct Cyc_String_pa_PrintArg_struct _tmpBD5;(_tmpBD5.tag=0,((_tmpBD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD6[0]=& _tmpBD5,Cyc_Tcutil_terr(e->loc,((_tmpBD7="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBD7,sizeof(char),59))),_tag_dyneither(_tmpBD6,sizeof(void*),1)))))));}{
# 2510
const char*_tmpBDA;void*_tmpBD9;(_tmpBD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDA="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBDA,sizeof(char),34))),_tag_dyneither(_tmpBD9,sizeof(void*),0)));};}}
# 2513
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A0);
Cyc_Toc_set_lhs(nv,0);{
# 2518
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4A1 == Cyc_Absyn_PostInc  || _tmp4A1 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2524
if(_tmp4A1 == Cyc_Absyn_PreDec  || _tmp4A1 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpBDB[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBDB[2]=
# 2528
Cyc_Absyn_signed_int_exp(change,0),((_tmpBDB[1]=
# 2527
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBDB[0]=
# 2526
Cyc_Toc_push_address_exp(_tmp4A0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDB,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2529
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2533
struct Cyc_Toc_functionSet*_tmp552=_tmp4A1 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2535
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp552,_tmp4A0);
struct Cyc_Absyn_Exp*_tmpBDC[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBDC[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBDC[0]=
# 2536
Cyc_Toc_push_address_exp(_tmp4A0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDC,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2538
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4A0)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4A0,0,Cyc_Toc_incr_lvalue,_tmp4A1);
e->r=_tmp4A0->r;}}}
# 2542
goto _LL210;};};}_LL21B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4A2=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4A2->tag != 3)goto _LL21D;else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;_tmp4A5=_tmp4A2->f3;}}_LL21C: {
# 2561 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4A3->topt);
void*e2_old_typ=(void*)_check_null(_tmp4A5->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4A3,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A3);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4A5);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4A3,e1_old_typ,_tmp4A4,_tmp4A5,e2_old_typ,f_tag,tagged_member_struct_type);
# 2572
return;}{
# 2574
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4A3,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4A3,_tmp4A4,_tmp4A5,ptr_type,is_dyneither,elt_type);
# 2580
return;}{
# 2584
int e1_poly=Cyc_Toc_is_poly_project(_tmp4A3);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A3);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4A5);{
# 2590
int done=0;
if(_tmp4A4 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _stmttmp64=(enum Cyc_Absyn_Primop)_tmp4A4->v;switch(_stmttmp64){case Cyc_Absyn_Plus: _LL28F:
 change=_tmp4A5;break;case Cyc_Absyn_Minus: _LL290:
# 2598
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4A5,0);break;default: _LL291: {
const char*_tmpBDF;void*_tmpBDE;(_tmpBDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDF="bad t ? pointer arithmetic",_tag_dyneither(_tmpBDF,sizeof(char),27))),_tag_dyneither(_tmpBDE,sizeof(void*),0)));}}}
# 2601
done=1;{
# 2603
struct Cyc_Absyn_Exp*_tmp557=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBE0[3];e->r=Cyc_Toc_fncall_exp_r(_tmp557,((_tmpBE0[2]=change,((_tmpBE0[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBE0[0]=
# 2604
Cyc_Toc_push_address_exp(_tmp4A3),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE0,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2607
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2610
enum Cyc_Absyn_Primop _stmttmp65=(enum Cyc_Absyn_Primop)_tmp4A4->v;switch(_stmttmp65){case Cyc_Absyn_Plus: _LL293:
# 2612
 done=1;
{struct Cyc_Absyn_Exp*_tmpBE1[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A3),((_tmpBE1[1]=_tmp4A5,((_tmpBE1[0]=_tmp4A3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE1,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL294: {
const char*_tmpBE4;void*_tmpBE3;(_tmpBE3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE4="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBE4,sizeof(char),39))),_tag_dyneither(_tmpBE3,sizeof(void*),0)));}}}}}
# 2619
if(!done){
# 2621
if(e1_poly)
_tmp4A5->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4A5->r,0));
# 2627
if(!Cyc_Absyn_is_lvalue(_tmp4A3)){
{struct _tuple19 _tmpBE7;struct _tuple19*_tmpBE6;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple19*),struct _tuple19*f_env))Cyc_Toc_lvalue_assign)(_tmp4A3,0,Cyc_Toc_assignop_lvalue,((_tmpBE6=_cycalloc(sizeof(struct _tuple19)* 1),((_tmpBE6[0]=(struct _tuple19)((_tmpBE7.f1=_tmp4A4,((_tmpBE7.f2=_tmp4A5,_tmpBE7)))),_tmpBE6)))));}
e->r=_tmp4A3->r;}}
# 2632
goto _LL210;};};};}_LL21D: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4A6=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4A6->tag != 5)goto _LL21F;else{_tmp4A7=_tmp4A6->f1;_tmp4A8=_tmp4A6->f2;_tmp4A9=_tmp4A6->f3;}}_LL21E:
# 2634
 Cyc_Toc_exp_to_c(nv,_tmp4A7);
Cyc_Toc_exp_to_c(nv,_tmp4A8);
Cyc_Toc_exp_to_c(nv,_tmp4A9);
goto _LL210;_LL21F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4AA=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4AA->tag != 6)goto _LL221;else{_tmp4AB=_tmp4AA->f1;_tmp4AC=_tmp4AA->f2;}}_LL220:
# 2639
 Cyc_Toc_exp_to_c(nv,_tmp4AB);
Cyc_Toc_exp_to_c(nv,_tmp4AC);
goto _LL210;_LL221: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4AD=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4AD->tag != 7)goto _LL223;else{_tmp4AE=_tmp4AD->f1;_tmp4AF=_tmp4AD->f2;}}_LL222:
# 2643
 Cyc_Toc_exp_to_c(nv,_tmp4AE);
Cyc_Toc_exp_to_c(nv,_tmp4AF);
goto _LL210;_LL223: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4B0=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4B0->tag != 8)goto _LL225;else{_tmp4B1=_tmp4B0->f1;_tmp4B2=_tmp4B0->f2;}}_LL224:
# 2647
 Cyc_Toc_exp_to_c(nv,_tmp4B1);
Cyc_Toc_exp_to_c(nv,_tmp4B2);
goto _LL210;_LL225: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4B3=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4B3->tag != 9)goto _LL227;else{_tmp4B4=_tmp4B3->f1;_tmp4B5=_tmp4B3->f2;if(_tmp4B3->f3 != 0)goto _LL227;}}_LL226:
# 2651
 Cyc_Toc_exp_to_c(nv,_tmp4B4);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4B5);
goto _LL210;_LL227: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4B6=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4B6->tag != 9)goto _LL229;else{_tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;if(_tmp4B6->f3 == 0)goto _LL229;_tmp4B9=(_tmp4B6->f3)->num_varargs;_tmp4BA=(_tmp4B6->f3)->injectors;_tmp4BB=(_tmp4B6->f3)->vai;}}_LL228: {
# 2663 "toc.cyc"
struct _RegionHandle _tmp55E=_new_region("r");struct _RegionHandle*r=& _tmp55E;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4B9,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4BB->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2671
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B8);
int num_normargs=num_args - _tmp4B9;
# 2675
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4B8=_tmp4B8->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B8))->hd);{
struct Cyc_List_List*_tmpBE8;new_args=((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->hd=(struct Cyc_Absyn_Exp*)_tmp4B8->hd,((_tmpBE8->tl=new_args,_tmpBE8))))));};}}
# 2680
{struct Cyc_Absyn_Exp*_tmpBEB[3];struct Cyc_List_List*_tmpBEA;new_args=((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBEB[2]=num_varargs_exp,((_tmpBEB[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBEB[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBEA->tl=new_args,_tmpBEA))))));}
# 2685
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2687
Cyc_Toc_exp_to_c(nv,_tmp4B7);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4B7,new_args,0),0);
# 2691
if(_tmp4BB->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_stmttmp62=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4BB->type));void*_tmp562=_stmttmp62;struct Cyc_Absyn_Datatypedecl*_tmp564;_LL297: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp562;if(_tmp563->tag != 3)goto _LL299;else{if((((_tmp563->f1).datatype_info).KnownDatatype).tag != 2)goto _LL299;_tmp564=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp563->f1).datatype_info).KnownDatatype).val);}}_LL298:
 tud=_tmp564;goto _LL296;_LL299:;_LL29A: {
const char*_tmpBEE;void*_tmpBED;(_tmpBED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEE="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBEE,sizeof(char),44))),_tag_dyneither(_tmpBED,sizeof(void*),0)));}_LL296:;}{
# 2697
unsigned int _tmpBF3;unsigned int _tmpBF2;struct _dyneither_ptr _tmpBF1;struct _tuple1**_tmpBF0;unsigned int _tmpBEF;struct _dyneither_ptr vs=(_tmpBEF=(unsigned int)_tmp4B9,((_tmpBF0=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpBEF)),((_tmpBF1=_tag_dyneither(_tmpBF0,sizeof(struct _tuple1*),_tmpBEF),((((_tmpBF2=_tmpBEF,_tmpBF4(& _tmpBF3,& _tmpBF2,& _tmpBF0))),_tmpBF1)))))));
# 2699
if(_tmp4B9 != 0){
# 2701
struct Cyc_List_List*_tmp567=0;
{int i=_tmp4B9 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBF5;_tmp567=((_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpBF5->tl=_tmp567,_tmpBF5))))));}}
# 2705
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp567,0),s,0);{
# 2708
int i=0;for(0;_tmp4B8 != 0;(((_tmp4B8=_tmp4B8->tl,_tmp4BA=_tmp4BA->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4B8->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_stmttmp63=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4BA))->hd;struct _tuple1*_tmp56A;struct Cyc_List_List*_tmp56B;struct Cyc_Absyn_Datatypefield*_tmp569=_stmttmp63;_tmp56A=_tmp569->name;_tmp56B=_tmp569->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp56B))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2723
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2726
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp56A),0),s,0);
# 2729
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp56A,tud->name)),0,s,0);};}};}else{
# 2736
struct _tuple16*_tmpBF6[3];struct Cyc_List_List*_tmp56C=(_tmpBF6[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF6[1]=
# 2736
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF6[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF6,sizeof(struct _tuple16*),3)))))));
# 2738
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2748
{int i=0;for(0;_tmp4B8 != 0;(_tmp4B8=_tmp4B8->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4B8->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4B8->hd,0),s,0);}}
# 2754
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2767 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2769
_npop_handler(0);goto _LL210;
# 2663 "toc.cyc"
;_pop_region(r);}_LL229: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4BC=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4BC->tag != 10)goto _LL22B;else{_tmp4BD=_tmp4BC->f1;}}_LL22A:
# 2772 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4BD);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4BD),0))->r;
goto _LL210;_LL22B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4BE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4BE->tag != 11)goto _LL22D;else{_tmp4BF=_tmp4BE->f1;}}_LL22C:
 Cyc_Toc_exp_to_c(nv,_tmp4BF);goto _LL210;_LL22D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4C0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4C0->tag != 12)goto _LL22F;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C0->f2;}}_LL22E:
# 2777
 Cyc_Toc_exp_to_c(nv,_tmp4C1);
# 2786 "toc.cyc"
for(0;_tmp4C2 != 0;_tmp4C2=_tmp4C2->tl){
enum Cyc_Absyn_KindQual _tmp572=(Cyc_Tcutil_typ_kind((void*)_tmp4C2->hd))->kind;
if(_tmp572 != Cyc_Absyn_EffKind  && _tmp572 != Cyc_Absyn_RgnKind){
{void*_stmttmp61=Cyc_Tcutil_compress((void*)_tmp4C2->hd);void*_tmp573=_stmttmp61;_LL29C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp573;if(_tmp574->tag != 2)goto _LL29E;}_LL29D:
 goto _LL29F;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp573;if(_tmp575->tag != 3)goto _LL2A0;}_LL29F:
 continue;_LL2A0:;_LL2A1:
# 2793
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4C1,0))->r;
goto _LL29B;_LL29B:;}
# 2796
break;}}
# 2799
goto _LL210;_LL22F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4C3->tag != 13)goto _LL231;else{_tmp4C4=(void**)((void**)& _tmp4C3->f1);_tmp4C5=_tmp4C3->f2;_tmp4C6=_tmp4C3->f3;_tmp4C7=_tmp4C3->f4;}}_LL230: {
# 2801
void*old_t2=(void*)_check_null(_tmp4C5->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4C4;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4C4=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4C5);
# 2808
{struct _tuple20 _tmpBF7;struct _tuple20 _stmttmp5D=(_tmpBF7.f1=Cyc_Tcutil_compress(old_t2),((_tmpBF7.f2=Cyc_Tcutil_compress(new_typ),_tmpBF7)));struct _tuple20 _tmp576=_stmttmp5D;struct Cyc_Absyn_PtrInfo _tmp578;struct Cyc_Absyn_PtrInfo _tmp57A;struct Cyc_Absyn_PtrInfo _tmp57C;_LL2A3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576.f1;if(_tmp577->tag != 5)goto _LL2A5;else{_tmp578=_tmp577->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576.f2;if(_tmp579->tag != 5)goto _LL2A5;else{_tmp57A=_tmp579->f1;}};_LL2A4: {
# 2810
int _tmp57E=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp578.ptr_atts).nullable);
int _tmp57F=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57A.ptr_atts).nullable);
void*_tmp580=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp578.ptr_atts).bounds);
void*_tmp581=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57A.ptr_atts).bounds);
int _tmp582=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp578.ptr_atts).zero_term);
int _tmp583=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57A.ptr_atts).zero_term);
{struct _tuple20 _tmpBF8;struct _tuple20 _stmttmp5F=(_tmpBF8.f1=_tmp580,((_tmpBF8.f2=_tmp581,_tmpBF8)));struct _tuple20 _tmp584=_stmttmp5F;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Exp*_tmp588;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Exp*_tmp58E;_LL2AA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp585=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f1;if(_tmp585->tag != 1)goto _LL2AC;else{_tmp586=_tmp585->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp587=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f2;if(_tmp587->tag != 1)goto _LL2AC;else{_tmp588=_tmp587->f1;}};_LL2AB:
# 2818
 if((!Cyc_Evexp_c_can_eval(_tmp586) || !Cyc_Evexp_c_can_eval(_tmp588)) && !
Cyc_Evexp_same_const_exp(_tmp586,_tmp588)){
const char*_tmpBFB;void*_tmpBFA;(_tmpBFA=0,Cyc_Tcutil_terr(e->loc,((_tmpBFB="can't validate cast due to potential size differences",_tag_dyneither(_tmpBFB,sizeof(char),54))),_tag_dyneither(_tmpBFA,sizeof(void*),0)));}
if(_tmp57E  && !_tmp57F){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFE;void*_tmpBFD;(_tmpBFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFE="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBFE,sizeof(char),44))),_tag_dyneither(_tmpBFD,sizeof(void*),0)));}
# 2827
if(_tmp4C7 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC02;void*_tmpC01[1];struct Cyc_String_pa_PrintArg_struct _tmpC00;(_tmpC00.tag=0,((_tmpC00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC01[0]=& _tmpC00,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC02="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC02,sizeof(char),41))),_tag_dyneither(_tmpC01,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4C5);
if(do_null_check){
if(!_tmp4C6){
const char*_tmpC05;void*_tmpC04;(_tmpC04=0,Cyc_Tcutil_warn(e->loc,((_tmpC05="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC05,sizeof(char),58))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}{
# 2834
struct Cyc_List_List*_tmpC06;e->r=Cyc_Toc_cast_it_r(*_tmp4C4,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC06=_cycalloc(sizeof(*_tmpC06)),((_tmpC06->hd=_tmp4C5,((_tmpC06->tl=0,_tmpC06)))))),0));};}else{
# 2837
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C5->r;}};}else{
# 2842
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C5->r;}
# 2849
goto _LL2A9;_LL2AC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp589=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f1;if(_tmp589->tag != 1)goto _LL2AE;else{_tmp58A=_tmp589->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp584.f2;if(_tmp58B->tag != 0)goto _LL2AE;};_LL2AD:
# 2851
 if(!Cyc_Evexp_c_can_eval(_tmp58A)){
const char*_tmpC09;void*_tmpC08;(_tmpC08=0,Cyc_Tcutil_terr(e->loc,((_tmpC09="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC09,sizeof(char),71))),_tag_dyneither(_tmpC08,sizeof(void*),0)));}
# 2854
if(_tmp4C7 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC0D;void*_tmpC0C[1];struct Cyc_String_pa_PrintArg_struct _tmpC0B;(_tmpC0B.tag=0,((_tmpC0B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC0C[0]=& _tmpC0B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC0D="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC0D,sizeof(char),44))),_tag_dyneither(_tmpC0C,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2858
if((_tmp582  && !(_tmp57A.elt_tq).real_const) && !_tmp583)
# 2861
_tmp58A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp58A,Cyc_Absyn_uint_exp(1,0),0);
# 2863
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp58A,_tmp4C5))->r;}else{
# 2865
struct Cyc_Absyn_Exp*_tmp5A0=Cyc_Toc__tag_dyneither_e;
# 2867
if(_tmp582){
# 2872
struct _tuple1*_tmp5A1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5A2=Cyc_Absyn_var_exp(_tmp5A1,0);
struct Cyc_Absyn_Exp*arg3;
# 2877
{void*_stmttmp60=_tmp4C5->r;void*_tmp5A3=_stmttmp60;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A3;if(_tmp5A4->tag != 0)goto _LL2B5;else{if(((_tmp5A4->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4:
# 2879
 arg3=_tmp58A;goto _LL2B2;_LL2B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A3;if(_tmp5A5->tag != 0)goto _LL2B7;else{if(((_tmp5A5->f1).Wstring_c).tag != 9)goto _LL2B7;}}_LL2B6:
# 2881
 arg3=_tmp58A;goto _LL2B2;_LL2B7:;_LL2B8:
# 2883
{struct Cyc_Absyn_Exp*_tmpC0E[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4C5),(
# 2885
(_tmpC0E[1]=_tmp58A,((_tmpC0E[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5A2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B2;_LL2B2:;}
# 2888
if(!_tmp583  && !(_tmp57A.elt_tq).real_const)
# 2891
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2893
struct Cyc_Absyn_Exp*_tmp5A7=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57A.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC0F[3];struct Cyc_Absyn_Exp*_tmp5A8=Cyc_Absyn_fncall_exp(_tmp5A0,((_tmpC0F[2]=arg3,((_tmpC0F[1]=_tmp5A7,((_tmpC0F[0]=_tmp5A2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5A9=Cyc_Absyn_exp_stmt(_tmp5A8,0);
_tmp5A9=Cyc_Absyn_declare_stmt(_tmp5A1,Cyc_Toc_typ_to_c(old_t2),_tmp4C5,_tmp5A9,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5A9);};}else{
# 2900
struct Cyc_Absyn_Exp*_tmpC10[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5A0,(
(_tmpC10[2]=_tmp58A,((_tmpC10[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57A.elt_typ),0),((_tmpC10[0]=_tmp4C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC10,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2906
goto _LL2A9;_LL2AE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp584.f1;if(_tmp58C->tag != 0)goto _LL2B0;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f2;if(_tmp58D->tag != 1)goto _LL2B0;else{_tmp58E=_tmp58D->f1;}};_LL2AF:
# 2908
 if(!Cyc_Evexp_c_can_eval(_tmp58E)){
const char*_tmpC13;void*_tmpC12;(_tmpC12=0,Cyc_Tcutil_terr(e->loc,((_tmpC13="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC13,sizeof(char),71))),_tag_dyneither(_tmpC12,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC16;void*_tmpC15;(_tmpC15=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC16="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC16,sizeof(char),45))),_tag_dyneither(_tmpC15,sizeof(void*),0)));}{
# 2920 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5B0=_tmp58E;
if(_tmp582  && !_tmp583)
_tmp5B0=Cyc_Absyn_add_exp(_tmp58E,Cyc_Absyn_uint_exp(1,0),0);{
# 2927
struct Cyc_Absyn_Exp*_tmp5B1=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC17[3];struct Cyc_Absyn_Exp*_tmp5B2=Cyc_Absyn_fncall_exp(_tmp5B1,(
(_tmpC17[2]=_tmp5B0,((_tmpC17[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp578.elt_typ),0),((_tmpC17[0]=_tmp4C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC17,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2932
if(_tmp57F){
struct Cyc_List_List*_tmpC18;_tmp5B2->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC18=_cycalloc(sizeof(*_tmpC18)),((_tmpC18->hd=Cyc_Absyn_copy_exp(_tmp5B2),((_tmpC18->tl=0,_tmpC18)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4C4,_tmp5B2);
goto _LL2A9;};};_LL2B0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp584.f1;if(_tmp58F->tag != 0)goto _LL2A9;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp590=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp584.f2;if(_tmp590->tag != 0)goto _LL2A9;};_LL2B1:
# 2940
 DynCast:
 if((_tmp582  && !_tmp583) && !(_tmp57A.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC1B;void*_tmpC1A;(_tmpC1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1B="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC1B,sizeof(char),70))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5B7=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC1C[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5B7,(
(_tmpC1C[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC1C[1]=
# 2946
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp578.elt_typ),0),((_tmpC1C[0]=_tmp4C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2949
goto _LL2A9;_LL2A9:;}
# 2951
goto _LL2A2;}_LL2A5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576.f1;if(_tmp57B->tag != 5)goto _LL2A7;else{_tmp57C=_tmp57B->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp576.f2;if(_tmp57D->tag != 6)goto _LL2A7;};_LL2A6:
# 2953
{void*_stmttmp5E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57C.ptr_atts).bounds);void*_tmp5BA=_stmttmp5E;_LL2BA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5BA;if(_tmp5BB->tag != 0)goto _LL2BC;}_LL2BB:
# 2955
 _tmp4C5->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4C5->r,_tmp4C5->loc),Cyc_Toc_curr_sp);goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 2958
goto _LL2A2;_LL2A7:;_LL2A8:
# 2960
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C5->r;
goto _LL2A2;_LL2A2:;}
# 2964
goto _LL210;}_LL231: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4C8=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4C8->tag != 14)goto _LL233;else{_tmp4C9=_tmp4C8->f1;}}_LL232:
# 2968
{void*_stmttmp5C=_tmp4C9->r;void*_tmp5BD=_stmttmp5C;struct _tuple1*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*_tmp5C1;struct Cyc_List_List*_tmp5C3;_LL2BF: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5BE=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BD;if(_tmp5BE->tag != 28)goto _LL2C1;else{_tmp5BF=_tmp5BE->f1;_tmp5C0=_tmp5BE->f2;_tmp5C1=_tmp5BE->f3;}}_LL2C0:
# 2970
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC20;void*_tmpC1F[1];struct Cyc_String_pa_PrintArg_struct _tmpC1E;(_tmpC1E.tag=0,((_tmpC1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C9->loc)),((_tmpC1F[0]=& _tmpC1E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC20="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC20,sizeof(char),42))),_tag_dyneither(_tmpC1F,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp5C7=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4C9->topt),_tmp5C0,1,0,_tmp5C1,_tmp5BF);
e->r=_tmp5C7->r;
e->topt=_tmp5C7->topt;
goto _LL2BE;};_LL2C1: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5C2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5BD;if(_tmp5C2->tag != 23)goto _LL2C3;else{_tmp5C3=_tmp5C2->f1;}}_LL2C2:
# 2978
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC24;void*_tmpC23[1];struct Cyc_String_pa_PrintArg_struct _tmpC22;(_tmpC22.tag=0,((_tmpC22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C9->loc)),((_tmpC23[0]=& _tmpC22,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC24="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC24,sizeof(char),42))),_tag_dyneither(_tmpC23,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5C3))->r;
goto _LL2BE;_LL2C3:;_LL2C4:
# 2984
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4C9);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4C9)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4C9,0,Cyc_Toc_address_lvalue,1);
# 2990
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4C9);}
# 2992
goto _LL2BE;_LL2BE:;}
# 2994
goto _LL210;_LL233: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4CA=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4CA->tag != 15)goto _LL235;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CA->f2;}}_LL234:
# 2997
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC28;void*_tmpC27[1];struct Cyc_String_pa_PrintArg_struct _tmpC26;(_tmpC26.tag=0,((_tmpC26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4CC->loc)),((_tmpC27[0]=& _tmpC26,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC28="%s: new at top-level",_tag_dyneither(_tmpC28,sizeof(char),21))),_tag_dyneither(_tmpC27,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp4CC->topt);
{void*_stmttmp53=_tmp4CC->r;void*_tmp5CE=_stmttmp53;struct Cyc_List_List*_tmp5D0;struct Cyc_Absyn_Vardecl*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D4;int _tmp5D5;struct Cyc_Absyn_Exp*_tmp5D7;void*_tmp5D8;int _tmp5D9;struct _tuple1*_tmp5DB;struct Cyc_List_List*_tmp5DC;struct Cyc_List_List*_tmp5DD;struct Cyc_Absyn_Aggrdecl*_tmp5DE;struct Cyc_List_List*_tmp5E0;_LL2C6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5CF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5CF->tag != 25)goto _LL2C8;else{_tmp5D0=_tmp5CF->f1;}}_LL2C7: {
# 3005
struct _tuple1*_tmp5E1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5E2=Cyc_Absyn_var_exp(_tmp5E1,0);
struct Cyc_Absyn_Stmt*_tmp5E3=Cyc_Toc_init_array(nv,new_e_type,_tmp5E2,_tmp5D0,Cyc_Absyn_exp_stmt(_tmp5E2,0));
void*old_elt_typ;
{void*_stmttmp5B=Cyc_Tcutil_compress(old_typ);void*_tmp5E4=_stmttmp5B;void*_tmp5E6;struct Cyc_Absyn_Tqual _tmp5E7;union Cyc_Absyn_Constraint*_tmp5E8;_LL2D3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E4;if(_tmp5E5->tag != 5)goto _LL2D5;else{_tmp5E6=(_tmp5E5->f1).elt_typ;_tmp5E7=(_tmp5E5->f1).elt_tq;_tmp5E8=((_tmp5E5->f1).ptr_atts).zero_term;}}_LL2D4:
# 3011
 old_elt_typ=_tmp5E6;goto _LL2D2;_LL2D5:;_LL2D6: {
# 3013
const char*_tmpC2B;void*_tmpC2A;old_elt_typ=(
(_tmpC2A=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2B="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC2B,sizeof(char),52))),_tag_dyneither(_tmpC2A,sizeof(void*),0))));}_LL2D2:;}{
# 3016
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5EB=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5EC=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5D0),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4CB == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5EC);else{
# 3024
struct Cyc_Absyn_Exp*r=_tmp4CB;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5EC);}
# 3029
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5E1,_tmp5EB,e1,_tmp5E3,0));
goto _LL2C5;};}_LL2C8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5D1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5D1->tag != 26)goto _LL2CA;else{_tmp5D2=_tmp5D1->f1;_tmp5D3=_tmp5D1->f2;_tmp5D4=_tmp5D1->f3;_tmp5D5=_tmp5D1->f4;}}_LL2C9: {
# 3042 "toc.cyc"
int is_dyneither_ptr=0;
{void*_stmttmp59=Cyc_Tcutil_compress(old_typ);void*_tmp5ED=_stmttmp59;void*_tmp5EF;struct Cyc_Absyn_Tqual _tmp5F0;union Cyc_Absyn_Constraint*_tmp5F1;union Cyc_Absyn_Constraint*_tmp5F2;_LL2D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5ED;if(_tmp5EE->tag != 5)goto _LL2DA;else{_tmp5EF=(_tmp5EE->f1).elt_typ;_tmp5F0=(_tmp5EE->f1).elt_tq;_tmp5F1=((_tmp5EE->f1).ptr_atts).bounds;_tmp5F2=((_tmp5EE->f1).ptr_atts).zero_term;}}_LL2D9:
# 3045
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5F1)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2D7;_LL2DA:;_LL2DB: {
const char*_tmpC2E;void*_tmpC2D;(_tmpC2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2E="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC2E,sizeof(char),42))),_tag_dyneither(_tmpC2D,sizeof(void*),0)));}_LL2D7:;}{
# 3049
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp5D4->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5D3);{
struct Cyc_Absyn_Exp*_tmp5F5=Cyc_Absyn_var_exp(max,0);
# 3057
if(_tmp5D5)
_tmp5F5=Cyc_Absyn_add_exp(_tmp5F5,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),_tmp5D2,Cyc_Absyn_var_exp(max,0),_tmp5D4,_tmp5D5,
Cyc_Toc_skip_stmt_dl(),1);
# 3063
struct _RegionHandle _tmp5F6=_new_region("r");struct _RegionHandle*r=& _tmp5F6;_push_region(r);{
struct _tuple21*_tmpC31;struct Cyc_List_List*_tmpC30;struct Cyc_List_List*decls=
(_tmpC30=_region_malloc(r,sizeof(*_tmpC30)),((_tmpC30->hd=((_tmpC31=_region_malloc(r,sizeof(*_tmpC31)),((_tmpC31->f1=max,((_tmpC31->f2=Cyc_Absyn_uint_typ,((_tmpC31->f3=_tmp5D3,_tmpC31)))))))),((_tmpC30->tl=0,_tmpC30)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4CB == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC32[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC32[1]=_tmp5F5,((_tmpC32[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC32,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3073
struct Cyc_Absyn_Exp*r=_tmp4CB;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC33[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC33[1]=_tmp5F5,((_tmpC33[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC33,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3080
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpC36;struct Cyc_List_List*_tmpC35;decls=((_tmpC35=_region_malloc(r,sizeof(*_tmpC35)),((_tmpC35->hd=((_tmpC36=_region_malloc(r,sizeof(*_tmpC36)),((_tmpC36->f1=a,((_tmpC36->f2=ptr_typ,((_tmpC36->f3=ainit,_tmpC36)))))))),((_tmpC35->tl=decls,_tmpC35))))));}
if(is_dyneither_ptr){
struct _tuple1*_tmp5FB=Cyc_Toc_temp_var();
void*_tmp5FC=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC37[3];struct Cyc_Absyn_Exp*_tmp5FE=
Cyc_Absyn_fncall_exp(_tmp5FD,(
(_tmpC37[2]=_tmp5F5,((_tmpC37[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC37[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3090
{struct _tuple21*_tmpC3A;struct Cyc_List_List*_tmpC39;decls=((_tmpC39=_region_malloc(r,sizeof(*_tmpC39)),((_tmpC39->hd=((_tmpC3A=_region_malloc(r,sizeof(*_tmpC3A)),((_tmpC3A->f1=_tmp5FB,((_tmpC3A->f2=_tmp5FC,((_tmpC3A->f3=_tmp5FE,_tmpC3A)))))))),((_tmpC39->tl=decls,_tmpC39))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp5FB,0),0),0);}else{
# 3093
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp602=decls;for(0;_tmp602 != 0;_tmp602=_tmp602->tl){
struct _tuple21 _stmttmp5A=*((struct _tuple21*)_tmp602->hd);struct _tuple1*_tmp604;void*_tmp605;struct Cyc_Absyn_Exp*_tmp606;struct _tuple21 _tmp603=_stmttmp5A;_tmp604=_tmp603.f1;_tmp605=_tmp603.f2;_tmp606=_tmp603.f3;
s=Cyc_Absyn_declare_stmt(_tmp604,_tmp605,_tmp606,s,0);}}
# 3098
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3100
_npop_handler(0);goto _LL2C5;
# 3063
;_pop_region(r);};};};}_LL2CA: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5D6=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5D6->tag != 27)goto _LL2CC;else{_tmp5D7=_tmp5D6->f1;_tmp5D8=(void*)_tmp5D6->f2;_tmp5D9=_tmp5D6->f3;}}_LL2CB: {
# 3104
int is_dyneither_ptr=0;
{void*_stmttmp57=Cyc_Tcutil_compress(old_typ);void*_tmp609=_stmttmp57;void*_tmp60B;struct Cyc_Absyn_Tqual _tmp60C;union Cyc_Absyn_Constraint*_tmp60D;union Cyc_Absyn_Constraint*_tmp60E;_LL2DD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp609;if(_tmp60A->tag != 5)goto _LL2DF;else{_tmp60B=(_tmp60A->f1).elt_typ;_tmp60C=(_tmp60A->f1).elt_tq;_tmp60D=((_tmp60A->f1).ptr_atts).bounds;_tmp60E=((_tmp60A->f1).ptr_atts).zero_term;}}_LL2DE:
# 3107
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp60D)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2DC;_LL2DF:;_LL2E0: {
const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3D="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC3D,sizeof(char),42))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}_LL2DC:;}{
# 3111
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=_tmp5D8;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5D7);{
struct Cyc_Absyn_Exp*_tmp611=Cyc_Absyn_var_exp(max,0);
# 3119
struct Cyc_Absyn_Stmt*s;
if(_tmp5D9){
_tmp611=Cyc_Absyn_add_exp(_tmp611,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0);
s=Cyc_Absyn_exp_stmt(ex,0);};}else{
# 3127
s=Cyc_Absyn_skip_stmt(0);}{
# 3129
struct _RegionHandle _tmp612=_new_region("r");struct _RegionHandle*r=& _tmp612;_push_region(r);{
struct _tuple21*_tmpC40;struct Cyc_List_List*_tmpC3F;struct Cyc_List_List*decls=
(_tmpC3F=_region_malloc(r,sizeof(*_tmpC3F)),((_tmpC3F->hd=((_tmpC40=_region_malloc(r,sizeof(*_tmpC40)),((_tmpC40->f1=max,((_tmpC40->f2=Cyc_Absyn_uint_typ,((_tmpC40->f3=_tmp5D7,_tmpC40)))))))),((_tmpC3F->tl=0,_tmpC3F)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4CB == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC41[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC41[1]=_tmp611,((_tmpC41[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC41,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3139
struct Cyc_Absyn_Exp*r=_tmp4CB;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC42[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC42[1]=_tmp611,((_tmpC42[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC42,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3146
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpC45;struct Cyc_List_List*_tmpC44;decls=((_tmpC44=_region_malloc(r,sizeof(*_tmpC44)),((_tmpC44->hd=((_tmpC45=_region_malloc(r,sizeof(*_tmpC45)),((_tmpC45->f1=a,((_tmpC45->f2=ptr_typ,((_tmpC45->f3=ainit,_tmpC45)))))))),((_tmpC44->tl=decls,_tmpC44))))));}
if(is_dyneither_ptr){
struct _tuple1*_tmp617=Cyc_Toc_temp_var();
void*_tmp618=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp619=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC46[3];struct Cyc_Absyn_Exp*_tmp61A=
Cyc_Absyn_fncall_exp(_tmp619,(
(_tmpC46[2]=_tmp611,((_tmpC46[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC46[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC46,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3156
{struct _tuple21*_tmpC49;struct Cyc_List_List*_tmpC48;decls=((_tmpC48=_region_malloc(r,sizeof(*_tmpC48)),((_tmpC48->hd=((_tmpC49=_region_malloc(r,sizeof(*_tmpC49)),((_tmpC49->f1=_tmp617,((_tmpC49->f2=_tmp618,((_tmpC49->f3=_tmp61A,_tmpC49)))))))),((_tmpC48->tl=decls,_tmpC48))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp617,0),0),0);}else{
# 3159
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp61E=decls;for(0;_tmp61E != 0;_tmp61E=_tmp61E->tl){
struct _tuple21 _stmttmp58=*((struct _tuple21*)_tmp61E->hd);struct _tuple1*_tmp620;void*_tmp621;struct Cyc_Absyn_Exp*_tmp622;struct _tuple21 _tmp61F=_stmttmp58;_tmp620=_tmp61F.f1;_tmp621=_tmp61F.f2;_tmp622=_tmp61F.f3;
s=Cyc_Absyn_declare_stmt(_tmp620,_tmp621,_tmp622,s,0);}}
# 3164
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3166
_npop_handler(0);goto _LL2C5;
# 3129
;_pop_region(r);};};};}_LL2CC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5DA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5DA->tag != 28)goto _LL2CE;else{_tmp5DB=_tmp5DA->f1;_tmp5DC=_tmp5DA->f2;_tmp5DD=_tmp5DA->f3;_tmp5DE=_tmp5DA->f4;}}_LL2CD: {
# 3170
struct Cyc_Absyn_Exp*_tmp625=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4CC->topt),_tmp5DC,1,_tmp4CB,_tmp5DD,_tmp5DB);
e->r=_tmp625->r;
e->topt=_tmp625->topt;
goto _LL2C5;}_LL2CE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5DF=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CE;if(_tmp5DF->tag != 23)goto _LL2D0;else{_tmp5E0=_tmp5DF->f1;}}_LL2CF:
# 3176
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4CB,_tmp5E0))->r;
goto _LL2C5;_LL2D0:;_LL2D1: {
# 3183
void*old_elt_typ=(void*)_check_null(_tmp4CC->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3186
struct _tuple1*_tmp626=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp627=Cyc_Absyn_var_exp(_tmp626,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp627,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4CB == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3193
struct Cyc_Absyn_Exp*r=_tmp4CB;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3201
int done=0;
{void*_stmttmp54=_tmp4CC->r;void*_tmp628=_stmttmp54;void*_tmp62A;struct Cyc_Absyn_Exp*_tmp62B;_LL2E2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp629=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp628;if(_tmp629->tag != 13)goto _LL2E4;else{_tmp62A=(void*)_tmp629->f1;_tmp62B=_tmp629->f2;}}_LL2E3:
# 3204
{struct _tuple20 _tmpC4A;struct _tuple20 _stmttmp55=(_tmpC4A.f1=Cyc_Tcutil_compress(_tmp62A),((_tmpC4A.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp62B->topt)),_tmpC4A)));struct _tuple20 _tmp62C=_stmttmp55;void*_tmp62E;union Cyc_Absyn_Constraint*_tmp62F;union Cyc_Absyn_Constraint*_tmp631;_LL2E7:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp62D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62C.f1;if(_tmp62D->tag != 5)goto _LL2E9;else{_tmp62E=(_tmp62D->f1).elt_typ;_tmp62F=((_tmp62D->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp630=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62C.f2;if(_tmp630->tag != 5)goto _LL2E9;else{_tmp631=((_tmp630->f1).ptr_atts).bounds;}};_LL2E8:
# 3207
{struct _tuple20 _tmpC4B;struct _tuple20 _stmttmp56=(_tmpC4B.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp62F),((_tmpC4B.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp631),_tmpC4B)));struct _tuple20 _tmp632=_stmttmp56;struct Cyc_Absyn_Exp*_tmp635;_LL2EC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp633=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp632.f1;if(_tmp633->tag != 0)goto _LL2EE;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp634=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp632.f2;if(_tmp634->tag != 1)goto _LL2EE;else{_tmp635=_tmp634->f1;}};_LL2ED:
# 3209
 Cyc_Toc_exp_to_c(nv,_tmp62B);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp636=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC4C[4];e->r=Cyc_Toc_fncall_exp_r(_tmp636,(
(_tmpC4C[3]=_tmp635,((_tmpC4C[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp62E),0),((_tmpC4C[1]=_tmp62B,((_tmpC4C[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4C,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
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
struct Cyc_Absyn_Stmt*_tmp63A=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp627),0);
struct Cyc_Absyn_Exp*_tmp63B=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4CC);
_tmp63A=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp627,_tmp63B,0),_tmp4CC,0),_tmp63A,0);{
# 3232
void*_tmp63C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp626,_tmp63C,mexp,_tmp63A,0));};}
# 3235
goto _LL2C5;};}_LL2C5:;}
# 3237
goto _LL210;};_LL235: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4CD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4CD->tag != 17)goto _LL237;else{_tmp4CE=_tmp4CD->f1;}}_LL236: {
# 3240
int _tmp63D=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4CE);
Cyc_Toc_set_in_sizeof(nv,_tmp63D);
goto _LL210;}_LL237: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4CF=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4CF->tag != 16)goto _LL239;else{_tmp4D0=(void*)_tmp4CF->f1;}}_LL238:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC4F;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC4E;e->r=(void*)((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E[0]=((_tmpC4F.tag=16,((_tmpC4F.f1=Cyc_Toc_typ_to_c(_tmp4D0),_tmpC4F)))),_tmpC4E))));}goto _LL210;_LL239: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4D1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4D1->tag != 18)goto _LL23B;else{_tmp4D2=(void*)_tmp4D1->f1;_tmp4D3=_tmp4D1->f2;}}_LL23A: {
# 3246
void*_tmp640=_tmp4D2;
struct Cyc_List_List*_tmp641=_tmp4D3;
for(0;_tmp641 != 0;_tmp641=_tmp641->tl){
void*_stmttmp51=(void*)_tmp641->hd;void*_tmp642=_stmttmp51;struct _dyneither_ptr*_tmp644;unsigned int _tmp646;_LL2F1: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp643=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp642;if(_tmp643->tag != 0)goto _LL2F3;else{_tmp644=_tmp643->f1;}}_LL2F2:
 goto _LL2F0;_LL2F3: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp645=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp642;if(_tmp645->tag != 1)goto _LL2F0;else{_tmp646=_tmp645->f1;}}_LL2F4:
# 3252
{void*_stmttmp52=Cyc_Tcutil_compress(_tmp640);void*_tmp647=_stmttmp52;union Cyc_Absyn_AggrInfoU _tmp649;struct Cyc_List_List*_tmp64B;struct Cyc_List_List*_tmp64D;struct Cyc_Absyn_Datatypefield*_tmp64F;_LL2F6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp648=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp647;if(_tmp648->tag != 11)goto _LL2F8;else{_tmp649=(_tmp648->f1).aggr_info;}}_LL2F7: {
# 3254
struct Cyc_Absyn_Aggrdecl*_tmp650=Cyc_Absyn_get_known_aggrdecl(_tmp649);
if(_tmp650->impl == 0){
const char*_tmpC52;void*_tmpC51;(_tmpC51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC52="struct fields must be known",_tag_dyneither(_tmpC52,sizeof(char),28))),_tag_dyneither(_tmpC51,sizeof(void*),0)));}
_tmp64B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp650->impl))->fields;goto _LL2F9;}_LL2F8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp64A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp647;if(_tmp64A->tag != 12)goto _LL2FA;else{_tmp64B=_tmp64A->f2;}}_LL2F9: {
# 3259
struct Cyc_Absyn_Aggrfield*_tmp653=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64B,(int)_tmp646);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC55;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC54;_tmp641->hd=(void*)((void*)((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=0,((_tmpC55.f1=_tmp653->name,_tmpC55)))),_tmpC54)))));}
_tmp640=_tmp653->type;
goto _LL2F5;}_LL2FA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp64C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp647;if(_tmp64C->tag != 10)goto _LL2FC;else{_tmp64D=_tmp64C->f1;}}_LL2FB:
# 3264
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC58;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC57;_tmp641->hd=(void*)((void*)((_tmpC57=_cycalloc(sizeof(*_tmpC57)),((_tmpC57[0]=((_tmpC58.tag=0,((_tmpC58.f1=Cyc_Absyn_fieldname((int)(_tmp646 + 1)),_tmpC58)))),_tmpC57)))));}
_tmp640=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64D,(int)_tmp646)).f2;
goto _LL2F5;_LL2FC: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp64E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp647;if(_tmp64E->tag != 4)goto _LL2FE;else{if((((_tmp64E->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2FE;_tmp64F=((struct _tuple2)(((_tmp64E->f1).field_info).KnownDatatypefield).val).f2;}}_LL2FD:
# 3268
 if(_tmp646 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC5B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC5A;_tmp641->hd=(void*)((void*)((_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC5B.tag=0,((_tmpC5B.f1=Cyc_Toc_tag_sp,_tmpC5B)))),_tmpC5A)))));}else{
# 3271
_tmp640=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64F->typs,(int)(_tmp646 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC5E;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC5D;_tmp641->hd=(void*)((void*)((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D[0]=((_tmpC5E.tag=0,((_tmpC5E.f1=Cyc_Absyn_fieldname((int)_tmp646),_tmpC5E)))),_tmpC5D)))));};}
# 3274
goto _LL2F5;_LL2FE:;_LL2FF:
{const char*_tmpC61;void*_tmpC60;(_tmpC60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC61="impossible type for offsetof tuple index",_tag_dyneither(_tmpC61,sizeof(char),41))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}
goto _LL2F5;_LL2F5:;}
# 3278
goto _LL2F0;_LL2F0:;}
# 3281
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC64;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC63;e->r=(void*)((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63[0]=((_tmpC64.tag=18,((_tmpC64.f1=Cyc_Toc_typ_to_c(_tmp4D2),((_tmpC64.f2=_tmp4D3,_tmpC64)))))),_tmpC63))));}
goto _LL210;}_LL23B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4D4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4D4->tag != 19)goto _LL23D;else{_tmp4D5=_tmp4D4->f1;}}_LL23C: {
# 3284
int _tmp660=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp661=Cyc_Tcutil_compress((void*)_check_null(_tmp4D5->topt));
{void*_tmp662=_tmp661;void*_tmp664;struct Cyc_Absyn_Tqual _tmp665;void*_tmp666;union Cyc_Absyn_Constraint*_tmp667;union Cyc_Absyn_Constraint*_tmp668;union Cyc_Absyn_Constraint*_tmp669;_LL301: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp663=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp662;if(_tmp663->tag != 5)goto _LL303;else{_tmp664=(_tmp663->f1).elt_typ;_tmp665=(_tmp663->f1).elt_tq;_tmp666=((_tmp663->f1).ptr_atts).rgn;_tmp667=((_tmp663->f1).ptr_atts).nullable;_tmp668=((_tmp663->f1).ptr_atts).bounds;_tmp669=((_tmp663->f1).ptr_atts).zero_term;}}_LL302:
# 3289
{void*_stmttmp4F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp668);void*_tmp66A=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp66C;_LL306: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp66B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp66A;if(_tmp66B->tag != 1)goto _LL308;else{_tmp66C=_tmp66B->f1;}}_LL307: {
# 3291
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4D5);
Cyc_Toc_exp_to_c(nv,_tmp4D5);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,Cyc_Tcutil_warn(e->loc,((_tmpC67="inserted null check due to dereference",_tag_dyneither(_tmpC67,sizeof(char),39))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}{
# 3297
struct Cyc_List_List*_tmpC68;_tmp4D5->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp661),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC68=_cycalloc(sizeof(*_tmpC68)),((_tmpC68->hd=Cyc_Absyn_copy_exp(_tmp4D5),((_tmpC68->tl=0,_tmpC68)))))),0));};}
# 3305
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp669)){
struct _tuple12 _stmttmp50=Cyc_Evexp_eval_const_uint_exp(_tmp66C);unsigned int _tmp672;int _tmp673;struct _tuple12 _tmp671=_stmttmp50;_tmp672=_tmp671.f1;_tmp673=_tmp671.f2;
# 3311
if(!_tmp673  || _tmp672 <= 0){
const char*_tmpC6B;void*_tmpC6A;(_tmpC6A=0,Cyc_Tcutil_terr(e->loc,((_tmpC6B="cannot determine dereference is in bounds",_tag_dyneither(_tmpC6B,sizeof(char),42))),_tag_dyneither(_tmpC6A,sizeof(void*),0)));}}
# 3314
goto _LL305;}_LL308: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp66D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp66A;if(_tmp66D->tag != 0)goto _LL305;}_LL309: {
# 3317
struct Cyc_Absyn_Exp*_tmp676=Cyc_Absyn_uint_exp(0,0);
_tmp676->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4D5,_tmp676);
Cyc_Toc_exp_to_c(nv,e);
goto _LL305;}_LL305:;}
# 3323
goto _LL300;_LL303:;_LL304: {
const char*_tmpC6E;void*_tmpC6D;(_tmpC6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6E="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC6E,sizeof(char),29))),_tag_dyneither(_tmpC6D,sizeof(void*),0)));}_LL300:;}
# 3326
Cyc_Toc_set_lhs(nv,_tmp660);
goto _LL210;};}_LL23D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4D6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4D6->tag != 20)goto _LL23F;else{_tmp4D7=_tmp4D6->f1;_tmp4D8=_tmp4D6->f2;_tmp4D9=_tmp4D6->f3;_tmp4DA=_tmp4D6->f4;}}_LL23E: {
# 3329
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4D7->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D7);
if(_tmp4D9  && _tmp4DA)
e->r=Cyc_Toc_check_tagged_union(_tmp4D7,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4D8,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3337
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL210;}_LL23F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4DB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4DB->tag != 21)goto _LL241;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;_tmp4DE=_tmp4DB->f3;_tmp4DF=_tmp4DB->f4;}}_LL240: {
# 3341
int _tmp679=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4DC->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4DC);
Cyc_Toc_exp_to_c(nv,_tmp4DC);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _stmttmp4C=Cyc_Toc_get_ptr_type(e1typ);void*_tmp67B;struct Cyc_Absyn_Tqual _tmp67C;void*_tmp67D;union Cyc_Absyn_Constraint*_tmp67E;union Cyc_Absyn_Constraint*_tmp67F;union Cyc_Absyn_Constraint*_tmp680;struct Cyc_Absyn_PtrInfo _tmp67A=_stmttmp4C;_tmp67B=_tmp67A.elt_typ;_tmp67C=_tmp67A.elt_tq;_tmp67D=(_tmp67A.ptr_atts).rgn;_tmp67E=(_tmp67A.ptr_atts).nullable;_tmp67F=(_tmp67A.ptr_atts).bounds;_tmp680=(_tmp67A.ptr_atts).zero_term;
{void*_stmttmp4D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp67F);void*_tmp681=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp683;_LL30B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp682=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp681;if(_tmp682->tag != 1)goto _LL30D;else{_tmp683=_tmp682->f1;}}_LL30C: {
# 3352
struct _tuple12 _stmttmp4E=Cyc_Evexp_eval_const_uint_exp(_tmp683);unsigned int _tmp686;int _tmp687;struct _tuple12 _tmp685=_stmttmp4E;_tmp686=_tmp685.f1;_tmp687=_tmp685.f2;
if(_tmp687){
if(_tmp686 < 1){
const char*_tmpC71;void*_tmpC70;(_tmpC70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC71="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC71,sizeof(char),44))),_tag_dyneither(_tmpC70,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC74;void*_tmpC73;(_tmpC73=0,Cyc_Tcutil_warn(e->loc,((_tmpC74="inserted null check due to dereference",_tag_dyneither(_tmpC74,sizeof(char),39))),_tag_dyneither(_tmpC73,sizeof(void*),0)));}{
# 3360
struct Cyc_Absyn_Exp*_tmpC75[1];_tmp4DC->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC75[0]=Cyc_Absyn_new_exp(_tmp4DC->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3365
if(!Cyc_Evexp_c_can_eval(_tmp683)){
const char*_tmpC78;void*_tmpC77;(_tmpC77=0,Cyc_Tcutil_terr(e->loc,((_tmpC78="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC78,sizeof(char),47))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}{
# 3369
struct Cyc_Absyn_Exp*_tmpC79[4];_tmp4DC->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC79[3]=
# 3373
Cyc_Absyn_uint_exp(0,0),((_tmpC79[2]=
# 3372
Cyc_Absyn_sizeoftyp_exp(_tmp67B,0),((_tmpC79[1]=_tmp683,((_tmpC79[0]=
# 3371
Cyc_Absyn_new_exp(_tmp4DC->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC79,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3375
goto _LL30A;}_LL30D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp684=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp681;if(_tmp684->tag != 0)goto _LL30A;}_LL30E: {
# 3378
void*ta1=Cyc_Toc_typ_to_c(_tmp67B);
{struct Cyc_Absyn_Exp*_tmpC7A[3];_tmp4DC->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp67C),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC7A[2]=
# 3383
Cyc_Absyn_uint_exp(0,0),((_tmpC7A[1]=
# 3382
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC7A[0]=
# 3381
Cyc_Absyn_new_exp(_tmp4DC->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3386
goto _LL30A;}_LL30A:;}
# 3388
if(_tmp4DE  && _tmp4DF)
e->r=Cyc_Toc_check_tagged_union(_tmp4DC,Cyc_Toc_typ_to_c(e1typ),_tmp67B,_tmp4DD,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3391
if(is_poly  && _tmp4DF)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp679);
goto _LL210;};};}_LL241: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4E0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4E0->tag != 22)goto _LL243;else{_tmp4E1=_tmp4E0->f1;_tmp4E2=_tmp4E0->f2;}}_LL242: {
# 3396
int _tmp691=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp692=Cyc_Tcutil_compress((void*)_check_null(_tmp4E1->topt));
# 3400
{void*_tmp693=_tmp692;struct Cyc_List_List*_tmp695;void*_tmp697;struct Cyc_Absyn_Tqual _tmp698;void*_tmp699;union Cyc_Absyn_Constraint*_tmp69A;union Cyc_Absyn_Constraint*_tmp69B;union Cyc_Absyn_Constraint*_tmp69C;_LL310: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp694=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp693;if(_tmp694->tag != 10)goto _LL312;else{_tmp695=_tmp694->f1;}}_LL311:
# 3403
 Cyc_Toc_exp_to_c(nv,_tmp4E1);
Cyc_Toc_exp_to_c(nv,_tmp4E2);{
struct _tuple12 _stmttmp4B=Cyc_Evexp_eval_const_uint_exp(_tmp4E2);unsigned int _tmp69E;int _tmp69F;struct _tuple12 _tmp69D=_stmttmp4B;_tmp69E=_tmp69D.f1;_tmp69F=_tmp69D.f2;
if(!_tmp69F){
const char*_tmpC7D;void*_tmpC7C;(_tmpC7C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC7D="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC7D,sizeof(char),44))),_tag_dyneither(_tmpC7C,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4E1,Cyc_Absyn_fieldname((int)(_tmp69E + 1)));
goto _LL30F;};_LL312: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp696=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp693;if(_tmp696->tag != 5)goto _LL314;else{_tmp697=(_tmp696->f1).elt_typ;_tmp698=(_tmp696->f1).elt_tq;_tmp699=((_tmp696->f1).ptr_atts).rgn;_tmp69A=((_tmp696->f1).ptr_atts).nullable;_tmp69B=((_tmp696->f1).ptr_atts).bounds;_tmp69C=((_tmp696->f1).ptr_atts).zero_term;}}_LL313: {
# 3411
struct Cyc_List_List*_tmp6A2=Cyc_Toc_get_relns(_tmp4E1);
int _tmp6A3=Cyc_Toc_need_null_check(_tmp4E1);
int _tmp6A4=Cyc_Toc_in_sizeof(nv);
# 3419
int in_bnds=_tmp6A4;
{void*_stmttmp49=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69B);void*_tmp6A5=_stmttmp49;_LL317:;_LL318:
# 3422
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp692,_tmp6A2,_tmp4E1,_tmp4E2);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC81;void*_tmpC80[1];struct Cyc_String_pa_PrintArg_struct _tmpC7F;(_tmpC7F.tag=0,((_tmpC7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC80[0]=& _tmpC7F,Cyc_Tcutil_warn(e->loc,((_tmpC81="bounds check necessary for %s",_tag_dyneither(_tmpC81,sizeof(char),30))),_tag_dyneither(_tmpC80,sizeof(void*),1)))))));}_LL316:;}
# 3429
Cyc_Toc_exp_to_c(nv,_tmp4E1);
Cyc_Toc_exp_to_c(nv,_tmp4E2);
++ Cyc_Toc_total_bounds_checks;
{void*_stmttmp4A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69B);void*_tmp6A9=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp6AB;_LL31A: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6AA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A9;if(_tmp6AA->tag != 1)goto _LL31C;else{_tmp6AB=_tmp6AA->f1;}}_LL31B: {
# 3434
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp69A) && (unsigned int)Cyc_Toc_need_null_check;
void*ta1=Cyc_Toc_typ_to_c(_tmp697);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp698);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC84;void*_tmpC83;(_tmpC83=0,Cyc_Tcutil_warn(e->loc,((_tmpC84="inserted null check due to dereference",_tag_dyneither(_tmpC84,sizeof(char),39))),_tag_dyneither(_tmpC83,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC85[1];_tmp4E1->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC85[0]=Cyc_Absyn_copy_exp(_tmp4E1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC85,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3446
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp69C)){
# 3448
if(!Cyc_Evexp_c_can_eval(_tmp6AB)){
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,Cyc_Tcutil_terr(e->loc,((_tmpC88="cannot determine subscript is in bounds",_tag_dyneither(_tmpC88,sizeof(char),40))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4E1);
struct Cyc_Absyn_Exp*_tmpC89[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC89[2]=_tmp4E2,((_tmpC89[1]=_tmp6AB,((_tmpC89[0]=_tmp4E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC89,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6AB)){
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,Cyc_Tcutil_terr(e->loc,((_tmpC8C="cannot determine subscript is in bounds",_tag_dyneither(_tmpC8C,sizeof(char),40))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,Cyc_Tcutil_warn(e->loc,((_tmpC8F="inserted null check due to dereference",_tag_dyneither(_tmpC8F,sizeof(char),39))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}{
# 3460
struct Cyc_Absyn_Exp*_tmpC90[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC90[3]=_tmp4E2,((_tmpC90[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC90[1]=_tmp6AB,((_tmpC90[0]=_tmp4E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC90,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3466
if(!Cyc_Evexp_c_can_eval(_tmp6AB)){
const char*_tmpC93;void*_tmpC92;(_tmpC92=0,Cyc_Tcutil_terr(e->loc,((_tmpC93="cannot determine subscript is in bounds",_tag_dyneither(_tmpC93,sizeof(char),40))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}{
# 3469
struct Cyc_Absyn_Exp*_tmpC94[2];_tmp4E2->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC94[1]=Cyc_Absyn_copy_exp(_tmp4E2),((_tmpC94[0]=_tmp6AB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC94,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3472
goto _LL319;}_LL31C: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6AC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6A9;if(_tmp6AC->tag != 0)goto _LL319;}_LL31D: {
# 3474
void*ta1=Cyc_Toc_typ_to_c(_tmp697);
if(in_bnds){
# 3478
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp698),
Cyc_Toc_member_exp(_tmp4E1,Cyc_Toc_curr_sp,0)),_tmp4E2);}else{
# 3483
struct Cyc_Absyn_Exp*_tmp6BB=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC95[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp698),
Cyc_Absyn_fncall_exp(_tmp6BB,(
(_tmpC95[2]=_tmp4E2,((_tmpC95[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC95[0]=_tmp4E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC95,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3489
goto _LL319;}_LL319:;}
# 3491
goto _LL30F;}_LL314:;_LL315: {
const char*_tmpC98;void*_tmpC97;(_tmpC97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC98="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC98,sizeof(char),49))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}_LL30F:;}
# 3494
Cyc_Toc_set_lhs(nv,_tmp691);
goto _LL210;};}_LL243: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4E3->tag != 23)goto _LL245;else{_tmp4E4=_tmp4E3->f1;}}_LL244:
# 3497
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E4))->r;else{
# 3502
struct Cyc_List_List*_tmp6BF=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E4);
void*_tmp6C0=Cyc_Toc_add_tuple_type(_tmp6BF);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4E4 != 0;(_tmp4E4=_tmp4E4->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E4->hd);{
struct _tuple16*_tmpC9B;struct Cyc_List_List*_tmpC9A;dles=((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A->hd=((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B->f1=0,((_tmpC9B->f2=(struct Cyc_Absyn_Exp*)_tmp4E4->hd,_tmpC9B)))))),((_tmpC9A->tl=dles,_tmpC9A))))));};}}
# 3509
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3512
goto _LL210;_LL245: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4E5=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4E5->tag != 25)goto _LL247;else{_tmp4E6=_tmp4E5->f1;}}_LL246:
# 3516
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E6);
{struct Cyc_List_List*_tmp6C3=_tmp4E6;for(0;_tmp6C3 != 0;_tmp6C3=_tmp6C3->tl){
struct _tuple16*_stmttmp48=(struct _tuple16*)_tmp6C3->hd;struct Cyc_Absyn_Exp*_tmp6C5;struct _tuple16*_tmp6C4=_stmttmp48;_tmp6C5=_tmp6C4->f2;
Cyc_Toc_exp_to_c(nv,_tmp6C5);}}
# 3521
goto _LL210;_LL247: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4E7->tag != 26)goto _LL249;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;_tmp4EA=_tmp4E7->f3;_tmp4EB=_tmp4E7->f4;}}_LL248: {
# 3525
struct _tuple12 _stmttmp47=Cyc_Evexp_eval_const_uint_exp(_tmp4E9);unsigned int _tmp6C7;int _tmp6C8;struct _tuple12 _tmp6C6=_stmttmp47;_tmp6C7=_tmp6C6.f1;_tmp6C8=_tmp6C6.f2;{
void*_tmp6C9=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4EA->topt));
Cyc_Toc_exp_to_c(nv,_tmp4EA);{
struct Cyc_List_List*es=0;
# 3530
if(!Cyc_Toc_is_zero(_tmp4EA)){
if(!_tmp6C8){
const char*_tmpC9E;void*_tmpC9D;(_tmpC9D=0,Cyc_Tcutil_terr(_tmp4E9->loc,((_tmpC9E="cannot determine value of constant",_tag_dyneither(_tmpC9E,sizeof(char),35))),_tag_dyneither(_tmpC9D,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6C7;++ i){
struct _tuple16*_tmpCA1;struct Cyc_List_List*_tmpCA0;es=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->hd=((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->f1=0,((_tmpCA1->f2=_tmp4EA,_tmpCA1)))))),((_tmpCA0->tl=es,_tmpCA0))))));}}
# 3536
if(_tmp4EB){
struct Cyc_Absyn_Exp*_tmp6CE=Cyc_Toc_cast_it(_tmp6C9,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpCA4;struct Cyc_List_List*_tmpCA3;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3->hd=((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4->f1=0,((_tmpCA4->f2=_tmp6CE,_tmpCA4)))))),((_tmpCA3->tl=0,_tmpCA3)))))));}}
# 3541
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL210;};};}_LL249: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp4EC=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4EC->tag != 27)goto _LL24B;else{_tmp4ED=_tmp4EC->f1;_tmp4EE=(void*)_tmp4EC->f2;_tmp4EF=_tmp4EC->f3;}}_LL24A:
# 3546
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL210;_LL24B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4F0=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4F0->tag != 28)goto _LL24D;else{_tmp4F1=_tmp4F0->f1;_tmp4F2=_tmp4F0->f2;_tmp4F3=_tmp4F0->f3;_tmp4F4=_tmp4F0->f4;}}_LL24C:
# 3551
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp6D1=Cyc_Toc_init_struct(nv,old_typ,_tmp4F2,0,0,_tmp4F3,_tmp4F1);
e->r=_tmp6D1->r;
e->topt=_tmp6D1->topt;}else{
# 3562
if(_tmp4F4 == 0){
const char*_tmpCA7;void*_tmpCA6;(_tmpCA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA7="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCA7,sizeof(char),38))),_tag_dyneither(_tmpCA6,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp4F4;
# 3566
struct _RegionHandle _tmp6D4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6D4;_push_region(rgn);
{struct Cyc_List_List*_tmp6D5=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4F3,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3570
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F4->impl))->tagged){
# 3572
struct _tuple22*_stmttmp45=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6D5))->hd;struct Cyc_Absyn_Aggrfield*_tmp6D7;struct Cyc_Absyn_Exp*_tmp6D8;struct _tuple22*_tmp6D6=_stmttmp45;_tmp6D7=_tmp6D6->f1;_tmp6D8=_tmp6D6->f2;{
void*_tmp6D9=(void*)_check_null(_tmp6D8->topt);
void*_tmp6DA=_tmp6D7->type;
Cyc_Toc_exp_to_c(nv,_tmp6D8);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6DA) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6D9))
_tmp6D8->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6D8->r,0));{
# 3581
int i=Cyc_Toc_get_member_offset(_tmp4F4,_tmp6D7->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpCAC;struct _tuple16*_tmpCAB;struct _tuple16*_tmpCAA[2];struct Cyc_List_List*_tmp6DB=(_tmpCAA[1]=((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->f1=0,((_tmpCAB->f2=_tmp6D8,_tmpCAB)))))),((_tmpCAA[0]=((_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->f1=0,((_tmpCAC->f2=field_tag_exp,_tmpCAC)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6DB,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCB2;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCB1;void*_tmpCB0[1];struct Cyc_List_List*ds=(_tmpCB0[0]=(void*)((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB1.tag=1,((_tmpCB1.f1=_tmp6D7->name,_tmpCB1)))),_tmpCB2)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB0,sizeof(void*),1)));
struct _tuple16*_tmpCB5;struct _tuple16*_tmpCB4[1];struct Cyc_List_List*dles=(_tmpCB4[0]=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->f1=ds,((_tmpCB5->f2=umem,_tmpCB5)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB4,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3590
struct Cyc_List_List*_tmp6E4=0;
struct Cyc_List_List*_tmp6E5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6E5 != 0;_tmp6E5=_tmp6E5->tl){
struct Cyc_List_List*_tmp6E6=_tmp6D5;for(0;_tmp6E6 != 0;_tmp6E6=_tmp6E6->tl){
if((*((struct _tuple22*)_tmp6E6->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6E5->hd){
struct _tuple22*_stmttmp46=(struct _tuple22*)_tmp6E6->hd;struct Cyc_Absyn_Aggrfield*_tmp6E8;struct Cyc_Absyn_Exp*_tmp6E9;struct _tuple22*_tmp6E7=_stmttmp46;_tmp6E8=_tmp6E7->f1;_tmp6E9=_tmp6E7->f2;{
void*_tmp6EA=Cyc_Toc_typ_to_c(_tmp6E8->type);
void*_tmp6EB=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6E9->topt));
Cyc_Toc_exp_to_c(nv,_tmp6E9);
# 3600
if(!Cyc_Tcutil_unify(_tmp6EA,_tmp6EB)){
# 3602
if(!Cyc_Tcutil_is_arithmetic_type(_tmp6EA) || !
Cyc_Tcutil_is_arithmetic_type(_tmp6EB))
_tmp6E9=Cyc_Toc_cast_it(_tmp6EA,Cyc_Absyn_copy_exp(_tmp6E9));}
{struct _tuple16*_tmpCB8;struct Cyc_List_List*_tmpCB7;_tmp6E4=((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->hd=((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->f1=0,((_tmpCB8->f2=_tmp6E9,_tmpCB8)))))),((_tmpCB7->tl=_tmp6E4,_tmpCB7))))));}
break;};}}}
# 3609
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E4));}}
# 3567
;_pop_region(rgn);};}
# 3613
goto _LL210;_LL24D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp4F5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4F5->tag != 29)goto _LL24F;else{_tmp4F6=(void*)_tmp4F5->f1;_tmp4F7=_tmp4F5->f2;}}_LL24E: {
# 3615
struct Cyc_List_List*fs;
{void*_stmttmp43=Cyc_Tcutil_compress(_tmp4F6);void*_tmp6EE=_stmttmp43;struct Cyc_List_List*_tmp6F0;_LL31F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6EF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6EE;if(_tmp6EF->tag != 12)goto _LL321;else{_tmp6F0=_tmp6EF->f2;}}_LL320:
 fs=_tmp6F0;goto _LL31E;_LL321:;_LL322: {
const char*_tmpCBC;void*_tmpCBB[1];struct Cyc_String_pa_PrintArg_struct _tmpCBA;(_tmpCBA.tag=0,((_tmpCBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F6)),((_tmpCBB[0]=& _tmpCBA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBC="anon struct has type %s",_tag_dyneither(_tmpCBC,sizeof(char),24))),_tag_dyneither(_tmpCBB,sizeof(void*),1)))))));}_LL31E:;}{
# 3620
struct _RegionHandle _tmp6F4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6F4;_push_region(rgn);{
struct Cyc_List_List*_tmp6F5=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4F7,Cyc_Absyn_StructA,fs);
for(0;_tmp6F5 != 0;_tmp6F5=_tmp6F5->tl){
struct _tuple22*_stmttmp44=(struct _tuple22*)_tmp6F5->hd;struct Cyc_Absyn_Aggrfield*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F8;struct _tuple22*_tmp6F6=_stmttmp44;_tmp6F7=_tmp6F6->f1;_tmp6F8=_tmp6F6->f2;{
void*_tmp6F9=(void*)_check_null(_tmp6F8->topt);
void*_tmp6FA=_tmp6F7->type;
Cyc_Toc_exp_to_c(nv,_tmp6F8);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6FA) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6F9))
_tmp6F8->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6F8->r,0));};}
# 3634
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F7);}
# 3636
_npop_handler(0);goto _LL210;
# 3620
;_pop_region(rgn);};}_LL24F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp4F8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4F8->tag != 30)goto _LL251;else{_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F8->f2;_tmp4FB=_tmp4F8->f3;}}_LL250: {
# 3639
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp6FB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6FC=Cyc_Absyn_var_exp(_tmp6FB,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4FB->name,_tmp4FA->name));
tag_exp=_tmp4FA->is_extensible?Cyc_Absyn_var_exp(_tmp4FB->name,0):
 Cyc_Toc_datatype_tag(_tmp4FA,_tmp4FB->name);
mem_exp=_tmp6FC;{
struct Cyc_List_List*_tmp6FD=_tmp4FB->typs;
# 3650
if(Cyc_Toc_is_toplevel(nv)){
# 3652
struct Cyc_List_List*dles=0;
for(0;_tmp4F9 != 0;(_tmp4F9=_tmp4F9->tl,_tmp6FD=_tmp6FD->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4F9->hd;
void*_tmp6FE=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6FD))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6FE))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpCBF;struct Cyc_List_List*_tmpCBE;dles=((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->hd=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=0,((_tmpCBF->f2=cur_e,_tmpCBF)))))),((_tmpCBE->tl=dles,_tmpCBE))))));};}
# 3663
{struct _tuple16*_tmpCC2;struct Cyc_List_List*_tmpCC1;dles=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=tag_exp,_tmpCC2)))))),((_tmpCC1->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpCC1))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3669
struct Cyc_List_List*_tmpCC3;struct Cyc_List_List*_tmp703=
(_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpCC3->tl=0,_tmpCC3)))));
# 3672
{int i=1;for(0;_tmp4F9 != 0;(((_tmp4F9=_tmp4F9->tl,i ++)),_tmp6FD=_tmp6FD->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4F9->hd;
void*_tmp704=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp6FD))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp704))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp705=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3682
struct Cyc_List_List*_tmpCC4;_tmp703=((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->hd=_tmp705,((_tmpCC4->tl=_tmp703,_tmpCC4))))));};}}{
# 3684
struct Cyc_Absyn_Stmt*_tmp707=Cyc_Absyn_exp_stmt(_tmp6FC,0);
struct Cyc_List_List*_tmpCC5;struct Cyc_Absyn_Stmt*_tmp708=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5->hd=_tmp707,((_tmpCC5->tl=_tmp703,_tmpCC5))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6FB,datatype_ctype,0,_tmp708,0));};}
# 3688
goto _LL210;};}_LL251: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4FC=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4FC->tag != 31)goto _LL253;}_LL252:
# 3690
 goto _LL254;_LL253: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4FD->tag != 32)goto _LL255;}_LL254:
 goto _LL210;_LL255: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp4FE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp4FE->tag != 33)goto _LL257;else{_tmp4FF=(_tmp4FE->f1).is_calloc;_tmp500=(_tmp4FE->f1).rgn;_tmp501=(_tmp4FE->f1).elt_type;_tmp502=(_tmp4FE->f1).num_elts;_tmp503=(_tmp4FE->f1).fat_result;}}_LL256: {
# 3694
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp501)));
Cyc_Toc_exp_to_c(nv,_tmp502);
# 3698
if(_tmp503){
struct _tuple1*_tmp70B=Cyc_Toc_temp_var();
struct _tuple1*_tmp70C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp4FF){
xexp=_tmp502;
if(_tmp500 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp500;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp70B,0));}else{
# 3709
pexp=Cyc_Toc_calloc_exp(*_tmp501,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp70B,0));}{
# 3711
struct Cyc_Absyn_Exp*_tmpCC6[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCC6[2]=
Cyc_Absyn_var_exp(_tmp70B,0),((_tmpCC6[1]=
# 3712
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCC6[0]=Cyc_Absyn_var_exp(_tmp70C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3715
if(_tmp500 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp500;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp70B,0));}else{
# 3720
pexp=Cyc_Toc_malloc_exp(*_tmp501,Cyc_Absyn_var_exp(_tmp70B,0));}{
# 3722
struct Cyc_Absyn_Exp*_tmpCC7[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpCC7[2]=
Cyc_Absyn_var_exp(_tmp70B,0),((_tmpCC7[1]=
# 3722
Cyc_Absyn_uint_exp(1,0),((_tmpCC7[0]=Cyc_Absyn_var_exp(_tmp70C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3725
struct Cyc_Absyn_Stmt*_tmp70F=Cyc_Absyn_declare_stmt(_tmp70B,Cyc_Absyn_uint_typ,_tmp502,
Cyc_Absyn_declare_stmt(_tmp70C,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp70F);};}else{
# 3730
if(_tmp4FF){
if(_tmp500 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp500;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp502))->r;}else{
# 3736
e->r=(Cyc_Toc_calloc_exp(*_tmp501,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp502))->r;}}else{
# 3739
if(_tmp500 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp500;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp502))->r;}else{
# 3744
e->r=(Cyc_Toc_malloc_exp(*_tmp501,_tmp502))->r;}}}
# 3748
goto _LL210;}_LL257: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp504=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp504->tag != 34)goto _LL259;else{_tmp505=_tmp504->f1;_tmp506=_tmp504->f2;}}_LL258: {
# 3757
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp505->topt);
void*e2_old_typ=(void*)_check_null(_tmp506->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCA="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpCCA,sizeof(char),57))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}{
# 3764
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3768
swap_fn=Cyc_Toc__swap_word_e;}{
# 3772
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp505,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp506,& f2_tag,& tagged_mem_type2,1);
# 3779
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp505);
Cyc_Toc_exp_to_c(nv,_tmp506);
Cyc_Toc_set_lhs(nv,0);
# 3785
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp505,f1_tag,tagged_mem_type1);else{
# 3789
_tmp505=Cyc_Toc_push_address_exp(_tmp505);}
# 3791
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp506,f2_tag,tagged_mem_type2);else{
# 3795
_tmp506=Cyc_Toc_push_address_exp(_tmp506);}
# 3797
{struct Cyc_Absyn_Exp*_tmpCCB[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCCB[1]=_tmp506,((_tmpCCB[0]=_tmp505,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCB,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3799
goto _LL210;};};}_LL259: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp507=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp507->tag != 37)goto _LL25B;else{_tmp508=_tmp507->f1;_tmp509=_tmp507->f2;}}_LL25A: {
# 3802
void*_tmp713=Cyc_Tcutil_compress((void*)_check_null(_tmp508->topt));
Cyc_Toc_exp_to_c(nv,_tmp508);
{void*_tmp714=_tmp713;struct Cyc_Absyn_Aggrdecl*_tmp716;_LL324: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp715=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp714;if(_tmp715->tag != 11)goto _LL326;else{if((((_tmp715->f1).aggr_info).KnownAggr).tag != 2)goto _LL326;_tmp716=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp715->f1).aggr_info).KnownAggr).val);}}_LL325: {
# 3806
struct Cyc_Absyn_Exp*_tmp717=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp716,_tmp509),0);
struct Cyc_Absyn_Exp*_tmp718=Cyc_Toc_member_exp(_tmp508,_tmp509,0);
struct Cyc_Absyn_Exp*_tmp719=Cyc_Toc_member_exp(_tmp718,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp719,_tmp717,0))->r;
goto _LL323;}_LL326:;_LL327: {
const char*_tmpCCE;void*_tmpCCD;(_tmpCCD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCE="non-aggregate type in tagcheck",_tag_dyneither(_tmpCCE,sizeof(char),31))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}_LL323:;}
# 3813
goto _LL210;}_LL25B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp50A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp50A->tag != 36)goto _LL25D;else{_tmp50B=_tmp50A->f1;}}_LL25C:
 Cyc_Toc_stmt_to_c(nv,_tmp50B);goto _LL210;_LL25D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp50C->tag != 35)goto _LL25F;}_LL25E: {
const char*_tmpCD1;void*_tmpCD0;(_tmpCD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD1="UnresolvedMem",_tag_dyneither(_tmpCD1,sizeof(char),14))),_tag_dyneither(_tmpCD0,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp50D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp50D->tag != 24)goto _LL261;}_LL260: {
const char*_tmpCD4;void*_tmpCD3;(_tmpCD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCD4="compoundlit",_tag_dyneither(_tmpCD4,sizeof(char),12))),_tag_dyneither(_tmpCD3,sizeof(void*),0)));}_LL261: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp50E->tag != 38)goto _LL263;}_LL262: {
const char*_tmpCD7;void*_tmpCD6;(_tmpCD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD7="valueof(-)",_tag_dyneither(_tmpCD7,sizeof(char),11))),_tag_dyneither(_tmpCD6,sizeof(void*),0)));}_LL263: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp496;if(_tmp50F->tag != 39)goto _LL210;}_LL264: {
const char*_tmpCDA;void*_tmpCD9;(_tmpCD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDA="__asm__",_tag_dyneither(_tmpCDA,sizeof(char),8))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}_LL210:;};}
# 3850 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3853
Cyc_Toc_skip_stmt_dl(),0);}
# 3856
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_stmttmp6A=e->r;void*_tmp724=_stmttmp6A;struct Cyc_Absyn_Exp*_tmp726;struct Cyc_Absyn_Exp*_tmp728;struct Cyc_Absyn_Exp*_tmp72A;_LL329: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp725=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp724;if(_tmp725->tag != 13)goto _LL32B;else{_tmp726=_tmp725->f2;}}_LL32A:
 return Cyc_Toc_path_length(_tmp726);_LL32B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp727=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp724;if(_tmp727->tag != 19)goto _LL32D;else{_tmp728=_tmp727->f1;}}_LL32C:
 return 1 + Cyc_Toc_path_length(_tmp728);_LL32D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp729=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp724;if(_tmp729->tag != 20)goto _LL32F;else{_tmp72A=_tmp729->f1;}}_LL32E:
 return Cyc_Toc_path_length(_tmp72A);_LL32F:;_LL330:
 return 0;_LL328:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple1*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3865
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3870
struct Cyc_Absyn_Stmt*s;
{void*_stmttmp6B=p->r;void*_tmp72B=_stmttmp6B;struct _tuple1*_tmp72D;struct Cyc_Absyn_Pat*_tmp72E;struct _tuple1*_tmp730;struct Cyc_Absyn_Vardecl*_tmp733;struct Cyc_Absyn_Pat*_tmp734;enum Cyc_Absyn_Sign _tmp737;int _tmp738;char _tmp73A;struct _dyneither_ptr _tmp73C;int _tmp73D;struct Cyc_Absyn_Enumdecl*_tmp73F;struct Cyc_Absyn_Enumfield*_tmp740;void*_tmp742;struct Cyc_Absyn_Enumfield*_tmp743;struct Cyc_Absyn_Datatypedecl*_tmp746;struct Cyc_Absyn_Datatypefield*_tmp747;struct Cyc_List_List*_tmp748;struct Cyc_List_List*_tmp74A;struct Cyc_List_List*_tmp74C;union Cyc_Absyn_AggrInfoU _tmp74F;struct Cyc_List_List*_tmp750;struct Cyc_Absyn_Pat*_tmp752;_LL332: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp72C=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp72C->tag != 1)goto _LL334;else{_tmp72D=(_tmp72C->f1)->name;_tmp72E=_tmp72C->f2;}}_LL333: {
# 3874
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp756=(void*)_check_null(_tmp72E->topt);
{struct _tuple24*_tmpCDD;struct Cyc_List_List*_tmpCDC;decls=((_tmpCDC=_region_malloc(rgn,sizeof(*_tmpCDC)),((_tmpCDC->hd=((_tmpCDD=_region_malloc(rgn,sizeof(*_tmpCDD)),((_tmpCDD->f1=v,((_tmpCDD->f2=Cyc_Toc_typ_to_c_array(_tmp756),_tmpCDD)))))),((_tmpCDC->tl=decls,_tmpCDC))))));}{
struct _tuple23 _tmp759=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp72D,Cyc_Absyn_var_exp(v,0)),rgn,_tmp756,
Cyc_Absyn_var_exp(v,0),path,_tmp72E,fail_stmt,decls);
nv=_tmp759.f1;
decls=_tmp759.f2;{
struct Cyc_Absyn_Stmt*_tmp75A=_tmp759.f3;
struct Cyc_Absyn_Stmt*_tmp75B=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp75B,_tmp75A,0);
goto _LL331;};};}_LL334: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp72F=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp72F->tag != 3)goto _LL336;else{_tmp730=(_tmp72F->f2)->name;}}_LL335: {
# 3887
struct _tuple1*_tmp75C=Cyc_Toc_temp_var();
void*_tmp75D=(void*)_check_null(p->topt);
{struct _tuple24*_tmpCE0;struct Cyc_List_List*_tmpCDF;decls=((_tmpCDF=_region_malloc(rgn,sizeof(*_tmpCDF)),((_tmpCDF->hd=((_tmpCE0=_region_malloc(rgn,sizeof(*_tmpCE0)),((_tmpCE0->f1=_tmp75C,((_tmpCE0->f2=Cyc_Toc_typ_to_c_array(_tmp75D),_tmpCE0)))))),((_tmpCDF->tl=decls,_tmpCDF))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp730,Cyc_Absyn_var_exp(_tmp75C,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp75C,0),r,0);
goto _LL331;}_LL336: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp731=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp731->tag != 0)goto _LL338;}_LL337:
 s=Cyc_Toc_skip_stmt_dl();goto _LL331;_LL338: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp732=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp732->tag != 2)goto _LL33A;else{_tmp733=_tmp732->f1;_tmp734=_tmp732->f2;}}_LL339: {
# 3896
struct _tuple1*_tmp760=Cyc_Toc_temp_var();
{struct _tuple24*_tmpCE3;struct Cyc_List_List*_tmpCE2;decls=((_tmpCE2=_region_malloc(rgn,sizeof(*_tmpCE2)),((_tmpCE2->hd=((_tmpCE3=_region_malloc(rgn,sizeof(*_tmpCE3)),((_tmpCE3->f1=_tmp760,((_tmpCE3->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCE3)))))),((_tmpCE2->tl=decls,_tmpCE2))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp733->name,Cyc_Absyn_var_exp(_tmp760,0));
# 3900
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp760,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp763=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp734,fail_stmt,decls);
_tmp763.f3=Cyc_Absyn_seq_stmt(s,_tmp763.f3,0);
return _tmp763;};}_LL33A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp735=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp735->tag != 8)goto _LL33C;}_LL33B:
# 3907
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL331;_LL33C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp736=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp736->tag != 9)goto _LL33E;else{_tmp737=_tmp736->f1;_tmp738=_tmp736->f2;}}_LL33D:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp737,_tmp738,0),fail_stmt);goto _LL331;_LL33E: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp739=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp739->tag != 10)goto _LL340;else{_tmp73A=_tmp739->f1;}}_LL33F:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp73A,0),fail_stmt);goto _LL331;_LL340: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp73B=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp73B->tag != 11)goto _LL342;else{_tmp73C=_tmp73B->f1;_tmp73D=_tmp73B->f2;}}_LL341:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp73C,_tmp73D,0),fail_stmt);goto _LL331;_LL342: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp73E=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp73E->tag != 12)goto _LL344;else{_tmp73F=_tmp73E->f1;_tmp740=_tmp73E->f2;}}_LL343:
# 3912
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCE6;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCE5;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE6.tag=31,((_tmpCE6.f1=_tmp740->name,((_tmpCE6.f2=_tmp73F,((_tmpCE6.f3=_tmp740,_tmpCE6)))))))),_tmpCE5)))),0),fail_stmt);}
goto _LL331;_LL344: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp741=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp741->tag != 13)goto _LL346;else{_tmp742=(void*)_tmp741->f1;_tmp743=_tmp741->f2;}}_LL345:
# 3915
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCE9;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCE8;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=((_tmpCE9.tag=32,((_tmpCE9.f1=_tmp743->name,((_tmpCE9.f2=_tmp742,((_tmpCE9.f3=_tmp743,_tmpCE9)))))))),_tmpCE8)))),0),fail_stmt);}
goto _LL331;_LL346: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp744=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp744->tag != 5)goto _LL348;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp745=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp744->f1)->r;if(_tmp745->tag != 7)goto _LL348;else{_tmp746=_tmp745->f1;_tmp747=_tmp745->f2;_tmp748=_tmp745->f3;}}}_LL347:
# 3925
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp768=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp747->name,_tmp746->name);
void*_tmp769=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp769,r);
struct Cyc_List_List*_tmp76A=_tmp747->typs;
for(0;_tmp748 != 0;(((_tmp748=_tmp748->tl,_tmp76A=((struct Cyc_List_List*)_check_null(_tmp76A))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp76B=(struct Cyc_Absyn_Pat*)_tmp748->hd;
if(_tmp76B->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp76C=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp76A))->hd)).f2;
void*_tmp76D=(void*)_check_null(_tmp76B->topt);
void*_tmp76E=Cyc_Toc_typ_to_c_array(_tmp76D);
struct Cyc_Absyn_Exp*_tmp76F=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp768,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp76C)))
_tmp76F=Cyc_Toc_cast_it(_tmp76E,_tmp76F);{
# 3943
struct _tuple23 _tmp770=Cyc_Toc_xlate_pat(nv,rgn,_tmp76D,_tmp76F,_tmp76F,_tmp76B,fail_stmt,decls);
# 3945
nv=_tmp770.f1;
decls=_tmp770.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp770.f3,0);};};}{
# 3949
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp768,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp746->is_extensible?Cyc_Absyn_var_exp(_tmp747->name,0): Cyc_Toc_datatype_tag(_tmp746,_tmp747->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3957
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp768,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3960
s=Cyc_Absyn_declare_stmt(_tmp768,_tmp769,rcast,s,0);
goto _LL331;};};_LL348: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp749=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp749->tag != 7)goto _LL34A;else{_tmp74A=_tmp749->f3;}}_LL349:
# 3963
 _tmp74C=_tmp74A;goto _LL34B;_LL34A: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp74B=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp74B->tag != 4)goto _LL34C;else{_tmp74C=_tmp74B->f1;}}_LL34B:
# 3971
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp74C != 0;(_tmp74C=_tmp74C->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp771=(struct Cyc_Absyn_Pat*)_tmp74C->hd;
if(_tmp771->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp772=(void*)_check_null(_tmp771->topt);
struct _dyneither_ptr*_tmp773=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp774=Cyc_Toc_xlate_pat(nv,rgn,_tmp772,Cyc_Toc_member_exp(r,_tmp773,0),
Cyc_Toc_member_exp(path,_tmp773,0),_tmp771,fail_stmt,decls);
nv=_tmp774.f1;
decls=_tmp774.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp774.f3,0);};}
# 3985
goto _LL331;};_LL34C: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp74D=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp74D->tag != 6)goto _LL34E;else{if(_tmp74D->f1 != 0)goto _LL34E;}}_LL34D: {
# 3987
const char*_tmpCEC;void*_tmpCEB;(_tmpCEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEC="unresolved aggregate pattern!",_tag_dyneither(_tmpCEC,sizeof(char),30))),_tag_dyneither(_tmpCEB,sizeof(void*),0)));}_LL34E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp74E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp74E->tag != 6)goto _LL350;else{if(_tmp74E->f1 == 0)goto _LL350;_tmp74F=(_tmp74E->f1)->aggr_info;_tmp750=_tmp74E->f3;}}_LL34F: {
# 3989
struct Cyc_Absyn_Aggrdecl*_tmp777=Cyc_Absyn_get_known_aggrdecl(_tmp74F);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp777->impl))->tagged){
# 3993
struct _tuple25*_stmttmp6C=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp750))->hd;struct Cyc_List_List*_tmp779;struct Cyc_Absyn_Pat*_tmp77A;struct _tuple25*_tmp778=_stmttmp6C;_tmp779=_tmp778->f1;_tmp77A=_tmp778->f2;{
struct _dyneither_ptr*f;
{void*_stmttmp6D=(void*)((struct Cyc_List_List*)_check_null(_tmp779))->hd;void*_tmp77B=_stmttmp6D;struct _dyneither_ptr*_tmp77D;_LL359: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp77C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp77B;if(_tmp77C->tag != 1)goto _LL35B;else{_tmp77D=_tmp77C->f1;}}_LL35A:
 f=_tmp77D;goto _LL358;_LL35B:;_LL35C: {
const char*_tmpCEF;void*_tmpCEE;(_tmpCEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEF="no field name in tagged union pattern",_tag_dyneither(_tmpCEF,sizeof(char),38))),_tag_dyneither(_tmpCEE,sizeof(void*),0)));}_LL358:;}{
# 4000
void*_tmp780=(void*)_check_null(_tmp77A->topt);
void*_tmp781=Cyc_Toc_typ_to_c_array(_tmp780);
# 4003
struct Cyc_Absyn_Exp*_tmp782=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp783=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp783=Cyc_Toc_cast_it(_tmp781,_tmp783);
_tmp782=Cyc_Toc_cast_it(_tmp781,_tmp782);{
struct _tuple23 _tmp784=Cyc_Toc_xlate_pat(nv,rgn,_tmp780,_tmp783,_tmp782,_tmp77A,fail_stmt,decls);
# 4011
nv=_tmp784.f1;
decls=_tmp784.f2;{
struct Cyc_Absyn_Stmt*_tmp785=_tmp784.f3;
struct Cyc_Absyn_Stmt*_tmp786=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp777,f),0),fail_stmt);
# 4019
s=Cyc_Absyn_seq_stmt(_tmp786,_tmp785,0);};};};};}else{
# 4022
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp750 != 0;_tmp750=_tmp750->tl){
struct _tuple25*_tmp787=(struct _tuple25*)_tmp750->hd;
struct Cyc_Absyn_Pat*_tmp788=(*_tmp787).f2;
if(_tmp788->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_stmttmp6E=(void*)((struct Cyc_List_List*)_check_null((*_tmp787).f1))->hd;void*_tmp789=_stmttmp6E;struct _dyneither_ptr*_tmp78B;_LL35E: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp78A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp789;if(_tmp78A->tag != 1)goto _LL360;else{_tmp78B=_tmp78A->f1;}}_LL35F:
 f=_tmp78B;goto _LL35D;_LL360:;_LL361: {
struct Cyc_Toc_Match_error_exn_struct _tmpCF2;struct Cyc_Toc_Match_error_exn_struct*_tmpCF1;(int)_throw((void*)((_tmpCF1=_cycalloc_atomic(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=Cyc_Toc_Match_error,_tmpCF2)),_tmpCF1)))));}_LL35D:;}{
# 4033
void*_tmp78E=(void*)_check_null(_tmp788->topt);
void*_tmp78F=Cyc_Toc_typ_to_c_array(_tmp78E);
struct Cyc_Absyn_Exp*_tmp790=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp777->impl))->fields,f)))->type))
_tmp790=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp78F),_tmp790);{
struct _tuple23 _tmp791=Cyc_Toc_xlate_pat(nv,rgn,_tmp78E,_tmp790,
Cyc_Toc_member_exp(path,f,0),_tmp788,fail_stmt,decls);
nv=_tmp791.f1;
decls=_tmp791.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp791.f3,0);};};};}}
# 4045
goto _LL331;}_LL350: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp751=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp751->tag != 5)goto _LL352;else{_tmp752=_tmp751->f1;}}_LL351: {
# 4048
void*_tmp792=(void*)_check_null(_tmp752->topt);
# 4051
struct _tuple23 _tmp793=Cyc_Toc_xlate_pat(nv,rgn,_tmp792,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp752,fail_stmt,decls);
# 4053
nv=_tmp793.f1;
decls=_tmp793.f2;{
struct Cyc_Absyn_Stmt*_tmp794=_tmp793.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4059
Cyc_Toc_skip_stmt_dl(),0),_tmp794,0);else{
# 4061
s=_tmp794;}
goto _LL331;};}_LL352: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp753=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp753->tag != 14)goto _LL354;}_LL353: {
const char*_tmpCF5;void*_tmpCF4;(_tmpCF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF5="unknownid pat",_tag_dyneither(_tmpCF5,sizeof(char),14))),_tag_dyneither(_tmpCF4,sizeof(void*),0)));}_LL354: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp754=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp754->tag != 15)goto _LL356;}_LL355: {
const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF8="unknowncall pat",_tag_dyneither(_tmpCF8,sizeof(char),16))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp755=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp72B;if(_tmp755->tag != 16)goto _LL331;}_LL357: {
const char*_tmpCFB;void*_tmpCFA;(_tmpCFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCFB="exp pat",_tag_dyneither(_tmpCFB,sizeof(char),8))),_tag_dyneither(_tmpCFA,sizeof(void*),0)));}_LL331:;}{
# 4067
struct _tuple23 _tmpCFC;return(_tmpCFC.f1=nv,((_tmpCFC.f2=decls,((_tmpCFC.f3=s,_tmpCFC)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4104 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4106
struct _tuple26*_tmpCFD;return(_tmpCFD=_region_malloc(r,sizeof(*_tmpCFD)),((_tmpCFD->f1=Cyc_Toc_fresh_label(),((_tmpCFD->f2=Cyc_Toc_fresh_label(),((_tmpCFD->f3=sc,_tmpCFD)))))));}
# 4109
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4111
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp79D=(void*)_check_null(e->topt);
# 4116
int leave_as_switch;
{void*_stmttmp6F=Cyc_Tcutil_compress(_tmp79D);void*_tmp79E=_stmttmp6F;_LL363: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp79F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp79E;if(_tmp79F->tag != 6)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7A0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp79E;if(_tmp7A0->tag != 13)goto _LL367;}_LL366:
 leave_as_switch=1;goto _LL362;_LL367:;_LL368:
 leave_as_switch=0;goto _LL362;_LL362:;}
# 4122
{struct Cyc_List_List*_tmp7A1=scs;for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7A1->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7A1->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4127
if(leave_as_switch){
# 4129
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7A2=scs;for(0;_tmp7A2 != 0;_tmp7A2=_tmp7A2->tl){
struct Cyc_Absyn_Stmt*_tmp7A3=((struct Cyc_Absyn_Switch_clause*)_tmp7A2->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7A2->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7A3,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7A4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7A4;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7A3);;_pop_region(rgn);};}}
# 4138
return;}{
# 4141
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4146
struct _RegionHandle _tmp7A5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7A5;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7A6=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4157
{struct Cyc_List_List*_tmp7A7=lscs;for(0;_tmp7A7 != 0;_tmp7A7=_tmp7A7->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp7A7->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7A7->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp7A7->tl))->hd)).f1;
struct _tuple23 _stmttmp70=Cyc_Toc_xlate_pat(_tmp7A6,rgn,_tmp79D,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4160
struct Cyc_Toc_Env*_tmp7A9;struct Cyc_List_List*_tmp7AA;struct Cyc_Absyn_Stmt*_tmp7AB;struct _tuple23 _tmp7A8=_stmttmp70;_tmp7A9=_tmp7A8.f1;_tmp7AA=_tmp7A8.f2;_tmp7AB=_tmp7A8.f3;
# 4163
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7AC=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7A9,_tmp7AC);
_tmp7AB=Cyc_Absyn_seq_stmt(_tmp7AB,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7AC,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4170
decls=_tmp7AA;
{struct Cyc_List_List*_tmpCFE;nvs=((_tmpCFE=_region_malloc(rgn,sizeof(*_tmpCFE)),((_tmpCFE->hd=_tmp7A9,((_tmpCFE->tl=nvs,_tmpCFE))))));}{
struct Cyc_List_List*_tmpCFF;test_stmts=((_tmpCFF=_region_malloc(rgn,sizeof(*_tmpCFF)),((_tmpCFF->hd=_tmp7AB,((_tmpCFF->tl=test_stmts,_tmpCFF))))));};}}
# 4174
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4179
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple26*_stmttmp71=(struct _tuple26*)lscs->hd;struct _dyneither_ptr*_tmp7B0;struct _dyneither_ptr*_tmp7B1;struct Cyc_Absyn_Switch_clause*_tmp7B2;struct _tuple26*_tmp7AF=_stmttmp71;_tmp7B0=_tmp7AF->f1;_tmp7B1=_tmp7AF->f2;_tmp7B2=_tmp7AF->f3;{
struct Cyc_Toc_Env*_tmp7B3=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7B2->body;
struct _RegionHandle _tmp7B4=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7B4;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple26*_stmttmp72=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp7B6;struct Cyc_Absyn_Switch_clause*_tmp7B7;struct _tuple26*_tmp7B5=_stmttmp72;_tmp7B6=_tmp7B5->f2;_tmp7B7=_tmp7B5->f3;{
struct Cyc_List_List*_tmp7B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7B7->pat_vars))->v)).f1);
_tmp7B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7B8);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7B3,end_l,_tmp7B6,_tmp7B8,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4194
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7B3,end_l),s);}
# 4196
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7B0,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7B1,s,0),0);{
struct Cyc_List_List*_tmpD00;stmts=((_tmpD00=_region_malloc(rgn,sizeof(*_tmpD00)),((_tmpD00->hd=s,((_tmpD00->tl=stmts,_tmpD00))))));};
# 4184
;_pop_region(rgn2);};}{
# 4200
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4203
for(decls;decls != 0;decls=decls->tl){
struct _tuple24*_stmttmp73=(struct _tuple24*)decls->hd;struct _tuple1*_tmp7BB;void*_tmp7BC;struct _tuple24*_tmp7BA=_stmttmp73;_tmp7BB=_tmp7BA->f1;_tmp7BC=_tmp7BA->f2;
res=Cyc_Absyn_declare_stmt(_tmp7BB,_tmp7BC,0,res,0);}
# 4208
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4147
;_pop_region(rgn);};};}
# 4213
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4219
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4221
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD01;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD01->tl=0,_tmpD01)))))),0),0);}
# 4225
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple27{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4230
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4232
while(1){
void*_stmttmp74=s->r;void*_tmp7BE=_stmttmp74;struct Cyc_Absyn_Exp*_tmp7C1;struct Cyc_Absyn_Stmt*_tmp7C3;struct Cyc_Absyn_Stmt*_tmp7C4;struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Exp*_tmp7C8;struct Cyc_Absyn_Stmt*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Stmt*_tmp7D1;struct Cyc_Absyn_Stmt*_tmp7D3;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D7;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_List_List*_tmp7DB;struct Cyc_List_List*_tmp7DD;struct Cyc_Absyn_Switch_clause**_tmp7DE;struct Cyc_Absyn_Decl*_tmp7E0;struct Cyc_Absyn_Stmt*_tmp7E1;struct _dyneither_ptr*_tmp7E3;struct Cyc_Absyn_Stmt*_tmp7E4;struct Cyc_Absyn_Stmt*_tmp7E6;struct Cyc_Absyn_Exp*_tmp7E7;struct Cyc_Absyn_Stmt*_tmp7E9;struct Cyc_List_List*_tmp7EA;struct Cyc_Absyn_Exp*_tmp7EC;_LL36A: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7BF=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7BF->tag != 0)goto _LL36C;}_LL36B:
# 4235
 return;_LL36C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7C0=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C0->tag != 1)goto _LL36E;else{_tmp7C1=_tmp7C0->f1;}}_LL36D:
# 4237
 Cyc_Toc_exp_to_c(nv,_tmp7C1);
return;_LL36E: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C2=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C2->tag != 2)goto _LL370;else{_tmp7C3=_tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}_LL36F:
# 4240
 Cyc_Toc_stmt_to_c(nv,_tmp7C3);
s=_tmp7C4;
continue;_LL370: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7C5=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C5->tag != 3)goto _LL372;else{_tmp7C6=_tmp7C5->f1;}}_LL371: {
# 4244
void*topt=0;
if(_tmp7C6 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7C6->topt));
Cyc_Toc_exp_to_c(nv,_tmp7C6);}
# 4250
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp7ED=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7EE=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7ED,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7ED,topt,_tmp7C6,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7EE,0),0))->r;}else{
# 4258
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4260
return;}_LL372: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7C7=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C7->tag != 4)goto _LL374;else{_tmp7C8=_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;_tmp7CA=_tmp7C7->f3;}}_LL373:
# 4262
 Cyc_Toc_exp_to_c(nv,_tmp7C8);
Cyc_Toc_stmt_to_c(nv,_tmp7C9);
s=_tmp7CA;
continue;_LL374: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7CB=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7CB->tag != 5)goto _LL376;else{_tmp7CC=(_tmp7CB->f1).f1;_tmp7CD=_tmp7CB->f2;}}_LL375:
# 4267
 Cyc_Toc_exp_to_c(nv,_tmp7CC);{
struct _RegionHandle _tmp7EF=_new_region("temp");struct _RegionHandle*temp=& _tmp7EF;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CD);
# 4271
_npop_handler(0);return;
# 4268
;_pop_region(temp);};_LL376: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7CE=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7CE->tag != 6)goto _LL378;else{_tmp7CF=_tmp7CE->f1;}}_LL377: {
# 4273
struct _dyneither_ptr**_tmp7F1;struct Cyc_Toc_Env*_tmp7F0=nv;_tmp7F1=_tmp7F0->break_lab;
if(_tmp7F1 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F1,0);{
# 4277
int dest_depth=_tmp7CF == 0?0: _tmp7CF->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL378: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7D0=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D0->tag != 7)goto _LL37A;else{_tmp7D1=_tmp7D0->f1;}}_LL379: {
# 4281
struct _dyneither_ptr**_tmp7F3;struct Cyc_Toc_Env*_tmp7F2=nv;_tmp7F3=_tmp7F2->continue_lab;
if(_tmp7F3 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F3,0);
_tmp7D3=_tmp7D1;goto _LL37B;}_LL37A: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7D2=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D2->tag != 8)goto _LL37C;else{_tmp7D3=_tmp7D2->f2;}}_LL37B:
# 4287
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7D3))->try_depth,s);
return;_LL37C: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7D4=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D4->tag != 9)goto _LL37E;else{_tmp7D5=_tmp7D4->f1;_tmp7D6=(_tmp7D4->f2).f1;_tmp7D7=(_tmp7D4->f3).f1;_tmp7D8=_tmp7D4->f4;}}_LL37D:
# 4291
 Cyc_Toc_exp_to_c(nv,_tmp7D5);Cyc_Toc_exp_to_c(nv,_tmp7D6);Cyc_Toc_exp_to_c(nv,_tmp7D7);{
struct _RegionHandle _tmp7F4=_new_region("temp");struct _RegionHandle*temp=& _tmp7F4;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D8);
# 4295
_npop_handler(0);return;
# 4292
;_pop_region(temp);};_LL37E: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7D9=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D9->tag != 10)goto _LL380;else{_tmp7DA=_tmp7D9->f1;_tmp7DB=_tmp7D9->f2;}}_LL37F:
# 4297
 Cyc_Toc_xlate_switch(nv,s,_tmp7DA,_tmp7DB);
return;_LL380: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7DC=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7DC->tag != 11)goto _LL382;else{_tmp7DD=_tmp7DC->f1;_tmp7DE=_tmp7DC->f2;}}_LL381: {
# 4300
struct Cyc_Toc_FallthruInfo*_tmp7F6;struct Cyc_Toc_Env*_tmp7F5=nv;_tmp7F6=_tmp7F5->fallthru_info;
if(_tmp7F6 == 0){
const char*_tmpD04;void*_tmpD03;(_tmpD03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD04="fallthru in unexpected place",_tag_dyneither(_tmpD04,sizeof(char),29))),_tag_dyneither(_tmpD03,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _stmttmp77=*_tmp7F6;struct _dyneither_ptr*_tmp7FA;struct Cyc_List_List*_tmp7FB;struct Cyc_Dict_Dict _tmp7FC;struct Cyc_Toc_FallthruInfo _tmp7F9=_stmttmp77;_tmp7FA=_tmp7F9.label;_tmp7FB=_tmp7F9.binders;_tmp7FC=_tmp7F9.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7FA,0);
# 4306
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7DE)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7FB);
struct Cyc_List_List*_tmp7FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DD);
for(0;_tmp7FD != 0;(_tmp7FD=_tmp7FD->tl,_tmp7FE=_tmp7FE->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7FE))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp7FC,(struct _tuple1*)_tmp7FD->hd),(struct Cyc_Absyn_Exp*)_tmp7FE->hd,0),s2,0);}
# 4314
s->r=s2->r;
return;};};};}_LL382: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7DF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7DF->tag != 12)goto _LL384;else{_tmp7E0=_tmp7DF->f1;_tmp7E1=_tmp7DF->f2;}}_LL383:
# 4320
{void*_stmttmp75=_tmp7E0->r;void*_tmp7FF=_stmttmp75;struct Cyc_Absyn_Vardecl*_tmp801;struct Cyc_Absyn_Pat*_tmp803;struct Cyc_Absyn_Exp*_tmp804;struct Cyc_List_List*_tmp806;struct Cyc_Absyn_Fndecl*_tmp808;struct Cyc_Absyn_Tvar*_tmp80A;struct Cyc_Absyn_Vardecl*_tmp80B;int _tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Tvar*_tmp80F;struct Cyc_Absyn_Vardecl*_tmp810;_LL38D: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp800=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp800->tag != 0)goto _LL38F;else{_tmp801=_tmp800->f1;}}_LL38E: {
# 4322
struct _RegionHandle _tmp811=_new_region("temp");struct _RegionHandle*temp=& _tmp811;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD07;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD06;struct Cyc_Toc_Env*_tmp812=Cyc_Toc_add_varmap(temp,nv,_tmp801->name,
Cyc_Absyn_varb_exp(_tmp801->name,(void*)((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06[0]=((_tmpD07.tag=4,((_tmpD07.f1=_tmp801,_tmpD07)))),_tmpD06)))),0));
Cyc_Toc_local_decl_to_c(_tmp812,_tmp812,_tmp801,_tmp7E1);}
# 4327
_npop_handler(0);goto _LL38C;
# 4322
;_pop_region(temp);}_LL38F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp802=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp802->tag != 2)goto _LL391;else{_tmp803=_tmp802->f1;_tmp804=_tmp802->f3;}}_LL390:
# 4331
{void*_stmttmp76=_tmp803->r;void*_tmp815=_stmttmp76;struct Cyc_Absyn_Vardecl*_tmp817;_LL39C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp816=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp815;if(_tmp816->tag != 1)goto _LL39E;else{_tmp817=_tmp816->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp818=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp816->f2)->r;if(_tmp818->tag != 0)goto _LL39E;};}}_LL39D: {
# 4333
struct _tuple1*old_name=_tmp817->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp817->name=new_name;
_tmp817->initializer=_tmp804;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD0A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD09;_tmp7E0->r=(void*)((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09[0]=((_tmpD0A.tag=0,((_tmpD0A.f1=_tmp817,_tmpD0A)))),_tmpD09))));}{
struct _RegionHandle _tmp81B=_new_region("temp");struct _RegionHandle*temp=& _tmp81B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD0D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD0C;struct Cyc_Toc_Env*_tmp81C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C[0]=((_tmpD0D.tag=4,((_tmpD0D.f1=_tmp817,_tmpD0D)))),_tmpD0C)))),0));
Cyc_Toc_local_decl_to_c(_tmp81C,nv,_tmp817,_tmp7E1);}
# 4344
_npop_handler(0);goto _LL39B;
# 4338
;_pop_region(temp);};}_LL39E:;_LL39F:
# 4349
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp803,(void*)_check_null(_tmp804->topt),_tmp804,_tmp7E1))->r;
goto _LL39B;_LL39B:;}
# 4352
goto _LL38C;_LL391: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp805=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp805->tag != 3)goto _LL393;else{_tmp806=_tmp805->f1;}}_LL392: {
# 4365 "toc.cyc"
struct Cyc_List_List*_tmp81F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp806);
if(_tmp81F == 0){
const char*_tmpD10;void*_tmpD0F;(_tmpD0F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD10="empty Letv_d",_tag_dyneither(_tmpD10,sizeof(char),13))),_tag_dyneither(_tmpD0F,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD13;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD12;_tmp7E0->r=(void*)((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD13.tag=0,((_tmpD13.f1=(struct Cyc_Absyn_Vardecl*)_tmp81F->hd,_tmpD13)))),_tmpD12))));}
_tmp81F=_tmp81F->tl;
for(0;_tmp81F != 0;_tmp81F=_tmp81F->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD16;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD15;struct Cyc_Absyn_Decl*_tmp824=Cyc_Absyn_new_decl((void*)((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=((_tmpD16.tag=0,((_tmpD16.f1=(struct Cyc_Absyn_Vardecl*)_tmp81F->hd,_tmpD16)))),_tmpD15)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp824,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4374
Cyc_Toc_stmt_to_c(nv,s);
goto _LL38C;}_LL393: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp807=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp807->tag != 1)goto _LL395;else{_tmp808=_tmp807->f1;}}_LL394: {
# 4377
struct _tuple1*_tmp827=_tmp808->name;
struct _RegionHandle _tmp828=_new_region("temp");struct _RegionHandle*temp=& _tmp828;_push_region(temp);{
struct Cyc_Toc_Env*_tmp829=Cyc_Toc_add_varmap(temp,nv,_tmp808->name,Cyc_Absyn_var_exp(_tmp827,0));
Cyc_Toc_fndecl_to_c(_tmp829,_tmp808,0);
Cyc_Toc_stmt_to_c(_tmp829,_tmp7E1);}
# 4383
_npop_handler(0);goto _LL38C;
# 4378
;_pop_region(temp);}_LL395: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp809=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp809->tag != 4)goto _LL397;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;_tmp80C=_tmp809->f3;_tmp80D=_tmp809->f4;}}_LL396: {
# 4385
struct Cyc_Absyn_Stmt*_tmp82A=_tmp7E1;
# 4401 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp80B->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4408
struct _RegionHandle _tmp82B=_new_region("temp");struct _RegionHandle*temp=& _tmp82B;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp82A);
# 4411
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp82A,0))->r;else{
if(_tmp80D == 0){
struct Cyc_Absyn_Exp*_tmpD19[1];struct Cyc_Absyn_Exp*_tmpD18[1];struct Cyc_List_List*_tmpD17;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD17->tl=0,_tmpD17)))))),0),
# 4420
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD18[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD18,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp82A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD19[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD19,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4426
Cyc_Toc_exp_to_c(nv,_tmp80D);{
struct Cyc_Absyn_Exp*_tmpD1B[1];struct Cyc_Absyn_Exp*_tmpD1A[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpD1A[1]=_tmp80D,((_tmpD1A[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4434
Cyc_Absyn_seq_stmt(_tmp82A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpD1B[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1B,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4439
_npop_handler(0);return;
# 4408
;_pop_region(temp);}_LL397: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp80E=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp80E->tag != 5)goto _LL399;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;}}_LL398: {
# 4442
struct _tuple1*old_name=_tmp810->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp810->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD2A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD29;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD28;struct Cyc_Absyn_Decl*_tmpD27;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD26;s->r=(void*)((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26[0]=((_tmpD2A.tag=12,((_tmpD2A.f1=((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->r=(void*)((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD28.tag=0,((_tmpD28.f1=_tmp810,_tmpD28)))),_tmpD29)))),((_tmpD27->loc=0,_tmpD27)))))),((_tmpD2A.f2=_tmp7E1,_tmpD2A)))))),_tmpD26))));}{
struct _RegionHandle _tmp836=_new_region("temp");struct _RegionHandle*temp=& _tmp836;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD2D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD2C;struct Cyc_Toc_Env*_tmp837=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((_tmpD2D.tag=4,((_tmpD2D.f1=_tmp810,_tmpD2D)))),_tmpD2C)))),0));
Cyc_Toc_local_decl_to_c(_tmp837,nv,_tmp810,_tmp7E1);}
# 4452
_npop_handler(0);return;
# 4446
;_pop_region(temp);};}_LL399:;_LL39A: {
# 4454
const char*_tmpD30;void*_tmpD2F;(_tmpD2F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD30="bad nested declaration within function",_tag_dyneither(_tmpD30,sizeof(char),39))),_tag_dyneither(_tmpD2F,sizeof(void*),0)));}_LL38C:;}
# 4456
return;_LL384: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7E2=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7E2->tag != 13)goto _LL386;else{_tmp7E3=_tmp7E2->f1;_tmp7E4=_tmp7E2->f2;}}_LL385:
# 4458
 s=_tmp7E4;continue;_LL386: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7E5=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7E5->tag != 14)goto _LL388;else{_tmp7E6=_tmp7E5->f1;_tmp7E7=(_tmp7E5->f2).f1;}}_LL387: {
# 4460
struct _RegionHandle _tmp83C=_new_region("temp");struct _RegionHandle*temp=& _tmp83C;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E6);
Cyc_Toc_exp_to_c(nv,_tmp7E7);
# 4464
_npop_handler(0);return;
# 4460
;_pop_region(temp);}_LL388: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7E8=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7E8->tag != 15)goto _LL38A;else{_tmp7E9=_tmp7E8->f1;_tmp7EA=_tmp7E8->f2;}}_LL389: {
# 4481 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4492
e_exp->topt=e_typ;{
struct _RegionHandle _tmp83D=_new_region("temp");struct _RegionHandle*temp=& _tmp83D;_push_region(temp);{
struct Cyc_Toc_Env*_tmp83E=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4496
Cyc_Toc_stmt_to_c(_tmp83E,_tmp7E9);{
struct Cyc_Absyn_Stmt*_tmp83F=Cyc_Absyn_seq_stmt(_tmp7E9,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4502
struct _tuple1*_tmp840=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp841=Cyc_Absyn_var_exp(_tmp840,0);
struct Cyc_Absyn_Vardecl*_tmp842=Cyc_Absyn_new_vardecl(_tmp840,Cyc_Absyn_exn_typ(),0);
_tmp841->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD3A;struct Cyc_Absyn_Pat*_tmpD39;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD38;struct Cyc_Absyn_Pat*_tmpD37;struct Cyc_Absyn_Pat*_tmp843=(_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->r=(void*)((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=((_tmpD38.tag=1,((_tmpD38.f1=_tmp842,((_tmpD38.f2=((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD39->topt=Cyc_Absyn_exn_typ(),((_tmpD39->loc=0,_tmpD39)))))))),_tmpD38)))))),_tmpD3A)))),((_tmpD37->topt=Cyc_Absyn_exn_typ(),((_tmpD37->loc=0,_tmpD37)))))));
struct Cyc_Absyn_Exp*_tmp844=Cyc_Absyn_throw_exp(_tmp841,0);
_tmp844->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp845=Cyc_Absyn_exp_stmt(_tmp844,0);
struct Cyc_Core_Opt*_tmpD49;struct _tuple27*_tmpD48;struct Cyc_Absyn_Vardecl**_tmpD47;struct Cyc_List_List*_tmpD46;struct Cyc_Absyn_Switch_clause*_tmpD45;struct Cyc_Absyn_Switch_clause*_tmp846=
(_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->pattern=_tmp843,((_tmpD45->pat_vars=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->v=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->f1=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47[0]=_tmp842,_tmpD47)))),((_tmpD48->f2=_tmp841,_tmpD48)))))),((_tmpD46->tl=0,_tmpD46)))))),_tmpD49)))),((_tmpD45->where_clause=0,((_tmpD45->body=_tmp845,((_tmpD45->loc=0,_tmpD45)))))))))));
# 4513
struct Cyc_List_List*_tmpD4A;struct Cyc_Absyn_Stmt*_tmp847=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7EA,(
(_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->hd=_tmp846,((_tmpD4A->tl=0,_tmpD4A))))))),0);
# 4517
Cyc_Toc_stmt_to_c(_tmp83E,_tmp847);{
# 4520
struct Cyc_List_List*_tmpD4B;struct Cyc_Absyn_Exp*_tmp848=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD4B->tl=0,_tmpD4B)))))),0);
# 4524
struct Cyc_List_List*_tmpD4C;struct Cyc_Absyn_Stmt*_tmp849=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD4C->tl=0,_tmpD4C)))))),0),0);
# 4528
struct Cyc_Absyn_Exp*_tmp84A=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp84B=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp849,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp84A,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp848,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp84B,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp83F,
# 4539
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp847,0),0),0),0),0),0))->r;};};};};}
# 4543
_npop_handler(0);return;
# 4493
;_pop_region(temp);};}_LL38A: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7EB=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7EB->tag != 16)goto _LL369;else{_tmp7EC=_tmp7EB->f1;}}_LL38B:
# 4545
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4548
Cyc_Toc_exp_to_c(nv,_tmp7EC);{
struct Cyc_List_List*_tmpD4D;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->hd=_tmp7EC,((_tmpD4D->tl=0,_tmpD4D)))))),0));};}
# 4551
return;_LL369:;}}
# 4560
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp859=_new_region("frgn");struct _RegionHandle*frgn=& _tmp859;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp85A=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp85B=f->args;for(0;_tmp85B != 0;_tmp85B=_tmp85B->tl){
struct _tuple1*_tmpD4E;struct _tuple1*_tmp85C=(_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E->f1=Cyc_Absyn_Loc_n,((_tmpD4E->f2=(*((struct _tuple9*)_tmp85B->hd)).f1,_tmpD4E)))));
(*((struct _tuple9*)_tmp85B->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp85B->hd)).f3);
_tmp85A=Cyc_Toc_add_varmap(frgn,_tmp85A,_tmp85C,Cyc_Absyn_var_exp(_tmp85C,0));}}
# 4575
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4579
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _stmttmp78=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp85F;struct Cyc_Absyn_Tqual _tmp860;void*_tmp861;int _tmp862;struct Cyc_Absyn_VarargInfo _tmp85E=_stmttmp78;_tmp85F=_tmp85E.name;_tmp860=_tmp85E.tq;_tmp861=_tmp85E.type;_tmp862=_tmp85E.inject;{
void*_tmp863=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp861,(void*)& Cyc_Absyn_HeapRgn_val,_tmp860,Cyc_Absyn_false_conref));
struct _tuple1*_tmpD4F;struct _tuple1*_tmp864=(_tmpD4F=_cycalloc(sizeof(*_tmpD4F)),((_tmpD4F->f1=Cyc_Absyn_Loc_n,((_tmpD4F->f2=(struct _dyneither_ptr*)_check_null(_tmp85F),_tmpD4F)))));
{struct _tuple9*_tmpD52;struct Cyc_List_List*_tmpD51;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51->hd=((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52->f1=_tmp85F,((_tmpD52->f2=_tmp860,((_tmpD52->f3=_tmp863,_tmpD52)))))))),((_tmpD51->tl=0,_tmpD51)))))));}
_tmp85A=Cyc_Toc_add_varmap(frgn,_tmp85A,_tmp864,Cyc_Absyn_var_exp(_tmp864,0));
f->cyc_varargs=0;};}
# 4588
{struct Cyc_List_List*_tmp868=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp868 != 0;_tmp868=_tmp868->tl){
((struct Cyc_Absyn_Vardecl*)_tmp868->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp868->hd)->type);}}
# 4591
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp85A),f->body);}
# 4567
;_pop_region(frgn);};}
# 4596
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3A0:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A1:
 return Cyc_Absyn_Extern;default: _LL3A2:
 return s;}}
# 4612 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple1*_tmp869=ad->name;
struct Cyc_Toc_TocState _stmttmp79=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp86B;struct Cyc_Dict_Dict*_tmp86C;struct Cyc_Toc_TocState _tmp86A=_stmttmp79;_tmp86B=_tmp86A.dyn;_tmp86C=_tmp86A.aggrs_so_far;{
struct _DynRegionFrame _tmp86D;struct _RegionHandle*d=_open_dynregion(& _tmp86D,_tmp86B);
{int seen_defn_before;
struct _tuple15**_tmp86E=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp86C,_tmp869);
if(_tmp86E == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpD53;v=((_tmpD53=_region_malloc(d,sizeof(*_tmpD53)),((_tmpD53->f1=ad,((_tmpD53->f2=Cyc_Absyn_strctq(_tmp869),_tmpD53))))));}else{
# 4624
struct _tuple15*_tmpD54;v=((_tmpD54=_region_malloc(d,sizeof(*_tmpD54)),((_tmpD54->f1=ad,((_tmpD54->f2=Cyc_Absyn_unionq_typ(_tmp869),_tmpD54))))));}
*_tmp86C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp86C,_tmp869,v);};}else{
# 4627
struct _tuple15*_stmttmp7A=*_tmp86E;struct Cyc_Absyn_Aggrdecl*_tmp872;void*_tmp873;struct _tuple15*_tmp871=_stmttmp7A;_tmp872=_tmp871->f1;_tmp873=_tmp871->f2;
if(_tmp872->impl == 0){
{struct _tuple15*_tmpD55;*_tmp86C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp86C,_tmp869,((_tmpD55=_region_malloc(d,sizeof(*_tmpD55)),((_tmpD55->f1=ad,((_tmpD55->f2=_tmp873,_tmpD55)))))));}
seen_defn_before=0;}else{
# 4632
seen_defn_before=1;}}{
# 4634
struct Cyc_Absyn_Aggrdecl*_tmpD56;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56->kind=ad->kind,((_tmpD56->sc=Cyc_Absyn_Public,((_tmpD56->name=ad->name,((_tmpD56->tvs=0,((_tmpD56->impl=0,((_tmpD56->attributes=ad->attributes,_tmpD56)))))))))))));
# 4640
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD57;new_ad->impl=((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57->exist_vars=0,((_tmpD57->rgn_po=0,((_tmpD57->fields=0,((_tmpD57->tagged=0,_tmpD57))))))))));}{
# 4645
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp876=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp876 != 0;_tmp876=_tmp876->tl){
# 4649
struct Cyc_Absyn_Aggrfield*_tmp877=(struct Cyc_Absyn_Aggrfield*)_tmp876->hd;
void*_tmp878=_tmp877->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp878)) && (
ad->kind == Cyc_Absyn_StructA  && _tmp876->tl == 0  || ad->kind == Cyc_Absyn_UnionA)){
# 4659
void*_stmttmp7B=Cyc_Tcutil_compress(_tmp878);void*_tmp879=_stmttmp7B;void*_tmp87B;struct Cyc_Absyn_Tqual _tmp87C;union Cyc_Absyn_Constraint*_tmp87D;unsigned int _tmp87E;_LL3A5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp87A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp879;if(_tmp87A->tag != 8)goto _LL3A7;else{_tmp87B=(_tmp87A->f1).elt_type;_tmp87C=(_tmp87A->f1).tq;_tmp87D=(_tmp87A->f1).zero_term;_tmp87E=(_tmp87A->f1).zt_loc;}}_LL3A6:
# 4661
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD5D;struct Cyc_Absyn_ArrayInfo _tmpD5C;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD5B;_tmp878=(void*)((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=((_tmpD5D.tag=8,((_tmpD5D.f1=((_tmpD5C.elt_type=_tmp87B,((_tmpD5C.tq=_tmp87C,((_tmpD5C.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD5C.zero_term=_tmp87D,((_tmpD5C.zt_loc=_tmp87E,_tmpD5C)))))))))),_tmpD5D)))),_tmpD5B))));}
goto _LL3A4;_LL3A7:;_LL3A8:
# 4664
 if(Cyc_Toc_abstract_aggr(_tmp878))goto _LL3A4;else{
if(ad->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpD60;void*_tmpD5F;(_tmpD5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD60="union with non-array fields of kind TA",_tag_dyneither(_tmpD60,sizeof(char),39))),_tag_dyneither(_tmpD5F,sizeof(void*),0)));}}_LL3A4:;}{
# 4669
struct Cyc_Absyn_Aggrfield*_tmpD61;struct Cyc_Absyn_Aggrfield*_tmp884=(_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->name=_tmp877->name,((_tmpD61->tq=Cyc_Toc_mt_tq,((_tmpD61->type=
# 4671
Cyc_Toc_typ_to_c(_tmp878),((_tmpD61->width=_tmp877->width,((_tmpD61->attributes=_tmp877->attributes,((_tmpD61->requires_clause=0,_tmpD61)))))))))))));
# 4678
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp885=_tmp884->type;
struct _dyneither_ptr*_tmp886=_tmp884->name;
const char*_tmpD66;void*_tmpD65[2];struct Cyc_String_pa_PrintArg_struct _tmpD64;struct Cyc_String_pa_PrintArg_struct _tmpD63;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD63.tag=0,((_tmpD63.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp886),((_tmpD64.tag=0,((_tmpD64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD65[0]=& _tmpD64,((_tmpD65[1]=& _tmpD63,Cyc_aprintf(((_tmpD66="_union_%s_%s",_tag_dyneither(_tmpD66,sizeof(char),13))),_tag_dyneither(_tmpD65,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD67;struct _dyneither_ptr*str=(_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67[0]=s,_tmpD67)));
struct Cyc_Absyn_Aggrfield*_tmpD68;struct Cyc_Absyn_Aggrfield*_tmp887=(_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->name=Cyc_Toc_val_sp,((_tmpD68->tq=Cyc_Toc_mt_tq,((_tmpD68->type=_tmp885,((_tmpD68->width=0,((_tmpD68->attributes=0,((_tmpD68->requires_clause=0,_tmpD68)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD69;struct Cyc_Absyn_Aggrfield*_tmp888=(_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->name=Cyc_Toc_tag_sp,((_tmpD69->tq=Cyc_Toc_mt_tq,((_tmpD69->type=Cyc_Absyn_sint_typ,((_tmpD69->width=0,((_tmpD69->attributes=0,((_tmpD69->requires_clause=0,_tmpD69)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD6A[2];struct Cyc_List_List*_tmp889=(_tmpD6A[1]=_tmp887,((_tmpD6A[0]=_tmp888,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD6A,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD6F;struct _tuple1*_tmpD6E;struct Cyc_Absyn_Aggrdecl*_tmpD6D;struct Cyc_Absyn_Aggrdecl*_tmp88A=(_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->kind=Cyc_Absyn_StructA,((_tmpD6D->sc=Cyc_Absyn_Public,((_tmpD6D->name=(
(_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->f1=Cyc_Absyn_Loc_n,((_tmpD6E->f2=str,_tmpD6E)))))),((_tmpD6D->tvs=0,((_tmpD6D->impl=(
(_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->exist_vars=0,((_tmpD6F->rgn_po=0,((_tmpD6F->fields=_tmp889,((_tmpD6F->tagged=0,_tmpD6F)))))))))),((_tmpD6D->attributes=0,_tmpD6D)))))))))))));
# 4690
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD75;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD74;struct Cyc_List_List*_tmpD73;Cyc_Toc_result_decls=((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=Cyc_Absyn_new_decl((void*)((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75[0]=((_tmpD74.tag=6,((_tmpD74.f1=_tmp88A,_tmpD74)))),_tmpD75)))),0),((_tmpD73->tl=Cyc_Toc_result_decls,_tmpD73))))));}
_tmp884->type=Cyc_Absyn_strct(str);}{
# 4693
struct Cyc_List_List*_tmpD76;new_fields=((_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->hd=_tmp884,((_tmpD76->tl=new_fields,_tmpD76))))));};};}}
# 4695
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4697
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD80;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD7F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD7E;struct Cyc_List_List*_tmpD7D;Cyc_Toc_result_decls=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->hd=((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->r=(void*)((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E[0]=((_tmpD7F.tag=6,((_tmpD7F.f1=new_ad,_tmpD7F)))),_tmpD7E)))),((_tmpD80->loc=0,_tmpD80)))))),((_tmpD7D->tl=Cyc_Toc_result_decls,_tmpD7D))))));}};}
# 4616
;_pop_dynregion(d);};}
# 4726 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct Cyc_Toc_TocState _stmttmp7C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp8A1;struct Cyc_Set_Set**_tmp8A2;struct Cyc_Toc_TocState _tmp8A0=_stmttmp7C;_tmp8A1=_tmp8A0.dyn;_tmp8A2=_tmp8A0.datatypes_so_far;{
struct _DynRegionFrame _tmp8A3;struct _RegionHandle*d=_open_dynregion(& _tmp8A3,_tmp8A1);{
struct _tuple1*_tmp8A4=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp8A2,_tmp8A4)){
_npop_handler(0);return;}
*_tmp8A2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp8A2,_tmp8A4);}{
# 4734
struct Cyc_List_List*_tmp8A5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8A5 != 0;_tmp8A5=_tmp8A5->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8A5->hd;
# 4737
struct Cyc_List_List*_tmp8A6=0;
int i=1;
{struct Cyc_List_List*_tmp8A7=f->typs;for(0;_tmp8A7 != 0;(_tmp8A7=_tmp8A7->tl,i ++)){
struct _dyneither_ptr*_tmp8A8=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD81;struct Cyc_Absyn_Aggrfield*_tmp8A9=(_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->name=_tmp8A8,((_tmpD81->tq=(*((struct _tuple11*)_tmp8A7->hd)).f1,((_tmpD81->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp8A7->hd)).f2),((_tmpD81->width=0,((_tmpD81->attributes=0,((_tmpD81->requires_clause=0,_tmpD81)))))))))))));
struct Cyc_List_List*_tmpD82;_tmp8A6=((_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->hd=_tmp8A9,((_tmpD82->tl=_tmp8A6,_tmpD82))))));}}
# 4745
{struct Cyc_Absyn_Aggrfield*_tmpD85;struct Cyc_List_List*_tmpD84;_tmp8A6=((_tmpD84=_cycalloc(sizeof(*_tmpD84)),((_tmpD84->hd=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->name=Cyc_Toc_tag_sp,((_tmpD85->tq=Cyc_Toc_mt_tq,((_tmpD85->type=Cyc_Absyn_sint_typ,((_tmpD85->width=0,((_tmpD85->attributes=0,((_tmpD85->requires_clause=0,_tmpD85)))))))))))))),((_tmpD84->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8A6),_tmpD84))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD88;struct Cyc_Absyn_Aggrdecl*_tmpD87;struct Cyc_Absyn_Aggrdecl*_tmp8AE=
(_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->kind=Cyc_Absyn_StructA,((_tmpD87->sc=Cyc_Absyn_Public,((_tmpD87->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD87->tvs=0,((_tmpD87->impl=(
# 4751
(_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->exist_vars=0,((_tmpD88->rgn_po=0,((_tmpD88->fields=_tmp8A6,((_tmpD88->tagged=0,_tmpD88)))))))))),((_tmpD87->attributes=0,_tmpD87)))))))))))));
# 4753
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD8E;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD8D;struct Cyc_List_List*_tmpD8C;Cyc_Toc_result_decls=((_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C->hd=Cyc_Absyn_new_decl((void*)((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[0]=((_tmpD8D.tag=6,((_tmpD8D.f1=_tmp8AE,_tmpD8D)))),_tmpD8E)))),0),((_tmpD8C->tl=Cyc_Toc_result_decls,_tmpD8C))))));};}};
# 4728
;_pop_dynregion(d);};}
# 4774 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct Cyc_Toc_TocState _stmttmp7D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp8B5;struct Cyc_Dict_Dict*_tmp8B6;struct Cyc_Toc_TocState _tmp8B4=_stmttmp7D;_tmp8B5=_tmp8B4.dyn;_tmp8B6=_tmp8B4.xdatatypes_so_far;{
struct _DynRegionFrame _tmp8B7;struct _RegionHandle*d=_open_dynregion(& _tmp8B7,_tmp8B5);
{struct _tuple1*_tmp8B8=xd->name;
struct Cyc_List_List*_tmp8B9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp8B9 != 0;_tmp8B9=_tmp8B9->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8B9->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8BB=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8BA,Cyc_Absyn_false_conref,0);
# 4786
int*_stmttmp7E=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8B6,f->name);int*_tmp8BC=_stmttmp7E;_LL3AA: if(_tmp8BC != 0)goto _LL3AC;_LL3AB: {
# 4788
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4792
struct Cyc_Absyn_Vardecl*_tmp8BD=Cyc_Absyn_new_vardecl(f->name,_tmp8BB,initopt);
_tmp8BD->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD94;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD93;struct Cyc_List_List*_tmpD92;Cyc_Toc_result_decls=((_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->hd=Cyc_Absyn_new_decl((void*)((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94[0]=((_tmpD93.tag=0,((_tmpD93.f1=_tmp8BD,_tmpD93)))),_tmpD94)))),0),((_tmpD92->tl=Cyc_Toc_result_decls,_tmpD92))))));}
*_tmp8B6=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp8B6,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8C1=f->typs;for(0;_tmp8C1 != 0;(_tmp8C1=_tmp8C1->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD9C;void*_tmpD9B[1];const char*_tmpD9A;struct _dyneither_ptr*_tmpD99;struct _dyneither_ptr*_tmp8C2=(_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99[0]=(struct _dyneither_ptr)((_tmpD9C.tag=1,((_tmpD9C.f1=(unsigned long)i,((_tmpD9B[0]=& _tmpD9C,Cyc_aprintf(((_tmpD9A="f%d",_tag_dyneither(_tmpD9A,sizeof(char),4))),_tag_dyneither(_tmpD9B,sizeof(void*),1)))))))),_tmpD99)));
struct Cyc_Absyn_Aggrfield*_tmpD9D;struct Cyc_Absyn_Aggrfield*_tmp8C3=(_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->name=_tmp8C2,((_tmpD9D->tq=(*((struct _tuple11*)_tmp8C1->hd)).f1,((_tmpD9D->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp8C1->hd)).f2),((_tmpD9D->width=0,((_tmpD9D->attributes=0,((_tmpD9D->requires_clause=0,_tmpD9D)))))))))))));
struct Cyc_List_List*_tmpD9E;fields=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->hd=_tmp8C3,((_tmpD9E->tl=fields,_tmpD9E))))));}}
# 4805
{struct Cyc_Absyn_Aggrfield*_tmpDA1;struct Cyc_List_List*_tmpDA0;fields=((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0->hd=((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->name=Cyc_Toc_tag_sp,((_tmpDA1->tq=Cyc_Toc_mt_tq,((_tmpDA1->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpDA1->width=0,((_tmpDA1->attributes=0,((_tmpDA1->requires_clause=0,_tmpDA1)))))))))))))),((_tmpDA0->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpDA0))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDA4;struct Cyc_Absyn_Aggrdecl*_tmpDA3;struct Cyc_Absyn_Aggrdecl*_tmp8CC=
(_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->kind=Cyc_Absyn_StructA,((_tmpDA3->sc=Cyc_Absyn_Public,((_tmpDA3->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpDA3->tvs=0,((_tmpDA3->impl=(
# 4812
(_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->exist_vars=0,((_tmpDA4->rgn_po=0,((_tmpDA4->fields=fields,((_tmpDA4->tagged=0,_tmpDA4)))))))))),((_tmpDA3->attributes=0,_tmpDA3)))))))))))));
# 4814
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDAA;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDA9;struct Cyc_List_List*_tmpDA8;Cyc_Toc_result_decls=((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->hd=Cyc_Absyn_new_decl((void*)((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDA9.tag=6,((_tmpDA9.f1=_tmp8CC,_tmpDA9)))),_tmpDAA)))),0),((_tmpDA8->tl=Cyc_Toc_result_decls,_tmpDA8))))));}
# 4816
goto _LL3A9;};};};}_LL3AC: if(_tmp8BC == 0)goto _LL3AE;if(*_tmp8BC != 0)goto _LL3AE;_LL3AD:
# 4818
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8D2=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8D3=Cyc_Absyn_new_vardecl(f->name,_tmp8BB,_tmp8D2);
_tmp8D3->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDB0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDAF;struct Cyc_List_List*_tmpDAE;Cyc_Toc_result_decls=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->hd=Cyc_Absyn_new_decl((void*)((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDAF.tag=0,((_tmpDAF.f1=_tmp8D3,_tmpDAF)))),_tmpDB0)))),0),((_tmpDAE->tl=Cyc_Toc_result_decls,_tmpDAE))))));}
*_tmp8B6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp8B6,f->name,1);}
# 4825
goto _LL3A9;_LL3AE:;_LL3AF:
 goto _LL3A9;_LL3A9:;}}
# 4779
;_pop_dynregion(d);};};}
# 4832
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4840
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4844
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_stmttmp7F=init->r;void*_tmp8D7=_stmttmp7F;struct Cyc_Absyn_Vardecl*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8DB;int _tmp8DC;_LL3B1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8D8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D7;if(_tmp8D8->tag != 26)goto _LL3B3;else{_tmp8D9=_tmp8D8->f1;_tmp8DA=_tmp8D8->f2;_tmp8DB=_tmp8D8->f3;_tmp8DC=_tmp8D8->f4;}}_LL3B2:
# 4851
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8D9,_tmp8DA,_tmp8DB,_tmp8DC,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3B0;_LL3B3:;_LL3B4:
# 4856
 if(vd->sc == Cyc_Absyn_Static){
# 4860
struct _RegionHandle _tmp8DD=_new_region("temp");struct _RegionHandle*temp=& _tmp8DD;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8DE=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8DE,init);}
# 4861
;_pop_region(temp);}else{
# 4865
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3B0;_LL3B0:;}else{
# 4870
void*_stmttmp80=Cyc_Tcutil_compress(old_typ);void*_tmp8DF=_stmttmp80;void*_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E2;union Cyc_Absyn_Constraint*_tmp8E3;_LL3B6:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8E0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DF;if(_tmp8E0->tag != 8)goto _LL3B8;else{_tmp8E1=(_tmp8E0->f1).elt_type;_tmp8E2=(_tmp8E0->f1).num_elts;_tmp8E3=(_tmp8E0->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E3))goto _LL3B8;_LL3B7:
 if(_tmp8E2 == 0){
const char*_tmpDB3;void*_tmpDB2;(_tmpDB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDB3="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpDB3,sizeof(char),55))),_tag_dyneither(_tmpDB2,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8E2;
struct Cyc_Absyn_Exp*_tmp8E6=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4878
struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8E6,_tmp8E7,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B5;};_LL3B8:;_LL3B9:
 goto _LL3B5;_LL3B5:;}}
# 4892
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8E8=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8E8;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8E9=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8EA=Cyc_Toc_share_env(prgn,nv);
struct _tuple23 _stmttmp81=
Cyc_Toc_xlate_pat(_tmp8EA,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4898
struct Cyc_Toc_Env*_tmp8EC;struct Cyc_List_List*_tmp8ED;struct Cyc_Absyn_Stmt*_tmp8EE;struct _tuple23 _tmp8EB=_stmttmp81;_tmp8EC=_tmp8EB.f1;_tmp8ED=_tmp8EB.f2;_tmp8EE=_tmp8EB.f3;
# 4901
Cyc_Toc_stmt_to_c(_tmp8EC,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8EE,s,0),0);
for(0;_tmp8ED != 0;_tmp8ED=_tmp8ED->tl){
struct _tuple24*_stmttmp82=(struct _tuple24*)_tmp8ED->hd;struct _tuple1*_tmp8F0;void*_tmp8F1;struct _tuple24*_tmp8EF=_stmttmp82;_tmp8F0=_tmp8EF->f1;_tmp8F1=_tmp8EF->f2;
s=Cyc_Absyn_declare_stmt(_tmp8F0,_tmp8F1,0,s,0);}}{
# 4908
struct Cyc_Absyn_Stmt*_tmp8F2=s;_npop_handler(0);return _tmp8F2;};
# 4895
;_pop_region(prgn);};}
# 4914
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_stmttmp83=e->r;void*_tmp8F3=_stmttmp83;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F7;struct Cyc_Absyn_Exp*_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_Absyn_Exp*_tmp901;struct Cyc_Absyn_Exp*_tmp903;struct Cyc_List_List*_tmp905;struct Cyc_Absyn_Exp*_tmp907;struct Cyc_Absyn_Exp*_tmp908;struct Cyc_Absyn_Exp*_tmp90A;struct Cyc_Absyn_Exp*_tmp90B;struct Cyc_Absyn_Exp*_tmp90D;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_Absyn_Exp*_tmp916;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91D;struct Cyc_List_List*_tmp91E;void**_tmp920;struct Cyc_Absyn_Exp*_tmp921;void**_tmp923;struct Cyc_List_List*_tmp924;struct Cyc_List_List*_tmp926;struct Cyc_List_List*_tmp928;void**_tmp92A;void**_tmp92C;struct Cyc_Absyn_Stmt*_tmp92E;struct Cyc_Absyn_MallocInfo*_tmp930;_LL3BB: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp8F4->tag != 19)goto _LL3BD;else{_tmp8F5=_tmp8F4->f1;}}_LL3BC:
 _tmp8F7=_tmp8F5;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8F6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp8F6->tag != 20)goto _LL3BF;else{_tmp8F7=_tmp8F6->f1;}}_LL3BE:
 _tmp8F9=_tmp8F7;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8F8=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp8F8->tag != 21)goto _LL3C1;else{_tmp8F9=_tmp8F8->f1;}}_LL3C0:
 _tmp8FB=_tmp8F9;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp8FA->tag != 14)goto _LL3C3;else{_tmp8FB=_tmp8FA->f1;}}_LL3C2:
 _tmp8FD=_tmp8FB;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8FC=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp8FC->tag != 10)goto _LL3C5;else{_tmp8FD=_tmp8FC->f1;}}_LL3C4:
 _tmp8FF=_tmp8FD;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp8FE->tag != 11)goto _LL3C7;else{_tmp8FF=_tmp8FE->f1;}}_LL3C6:
 _tmp901=_tmp8FF;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp900=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp900->tag != 17)goto _LL3C9;else{_tmp901=_tmp900->f1;}}_LL3C8:
 _tmp903=_tmp901;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp902=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp902->tag != 4)goto _LL3CB;else{_tmp903=_tmp902->f1;}}_LL3CA:
 Cyc_Toc_exptypes_to_c(_tmp903);goto _LL3BA;_LL3CB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp904=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp904->tag != 2)goto _LL3CD;else{_tmp905=_tmp904->f2;}}_LL3CC:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp905);goto _LL3BA;_LL3CD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp906=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp906->tag != 6)goto _LL3CF;else{_tmp907=_tmp906->f1;_tmp908=_tmp906->f2;}}_LL3CE:
 _tmp90A=_tmp907;_tmp90B=_tmp908;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp909=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp909->tag != 7)goto _LL3D1;else{_tmp90A=_tmp909->f1;_tmp90B=_tmp909->f2;}}_LL3D0:
 _tmp90D=_tmp90A;_tmp90E=_tmp90B;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp90C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp90C->tag != 8)goto _LL3D3;else{_tmp90D=_tmp90C->f1;_tmp90E=_tmp90C->f2;}}_LL3D2:
 _tmp910=_tmp90D;_tmp911=_tmp90E;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp90F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp90F->tag != 22)goto _LL3D5;else{_tmp910=_tmp90F->f1;_tmp911=_tmp90F->f2;}}_LL3D4:
 _tmp913=_tmp910;_tmp914=_tmp911;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp912=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp912->tag != 34)goto _LL3D7;else{_tmp913=_tmp912->f1;_tmp914=_tmp912->f2;}}_LL3D6:
 _tmp916=_tmp913;_tmp917=_tmp914;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp915=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp915->tag != 3)goto _LL3D9;else{_tmp916=_tmp915->f1;_tmp917=_tmp915->f3;}}_LL3D8:
 Cyc_Toc_exptypes_to_c(_tmp916);Cyc_Toc_exptypes_to_c(_tmp917);goto _LL3BA;_LL3D9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp918=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp918->tag != 5)goto _LL3DB;else{_tmp919=_tmp918->f1;_tmp91A=_tmp918->f2;_tmp91B=_tmp918->f3;}}_LL3DA:
# 4932
 Cyc_Toc_exptypes_to_c(_tmp919);Cyc_Toc_exptypes_to_c(_tmp91A);Cyc_Toc_exptypes_to_c(_tmp91B);goto _LL3BA;_LL3DB: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp91C=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp91C->tag != 9)goto _LL3DD;else{_tmp91D=_tmp91C->f1;_tmp91E=_tmp91C->f2;}}_LL3DC:
# 4934
 Cyc_Toc_exptypes_to_c(_tmp91D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp91E);goto _LL3BA;_LL3DD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp91F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp91F->tag != 13)goto _LL3DF;else{_tmp920=(void**)((void**)& _tmp91F->f1);_tmp921=_tmp91F->f2;}}_LL3DE:
*_tmp920=Cyc_Toc_typ_to_c(*_tmp920);Cyc_Toc_exptypes_to_c(_tmp921);goto _LL3BA;_LL3DF: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp922=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp922->tag != 24)goto _LL3E1;else{_tmp923=(void**)&(_tmp922->f1)->f3;_tmp924=_tmp922->f2;}}_LL3E0:
# 4937
*_tmp923=Cyc_Toc_typ_to_c(*_tmp923);
_tmp926=_tmp924;goto _LL3E2;_LL3E1: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp925=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp925->tag != 35)goto _LL3E3;else{_tmp926=_tmp925->f2;}}_LL3E2:
 _tmp928=_tmp926;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp927=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp927->tag != 25)goto _LL3E5;else{_tmp928=_tmp927->f1;}}_LL3E4:
# 4941
 for(0;_tmp928 != 0;_tmp928=_tmp928->tl){
struct _tuple16 _stmttmp84=*((struct _tuple16*)_tmp928->hd);struct Cyc_Absyn_Exp*_tmp941;struct _tuple16 _tmp940=_stmttmp84;_tmp941=_tmp940.f2;
Cyc_Toc_exptypes_to_c(_tmp941);}
# 4945
goto _LL3BA;_LL3E5: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp929=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp929->tag != 18)goto _LL3E7;else{_tmp92A=(void**)((void**)& _tmp929->f1);}}_LL3E6:
 _tmp92C=_tmp92A;goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp92B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp92B->tag != 16)goto _LL3E9;else{_tmp92C=(void**)((void**)& _tmp92B->f1);}}_LL3E8:
*_tmp92C=Cyc_Toc_typ_to_c(*_tmp92C);goto _LL3BA;_LL3E9: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp92D=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp92D->tag != 36)goto _LL3EB;else{_tmp92E=_tmp92D->f1;}}_LL3EA:
 Cyc_Toc_stmttypes_to_c(_tmp92E);goto _LL3BA;_LL3EB: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp92F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp92F->tag != 33)goto _LL3ED;else{_tmp930=(struct Cyc_Absyn_MallocInfo*)& _tmp92F->f1;}}_LL3EC:
# 4950
 if(_tmp930->elt_type != 0){
void**_tmpDB4;_tmp930->elt_type=((_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp930->elt_type))),_tmpDB4))));}
Cyc_Toc_exptypes_to_c(_tmp930->num_elts);
goto _LL3BA;_LL3ED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp931=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp931->tag != 0)goto _LL3EF;}_LL3EE:
 goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp932=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp932->tag != 1)goto _LL3F1;}_LL3F0:
 goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp933=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp933->tag != 31)goto _LL3F3;}_LL3F2:
 goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp934=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp934->tag != 39)goto _LL3F5;}_LL3F4:
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp935=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp935->tag != 32)goto _LL3F7;}_LL3F6:
 goto _LL3BA;_LL3F7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp936=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp936->tag != 29)goto _LL3F9;}_LL3F8:
# 4960
 goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp937=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp937->tag != 30)goto _LL3FB;}_LL3FA:
 goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp938=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp938->tag != 28)goto _LL3FD;}_LL3FC:
 goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp939=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp939->tag != 26)goto _LL3FF;}_LL3FE:
 goto _LL400;_LL3FF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp93A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp93A->tag != 27)goto _LL401;}_LL400:
 goto _LL402;_LL401: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp93B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp93B->tag != 23)goto _LL403;}_LL402:
 goto _LL404;_LL403: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp93C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp93C->tag != 12)goto _LL405;}_LL404:
 goto _LL406;_LL405: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp93D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp93D->tag != 15)goto _LL407;}_LL406:
 goto _LL408;_LL407: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp93E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp93E->tag != 38)goto _LL409;}_LL408:
 goto _LL40A;_LL409: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp93F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8F3;if(_tmp93F->tag != 37)goto _LL3BA;}_LL40A:
# 4970
{const char*_tmpDB7;void*_tmpDB6;(_tmpDB6=0,Cyc_Tcutil_terr(e->loc,((_tmpDB7="Cyclone expression within C code",_tag_dyneither(_tmpDB7,sizeof(char),33))),_tag_dyneither(_tmpDB6,sizeof(void*),0)));}
goto _LL3BA;_LL3BA:;}
# 4975
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_stmttmp85=d->r;void*_tmp945=_stmttmp85;struct Cyc_Absyn_Vardecl*_tmp947;struct Cyc_Absyn_Fndecl*_tmp949;struct Cyc_Absyn_Aggrdecl*_tmp94B;struct Cyc_Absyn_Enumdecl*_tmp94D;struct Cyc_Absyn_Typedefdecl*_tmp94F;_LL40C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp946=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp946->tag != 0)goto _LL40E;else{_tmp947=_tmp946->f1;}}_LL40D:
# 4978
 _tmp947->type=Cyc_Toc_typ_to_c(_tmp947->type);
if(_tmp947->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp947->initializer));
goto _LL40B;_LL40E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp948=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp948->tag != 1)goto _LL410;else{_tmp949=_tmp948->f1;}}_LL40F:
# 4982
 _tmp949->ret_type=Cyc_Toc_typ_to_c(_tmp949->ret_type);
{struct Cyc_List_List*_tmp95B=_tmp949->args;for(0;_tmp95B != 0;_tmp95B=_tmp95B->tl){
(*((struct _tuple9*)_tmp95B->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp95B->hd)).f3);}}
# 4986
goto _LL40B;_LL410: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp94A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp94A->tag != 6)goto _LL412;else{_tmp94B=_tmp94A->f1;}}_LL411:
 Cyc_Toc_aggrdecl_to_c(_tmp94B,1);goto _LL40B;_LL412: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp94C->tag != 8)goto _LL414;else{_tmp94D=_tmp94C->f1;}}_LL413:
# 4989
 if(_tmp94D->fields != 0){
struct Cyc_List_List*_tmp95C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp94D->fields))->v;for(0;_tmp95C != 0;_tmp95C=_tmp95C->tl){
struct Cyc_Absyn_Enumfield*_tmp95D=(struct Cyc_Absyn_Enumfield*)_tmp95C->hd;
if(_tmp95D->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp95D->tag));}}
# 4994
goto _LL40B;_LL414: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp94E=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp94E->tag != 9)goto _LL416;else{_tmp94F=_tmp94E->f1;}}_LL415:
 _tmp94F->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp94F->defn));goto _LL40B;_LL416: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp950=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp950->tag != 2)goto _LL418;}_LL417:
 goto _LL419;_LL418: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp951=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp951->tag != 3)goto _LL41A;}_LL419:
 goto _LL41B;_LL41A: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp952=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp952->tag != 7)goto _LL41C;}_LL41B:
 goto _LL41D;_LL41C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp953=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp953->tag != 10)goto _LL41E;}_LL41D:
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp954=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp954->tag != 11)goto _LL420;}_LL41F:
 goto _LL421;_LL420: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp955=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp955->tag != 12)goto _LL422;}_LL421:
 goto _LL423;_LL422: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp956=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp956->tag != 13)goto _LL424;}_LL423:
 goto _LL425;_LL424: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp957=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp957->tag != 4)goto _LL426;}_LL425:
 goto _LL427;_LL426: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp958=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp958->tag != 5)goto _LL428;}_LL427:
# 5005
{const char*_tmpDBA;void*_tmpDB9;(_tmpDB9=0,Cyc_Tcutil_terr(d->loc,((_tmpDBA="Cyclone declaration within C code",_tag_dyneither(_tmpDBA,sizeof(char),34))),_tag_dyneither(_tmpDB9,sizeof(void*),0)));}
goto _LL40B;_LL428: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp959=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp959->tag != 14)goto _LL42A;}_LL429:
 goto _LL42B;_LL42A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp95A=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp945;if(_tmp95A->tag != 15)goto _LL40B;}_LL42B:
# 5009
 goto _LL40B;_LL40B:;}
# 5013
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_stmttmp86=s->r;void*_tmp960=_stmttmp86;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Stmt*_tmp964;struct Cyc_Absyn_Stmt*_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp969;struct Cyc_Absyn_Stmt*_tmp96A;struct Cyc_Absyn_Stmt*_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Stmt*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Stmt*_tmp973;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_List_List*_tmp976;struct Cyc_Absyn_Decl*_tmp978;struct Cyc_Absyn_Stmt*_tmp979;struct Cyc_Absyn_Stmt*_tmp97B;struct Cyc_Absyn_Exp*_tmp97C;_LL42D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp961=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp961->tag != 1)goto _LL42F;else{_tmp962=_tmp961->f1;}}_LL42E:
 Cyc_Toc_exptypes_to_c(_tmp962);goto _LL42C;_LL42F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp963=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp963->tag != 2)goto _LL431;else{_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;}}_LL430:
 Cyc_Toc_stmttypes_to_c(_tmp964);Cyc_Toc_stmttypes_to_c(_tmp965);goto _LL42C;_LL431: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp966=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp966->tag != 3)goto _LL433;else{_tmp967=_tmp966->f1;}}_LL432:
 if(_tmp967 != 0)Cyc_Toc_exptypes_to_c(_tmp967);goto _LL42C;_LL433: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp968=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp968->tag != 4)goto _LL435;else{_tmp969=_tmp968->f1;_tmp96A=_tmp968->f2;_tmp96B=_tmp968->f3;}}_LL434:
# 5019
 Cyc_Toc_exptypes_to_c(_tmp969);Cyc_Toc_stmttypes_to_c(_tmp96A);Cyc_Toc_stmttypes_to_c(_tmp96B);goto _LL42C;_LL435: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp96C=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp96C->tag != 5)goto _LL437;else{_tmp96D=(_tmp96C->f1).f1;_tmp96E=_tmp96C->f2;}}_LL436:
# 5021
 Cyc_Toc_exptypes_to_c(_tmp96D);Cyc_Toc_stmttypes_to_c(_tmp96E);goto _LL42C;_LL437: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp96F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp96F->tag != 9)goto _LL439;else{_tmp970=_tmp96F->f1;_tmp971=(_tmp96F->f2).f1;_tmp972=(_tmp96F->f3).f1;_tmp973=_tmp96F->f4;}}_LL438:
# 5023
 Cyc_Toc_exptypes_to_c(_tmp970);Cyc_Toc_exptypes_to_c(_tmp971);Cyc_Toc_exptypes_to_c(_tmp972);
Cyc_Toc_stmttypes_to_c(_tmp973);goto _LL42C;_LL439: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp974=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp974->tag != 10)goto _LL43B;else{_tmp975=_tmp974->f1;_tmp976=_tmp974->f2;}}_LL43A:
# 5026
 Cyc_Toc_exptypes_to_c(_tmp975);
for(0;_tmp976 != 0;_tmp976=_tmp976->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp976->hd)->body);}
goto _LL42C;_LL43B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp977=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp977->tag != 12)goto _LL43D;else{_tmp978=_tmp977->f1;_tmp979=_tmp977->f2;}}_LL43C:
 Cyc_Toc_decltypes_to_c(_tmp978);Cyc_Toc_stmttypes_to_c(_tmp979);goto _LL42C;_LL43D: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp97A=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97A->tag != 14)goto _LL43F;else{_tmp97B=_tmp97A->f1;_tmp97C=(_tmp97A->f2).f1;}}_LL43E:
 Cyc_Toc_stmttypes_to_c(_tmp97B);Cyc_Toc_exptypes_to_c(_tmp97C);goto _LL42C;_LL43F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp97D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97D->tag != 0)goto _LL441;}_LL440:
 goto _LL442;_LL441: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp97E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97E->tag != 6)goto _LL443;}_LL442:
 goto _LL444;_LL443: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp97F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp97F->tag != 7)goto _LL445;}_LL444:
 goto _LL446;_LL445: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp980=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp980->tag != 8)goto _LL447;}_LL446:
 goto _LL42C;_LL447: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp981=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp981->tag != 11)goto _LL449;}_LL448:
 goto _LL44A;_LL449: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp982=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp982->tag != 13)goto _LL44B;}_LL44A:
 goto _LL44C;_LL44B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp983=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp983->tag != 15)goto _LL44D;}_LL44C:
 goto _LL44E;_LL44D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp984=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp960;if(_tmp984->tag != 16)goto _LL42C;}_LL44E:
{const char*_tmpDBD;void*_tmpDBC;(_tmpDBC=0,Cyc_Tcutil_terr(s->loc,((_tmpDBD="Cyclone statement in C code",_tag_dyneither(_tmpDBD,sizeof(char),28))),_tag_dyneither(_tmpDBC,sizeof(void*),0)));}
goto _LL42C;_LL42C:;}
# 5047
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpDC0;void*_tmpDBF;(_tmpDBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDC0="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDC0,sizeof(char),29))),_tag_dyneither(_tmpDBF,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_stmttmp87=d->r;void*_tmp989=_stmttmp87;struct Cyc_Absyn_Vardecl*_tmp98B;struct Cyc_Absyn_Fndecl*_tmp98D;struct Cyc_Absyn_Aggrdecl*_tmp993;struct Cyc_Absyn_Datatypedecl*_tmp995;struct Cyc_Absyn_Enumdecl*_tmp997;struct Cyc_Absyn_Typedefdecl*_tmp999;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99F;struct Cyc_List_List*_tmp9A1;struct Cyc_List_List*_tmp9A3;_LL450: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp98A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98A->tag != 0)goto _LL452;else{_tmp98B=_tmp98A->f1;}}_LL451: {
# 5054
struct _tuple1*_tmp9A4=_tmp98B->name;
# 5056
if(_tmp98B->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpDC1;_tmp9A4=((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->f1=Cyc_Absyn_Rel_n(0),((_tmpDC1->f2=(*_tmp9A4).f2,_tmpDC1))))));}
if(_tmp98B->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp98B->initializer));else{
# 5062
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp98B->initializer));}}
# 5064
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpDC4;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDC3;nv=Cyc_Toc_add_varmap(r,nv,_tmp98B->name,Cyc_Absyn_varb_exp(_tmp9A4,(void*)((_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC4.tag=1,((_tmpDC4.f1=_tmp98B,_tmpDC4)))),_tmpDC3)))),0));}
_tmp98B->name=_tmp9A4;
_tmp98B->sc=Cyc_Toc_scope_to_c(_tmp98B->sc);
_tmp98B->type=Cyc_Toc_typ_to_c(_tmp98B->type);
{struct Cyc_List_List*_tmpDC5;Cyc_Toc_result_decls=((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->hd=d,((_tmpDC5->tl=Cyc_Toc_result_decls,_tmpDC5))))));}
goto _LL44F;}_LL452: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp98C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98C->tag != 1)goto _LL454;else{_tmp98D=_tmp98C->f1;}}_LL453: {
# 5071
struct _tuple1*_tmp9A9=_tmp98D->name;
# 5073
if(_tmp98D->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpDC6;_tmp9A9=((_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->f1=Cyc_Absyn_Abs_n(0,1),((_tmpDC6->f2=(*_tmp9A9).f2,_tmpDC6))))));}
_tmp98D->sc=Cyc_Absyn_Public;}
# 5077
nv=Cyc_Toc_add_varmap(r,nv,_tmp98D->name,Cyc_Absyn_var_exp(_tmp9A9,0));
_tmp98D->name=_tmp9A9;
Cyc_Toc_fndecl_to_c(nv,_tmp98D,cinclude);
{struct Cyc_List_List*_tmpDC7;Cyc_Toc_result_decls=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->hd=d,((_tmpDC7->tl=Cyc_Toc_result_decls,_tmpDC7))))));}
goto _LL44F;}_LL454: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp98E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98E->tag != 2)goto _LL456;}_LL455:
 goto _LL457;_LL456: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp98F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp98F->tag != 3)goto _LL458;}_LL457: {
# 5084
const char*_tmpDCA;void*_tmpDC9;(_tmpDC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDCA="letdecl at toplevel",_tag_dyneither(_tmpDCA,sizeof(char),20))),_tag_dyneither(_tmpDC9,sizeof(void*),0)));}_LL458: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp990=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp990->tag != 4)goto _LL45A;}_LL459: {
# 5086
const char*_tmpDCD;void*_tmpDCC;(_tmpDCC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDCD="region decl at toplevel",_tag_dyneither(_tmpDCD,sizeof(char),24))),_tag_dyneither(_tmpDCC,sizeof(void*),0)));}_LL45A: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp991=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp991->tag != 5)goto _LL45C;}_LL45B: {
# 5088
const char*_tmpDD0;void*_tmpDCF;(_tmpDCF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDD0="alias decl at toplevel",_tag_dyneither(_tmpDD0,sizeof(char),23))),_tag_dyneither(_tmpDCF,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp992=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp992->tag != 6)goto _LL45E;else{_tmp993=_tmp992->f1;}}_LL45D:
# 5090
 Cyc_Toc_aggrdecl_to_c(_tmp993,1);
goto _LL44F;_LL45E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp994=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp994->tag != 7)goto _LL460;else{_tmp995=_tmp994->f1;}}_LL45F:
# 5093
 if(_tmp995->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp995);else{
# 5096
Cyc_Toc_datatypedecl_to_c(_tmp995);}
goto _LL44F;_LL460: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp996=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp996->tag != 8)goto _LL462;else{_tmp997=_tmp996->f1;}}_LL461:
# 5099
 Cyc_Toc_enumdecl_to_c(_tmp997);
{struct Cyc_List_List*_tmpDD1;Cyc_Toc_result_decls=((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->hd=d,((_tmpDD1->tl=Cyc_Toc_result_decls,_tmpDD1))))));}
goto _LL44F;_LL462: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp998=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp998->tag != 9)goto _LL464;else{_tmp999=_tmp998->f1;}}_LL463:
# 5103
 _tmp999->name=_tmp999->name;
_tmp999->tvs=0;
if(_tmp999->defn != 0)
_tmp999->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp999->defn));else{
# 5108
enum Cyc_Absyn_KindQual _stmttmp88=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp999->kind))->v)->kind;switch(_stmttmp88){case Cyc_Absyn_BoxKind: _LL470:
 _tmp999->defn=Cyc_Absyn_void_star_typ();break;default: _LL471:
 _tmp999->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5113
{struct Cyc_List_List*_tmpDD2;Cyc_Toc_result_decls=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->hd=d,((_tmpDD2->tl=Cyc_Toc_result_decls,_tmpDD2))))));}
goto _LL44F;_LL464: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp99A=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99A->tag != 14)goto _LL466;}_LL465:
 goto _LL467;_LL466: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp99B=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99B->tag != 15)goto _LL468;}_LL467:
# 5117
 goto _LL44F;_LL468: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp99C=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99C->tag != 10)goto _LL46A;else{_tmp99D=_tmp99C->f2;}}_LL469:
 _tmp99F=_tmp99D;goto _LL46B;_LL46A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp99E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp99E->tag != 11)goto _LL46C;else{_tmp99F=_tmp99E->f2;}}_LL46B:
 _tmp9A1=_tmp99F;goto _LL46D;_LL46C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9A0=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp9A0->tag != 12)goto _LL46E;else{_tmp9A1=_tmp9A0->f1;}}_LL46D:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9A1,top,cinclude);goto _LL44F;_LL46E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9A2=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp989;if(_tmp9A2->tag != 13)goto _LL44F;else{_tmp9A3=_tmp9A2->f1;}}_LL46F:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9A3,top,1);goto _LL44F;_LL44F:;};}
# 5124
return nv;}
# 5128
static void Cyc_Toc_init(){
struct Cyc_Core_NewRegion _stmttmp89=Cyc_Core__new_dynregion("internal-error",0);struct _DynRegionHandle*_tmp9B5;struct Cyc_Core_NewRegion _tmp9B4=_stmttmp89;_tmp9B5=_tmp9B4.dynregion;{
struct _DynRegionFrame _tmp9B6;struct _RegionHandle*d=_open_dynregion(& _tmp9B6,_tmp9B5);{
struct Cyc_Dict_Dict*_tmpDDF;struct Cyc_Dict_Dict*_tmpDDE;struct Cyc_Set_Set**_tmpDDD;struct Cyc_List_List**_tmpDDC;struct Cyc_Dict_Dict*_tmpDDB;struct Cyc_List_List**_tmpDDA;struct Cyc_Toc_TocState*_tmpDD9;Cyc_Toc_toc_state=(
(_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->dyn=(struct _DynRegionHandle*)_tmp9B5,((_tmpDD9->tuple_types=(struct Cyc_List_List**)(
(_tmpDDA=_region_malloc(d,sizeof(*_tmpDDA)),((_tmpDDA[0]=0,_tmpDDA)))),((_tmpDD9->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDDB=_region_malloc(d,sizeof(*_tmpDDB)),((_tmpDDB[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDDB)))),((_tmpDD9->abs_struct_types=(struct Cyc_List_List**)(
(_tmpDDC=_region_malloc(d,sizeof(*_tmpDDC)),((_tmpDDC[0]=0,_tmpDDC)))),((_tmpDD9->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpDDD=_region_malloc(d,sizeof(*_tmpDDD)),((_tmpDDD[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDDD)))),((_tmpDD9->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDDE=_region_malloc(d,sizeof(*_tmpDDE)),((_tmpDDE[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDDE)))),((_tmpDD9->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpDDF=_region_malloc(d,sizeof(*_tmpDDF)),((_tmpDDF[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpDDF)))),((_tmpDD9->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDD9))))))))))))))))));}
# 5142
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDE0;Cyc_Toc_globals=_tag_dyneither(((_tmpDE0=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDE0[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpDE0[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpDE0[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpDE0[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpDE0[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpDE0[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpDE0[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpDE0[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpDE0[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDE0[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpDE0[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpDE0[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpDE0[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpDE0[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDE0[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpDE0[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpDE0[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDE0[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpDE0[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDE0[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDE0[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDE0[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDE0[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpDE0[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpDE0[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpDE0[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpDE0[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpDE0[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpDE0[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpDE0[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpDE0[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpDE0[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpDE0[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpDE0[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpDE0[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpDE0[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpDE0[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpDE0[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDE0[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpDE0[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpDE0[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpDE0)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5130
;_pop_dynregion(d);};}
# 5195
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9BF=_new_region("start");struct _RegionHandle*start=& _tmp9BF;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5204
struct Cyc_Toc_TocState _stmttmp8A=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));struct _DynRegionHandle*_tmp9C1;struct Cyc_Xarray_Xarray*_tmp9C2;struct Cyc_Toc_TocState _tmp9C0=_stmttmp8A;_tmp9C1=_tmp9C0.dyn;_tmp9C2=_tmp9C0.temp_labels;
{struct _DynRegionFrame _tmp9C3;struct _RegionHandle*d=_open_dynregion(& _tmp9C3,_tmp9C1);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9C2);;_pop_dynregion(d);}
# 5208
Cyc_Core_free_dynregion(_tmp9C1);};{
# 5210
struct Cyc_List_List*_tmp9C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9C4;};
# 5197
;_pop_region(start);};}
