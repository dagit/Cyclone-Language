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
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 907
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 945
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 947
extern void*Cyc_Absyn_sint_typ;
# 959
void*Cyc_Absyn_exn_typ();
# 970
extern void*Cyc_Absyn_bounds_one;
# 978
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 986
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 989
void*Cyc_Absyn_void_star_typ();
# 991
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 997
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1104
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1145
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1158
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1162
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1176
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
# 238 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 246
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 248
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 258
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 318
void*Cyc_Tcutil_snd_tqt(struct _tuple10*);
# 347
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 80
typedef union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_reln_op_t;
# 82
union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_RNumelts(struct Cyc_Absyn_Vardecl*);
# 88
enum Cyc_CfFlowInfo_Relation{Cyc_CfFlowInfo_Req  = 0,Cyc_CfFlowInfo_Rneq  = 1,Cyc_CfFlowInfo_Rlte  = 2,Cyc_CfFlowInfo_Rlt  = 3};
typedef enum Cyc_CfFlowInfo_Relation Cyc_CfFlowInfo_relation_t;struct Cyc_CfFlowInfo_Reln{union Cyc_CfFlowInfo_RelnOp rop1;enum Cyc_CfFlowInfo_Relation relation;union Cyc_CfFlowInfo_RelnOp rop2;};
# 95
typedef struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_reln_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 110
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 115
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 122
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 149
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 165 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 184
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
# 228 "cf_flowinfo.h"
struct Cyc_Absyn_Vardecl*Cyc_CfFlowInfo_nonesc_vardecl(void*b);
# 231
int Cyc_CfFlowInfo_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_RelnOp*p);
# 233
struct Cyc_List_List*Cyc_CfFlowInfo_add_relation(struct _RegionHandle*rgn,union Cyc_CfFlowInfo_RelnOp rop1,enum Cyc_CfFlowInfo_Relation r,union Cyc_CfFlowInfo_RelnOp rop2,struct Cyc_List_List*relns);
# 251
int Cyc_CfFlowInfo_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 284
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 65 "toc.cyc"
extern int Cyc_noexpand_r;
# 76 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 85
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};
# 106
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple13{struct _tuple0*f1;struct _tuple0*f2;};
# 108
int Cyc_Toc_qvar_tag_cmp(struct _tuple13*x,struct _tuple13*y){
struct _tuple0*_tmp1;struct _tuple0*_tmp2;struct _tuple13 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
struct _tuple0*_tmp4;struct _tuple0*_tmp5;struct _tuple13 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple14{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 116
void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
{struct _tuple14**v=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 123
void*_tmpC;struct _tuple14*_tmpB=*v;_tmpC=_tmpB->f2;{
void*_tmpD=_tmpC;_npop_handler(0);return _tmpD;};}}
# 119
;_pop_dynregion(d);};}
# 129
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 136
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 139
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9C5;void*_tmp9C4;(_tmp9C4=0,Cyc_fprintf(Cyc_stderr,((_tmp9C5="\n",_tag_dyneither(_tmp9C5,sizeof(char),2))),_tag_dyneither(_tmp9C4,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9C8;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9C7;(int)_throw((void*)((_tmp9C7=_cycalloc_atomic(sizeof(*_tmp9C7)),((_tmp9C7[0]=((_tmp9C8.tag=Cyc_Toc_Toc_Unimplemented,_tmp9C8)),_tmp9C7)))));};}
# 144
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 147
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9CB;void*_tmp9CA;(_tmp9CA=0,Cyc_fprintf(Cyc_stderr,((_tmp9CB="\n",_tag_dyneither(_tmp9CB,sizeof(char),2))),_tag_dyneither(_tmp9CA,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9CE;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9CD;(int)_throw((void*)((_tmp9CD=_cycalloc_atomic(sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9CE.tag=Cyc_Toc_Toc_Impossible,_tmp9CE)),_tmp9CD)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
# 162 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp16,_tmp16,_tmp16 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp17[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp17,_tmp17,_tmp17 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp18[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp18,_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp19[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp19,_tmp19,_tmp19 + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1A[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp1A,_tmp1A,_tmp1A + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1B[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1B,_tmp1B,_tmp1B + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp1C[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp1D[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1D,_tmp1D,_tmp1D + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 175
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp1E[7]="_throw";
# 184 "toc.cyc"
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
# 257
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 260
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 262
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 264
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmp9CF;skip_stmt_opt=((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=Cyc_Absyn_skip_stmt(0),_tmp9CF))));}
return*skip_stmt_opt;}
# 273
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 276
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9D2;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9D1;return(void*)((_tmp9D1=_cycalloc(sizeof(*_tmp9D1)),((_tmp9D1[0]=((_tmp9D2.tag=13,((_tmp9D2.f1=t,((_tmp9D2.f2=e,((_tmp9D2.f3=0,((_tmp9D2.f4=Cyc_Absyn_No_coercion,_tmp9D2)))))))))),_tmp9D1))));}
# 279
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9D5;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9D4;return(void*)((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=19,((_tmp9D5.f1=e,_tmp9D5)))),_tmp9D4))));}
# 282
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9D8;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9D7;return(void*)((_tmp9D7=_cycalloc(sizeof(*_tmp9D7)),((_tmp9D7[0]=((_tmp9D8.tag=22,((_tmp9D8.f1=e1,((_tmp9D8.f2=e2,_tmp9D8)))))),_tmp9D7))));}
# 285
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9DB;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9DA;return(void*)((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=36,((_tmp9DB.f1=s,_tmp9DB)))),_tmp9DA))));}
# 288
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9DE;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9DD;return(void*)((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DE.tag=16,((_tmp9DE.f1=t,_tmp9DE)))),_tmp9DD))));}
# 291
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9E1;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9E0;return(void*)((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=9,((_tmp9E1.f1=e,((_tmp9E1.f2=es,((_tmp9E1.f3=0,((_tmp9E1.f4=1,_tmp9E1)))))))))),_tmp9E0))));}
# 294
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9E4;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9E3;return(void*)((_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=1,((_tmp9E4.f1=e,_tmp9E4)))),_tmp9E3))));}
# 297
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9E7;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9E6;return(void*)((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=2,((_tmp9E7.f1=s1,((_tmp9E7.f2=s2,_tmp9E7)))))),_tmp9E6))));}
# 300
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9EA;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9E9;return(void*)((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((_tmp9E9[0]=((_tmp9EA.tag=5,((_tmp9EA.f1=e1,((_tmp9EA.f2=e2,((_tmp9EA.f3=e3,_tmp9EA)))))))),_tmp9E9))));}
# 303
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9ED;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9EC;return(void*)((_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=20,((_tmp9ED.f1=e,((_tmp9ED.f2=n,((_tmp9ED.f3=0,((_tmp9ED.f4=0,_tmp9ED)))))))))),_tmp9EC))));}
# 306
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp9F0;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp9EF;return(void*)((_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF[0]=((_tmp9F0.tag=21,((_tmp9F0.f1=e,((_tmp9F0.f2=n,((_tmp9F0.f3=0,((_tmp9F0.f4=0,_tmp9F0)))))))))),_tmp9EF))));}
# 309
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 312
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp9F3;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9F2;return(void*)((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=35,((_tmp9F3.f1=tdopt,((_tmp9F3.f2=ds,_tmp9F3)))))),_tmp9F2))));}
# 314
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp9F6;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9F5;return(void*)((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=((_tmp9F6.tag=8,((_tmp9F6.f1=v,((_tmp9F6.f2=s,_tmp9F6)))))),_tmp9F5))));}
# 317
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 321
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpC5=e->r;struct Cyc_Absyn_Exp*_tmpC7;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC5;if(_tmpC6->tag != 19)goto _LL3;else{_tmpC7=_tmpC6->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpC7,f,loc);_LL3:;_LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 338
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 347
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 356
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 365
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 376
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpC8=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpCA;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC8;if(_tmpC9->tag != 6)goto _LL8;else{_tmpCA=_tmpC9->f2;}}_LL7:
# 380
 switch(_tmpCA){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmp9FC;const char*_tmp9FB;struct Cyc_Core_Impossible_exn_struct*_tmp9FA;(int)_throw((void*)((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FC.tag=Cyc_Core_Impossible,((_tmp9FC.f1=((_tmp9FB="impossible IntType (not char, short or int)",_tag_dyneither(_tmp9FB,sizeof(char),44))),_tmp9FC)))),_tmp9FA)))));}}
# 386
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCB->tag != 7)goto _LLA;else{if(_tmpCB->f1 != 0)goto _LLA;}}_LL9:
# 388
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCC->tag != 7)goto _LLC;else{if(_tmpCC->f1 != 1)goto _LLC;}}_LLB:
# 391
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC8;if(_tmpCD->tag != 7)goto _LLE;}_LLD:
# 394
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC8;if(_tmpCE->tag != 5)goto _LL10;}_LLF:
# 397
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 400
struct Cyc_Core_Impossible_exn_struct _tmpA09;const char*_tmpA08;void*_tmpA07[1];struct Cyc_String_pa_PrintArg_struct _tmpA06;struct Cyc_Core_Impossible_exn_struct*_tmpA05;(int)_throw((void*)((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05[0]=((_tmpA09.tag=Cyc_Core_Impossible,((_tmpA09.f1=(struct _dyneither_ptr)((_tmpA06.tag=0,((_tmpA06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA07[0]=& _tmpA06,Cyc_aprintf(((_tmpA08="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA08,sizeof(char),67))),_tag_dyneither(_tmpA07,sizeof(void*),1)))))))),_tmpA09)))),_tmpA05)))));}_LL5:;}
# 402
return function;}
# 404
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 407
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpD7=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpD9;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL1A;else{_tmpD9=(_tmpD8->f1).elt_typ;}}_LL19:
# 410
 return Cyc_Toc_getFunctionType(fS,_tmpD9);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpA0F;const char*_tmpA0E;struct Cyc_Core_Impossible_exn_struct*_tmpA0D;(int)_throw((void*)((_tmpA0D=_cycalloc(sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0F.tag=Cyc_Core_Impossible,((_tmpA0F.f1=((_tmpA0E="impossible type (not pointer)",_tag_dyneither(_tmpA0E,sizeof(char),30))),_tmpA0F)))),_tmpA0D)))));}_LL17:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 418
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpDD=e->r;char _tmpDF;struct _dyneither_ptr _tmpE1;short _tmpE3;int _tmpE5;long long _tmpE7;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF4;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpDE->tag != 0)goto _LL1F;else{if(((_tmpDE->f1).Char_c).tag != 2)goto _LL1F;_tmpDF=((struct _tuple3)((_tmpDE->f1).Char_c).val).f2;}}_LL1E:
 return _tmpDF == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpE0->tag != 0)goto _LL21;else{if(((_tmpE0->f1).Wchar_c).tag != 3)goto _LL21;_tmpE1=(struct _dyneither_ptr)((_tmpE0->f1).Wchar_c).val;}}_LL20: {
# 422
unsigned long _tmpF5=Cyc_strlen((struct _dyneither_ptr)_tmpE1);
int i=0;
if(_tmpF5 >= 2  && *((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),1))== 'x'  && _tmpF5 >= 3) && *((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmpF5;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpE1,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 432
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
# 443
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple15*)_tmpF4->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 451
static int Cyc_Toc_is_nullable(void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpF8;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 5)goto _LL3A;else{_tmpF8=((_tmpF7->f1).ptr_atts).nullable;}}_LL39:
# 454
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8);_LL3A:;_LL3B: {
const char*_tmpA12;void*_tmpA11;(_tmpA11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA12="is_nullable",_tag_dyneither(_tmpA12,sizeof(char),12))),_tag_dyneither(_tmpA11,sizeof(void*),0)));}_LL37:;}
# 460
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
# 473
{struct Cyc_List_List*_tmp102=nms;for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp102->hd));}}{
char*_tmpA14;unsigned int _tmpA13;struct _dyneither_ptr buf=(_tmpA13=len,((_tmpA14=_cyccalloc_atomic(sizeof(char),_tmpA13),_tag_dyneither(_tmpA14,sizeof(char),_tmpA13))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmpA17;char _tmpA16;struct _dyneither_ptr _tmpA15;(_tmpA15=p,((_tmpA16=*((char*)_check_dyneither_subscript(_tmpA15,sizeof(char),0)),((_tmpA17='_',((_get_dyneither_size(_tmpA15,sizeof(char))== 1  && (_tmpA16 == '\000'  && _tmpA17 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA15.curr)=_tmpA17)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 484
{char _tmpA1A;char _tmpA19;struct _dyneither_ptr _tmpA18;(_tmpA18=p,((_tmpA19=*((char*)_check_dyneither_subscript(_tmpA18,sizeof(char),0)),((_tmpA1A='_',((_get_dyneither_size(_tmpA18,sizeof(char))== 1  && (_tmpA19 == '\000'  && _tmpA1A != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA18.curr)=_tmpA1A)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA1B;Cyc_strcpy(p,((_tmpA1B="_struct",_tag_dyneither(_tmpA1B,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 496
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple13 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple13 _tmpA1C;pair=((_tmpA1C.f1=fieldname,((_tmpA1C.f2=dtname,_tmpA1C))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple13*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple13*_tmpA1D;struct _tuple13*_tmp115=(_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D->f1=fieldname,((_tmpA1D->f2=dtname,_tmpA1D)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA20;struct _tuple0*_tmpA1F;struct _tuple0*res=(_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F->f1=_tmp117,((_tmpA1F->f2=((_tmpA20=_cycalloc(sizeof(*_tmpA20)),((_tmpA20[0]=newvar,_tmpA20)))),_tmpA1F)))));
*_tmp110=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple13*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp110,_tmp115,res);{
struct _tuple0*_tmp119=res;_npop_handler(0);return _tmp119;};};};}
# 503
;_pop_dynregion(d);};};}
# 514
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple16{void*f1;struct Cyc_List_List*f2;};
# 518
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct _DynRegionHandle*_tmp11F;struct Cyc_List_List**_tmp120;struct Cyc_Toc_TocState _tmp11E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp11F=_tmp11E.dyn;_tmp120=_tmp11E.tuple_types;{
struct _DynRegionFrame _tmp121;struct _RegionHandle*d=_open_dynregion(& _tmp121,_tmp11F);
# 522
{struct Cyc_List_List*_tmp122=*_tmp120;for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
void*_tmp124;struct Cyc_List_List*_tmp125;struct _tuple16*_tmp123=(struct _tuple16*)_tmp122->hd;_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;{
struct Cyc_List_List*_tmp126=tqs0;
for(0;_tmp126 != 0  && _tmp125 != 0;(_tmp126=_tmp126->tl,_tmp125=_tmp125->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp126->hd)).f2,(void*)_tmp125->hd))
break;}
if(_tmp126 == 0  && _tmp125 == 0){
void*_tmp127=_tmp124;_npop_handler(0);return _tmp127;}};}}{
# 532
struct Cyc_Int_pa_PrintArg_struct _tmpA28;void*_tmpA27[1];const char*_tmpA26;struct _dyneither_ptr*_tmpA25;struct _dyneither_ptr*xname=(_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25[0]=(struct _dyneither_ptr)((_tmpA28.tag=1,((_tmpA28.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA27[0]=& _tmpA28,Cyc_aprintf(((_tmpA26="_tuple%d",_tag_dyneither(_tmpA26,sizeof(char),9))),_tag_dyneither(_tmpA27,sizeof(void*),1)))))))),_tmpA25)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA2B;struct Cyc_List_List*_tmpA2A;_tmp128=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->hd=((_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B->name=Cyc_Absyn_fieldname(i),((_tmpA2B->tq=Cyc_Toc_mt_tq,((_tmpA2B->type=(void*)ts2->hd,((_tmpA2B->width=0,((_tmpA2B->attributes=0,_tmpA2B)))))))))))),((_tmpA2A->tl=_tmp128,_tmpA2A))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA30;struct _tuple0*_tmpA2F;struct Cyc_Absyn_Aggrdecl*_tmpA2E;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((_tmpA2E->kind=Cyc_Absyn_StructA,((_tmpA2E->sc=Cyc_Absyn_Public,((_tmpA2E->name=(
(_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F->f1=Cyc_Absyn_Rel_n(0),((_tmpA2F->f2=xname,_tmpA2F)))))),((_tmpA2E->tvs=0,((_tmpA2E->impl=(
(_tmpA30=_cycalloc(sizeof(*_tmpA30)),((_tmpA30->exist_vars=0,((_tmpA30->rgn_po=0,((_tmpA30->fields=_tmp128,((_tmpA30->tagged=0,_tmpA30)))))))))),((_tmpA2E->attributes=0,_tmpA2E)))))))))))));
# 547
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA36;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA35;struct Cyc_List_List*_tmpA34;Cyc_Toc_result_decls=((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->hd=Cyc_Absyn_new_decl((void*)((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36[0]=((_tmpA35.tag=6,((_tmpA35.f1=_tmp12B,_tmpA35)))),_tmpA36)))),0),((_tmpA34->tl=Cyc_Toc_result_decls,_tmpA34))))));}
{struct _tuple16*_tmpA39;struct Cyc_List_List*_tmpA38;*_tmp120=((_tmpA38=_region_malloc(d,sizeof(*_tmpA38)),((_tmpA38->hd=((_tmpA39=_region_malloc(d,sizeof(*_tmpA39)),((_tmpA39->f1=x,((_tmpA39->f2=ts,_tmpA39)))))),((_tmpA38->tl=*_tmp120,_tmpA38))))));}{
void*_tmp131=x;_npop_handler(0);return _tmp131;};};};
# 522
;_pop_dynregion(d);};}struct _tuple17{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 558
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 564
struct _DynRegionHandle*_tmp13A;struct Cyc_List_List**_tmp13B;struct Cyc_Toc_TocState _tmp139=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp13A=_tmp139.dyn;_tmp13B=_tmp139.abs_struct_types;{
# 567
struct _DynRegionFrame _tmp13C;struct _RegionHandle*d=_open_dynregion(& _tmp13C,_tmp13A);
# 569
{struct Cyc_List_List*_tmp13D=*_tmp13B;for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
struct _tuple0*_tmp13F;struct Cyc_List_List*_tmp140;void*_tmp141;struct _tuple17*_tmp13E=(struct _tuple17*)_tmp13D->hd;_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;
# 572
if(Cyc_Absyn_qvar_cmp(_tmp13F,struct_name)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp140)){
int okay=1;
{struct Cyc_List_List*_tmp142=type_args;for(0;_tmp142 != 0;(_tmp142=_tmp142->tl,_tmp140=_tmp140->tl)){
void*_tmp143=(void*)_tmp142->hd;
void*_tmp144=(void*)((struct Cyc_List_List*)_check_null(_tmp140))->hd;
# 579
{struct Cyc_Absyn_Kind*_tmp145=Cyc_Tcutil_typ_kind(_tmp143);_LL46: if(_tmp145->kind != Cyc_Absyn_EffKind)goto _LL48;_LL47:
 goto _LL49;_LL48: if(_tmp145->kind != Cyc_Absyn_RgnKind)goto _LL4A;_LL49:
# 584
 continue;_LL4A:;_LL4B:
# 587
 if(Cyc_Tcutil_unify(_tmp143,_tmp144) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp143),Cyc_Toc_typ_to_c(_tmp144)))
# 589
continue;
# 592
okay=0;
goto _LL45;_LL45:;}
# 595
break;}}
# 597
if(okay){
# 599
void*_tmp146=_tmp141;_npop_handler(0);return _tmp146;}}}}{
# 607
struct Cyc_Int_pa_PrintArg_struct _tmpA41;void*_tmpA40[1];const char*_tmpA3F;struct _dyneither_ptr*_tmpA3E;struct _dyneither_ptr*xname=(_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=(struct _dyneither_ptr)((_tmpA41.tag=1,((_tmpA41.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA40[0]=& _tmpA41,Cyc_aprintf(((_tmpA3F="_tuple%d",_tag_dyneither(_tmpA3F,sizeof(char),9))),_tag_dyneither(_tmpA40,sizeof(void*),1)))))))),_tmpA3E)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 611
{struct _tuple17*_tmpA44;struct Cyc_List_List*_tmpA43;*_tmp13B=((_tmpA43=_region_malloc(d,sizeof(*_tmpA43)),((_tmpA43->hd=((_tmpA44=_region_malloc(d,sizeof(*_tmpA44)),((_tmpA44->f1=struct_name,((_tmpA44->f2=type_args,((_tmpA44->f3=x,_tmpA44)))))))),((_tmpA43->tl=*_tmp13B,_tmpA43))))));}{
# 614
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 621
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA47;struct Cyc_List_List*_tmpA46;_tmp147=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->hd=((_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47->name=_tmp14C->name,((_tmpA47->tq=Cyc_Toc_mt_tq,((_tmpA47->type=t,((_tmpA47->width=_tmp14C->width,((_tmpA47->attributes=_tmp14C->attributes,_tmpA47)))))))))))),((_tmpA46->tl=_tmp147,_tmpA46))))));};}
# 625
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA4C;struct _tuple0*_tmpA4B;struct Cyc_Absyn_Aggrdecl*_tmpA4A;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->kind=Cyc_Absyn_StructA,((_tmpA4A->sc=Cyc_Absyn_Public,((_tmpA4A->name=(
(_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B->f1=Cyc_Absyn_Rel_n(0),((_tmpA4B->f2=xname,_tmpA4B)))))),((_tmpA4A->tvs=0,((_tmpA4A->impl=(
(_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->exist_vars=0,((_tmpA4C->rgn_po=0,((_tmpA4C->fields=_tmp147,((_tmpA4C->tagged=0,_tmpA4C)))))))))),((_tmpA4A->attributes=0,_tmpA4A)))))))))))));
# 633
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA52;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA51;struct Cyc_List_List*_tmpA50;Cyc_Toc_result_decls=((_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50->hd=Cyc_Absyn_new_decl((void*)((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA51.tag=6,((_tmpA51.f1=_tmp14F,_tmpA51)))),_tmpA52)))),0),((_tmpA50->tl=Cyc_Toc_result_decls,_tmpA50))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 615
;_pop_region(r);};};
# 569
;_pop_dynregion(d);};}
# 642
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA5F;const char*_tmpA5E;void*_tmpA5D[1];struct Cyc_Int_pa_PrintArg_struct _tmpA5C;struct _tuple0*_tmpA5B;struct _tuple0*res=(_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->f1=Cyc_Absyn_Loc_n,((_tmpA5B->f2=((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F[0]=(struct _dyneither_ptr)((_tmpA5C.tag=1,((_tmpA5C.f1=(unsigned int)_tmp15B,((_tmpA5D[0]=& _tmpA5C,Cyc_aprintf(((_tmpA5E="_tmp%X",_tag_dyneither(_tmpA5E,sizeof(char),7))),_tag_dyneither(_tmpA5D,sizeof(void*),1)))))))),_tmpA5F)))),_tmpA5B)))));
return res;}
# 650
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA67;void*_tmpA66[1];const char*_tmpA65;struct _dyneither_ptr*_tmpA64;struct _dyneither_ptr*res=(_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=(struct _dyneither_ptr)((_tmpA67.tag=1,((_tmpA67.f1=(unsigned int)_tmp165,((_tmpA66[0]=& _tmpA67,Cyc_aprintf(((_tmpA65="_LL%X",_tag_dyneither(_tmpA65,sizeof(char),6))),_tag_dyneither(_tmpA66,sizeof(void*),1)))))))),_tmpA64)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA6A;void*_tmpA69;(_tmpA69=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA6A="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA6A,sizeof(char),43))),_tag_dyneither(_tmpA69,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp169=res;_npop_handler(0);return _tmp169;};};}
# 653
;_pop_dynregion(d);};}
# 666
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple0*name){
int ans=0;
struct Cyc_List_List*_tmp16E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp16E))->hd)->name)!= 0){
++ ans;
_tmp16E=_tmp16E->tl;}
# 673
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 679
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){
# 684
struct _dyneither_ptr*_tmp170;struct Cyc_Absyn_Tqual _tmp171;void*_tmp172;struct _tuple8 _tmp16F=*a;_tmp170=_tmp16F.f1;_tmp171=_tmp16F.f2;_tmp172=_tmp16F.f3;{
struct _tuple8*_tmpA6B;return(_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->f1=_tmp170,((_tmpA6B->f2=_tmp171,((_tmpA6B->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA6B)))))));};}
# 687
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA6C;return(_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->f1=_tmp175,((_tmpA6C->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA6C)))));};}
# 706 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_Exp*_tmp17C;union Cyc_Absyn_Constraint*_tmp17D;unsigned int _tmp17E;void*_tmp180;_LL4D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp178;if(_tmp179->tag != 8)goto _LL4F;else{_tmp17A=(_tmp179->f1).elt_type;_tmp17B=(_tmp179->f1).tq;_tmp17C=(_tmp179->f1).num_elts;_tmp17D=(_tmp179->f1).zero_term;_tmp17E=(_tmp179->f1).zt_loc;}}_LL4E:
# 709
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp17A),_tmp17B);_LL4F:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp178;if(_tmp17F->tag != 1)goto _LL51;else{_tmp180=(void*)_tmp17F->f2;}}if(!(_tmp180 != 0))goto _LL51;_LL50:
 return Cyc_Toc_typ_to_c_array(_tmp180);_LL51:;_LL52:
 return Cyc_Toc_typ_to_c(t);_LL4C:;}
# 715
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 717
struct Cyc_Absyn_Aggrfield*_tmpA6D;return(_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->name=f->name,((_tmpA6D->tq=Cyc_Toc_mt_tq,((_tmpA6D->type=
# 719
Cyc_Toc_typ_to_c(f->type),((_tmpA6D->width=f->width,((_tmpA6D->attributes=f->attributes,_tmpA6D)))))))))));}
# 723
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 725
return;}
# 728
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA6E;cs=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA6E))));}
# 733
return*cs;}
# 735
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA6F;r=((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA6F))));}
# 740
return*r;}
# 742
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA70;r=((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA70))));}
# 747
return*r;}
# 749
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp185=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp187;_LL54: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp185;if(_tmp186->tag != 2)goto _LL56;else{_tmp187=_tmp186->f1;}}_LL55:
# 752
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL56:;_LL57:
 return 0;_LL53:;}
# 756
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp188=Cyc_Tcutil_typ_kind(t);_LL59: if(_tmp188->kind != Cyc_Absyn_AnyKind)goto _LL5B;_LL5A:
 return 1;_LL5B:;_LL5C:
 return 0;_LL58:;}
# 763
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp189=t;void**_tmp18C;struct Cyc_Absyn_Tvar*_tmp18E;struct Cyc_Absyn_Datatypedecl*_tmp191;struct Cyc_Absyn_Datatypefield*_tmp192;void*_tmp195;struct Cyc_Absyn_Tqual _tmp196;union Cyc_Absyn_Constraint*_tmp197;void*_tmp19B;struct Cyc_Absyn_Tqual _tmp19C;struct Cyc_Absyn_Exp*_tmp19D;unsigned int _tmp19E;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;struct Cyc_List_List*_tmp1A2;int _tmp1A3;struct Cyc_Absyn_VarargInfo*_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;enum Cyc_Absyn_AggrKind _tmp1A9;struct Cyc_List_List*_tmp1AA;union Cyc_Absyn_AggrInfoU _tmp1AC;struct Cyc_List_List*_tmp1AD;struct _tuple0*_tmp1AF;struct Cyc_List_List*_tmp1B1;struct _tuple0*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Typedefdecl*_tmp1B5;void*_tmp1B6;void*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Aggrdecl*_tmp1C5;struct Cyc_Absyn_Enumdecl*_tmp1C8;struct Cyc_Absyn_Datatypedecl*_tmp1CB;void**_tmp1CC;_LL5E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp189;if(_tmp18A->tag != 0)goto _LL60;}_LL5F:
 return t;_LL60: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp189;if(_tmp18B->tag != 1)goto _LL62;else{_tmp18C=(void**)((void**)& _tmp18B->f2);}}_LL61:
# 767
 if(*_tmp18C == 0){
*_tmp18C=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 771
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp18C));_LL62: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp189;if(_tmp18D->tag != 2)goto _LL64;else{_tmp18E=_tmp18D->f1;}}_LL63:
# 773
 if((Cyc_Tcutil_tvar_kind(_tmp18E,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 776
return(void*)& Cyc_Absyn_VoidType_val;else{
# 778
return Cyc_Absyn_void_star_typ();}_LL64: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp18F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp189;if(_tmp18F->tag != 3)goto _LL66;}_LL65:
# 780
 return(void*)& Cyc_Absyn_VoidType_val;_LL66: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp190->tag != 4)goto _LL68;else{if((((_tmp190->f1).field_info).KnownDatatypefield).tag != 2)goto _LL68;_tmp191=((struct _tuple1)(((_tmp190->f1).field_info).KnownDatatypefield).val).f1;_tmp192=((struct _tuple1)(((_tmp190->f1).field_info).KnownDatatypefield).val).f2;}}_LL67:
# 782
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp192->name,_tmp191->name));_LL68: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp189;if(_tmp193->tag != 4)goto _LL6A;}_LL69: {
const char*_tmpA73;void*_tmpA72;(_tmpA72=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA73="unresolved DatatypeFieldType",_tag_dyneither(_tmpA73,sizeof(char),29))),_tag_dyneither(_tmpA72,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
# 787
 _tmp195=Cyc_Toc_typ_to_c(_tmp195);{
void*_tmp1CF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp197);_LL9B: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1D0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1CF;if(_tmp1D0->tag != 0)goto _LL9D;}_LL9C:
 return Cyc_Toc_dyneither_ptr_typ;_LL9D:;_LL9E:
 return Cyc_Absyn_star_typ(_tmp195,(void*)& Cyc_Absyn_HeapRgn_val,_tmp196,Cyc_Absyn_false_conref);_LL9A:;};_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp189;if(_tmp198->tag != 6)goto _LL6E;}_LL6D:
# 792
 goto _LL6F;_LL6E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp189;if(_tmp199->tag != 7)goto _LL70;}_LL6F:
 return t;_LL70: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp19A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp189;if(_tmp19A->tag != 8)goto _LL72;else{_tmp19B=(_tmp19A->f1).elt_type;_tmp19C=(_tmp19A->f1).tq;_tmp19D=(_tmp19A->f1).num_elts;_tmp19E=(_tmp19A->f1).zt_loc;}}_LL71:
# 795
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp19B),_tmp19C,_tmp19D,Cyc_Absyn_false_conref,_tmp19E);_LL72: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189;if(_tmp19F->tag != 9)goto _LL74;else{_tmp1A0=(_tmp19F->f1).ret_tqual;_tmp1A1=(_tmp19F->f1).ret_typ;_tmp1A2=(_tmp19F->f1).args;_tmp1A3=(_tmp19F->f1).c_varargs;_tmp1A4=(_tmp19F->f1).cyc_varargs;_tmp1A5=(_tmp19F->f1).attributes;}}_LL73: {
# 801
struct Cyc_List_List*_tmp1D1=0;
for(0;_tmp1A5 != 0;_tmp1A5=_tmp1A5->tl){
void*_tmp1D2=(void*)_tmp1A5->hd;_LLA0: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1D3=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D3->tag != 4)goto _LLA2;}_LLA1:
 goto _LLA3;_LLA2: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1D4=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D4->tag != 5)goto _LLA4;}_LLA3:
 goto _LLA5;_LLA4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1D5=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D5->tag != 19)goto _LLA6;}_LLA5:
 continue;_LLA6: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1D6=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D6->tag != 22)goto _LLA8;}_LLA7:
 continue;_LLA8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1D7=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D7->tag != 21)goto _LLAA;}_LLA9:
 continue;_LLAA: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1D8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1D2;if(_tmp1D8->tag != 20)goto _LLAC;}_LLAB:
 continue;_LLAC:;_LLAD:
{struct Cyc_List_List*_tmpA74;_tmp1D1=((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->hd=(void*)_tmp1A5->hd,((_tmpA74->tl=_tmp1D1,_tmpA74))))));}goto _LL9F;_LL9F:;}{
# 812
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 815
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA75;struct _tuple8*_tmp1DC=(_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->f1=_tmp1A4->name,((_tmpA75->f2=_tmp1A4->tq,((_tmpA75->f3=_tmp1DB,_tmpA75)))))));
struct Cyc_List_List*_tmpA76;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=_tmp1DC,((_tmpA76->tl=0,_tmpA76)))))));}{
# 819
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA7C;struct Cyc_Absyn_FnInfo _tmpA7B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA7A;return(void*)((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA7C.tag=9,((_tmpA7C.f1=((_tmpA7B.tvars=0,((_tmpA7B.effect=0,((_tmpA7B.ret_tqual=_tmp1A0,((_tmpA7B.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA7B.args=_tmp1DA,((_tmpA7B.c_varargs=_tmp1A3,((_tmpA7B.cyc_varargs=0,((_tmpA7B.rgn_po=0,((_tmpA7B.attributes=_tmp1D1,_tmpA7B)))))))))))))))))),_tmpA7C)))),_tmpA7A))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 824
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 829
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA7F;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA7E;return(void*)((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=12,((_tmpA7F.f1=_tmp1A9,((_tmpA7F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA7F)))))),_tmpA7E))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
# 833
{union Cyc_Absyn_AggrInfoU _tmp1E4=_tmp1AC;_LLAF: if((_tmp1E4.UnknownAggr).tag != 1)goto _LLB1;_LLB0:
 return t;_LLB1:;_LLB2:
 goto _LLAE;_LLAE:;}{
# 837
struct Cyc_Absyn_Aggrdecl*_tmp1E5=Cyc_Absyn_get_known_aggrdecl(_tmp1AC);
# 841
if(_tmp1E5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp1E6=_tmp1E5->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;
if(_tmp1E6 == 0)return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);
for(0;_tmp1E6->tl != 0;_tmp1E6=_tmp1E6->tl){;}{
void*_tmp1E7=((struct Cyc_Absyn_Aggrfield*)_tmp1E6->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp1E7))){
# 851
struct _RegionHandle _tmp1E8=_new_region("r");struct _RegionHandle*r=& _tmp1E8;_push_region(r);
{struct Cyc_List_List*_tmp1E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp1E5->tvs,_tmp1AD);
void*_tmp1EA=Cyc_Tcutil_rsubstitute(r,_tmp1E9,_tmp1E7);
if(Cyc_Toc_is_abstract_type(_tmp1EA)){void*_tmp1EB=Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp1EB;}{
void*_tmp1EC=Cyc_Toc_add_struct_type(_tmp1E5->name,_tmp1E5->tvs,_tmp1AD,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields);_npop_handler(0);return _tmp1EC;};}
# 852
;_pop_region(r);}
# 857
return Cyc_Toc_aggrdecl_type(_tmp1E5->name,Cyc_Absyn_strctq);};};};_LL7A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp189;if(_tmp1AE->tag != 13)goto _LL7C;else{_tmp1AF=_tmp1AE->f1;}}_LL7B:
 return t;_LL7C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1B0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp189;if(_tmp1B0->tag != 14)goto _LL7E;else{_tmp1B1=_tmp1B0->f1;}}_LL7D:
 Cyc_Toc_enumfields_to_c(_tmp1B1);return t;_LL7E: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp189;if(_tmp1B2->tag != 17)goto _LL80;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;_tmp1B5=_tmp1B2->f3;_tmp1B6=(void*)_tmp1B2->f4;}}_LL7F:
# 861
 if(_tmp1B6 == 0  || Cyc_noexpand_r){
if(_tmp1B4 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA82;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA81;return(void*)((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=17,((_tmpA82.f1=_tmp1B3,((_tmpA82.f2=0,((_tmpA82.f3=_tmp1B5,((_tmpA82.f4=0,_tmpA82)))))))))),_tmpA81))));}else{
return t;}}else{
# 866
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA85;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA84;return(void*)((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=17,((_tmpA85.f1=_tmp1B3,((_tmpA85.f2=0,((_tmpA85.f3=_tmp1B5,((_tmpA85.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA85)))))))))),_tmpA84))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
 return Cyc_Absyn_uint_typ;_LL82: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp189;if(_tmp1B8->tag != 15)goto _LL84;else{_tmp1B9=(void*)_tmp1B8->f1;}}_LL83:
 return Cyc_Toc_rgn_typ();_LL84: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1BA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp189;if(_tmp1BA->tag != 16)goto _LL86;}_LL85:
 return Cyc_Toc_dyn_rgn_typ();_LL86: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BB->tag != 20)goto _LL88;}_LL87:
# 872
 goto _LL89;_LL88: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BC->tag != 21)goto _LL8A;}_LL89:
 goto _LL8B;_LL8A: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp189;if(_tmp1BD->tag != 22)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp189;if(_tmp1BE->tag != 23)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1BF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp189;if(_tmp1BF->tag != 24)goto _LL90;}_LL8F:
 goto _LL91;_LL90: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp189;if(_tmp1C0->tag != 25)goto _LL92;}_LL91:
 return Cyc_Absyn_void_star_typ();_LL92: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp189;if(_tmp1C1->tag != 18)goto _LL94;else{_tmp1C2=_tmp1C1->f1;}}_LL93:
# 882
 return t;_LL94: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C3->tag != 26)goto _LL96;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1C4=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1C3->f1)->r;if(_tmp1C4->tag != 0)goto _LL96;else{_tmp1C5=_tmp1C4->f1;}}}_LL95:
# 884
 Cyc_Toc_aggrdecl_to_c(_tmp1C5,1);
if(_tmp1C5->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1C5->name,Cyc_Absyn_strctq);}_LL96: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C6->tag != 26)goto _LL98;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1C7=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1C6->f1)->r;if(_tmp1C7->tag != 1)goto _LL98;else{_tmp1C8=_tmp1C7->f1;}}}_LL97:
# 889
 Cyc_Toc_enumdecl_to_c(_tmp1C8);
return t;_LL98: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp189;if(_tmp1C9->tag != 26)goto _LL5D;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1CA=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1C9->f1)->r;if(_tmp1CA->tag != 2)goto _LL5D;else{_tmp1CB=_tmp1CA->f1;}}_tmp1CC=_tmp1C9->f2;}}_LL99:
# 892
 Cyc_Toc_datatypedecl_to_c(_tmp1CB);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1CC)));_LL5D:;}
# 897
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp1F1=t;void*_tmp1F3;struct Cyc_Absyn_Tqual _tmp1F4;_LLB4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1F2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1F1;if(_tmp1F2->tag != 8)goto _LLB6;else{_tmp1F3=(_tmp1F2->f1).elt_type;_tmp1F4=(_tmp1F2->f1).tq;}}_LLB5:
# 900
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1F3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1F4,Cyc_Absyn_false_conref),e);_LLB6:;_LLB7:
 return Cyc_Toc_cast_it(t,e);_LLB3:;}
# 907
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
# 924
{union Cyc_Absyn_AggrInfoU _tmp20D=_tmp201;_LLDE: if((_tmp20D.UnknownAggr).tag != 1)goto _LLE0;_LLDF:
 return 0;_LLE0:;_LLE1:
 goto _LLDD;_LLDD:;}{
# 928
struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(_tmp201);
if(_tmp20E->impl == 0)
return 0;
{struct Cyc_List_List*_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->fields;for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type))return 0;}}
return 1;};_LLCD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp202->tag != 12)goto _LLCF;else{_tmp203=_tmp202->f2;}}_LLCE:
# 935
 for(0;_tmp203 != 0;_tmp203=_tmp203->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp203->hd)->type))return 0;}
return 1;_LLCF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1F5;if(_tmp204->tag != 4)goto _LLD1;else{if((((_tmp204->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD1;_tmp205=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f1;_tmp206=((struct _tuple1)(((_tmp204->f1).field_info).KnownDatatypefield).val).f2;}}_LLD0:
# 939
 _tmp208=_tmp206->typs;goto _LLD2;_LLD1: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1F5;if(_tmp207->tag != 10)goto _LLD3;else{_tmp208=_tmp207->f1;}}_LLD2:
# 941
 for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp208->hd)).f2))return 0;}
return 1;_LLD3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1F5;if(_tmp209->tag != 3)goto _LLD5;}_LLD4:
# 946
 goto _LLD6;_LLD5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F5;if(_tmp20A->tag != 5)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1F5;if(_tmp20B->tag != 16)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F5;if(_tmp20C->tag != 15)goto _LLDB;}_LLDA:
 return 0;_LLDB:;_LLDC: {
const char*_tmpA89;void*_tmpA88[1];struct Cyc_String_pa_PrintArg_struct _tmpA87;(_tmpA87.tag=0,((_tmpA87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA88[0]=& _tmpA87,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA89="atomic_typ:  bad type %s",_tag_dyneither(_tmpA89,sizeof(char),25))),_tag_dyneither(_tmpA88,sizeof(void*),1)))))));}_LLB8:;}
# 954
static int Cyc_Toc_is_void_star(void*t){
void*_tmp213=Cyc_Tcutil_compress(t);void*_tmp215;_LLE3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp213;if(_tmp214->tag != 5)goto _LLE5;else{_tmp215=(_tmp214->f1).elt_typ;}}_LLE4: {
# 957
void*_tmp216=Cyc_Tcutil_compress(_tmp215);_LLE8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp216;if(_tmp217->tag != 0)goto _LLEA;}_LLE9:
 return 1;_LLEA:;_LLEB:
 return 0;_LLE7:;}_LLE5:;_LLE6:
# 961
 return 0;_LLE2:;}
# 965
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 969
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 974
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp218=Cyc_Tcutil_compress(t);union Cyc_Absyn_AggrInfoU _tmp21A;struct Cyc_List_List*_tmp21C;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp218;if(_tmp219->tag != 11)goto _LLEF;else{_tmp21A=(_tmp219->f1).aggr_info;}}_LLEE: {
# 977
struct Cyc_Absyn_Aggrdecl*_tmp21D=Cyc_Absyn_get_known_aggrdecl(_tmp21A);
if(_tmp21D->impl == 0){
const char*_tmpA8C;void*_tmpA8B;(_tmpA8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8C="is_poly_field: type missing fields",_tag_dyneither(_tmpA8C,sizeof(char),35))),_tag_dyneither(_tmpA8B,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 982
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpA90;void*_tmpA8F[1];struct Cyc_String_pa_PrintArg_struct _tmpA8E;(_tmpA8E.tag=0,((_tmpA8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpA8F[0]=& _tmpA8E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA90="is_poly_field: bad field %s",_tag_dyneither(_tmpA90,sizeof(char),28))),_tag_dyneither(_tmpA8F,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpA94;void*_tmpA93[1];struct Cyc_String_pa_PrintArg_struct _tmpA92;(_tmpA92.tag=0,((_tmpA92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA93[0]=& _tmpA92,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA94="is_poly_field: bad type %s",_tag_dyneither(_tmpA94,sizeof(char),27))),_tag_dyneither(_tmpA93,sizeof(void*),1)))))));}_LLEC:;}
# 993
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp227=e->r;struct Cyc_Absyn_Exp*_tmp229;struct _dyneither_ptr*_tmp22A;struct Cyc_Absyn_Exp*_tmp22C;struct _dyneither_ptr*_tmp22D;_LLF4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp228->tag != 20)goto _LLF6;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLF5:
# 996
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp229->topt),_tmp22A) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLF6: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp227;if(_tmp22B->tag != 21)goto _LLF8;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22B->f2;}}_LLF7: {
# 999
void*_tmp22E=Cyc_Tcutil_compress((void*)_check_null(_tmp22C->topt));void*_tmp230;_LLFB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 5)goto _LLFD;else{_tmp230=(_tmp22F->f1).elt_typ;}}_LLFC:
# 1001
 return Cyc_Toc_is_poly_field(_tmp230,_tmp22D) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFD:;_LLFE: {
const char*_tmpA98;void*_tmpA97[1];struct Cyc_String_pa_PrintArg_struct _tmpA96;(_tmpA96.tag=0,((_tmpA96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpA97[0]=& _tmpA96,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA98="is_poly_project: bad type %s",_tag_dyneither(_tmpA98,sizeof(char),29))),_tag_dyneither(_tmpA97,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1004
 return 0;_LLF3:;}
# 1009
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA99;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->hd=s,((_tmpA99->tl=0,_tmpA99)))))),0);}
# 1013
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpA9A;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->hd=s,((_tmpA9A->tl=0,_tmpA9A)))))),0);}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpA9B[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpA9B[1]=s,((_tmpA9B[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpA9C[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpA9C[1]=n,((_tmpA9C[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1031
struct Cyc_Absyn_Exp*_tmpA9D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpA9D[1]=n,((_tmpA9D[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpA9E[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpA9E[2]=n,((_tmpA9E[1]=s,((_tmpA9E[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpA9F;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->hd=e,((_tmpA9F->tl=0,_tmpA9F)))))),0);}
# 1043
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpAA0[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAA0,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1049
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1057
int is_string=0;
{void*_tmp23C=e->r;_LL100: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23D->tag != 0)goto _LL102;else{if(((_tmp23D->f1).String_c).tag != 8)goto _LL102;}}_LL101:
 is_string=1;goto _LLFF;_LL102: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp23C;if(_tmp23E->tag != 0)goto _LL104;else{if(((_tmp23E->f1).Wstring_c).tag != 9)goto _LL104;}}_LL103:
 is_string=1;goto _LLFF;_LL104:;_LL105:
 goto _LLFF;_LLFF:;}{
# 1063
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple0*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAA6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAA5;struct Cyc_List_List*_tmpAA4;Cyc_Toc_result_decls=((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->hd=Cyc_Absyn_new_decl((void*)((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6[0]=((_tmpAA5.tag=0,((_tmpAA5.f1=vd,_tmpAA5)))),_tmpAA6)))),0),((_tmpAA4->tl=Cyc_Toc_result_decls,_tmpAA4))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1073
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1075
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1077
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple15*_tmpAAD;struct _tuple15*_tmpAAC;struct _tuple15*_tmpAAB;struct _tuple15*_tmpAAA[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpAAA[2]=(
# 1080
(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->f1=0,((_tmpAAB->f2=xplussz,_tmpAAB)))))),((_tmpAAA[1]=(
# 1079
(_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->f1=0,((_tmpAAC->f2=xexp,_tmpAAC)))))),((_tmpAAA[0]=(
# 1078
(_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->f1=0,((_tmpAAD->f2=xexp,_tmpAAD)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAAA,sizeof(struct _tuple15*),3)))))))),0);}
# 1081
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1124 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1126
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp247;struct Cyc_Toc_Env*_tmp246=nv;_tmp247=_tmp246->toplevel;
return _tmp247;}
# 1130
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp249;struct Cyc_Toc_Env*_tmp248=nv;_tmp249=_tmp248->in_lhs;
return*_tmp249;}
# 1135
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
struct Cyc_Dict_Dict _tmp24B;struct Cyc_Toc_Env*_tmp24A=nv;_tmp24B=_tmp24A->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp24B,x);}
# 1141
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpAB5;struct Cyc_Absyn_Exp**_tmpAB4;int*_tmpAB3;struct Cyc_Toc_Env*_tmpAB2;return(_tmpAB2=_region_malloc(r,sizeof(*_tmpAB2)),((_tmpAB2->break_lab=(struct _dyneither_ptr**)0,((_tmpAB2->continue_lab=(struct _dyneither_ptr**)0,((_tmpAB2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAB2->varmap=(struct Cyc_Dict_Dict)
# 1145
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpAB2->toplevel=(int)1,((_tmpAB2->in_lhs=(int*)(
# 1147
(_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((_tmpAB3[0]=0,_tmpAB3)))),((_tmpAB2->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpAB2->struct_info).varsizeexp=((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=0,_tmpAB4)))),(((_tmpAB2->struct_info).lhs_exp=0,_tmpAB2->struct_info)))),((_tmpAB2->in_sizeof=(int*)(
(_tmpAB5=_region_malloc(r,sizeof(*_tmpAB5)),((_tmpAB5[0]=0,_tmpAB5)))),((_tmpAB2->rgn=(struct _RegionHandle*)r,_tmpAB2)))))))))))))))))));}
# 1153
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB6;return(_tmpAB6=_region_malloc(r,sizeof(*_tmpAB6)),((_tmpAB6->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpAB6->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpAB6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpAB6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpAB6->toplevel=(int)_tmp255,((_tmpAB6->in_lhs=(int*)_tmp256,((_tmpAB6->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpAB6->in_sizeof=(int*)_tmp258,((_tmpAB6->rgn=(struct _RegionHandle*)r,_tmpAB6)))))))))))))))))));};}
# 1158
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB7;return(_tmpAB7=_region_malloc(r,sizeof(*_tmpAB7)),((_tmpAB7->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpAB7->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpAB7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpAB7->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpAB7->toplevel=(int)0,((_tmpAB7->in_lhs=(int*)_tmp260,((_tmpAB7->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpAB7->in_sizeof=(int*)_tmp262,((_tmpAB7->rgn=(struct _RegionHandle*)r,_tmpAB7)))))))))))))))))));};}
# 1162
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB8;return(_tmpAB8=_region_malloc(r,sizeof(*_tmpAB8)),((_tmpAB8->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpAB8->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpAB8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpAB8->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpAB8->toplevel=(int)1,((_tmpAB8->in_lhs=(int*)_tmp26A,((_tmpAB8->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpAB8->in_sizeof=(int*)_tmp26C,((_tmpAB8->rgn=(struct _RegionHandle*)r,_tmpAB8)))))))))))))))))));};}
# 1166
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp26F;struct Cyc_Toc_Env*_tmp26E=e;_tmp26F=_tmp26E->in_lhs;
*_tmp26F=b;}
# 1170
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp271;struct Cyc_Toc_Env*_tmp270=e;_tmp271=_tmp270->in_sizeof;{
int _tmp272=*_tmp271;
*_tmp271=b;
return _tmp272;};}
# 1176
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp274;struct Cyc_Toc_Env*_tmp273=e;_tmp274=_tmp273->in_sizeof;
return*_tmp274;}
# 1181
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1183
struct _dyneither_ptr**_tmp276;struct _dyneither_ptr**_tmp277;struct Cyc_Toc_FallthruInfo*_tmp278;struct Cyc_Dict_Dict _tmp279;int _tmp27A;int*_tmp27B;struct Cyc_Absyn_Exp**_tmp27C;int*_tmp27D;struct Cyc_Toc_Env*_tmp275=e;_tmp276=_tmp275->break_lab;_tmp277=_tmp275->continue_lab;_tmp278=_tmp275->fallthru_info;_tmp279=_tmp275->varmap;_tmp27A=_tmp275->toplevel;_tmp27B=_tmp275->in_lhs;_tmp27C=(_tmp275->struct_info).varsizeexp;_tmp27D=_tmp275->in_sizeof;{
struct Cyc_Toc_Env*_tmpAB9;return(_tmpAB9=_region_malloc(r,sizeof(*_tmpAB9)),((_tmpAB9->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpAB9->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpAB9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpAB9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpAB9->toplevel=(int)_tmp27A,((_tmpAB9->in_lhs=(int*)_tmp27B,((_tmpAB9->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpAB9->struct_info).varsizeexp=_tmp27C,(((_tmpAB9->struct_info).lhs_exp=exp,_tmpAB9->struct_info)))),((_tmpAB9->in_sizeof=(int*)_tmp27D,((_tmpAB9->rgn=(struct _RegionHandle*)r,_tmpAB9)))))))))))))))))));};}
# 1189
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp27F=(*x).f1;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1192
const char*_tmpABD;void*_tmpABC[1];struct Cyc_String_pa_PrintArg_struct _tmpABB;(_tmpABB.tag=0,((_tmpABB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpABC[0]=& _tmpABB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpABD="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpABD,sizeof(char),30))),_tag_dyneither(_tmpABC,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1195
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpABE;return(_tmpABE=_region_malloc(r,sizeof(*_tmpABE)),((_tmpABE->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpABE->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpABE->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpABE->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpABE->toplevel=(int)_tmp288,((_tmpABE->in_lhs=(int*)_tmp289,((_tmpABE->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpABE->in_sizeof=(int*)_tmp28B,((_tmpABE->rgn=(struct _RegionHandle*)r,_tmpABE)))))))))))))))))));};};}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpABF;return(_tmpABF=_region_malloc(r,sizeof(*_tmpABF)),((_tmpABF->break_lab=(struct _dyneither_ptr**)0,((_tmpABF->continue_lab=(struct _dyneither_ptr**)0,((_tmpABF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpABF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpABF->toplevel=(int)_tmp293,((_tmpABF->in_lhs=(int*)_tmp294,((_tmpABF->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpABF->in_sizeof=(int*)_tmp296,((_tmpABF->rgn=(struct _RegionHandle*)r,_tmpABF)))))))))))))))))));};}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1213
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpAC0;fallthru_vars=((_tmpAC0=_region_malloc(r,sizeof(*_tmpAC0)),((_tmpAC0->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpAC0->tl=fallthru_vars,_tmpAC0))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpAC1;struct Cyc_Toc_FallthruInfo*fi=
(_tmpAC1=_region_malloc(r,sizeof(*_tmpAC1)),((_tmpAC1->label=fallthru_l,((_tmpAC1->binders=fallthru_vars,((_tmpAC1->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpAC1)))))));
struct _dyneither_ptr**_tmpAC4;struct Cyc_Toc_Env*_tmpAC3;return(_tmpAC3=_region_malloc(r,sizeof(*_tmpAC3)),((_tmpAC3->break_lab=(struct _dyneither_ptr**)((_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4[0]=break_l,_tmpAC4)))),((_tmpAC3->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpAC3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAC3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpAC3->toplevel=(int)_tmp29E,((_tmpAC3->in_lhs=(int*)_tmp29F,((_tmpAC3->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpAC3->in_sizeof=(int*)_tmp2A1,((_tmpAC3->rgn=(struct _RegionHandle*)r,_tmpAC3)))))))))))))))))));};};};}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1228
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpAC7;struct Cyc_Toc_Env*_tmpAC6;return(_tmpAC6=_region_malloc(r,sizeof(*_tmpAC6)),((_tmpAC6->break_lab=(struct _dyneither_ptr**)((_tmpAC7=_region_malloc(r,sizeof(*_tmpAC7)),((_tmpAC7[0]=break_l,_tmpAC7)))),((_tmpAC6->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpAC6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAC6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpAC6->toplevel=(int)_tmp2AC,((_tmpAC6->in_lhs=(int*)_tmp2AD,((_tmpAC6->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpAC6->in_sizeof=(int*)_tmp2AF,((_tmpAC6->rgn=(struct _RegionHandle*)r,_tmpAC6)))))))))))))))))));};}
# 1235
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1238
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpACA;struct Cyc_Toc_Env*_tmpAC9;return(_tmpAC9=_region_malloc(r,sizeof(*_tmpAC9)),((_tmpAC9->break_lab=(struct _dyneither_ptr**)0,((_tmpAC9->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpAC9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpACA=_region_malloc(r,sizeof(*_tmpACA)),((_tmpACA->label=next_l,((_tmpACA->binders=0,((_tmpACA->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpACA)))))))),((_tmpAC9->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpAC9->toplevel=(int)_tmp2B7,((_tmpAC9->in_lhs=(int*)_tmp2B8,((_tmpAC9->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpAC9->in_sizeof=(int*)_tmp2BA,((_tmpAC9->rgn=(struct _RegionHandle*)r,_tmpAC9)))))))))))))))))));};}
# 1253 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1256
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2BD=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1261
{const char*_tmpACD;void*_tmpACC;(_tmpACC=0,Cyc_Tcutil_terr(e->loc,((_tmpACD="dereference of NULL pointer",_tag_dyneither(_tmpACD,sizeof(char),28))),_tag_dyneither(_tmpACC,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1266
 return 1;_LL114:;_LL115: {
const char*_tmpAD0;void*_tmpACF;(_tmpACF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD0="need_null_check",_tag_dyneither(_tmpAD0,sizeof(char),16))),_tag_dyneither(_tmpACF,sizeof(void*),0)));}_LL10B:;}
# 1271
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2C6=e->annot;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1276
{const char*_tmpAD3;void*_tmpAD2;(_tmpAD2=0,Cyc_Tcutil_terr(e->loc,((_tmpAD3="dereference of NULL pointer",_tag_dyneither(_tmpAD3,sizeof(char),28))),_tag_dyneither(_tmpAD2,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAD6;void*_tmpAD5;(_tmpAD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD6="get_relns",_tag_dyneither(_tmpAD6,sizeof(char),10))),_tag_dyneither(_tmpAD5,sizeof(void*),0)));}_LL116:;}static char _tmp2D5[8]="*bogus*";
# 1286
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1296 "toc.cyc"
union Cyc_CfFlowInfo_RelnOp rop_i=Cyc_CfFlowInfo_RConst(0);
int valid_rop_i=Cyc_CfFlowInfo_exp2relnop(i,& rop_i);
# 1299
struct Cyc_Absyn_Vardecl*x;
{void*_tmp2D1=a->r;void*_tmp2D3;_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2D2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2D1;if(_tmp2D2->tag != 1)goto _LL124;else{_tmp2D3=(void*)_tmp2D2->f2;}}_LL123: {
# 1302
struct Cyc_Absyn_Vardecl*_tmp2D4=Cyc_CfFlowInfo_nonesc_vardecl(_tmp2D3);
if(_tmp2D4 == 0)goto _LL125;
x=_tmp2D4;
goto _LL121;}_LL124:;_LL125: {
# 1308
static struct _dyneither_ptr bogus_string={_tmp2D5,_tmp2D5,_tmp2D5 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1311
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1317
x=& bogus_vardecl;
x->type=a_typ;}_LL121:;}{
# 1320
void*_tmp2D6=a_typ;
union Cyc_CfFlowInfo_RelnOp rop_a=Cyc_CfFlowInfo_RNumelts(x);
# 1325
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp2D6);
if(bound != 0){
union Cyc_CfFlowInfo_RelnOp rop_bound=Cyc_CfFlowInfo_RConst(0);
if(Cyc_CfFlowInfo_exp2relnop(bound,& rop_bound))
relns=Cyc_CfFlowInfo_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_CfFlowInfo_Rlte,rop_a,relns);}
# 1334
if(valid_rop_i){
struct Cyc_List_List*_tmp2D7=Cyc_CfFlowInfo_add_relation(Cyc_Core_heap_region,rop_a,Cyc_CfFlowInfo_Rlte,rop_i,relns);
# 1341
if(!Cyc_CfFlowInfo_consistent_relations(_tmp2D7))return 1;}
# 1344
inner_loop: {
void*_tmp2D8=i->r;void*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DB;struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;_LL127: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2D9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8;if(_tmp2D9->tag != 13)goto _LL129;else{_tmp2DA=(void*)_tmp2D9->f1;_tmp2DB=_tmp2D9->f2;}}_LL128:
 i=_tmp2DB;goto inner_loop;_LL129: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2DC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2D8;if(_tmp2DC->tag != 2)goto _LL12B;else{if(_tmp2DC->f1 != Cyc_Absyn_Mod)goto _LL12B;if(_tmp2DC->f2 == 0)goto _LL12B;_tmp2DD=(struct Cyc_Absyn_Exp*)(_tmp2DC->f2)->hd;if((_tmp2DC->f2)->tl == 0)goto _LL12B;_tmp2DE=(struct Cyc_Absyn_Exp*)((_tmp2DC->f2)->tl)->hd;}}_LL12A: {
# 1350
union Cyc_CfFlowInfo_RelnOp rop_z=Cyc_CfFlowInfo_RConst(0);
if(Cyc_CfFlowInfo_exp2relnop(_tmp2DE,& rop_z)){
# 1353
struct Cyc_List_List*_tmp2DF=Cyc_CfFlowInfo_add_relation(Cyc_Core_heap_region,rop_a,Cyc_CfFlowInfo_Rlt,rop_z,relns);
# 1355
return !Cyc_CfFlowInfo_consistent_relations(_tmp2DF);}
# 1357
goto _LL126;}_LL12B:;_LL12C:
 goto _LL126;_LL126:;}
# 1360
return 0;};}
# 1363
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAD9;void*_tmpAD8;(_tmpAD8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD9="Missing type in primop ",_tag_dyneither(_tmpAD9,sizeof(char),24))),_tag_dyneither(_tmpAD8,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1367
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpADC;void*_tmpADB;(_tmpADB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADC="Missing type in primop ",_tag_dyneither(_tmpADC,sizeof(char),24))),_tag_dyneither(_tmpADB,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1371
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpADD;return(_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->f1=Cyc_Toc_mt_tq,((_tmpADD->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpADD)))));}
# 1374
static struct _tuple15*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple15*_tmpADE;return(_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->f1=0,((_tmpADE->f2=e,_tmpADE)))));};}
# 1381
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1384
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp2E6=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp2E6=Cyc_Absyn_add_exp(_tmp2E6,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1392
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp2E6): Cyc_Toc_malloc_ptr(_tmp2E6);else{
# 1396
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp2E6);}}else{
# 1401
t=struct_typ;
eo=0;}
# 1404
if(do_declare){
void*_tmp2E7=x->r;struct _tuple0*_tmp2E9;_LL12E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E7;if(_tmp2E8->tag != 1)goto _LL130;else{_tmp2E9=_tmp2E8->f1;}}_LL12F:
# 1407
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp2E9,t,eo,s,0),0);_LL130:;_LL131: {
# 1409
const char*_tmpAE1;void*_tmpAE0;(_tmpAE0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE1="bogus x to make_struct",_tag_dyneither(_tmpAE1,sizeof(char),23))),_tag_dyneither(_tmpAE0,sizeof(void*),0)));}_LL12D:;}else{
# 1413
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1416
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1420
static int Cyc_Toc_array_or_aggr_with_array(void*typ){
void*_tmp2EC=Cyc_Tcutil_compress(typ);union Cyc_Absyn_AggrInfoU _tmp2EF;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2F2;_LL133: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2EC;if(_tmp2ED->tag != 8)goto _LL135;}_LL134:
 return 1;_LL135: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2EC;if(_tmp2EE->tag != 11)goto _LL137;else{_tmp2EF=(_tmp2EE->f1).aggr_info;_tmp2F0=(_tmp2EE->f1).targs;}}_LL136: {
# 1424
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp2EF);
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp2F2=aggrfields;goto _LL138;}_LL137: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2EC;if(_tmp2F1->tag != 12)goto _LL139;else{_tmp2F2=_tmp2F1->f2;}}_LL138:
# 1428
 if(_tmp2F2 == 0)return 0;{
struct Cyc_List_List*fields;
for(fields=_tmp2F2;fields->tl != 0;fields=fields->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp2F3=(struct Cyc_Absyn_Aggrfield*)fields->hd;
return Cyc_Toc_array_or_aggr_with_array(_tmp2F3->type);};};_LL139:;_LL13A:
# 1434
 return 0;_LL132:;}
# 1438
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp2F4=Cyc_Tcutil_compress(t);void*_tmp2F6;_LL13C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2F5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F4;if(_tmp2F5->tag != 8)goto _LL13E;else{_tmp2F6=(_tmp2F5->f1).elt_type;}}_LL13D:
 return _tmp2F6;_LL13E:;_LL13F: {
const char*_tmpAE5;void*_tmpAE4[1];struct Cyc_String_pa_PrintArg_struct _tmpAE3;(_tmpAE3.tag=0,((_tmpAE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAE4[0]=& _tmpAE3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE5="array_elt_type called on %s",_tag_dyneither(_tmpAE5,sizeof(char),28))),_tag_dyneither(_tmpAE4,sizeof(void*),1)))))));}_LL13B:;}
# 1445
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1451
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1456
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1461
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp2FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp2FA != 0;_tmp2FA=_tmp2FA->tl){
# 1465
struct Cyc_List_List*_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FD;struct _tuple15*_tmp2FB=(struct _tuple15*)_tmp2FA->hd;_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;{
# 1469
struct Cyc_Absyn_Exp*e_index;
if(_tmp2FC == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1473
if(_tmp2FC->tl != 0){const char*_tmpAE8;void*_tmpAE7;(_tmpAE7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAE8="multiple designators in array",_tag_dyneither(_tmpAE8,sizeof(char),30))),_tag_dyneither(_tmpAE7,sizeof(void*),0)));}{
void*_tmp300=(void*)_tmp2FC->hd;
void*_tmp301=_tmp300;struct Cyc_Absyn_Exp*_tmp303;_LL141: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp302=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp301;if(_tmp302->tag != 0)goto _LL143;else{_tmp303=_tmp302->f1;}}_LL142:
# 1477
 Cyc_Toc_exp_to_c(nv,_tmp303);
e_index=_tmp303;
goto _LL140;_LL143: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp304=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp301;if(_tmp304->tag != 1)goto _LL140;}_LL144: {
const char*_tmpAEB;void*_tmpAEA;(_tmpAEA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAEB="field name designators in array",_tag_dyneither(_tmpAEB,sizeof(char),32))),_tag_dyneither(_tmpAEA,sizeof(void*),0)));}_LL140:;};}{
# 1483
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp307=_tmp2FD->r;struct Cyc_List_List*_tmp309;struct Cyc_Absyn_Vardecl*_tmp30B;struct Cyc_Absyn_Exp*_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;int _tmp30E;void*_tmp310;struct Cyc_List_List*_tmp311;_LL146: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp308->tag != 25)goto _LL148;else{_tmp309=_tmp308->f1;}}_LL147:
# 1486
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp309,s);
goto _LL145;_LL148: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp30A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp30A->tag != 26)goto _LL14A;else{_tmp30B=_tmp30A->f1;_tmp30C=_tmp30A->f2;_tmp30D=_tmp30A->f3;_tmp30E=_tmp30A->f4;}}_LL149:
# 1489
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp30B,_tmp30C,_tmp30D,_tmp30E,s,0);
# 1491
goto _LL145;_LL14A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp30F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp30F->tag != 29)goto _LL14C;else{_tmp310=(void*)_tmp30F->f1;_tmp311=_tmp30F->f2;}}_LL14B:
# 1493
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp310,_tmp311,s);
goto _LL145;_LL14C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp312=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp312->tag != 27)goto _LL14E;}_LL14D:
# 1496
 goto _LL145;_LL14E:;_LL14F:
# 1498
 Cyc_Toc_exp_to_c(nv,_tmp2FD);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp2FD),0),s,0);
goto _LL145;_LL145:;};};}}
# 1504
return s;}
# 1509
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1514
struct _tuple0*_tmp313=vd->name;
void*expected_elt_type;
{void*_tmp314=Cyc_Tcutil_compress(comprehension_type);void*_tmp316;void*_tmp318;_LL151: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp314;if(_tmp315->tag != 8)goto _LL153;else{_tmp316=(_tmp315->f1).elt_type;}}_LL152:
# 1518
 expected_elt_type=_tmp316;goto _LL150;_LL153: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp317=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp314;if(_tmp317->tag != 5)goto _LL155;else{_tmp318=(_tmp317->f1).elt_typ;}}_LL154:
# 1520
 expected_elt_type=_tmp318;goto _LL150;_LL155:;_LL156: {
const char*_tmpAEE;void*_tmpAED;(_tmpAED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEE="init_comprehension passed a bad type!",_tag_dyneither(_tmpAEE,sizeof(char),38))),_tag_dyneither(_tmpAED,sizeof(void*),0)));}_LL150:;}{
# 1523
void*_tmp31B=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1527
{void*_tmp31C=e2->r;_LL158: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp31D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp31C;if(_tmp31D->tag != 27)goto _LL15A;}_LL159:
 return Cyc_Absyn_skip_stmt(0);_LL15A:;_LL15B:
 goto _LL157;_LL157:;}{
# 1531
struct _RegionHandle _tmp31E=_new_region("r2");struct _RegionHandle*r2=& _tmp31E;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpAF1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpAF0;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp313,Cyc_Absyn_varb_exp(_tmp313,(void*)((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF1.tag=4,((_tmpAF1.f1=vd,_tmpAF1)))),_tmpAF0)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp313,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp313,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp313,0),0);
# 1539
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp313,0),0);
struct Cyc_Absyn_Stmt*body;
# 1543
{void*_tmp31F=e2->r;struct Cyc_List_List*_tmp321;struct Cyc_Absyn_Vardecl*_tmp323;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*_tmp325;int _tmp326;void*_tmp328;struct Cyc_List_List*_tmp329;_LL15D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp320=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp31F;if(_tmp320->tag != 25)goto _LL15F;else{_tmp321=_tmp320->f1;}}_LL15E:
# 1545
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp321,Cyc_Toc_skip_stmt_dl());
goto _LL15C;_LL15F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp322=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp31F;if(_tmp322->tag != 26)goto _LL161;else{_tmp323=_tmp322->f1;_tmp324=_tmp322->f2;_tmp325=_tmp322->f3;_tmp326=_tmp322->f4;}}_LL160:
# 1548
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp323,_tmp324,_tmp325,_tmp326,Cyc_Toc_skip_stmt_dl(),0);
goto _LL15C;_LL161: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp327=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp31F;if(_tmp327->tag != 29)goto _LL163;else{_tmp328=(void*)_tmp327->f1;_tmp329=_tmp327->f2;}}_LL162:
# 1551
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp328,_tmp329,Cyc_Toc_skip_stmt_dl());
goto _LL15C;_LL163:;_LL164:
# 1554
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL15C;_LL15C:;}{
# 1558
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1560
if(zero_term){
# 1565
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp31B,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1570
struct Cyc_Absyn_Stmt*_tmp32A=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp313,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp32A;};};}
# 1532
;_pop_region(r2);};};}
# 1576
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1579
{struct Cyc_List_List*_tmp32D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp32D != 0;_tmp32D=_tmp32D->tl){
struct Cyc_List_List*_tmp32F;struct Cyc_Absyn_Exp*_tmp330;struct _tuple15*_tmp32E=(struct _tuple15*)_tmp32D->hd;_tmp32F=_tmp32E->f1;_tmp330=_tmp32E->f2;
if(_tmp32F == 0){
const char*_tmpAF4;void*_tmpAF3;(_tmpAF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF4="empty designator list",_tag_dyneither(_tmpAF4,sizeof(char),22))),_tag_dyneither(_tmpAF3,sizeof(void*),0)));}
if(_tmp32F->tl != 0){
const char*_tmpAF7;void*_tmpAF6;(_tmpAF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF7="too many designators in anonymous struct",_tag_dyneither(_tmpAF7,sizeof(char),41))),_tag_dyneither(_tmpAF6,sizeof(void*),0)));}{
void*_tmp335=(void*)_tmp32F->hd;struct _dyneither_ptr*_tmp337;_LL166: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp336=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp335;if(_tmp336->tag != 1)goto _LL168;else{_tmp337=_tmp336->f1;}}_LL167: {
# 1587
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp337,0);
{void*_tmp338=_tmp330->r;struct Cyc_List_List*_tmp33A;struct Cyc_Absyn_Vardecl*_tmp33C;struct Cyc_Absyn_Exp*_tmp33D;struct Cyc_Absyn_Exp*_tmp33E;int _tmp33F;void*_tmp341;struct Cyc_List_List*_tmp342;_LL16B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp339=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp338;if(_tmp339->tag != 25)goto _LL16D;else{_tmp33A=_tmp339->f1;}}_LL16C:
# 1590
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp330->topt),lval,_tmp33A,s);goto _LL16A;_LL16D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp33B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp338;if(_tmp33B->tag != 26)goto _LL16F;else{_tmp33C=_tmp33B->f1;_tmp33D=_tmp33B->f2;_tmp33E=_tmp33B->f3;_tmp33F=_tmp33B->f4;}}_LL16E:
# 1592
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp330->topt),lval,_tmp33C,_tmp33D,_tmp33E,_tmp33F,s,0);
goto _LL16A;_LL16F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp340=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp338;if(_tmp340->tag != 29)goto _LL171;else{_tmp341=(void*)_tmp340->f1;_tmp342=_tmp340->f2;}}_LL170:
# 1595
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp341,_tmp342,s);goto _LL16A;_LL171: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp343=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp338;if(_tmp343->tag != 27)goto _LL173;}_LL172:
# 1597
 goto _LL16A;_LL173:;_LL174:
# 1599
 Cyc_Toc_exp_to_c(nv,_tmp330);
# 1601
if(Cyc_Toc_is_poly_field(struct_type,_tmp337))
_tmp330=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp330);
# 1604
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp330,0),0),s,0);
goto _LL16A;_LL16A:;}
# 1607
goto _LL165;}_LL168:;_LL169: {
const char*_tmpAFA;void*_tmpAF9;(_tmpAF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFA="array designator in struct",_tag_dyneither(_tmpAFA,sizeof(char),27))),_tag_dyneither(_tmpAF9,sizeof(void*),0)));}_LL165:;};}}
# 1611
return s;}
# 1616
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1620
struct _RegionHandle _tmp346=_new_region("r");struct _RegionHandle*r=& _tmp346;_push_region(r);
{struct Cyc_List_List*_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp348=Cyc_Toc_add_tuple_type(_tmp347);
# 1624
struct _tuple0*_tmp349=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp34A=Cyc_Absyn_var_exp(_tmp349,0);
struct Cyc_Absyn_Stmt*_tmp34B=Cyc_Absyn_exp_stmt(_tmp34A,0);
# 1628
struct Cyc_Absyn_Exp*(*_tmp34C)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1630
int is_atomic=1;
struct Cyc_List_List*_tmp34D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34D);for(0;_tmp34D != 0;(_tmp34D=_tmp34D->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp34D->hd;
struct Cyc_Absyn_Exp*lval=_tmp34C(_tmp34A,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp34E=e->r;struct Cyc_List_List*_tmp350;struct Cyc_Absyn_Vardecl*_tmp352;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_Exp*_tmp354;int _tmp355;_LL176: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp34F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp34E;if(_tmp34F->tag != 25)goto _LL178;else{_tmp350=_tmp34F->f1;}}_LL177:
# 1638
 _tmp34B=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp350,_tmp34B);
goto _LL175;_LL178: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp34E;if(_tmp351->tag != 26)goto _LL17A;else{_tmp352=_tmp351->f1;_tmp353=_tmp351->f2;_tmp354=_tmp351->f3;_tmp355=_tmp351->f4;}}_LL179:
# 1641
 _tmp34B=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp352,_tmp353,_tmp354,_tmp355,_tmp34B,0);
# 1643
goto _LL175;_LL17A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp356=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp34E;if(_tmp356->tag != 27)goto _LL17C;}_LL17B:
# 1645
 goto _LL175;_LL17C:;_LL17D:
# 1648
 Cyc_Toc_exp_to_c(nv,e);
_tmp34B=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp34C(_tmp34A,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp34B,0);
# 1651
goto _LL175;_LL175:;};}}{
# 1654
struct Cyc_Absyn_Exp*_tmp357=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp349,0),_tmp348,_tmp34B,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp357;};}
# 1621
;_pop_region(r);}
# 1658
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp358=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp358 != 0;_tmp358=_tmp358->tl){
struct Cyc_Absyn_Aggrfield*_tmp359=(struct Cyc_Absyn_Aggrfield*)_tmp358->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp359->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1665
struct Cyc_String_pa_PrintArg_struct _tmpB02;void*_tmpB01[1];const char*_tmpB00;void*_tmpAFF;(_tmpAFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB02.tag=0,((_tmpB02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB01[0]=& _tmpB02,Cyc_aprintf(((_tmpB00="get_member_offset %s failed",_tag_dyneither(_tmpB00,sizeof(char),28))),_tag_dyneither(_tmpB01,sizeof(void*),1)))))))),_tag_dyneither(_tmpAFF,sizeof(void*),0)));};}
# 1668
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp35E=Cyc_Tcutil_compress(*typ);void*_tmp360;struct Cyc_Absyn_Tqual _tmp361;struct Cyc_Absyn_Exp*_tmp362;union Cyc_Absyn_Constraint*_tmp363;unsigned int _tmp364;_LL17F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35E;if(_tmp35F->tag != 8)goto _LL181;else{_tmp360=(_tmp35F->f1).elt_type;_tmp361=(_tmp35F->f1).tq;_tmp362=(_tmp35F->f1).num_elts;_tmp363=(_tmp35F->f1).zero_term;_tmp364=(_tmp35F->f1).zt_loc;}}_LL180:
# 1673
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp362))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB08;struct Cyc_Absyn_ArrayInfo _tmpB07;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB06;*typ=(void*)((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06[0]=((_tmpB08.tag=8,((_tmpB08.f1=((_tmpB07.elt_type=_tmp360,((_tmpB07.tq=_tmp361,((_tmpB07.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB07.zero_term=_tmp363,((_tmpB07.zt_loc=_tmp364,_tmpB07)))))))))),_tmpB08)))),_tmpB06))));}
return 1;}
# 1677
goto _LL17E;_LL181:;_LL182:
 goto _LL17E;_LL17E:;}
# 1680
return 0;}
# 1683
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1685
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1689
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp368=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_Absyn_Aggrfield*_tmpB09;struct Cyc_Absyn_Aggrfield*_tmp369=(_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->name=_tmp368->name,((_tmpB09->tq=Cyc_Toc_mt_tq,((_tmpB09->type=new_field_type,((_tmpB09->width=_tmp368->width,((_tmpB09->attributes=_tmp368->attributes,_tmpB09)))))))))));
# 1696
struct Cyc_List_List*_tmpB0A;new_fields=((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->hd=_tmp369,((_tmpB0A->tl=new_fields,_tmpB0A))))));}else{
# 1698
struct Cyc_List_List*_tmpB0B;new_fields=((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB0B->tl=new_fields,_tmpB0B))))));}}{
# 1700
struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB13;void*_tmpB12[1];const char*_tmpB11;struct _dyneither_ptr*_tmpB10;struct _dyneither_ptr*name=
(_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10[0]=(struct _dyneither_ptr)((_tmpB13.tag=1,((_tmpB13.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB12[0]=& _tmpB13,Cyc_aprintf(((_tmpB11="_genStruct%d",_tag_dyneither(_tmpB11,sizeof(char),13))),_tag_dyneither(_tmpB12,sizeof(void*),1)))))))),_tmpB10)));
struct _tuple0*_tmpB14;struct _tuple0*qv=(_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB14->f2=name,_tmpB14)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB15;struct Cyc_Absyn_AggrdeclImpl*_tmp36E=(_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpB15->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpB15->fields=_tmp36D,((_tmpB15->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpB15)))))))));
# 1708
struct Cyc_Absyn_Aggrdecl*_tmpB16;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->kind=ad->kind,((_tmpB16->sc=ad->sc,((_tmpB16->name=qv,((_tmpB16->tvs=ad->tvs,((_tmpB16->impl=_tmp36E,((_tmpB16->attributes=ad->attributes,_tmpB16)))))))))))));
# 1714
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1718
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1724
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple0*_tmp376=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp376,0);
do_declare=1;}else{
# 1732
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1735
struct Cyc_Absyn_Stmt*_tmp377=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1739
struct Cyc_Absyn_Exp**varexp;
{void*_tmp378=_tmp377->r;struct Cyc_Absyn_Exp**_tmp37A;_LL184: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp379=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp378;if(_tmp379->tag != 1)goto _LL186;else{_tmp37A=(struct Cyc_Absyn_Exp**)& _tmp379->f1;}}_LL185:
 varexp=_tmp37A;goto _LL183;_LL186:;_LL187: {
const char*_tmpB19;void*_tmpB18;(_tmpB18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB19="stmt not an expression!",_tag_dyneither(_tmpB19,sizeof(char),24))),_tag_dyneither(_tmpB18,sizeof(void*),0)));}_LL183:;}{
# 1745
struct Cyc_Absyn_Exp*(*_tmp37D)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1749
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp37E=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp380;struct Cyc_List_List*_tmp381;_LL189: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37E;if(_tmp37F->tag != 11)goto _LL18B;else{_tmp380=(_tmp37F->f1).aggr_info;_tmp381=(_tmp37F->f1).targs;}}_LL18A:
# 1756
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp380);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp381;
goto _LL188;_LL18B:;_LL18C: {
const char*_tmpB1C;void*_tmpB1B;(_tmpB1B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1C="init_struct: bad struct type",_tag_dyneither(_tmpB1C,sizeof(char),29))),_tag_dyneither(_tmpB1B,sizeof(void*),0)));}_LL188:;}
# 1763
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp384=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp384->tl != 0;_tmp384=_tmp384->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp385=(struct Cyc_Absyn_Aggrfield*)_tmp384->hd;
struct _RegionHandle _tmp386=_new_region("temp");struct _RegionHandle*temp=& _tmp386;_push_region(temp);
{struct Cyc_List_List*_tmp387=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1772
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp387,_tmp385->type))){
struct Cyc_List_List*_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp389=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp387,_tmp388);
# 1776
struct Cyc_List_List*new_fields=0;
for(_tmp384=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp384 != 0;_tmp384=_tmp384->tl){
struct Cyc_Absyn_Aggrfield*_tmp38A=(struct Cyc_Absyn_Aggrfield*)_tmp384->hd;
struct Cyc_Absyn_Aggrfield*_tmpB1D;struct Cyc_Absyn_Aggrfield*_tmp38B=(_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->name=_tmp38A->name,((_tmpB1D->tq=Cyc_Toc_mt_tq,((_tmpB1D->type=
# 1781
Cyc_Tcutil_rsubstitute(temp,_tmp389,_tmp38A->type),((_tmpB1D->width=_tmp38A->width,((_tmpB1D->attributes=_tmp38A->attributes,_tmpB1D)))))))))));
# 1788
if(_tmp384->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp38B->type)){
struct _dyneither_ptr**_tmpB1E;index=((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E[0]=_tmp38B->name,_tmpB1E))));}}{
# 1795
struct Cyc_List_List*_tmpB1F;new_fields=((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->hd=_tmp38B,((_tmpB1F->tl=new_fields,_tmpB1F))))));};}
# 1797
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB27;void*_tmpB26[1];const char*_tmpB25;struct _dyneither_ptr*_tmpB24;struct _dyneither_ptr*name=
(_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24[0]=(struct _dyneither_ptr)((_tmpB27.tag=1,((_tmpB27.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB26[0]=& _tmpB27,Cyc_aprintf(((_tmpB25="_genStruct%d",_tag_dyneither(_tmpB25,sizeof(char),13))),_tag_dyneither(_tmpB26,sizeof(void*),1)))))))),_tmpB24)));
struct _tuple0*_tmpB28;struct _tuple0*qv=(_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB28->f2=name,_tmpB28)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB29;struct Cyc_Absyn_AggrdeclImpl*_tmp38F=(_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29->exist_vars=0,((_tmpB29->rgn_po=0,((_tmpB29->fields=aggrfields,((_tmpB29->tagged=0,_tmpB29)))))))));
# 1806
struct Cyc_Absyn_Aggrdecl*_tmpB2A;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->kind=Cyc_Absyn_StructA,((_tmpB2A->sc=Cyc_Absyn_Public,((_tmpB2A->name=qv,((_tmpB2A->tvs=0,((_tmpB2A->impl=_tmp38F,((_tmpB2A->attributes=0,_tmpB2A)))))))))))));
# 1812
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB34;struct Cyc_Absyn_Aggrdecl**_tmpB33;struct Cyc_Absyn_AggrInfo _tmpB32;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB31;struct_type=(void*)((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=((_tmpB34.tag=11,((_tmpB34.f1=((_tmpB32.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33[0]=new_ad,_tmpB33))))),((_tmpB32.targs=0,_tmpB32)))),_tmpB34)))),_tmpB31))));}
# 1816
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1768
;_pop_region(temp);};}{
# 1820
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp39B=_new_region("r");struct _RegionHandle*r=& _tmp39B;_push_region(r);
{struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp39C != 0;_tmp39C=_tmp39C->tl){
struct Cyc_List_List*_tmp39E;struct Cyc_Absyn_Exp*_tmp39F;struct _tuple15*_tmp39D=(struct _tuple15*)_tmp39C->hd;_tmp39E=_tmp39D->f1;_tmp39F=_tmp39D->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp39F->topt));
if(_tmp39E == 0){
const char*_tmpB37;void*_tmpB36;(_tmpB36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="empty designator list",_tag_dyneither(_tmpB37,sizeof(char),22))),_tag_dyneither(_tmpB36,sizeof(void*),0)));}
if(_tmp39E->tl != 0){
# 1830
struct _tuple0*_tmp3A2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A3=Cyc_Absyn_var_exp(_tmp3A2,0);
for(0;_tmp39E != 0;_tmp39E=_tmp39E->tl){
void*_tmp3A4=(void*)_tmp39E->hd;struct _dyneither_ptr*_tmp3A6;_LL18E: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A5=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A4;if(_tmp3A5->tag != 1)goto _LL190;else{_tmp3A6=_tmp3A5->f1;}}_LL18F:
# 1836
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3A6))
_tmp3A3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A3);
_tmp377=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37D(xexp,_tmp3A6,0),_tmp3A3,0),0),_tmp377,0);
# 1840
goto _LL18D;_LL190:;_LL191: {
const char*_tmpB3A;void*_tmpB39;(_tmpB39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="array designator in struct",_tag_dyneither(_tmpB3A,sizeof(char),27))),_tag_dyneither(_tmpB39,sizeof(void*),0)));}_LL18D:;}
# 1844
Cyc_Toc_exp_to_c(nv,_tmp39F);
_tmp377=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A3,_tmp39F,0),0),_tmp377,0);}else{
# 1847
void*_tmp3A9=(void*)_tmp39E->hd;struct _dyneither_ptr*_tmp3AB;_LL193: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3AA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3A9;if(_tmp3AA->tag != 1)goto _LL195;else{_tmp3AB=_tmp3AA->f1;}}_LL194: {
# 1849
struct Cyc_Absyn_Exp*lval=_tmp37D(xexp,_tmp3AB,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3AB);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp377=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp377,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1858
{void*_tmp3AC=_tmp39F->r;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;int _tmp3B3;struct Cyc_Absyn_Exp*_tmp3B5;void*_tmp3B7;struct Cyc_List_List*_tmp3B8;_LL198: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3AD=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3AD->tag != 25)goto _LL19A;else{_tmp3AE=_tmp3AD->f1;}}_LL199:
# 1860
 _tmp377=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3AB)))->type,lval,_tmp3AE,_tmp377);
# 1862
goto _LL197;_LL19A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3AF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3AF->tag != 26)goto _LL19C;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3AF->f2;_tmp3B2=_tmp3AF->f3;_tmp3B3=_tmp3AF->f4;}}_LL19B:
# 1864
 _tmp377=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3AB)))->type,lval,_tmp3B0,_tmp3B1,_tmp3B2,_tmp3B3,_tmp377,0);
# 1868
_tmp3B5=_tmp3B1;goto _LL19D;_LL19C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3B4->tag != 27)goto _LL19E;else{_tmp3B5=_tmp3B4->f1;}}_LL19D:
# 1872
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3AB)){
void*_tmp3B9=Cyc_Toc_typ_to_c((void*)_check_null(_tmp3B5->topt));
struct _tuple5 _tmpB3E;union Cyc_Absyn_Cnst _tmpB3D;struct Cyc_Absyn_Exp*_tmpB3B[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpB3B[1]=Cyc_Absyn_sizeoftyp_exp(_tmp3B9,0),((_tmpB3B[0]=_tmp3B5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpB3D.Int_c).val=((_tmpB3E.f1=Cyc_Absyn_Unsigned,((_tmpB3E.f2=(int)sizeof(double),_tmpB3E)))),(((_tmpB3D.Int_c).tag=5,_tmpB3D)))),0),0);}
# 1879
goto _LL197;_LL19E: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3B6->tag != 29)goto _LL1A0;else{_tmp3B7=(void*)_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;}}_LL19F:
# 1881
 _tmp377=Cyc_Toc_init_anon_struct(nv,lval,_tmp3B7,_tmp3B8,_tmp377);goto _LL197;_LL1A0:;_LL1A1: {
# 1883
void*old_e_typ=(void*)_check_null(_tmp39F->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp39F);
{void*_tmp3BD=old_e_typ;_LL1A3: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3BE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3BD;if(_tmp3BE->tag != 11)goto _LL1A5;}_LL1A4:
# 1890
 if(old_e_typ != _tmp39F->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp39F->topt))){
# 1894
ad=Cyc_Toc_update_aggr_type(ad,_tmp3AB,(void*)_check_null(_tmp39F->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB48;struct Cyc_Absyn_Aggrdecl**_tmpB47;struct Cyc_Absyn_AggrInfo _tmpB46;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB45;struct_type=(void*)(
(_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((_tmpB48.tag=11,((_tmpB48.f1=((_tmpB46.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47[0]=ad,_tmpB47))))),((_tmpB46.targs=0,_tmpB46)))),_tmpB48)))),_tmpB45))));}
# 1898
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1900
goto _LL1A2;_LL1A5:;_LL1A6:
 goto _LL1A2;_LL1A2:;}{
# 1903
struct Cyc_Absyn_Aggrfield*_tmp3C3=Cyc_Absyn_lookup_field(aggrfields,_tmp3AB);
# 1905
if(Cyc_Toc_is_poly_field(struct_type,_tmp3AB) && !was_ptr_type)
_tmp39F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39F);
# 1908
if(exist_types != 0)
_tmp39F=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3C3))->type),_tmp39F);
# 1911
_tmp377=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp39F,0),0),_tmp377,0);
goto _LL197;};}_LL197:;}
# 1914
goto _LL192;}_LL195:;_LL196: {
const char*_tmpB4B;void*_tmpB4A;(_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="array designator in struct",_tag_dyneither(_tmpB4B,sizeof(char),27))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}_LL192:;}}}
# 1921
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 1926
struct Cyc_Absyn_Exp*_tmp3C6=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp377,pointer,rgnopt,is_atomic,do_declare);
# 1928
_tmp3C6->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp3C7=_tmp3C6;_npop_handler(0);return _tmp3C7;};};
# 1822
;_pop_region(r);};};};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1934
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple18*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1937
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1940
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB4E;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB4D;return Cyc_Absyn_new_exp((void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4E.tag=4,((_tmpB4E.f1=e1,((_tmpB4E.f2=incr,_tmpB4E)))))),_tmpB4D)))),0);}
# 1944
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1953
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1955
void*_tmp3CA=e1->r;struct Cyc_Absyn_Stmt*_tmp3CC;void*_tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D1;struct _dyneither_ptr*_tmp3D2;int _tmp3D3;int _tmp3D4;_LL1A8: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3CB=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3CA;if(_tmp3CB->tag != 36)goto _LL1AA;else{_tmp3CC=_tmp3CB->f1;}}_LL1A9:
 Cyc_Toc_lvalue_assign_stmt(_tmp3CC,fs,f,f_env);goto _LL1A7;_LL1AA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3CD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CA;if(_tmp3CD->tag != 13)goto _LL1AC;else{_tmp3CE=(void*)_tmp3CD->f1;_tmp3CF=_tmp3CD->f2;}}_LL1AB:
 Cyc_Toc_lvalue_assign(_tmp3CF,fs,f,f_env);goto _LL1A7;_LL1AC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp3D0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp3CA;if(_tmp3D0->tag != 20)goto _LL1AE;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;_tmp3D3=_tmp3D0->f3;_tmp3D4=_tmp3D0->f4;}}_LL1AD:
# 1960
 e1->r=_tmp3D1->r;
{struct Cyc_List_List*_tmpB4F;Cyc_Toc_lvalue_assign(e1,((_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->hd=_tmp3D2,((_tmpB4F->tl=fs,_tmpB4F)))))),f,f_env);}
goto _LL1A7;_LL1AE:;_LL1AF: {
# 1968
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1970
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1A7;}_LL1A7:;}
# 1976
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1978
void*_tmp3D6=s->r;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Decl*_tmp3DA;struct Cyc_Absyn_Stmt*_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DD;_LL1B1: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3D7=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3D7->tag != 1)goto _LL1B3;else{_tmp3D8=_tmp3D7->f1;}}_LL1B2:
 Cyc_Toc_lvalue_assign(_tmp3D8,fs,f,f_env);goto _LL1B0;_LL1B3: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3D9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3D9->tag != 12)goto _LL1B5;else{_tmp3DA=_tmp3D9->f1;_tmp3DB=_tmp3D9->f2;}}_LL1B4:
# 1981
 Cyc_Toc_lvalue_assign_stmt(_tmp3DB,fs,f,f_env);goto _LL1B0;_LL1B5: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3DC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3D6;if(_tmp3DC->tag != 2)goto _LL1B7;else{_tmp3DD=_tmp3DC->f2;}}_LL1B6:
 Cyc_Toc_lvalue_assign_stmt(_tmp3DD,fs,f,f_env);goto _LL1B0;_LL1B7:;_LL1B8: {
const char*_tmpB53;void*_tmpB52[1];struct Cyc_String_pa_PrintArg_struct _tmpB51;(_tmpB51.tag=0,((_tmpB51.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB52[0]=& _tmpB51,Cyc_Toc_toc_impos(((_tmpB53="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB53,sizeof(char),23))),_tag_dyneither(_tmpB52,sizeof(void*),1)))))));}_LL1B0:;}
# 1987
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1991
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp3E1=e->r;void**_tmp3E3;struct Cyc_Absyn_Exp**_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Stmt*_tmp3E8;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3E2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3E2->tag != 13)goto _LL1BC;else{_tmp3E3=(void**)((void**)& _tmp3E2->f1);_tmp3E4=(struct Cyc_Absyn_Exp**)& _tmp3E2->f2;}}_LL1BB:
# 1994
*_tmp3E4=Cyc_Toc_push_address_exp(*_tmp3E4);
*_tmp3E3=Cyc_Absyn_cstar_typ(*_tmp3E3,Cyc_Toc_mt_tq);
return e;_LL1BC: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp3E5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3E5->tag != 19)goto _LL1BE;else{_tmp3E6=_tmp3E5->f1;}}_LL1BD:
# 1998
 return _tmp3E6;_LL1BE: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp3E7=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp3E1;if(_tmp3E7->tag != 36)goto _LL1C0;else{_tmp3E8=_tmp3E7->f1;}}_LL1BF:
# 2002
 Cyc_Toc_push_address_stmt(_tmp3E8);
return e;_LL1C0:;_LL1C1:
# 2005
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB57;void*_tmpB56[1];struct Cyc_String_pa_PrintArg_struct _tmpB55;(_tmpB55.tag=0,((_tmpB55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB56[0]=& _tmpB55,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="can't take & of exp %s",_tag_dyneither(_tmpB57,sizeof(char),23))),_tag_dyneither(_tmpB56,sizeof(void*),1)))))));};_LL1B9:;}
# 2010
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp3EC=s->r;struct Cyc_Absyn_Stmt*_tmp3EE;struct Cyc_Absyn_Stmt*_tmp3F0;struct Cyc_Absyn_Exp**_tmp3F2;_LL1C3: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3ED=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3EC;if(_tmp3ED->tag != 2)goto _LL1C5;else{_tmp3EE=_tmp3ED->f2;}}_LL1C4:
 _tmp3F0=_tmp3EE;goto _LL1C6;_LL1C5: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3EF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3EC;if(_tmp3EF->tag != 12)goto _LL1C7;else{_tmp3F0=_tmp3EF->f2;}}_LL1C6:
 Cyc_Toc_push_address_stmt(_tmp3F0);goto _LL1C2;_LL1C7: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3F1=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3EC;if(_tmp3F1->tag != 1)goto _LL1C9;else{_tmp3F2=(struct Cyc_Absyn_Exp**)& _tmp3F1->f1;}}_LL1C8:
*_tmp3F2=Cyc_Toc_push_address_exp(*_tmp3F2);goto _LL1C2;_LL1C9:;_LL1CA: {
# 2016
const char*_tmpB5B;void*_tmpB5A[1];struct Cyc_String_pa_PrintArg_struct _tmpB59;(_tmpB59.tag=0,((_tmpB59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB5A[0]=& _tmpB59,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5B="can't take & of stmt %s",_tag_dyneither(_tmpB5B,sizeof(char),24))),_tag_dyneither(_tmpB5A,sizeof(void*),1)))))));}_LL1C2:;}
# 2021
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2023
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2025
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB5C;result=((_tmpB5C=_region_malloc(r2,sizeof(*_tmpB5C)),((_tmpB5C->hd=f(x->hd,env),((_tmpB5C->tl=0,_tmpB5C))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB5D;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB5D=_region_malloc(r2,sizeof(*_tmpB5D)),((_tmpB5D->hd=f(x->hd,env),((_tmpB5D->tl=0,_tmpB5D))))));}
prev=prev->tl;}
# 2032
return result;}
# 2034
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2038
static struct _tuple15*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple15*_tmpB5E;return(_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->f1=0,((_tmpB5E->f2=e,_tmpB5E)))));}
# 2042
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp3F9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp3FB;_LL1CC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F9;if(_tmp3FA->tag != 5)goto _LL1CE;else{_tmp3FB=_tmp3FA->f1;}}_LL1CD:
 return _tmp3FB;_LL1CE:;_LL1CF: {
const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB61,sizeof(char),28))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}_LL1CB:;}
# 2052
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp3FE=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp401;enum Cyc_Absyn_Sign _tmp403;enum Cyc_Absyn_Sign _tmp407;enum Cyc_Absyn_Sign _tmp409;enum Cyc_Absyn_Sign _tmp40B;int _tmp40F;_LL1D1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FE;if(_tmp3FF->tag != 5)goto _LL1D3;}_LL1D2:
 res=Cyc_Absyn_null_exp(0);goto _LL1D0;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE;if(_tmp400->tag != 6)goto _LL1D5;else{_tmp401=_tmp400->f1;if(_tmp400->f2 != Cyc_Absyn_Char_sz)goto _LL1D5;}}_LL1D4:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp401,'\000'),0);goto _LL1D0;_LL1D5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE;if(_tmp402->tag != 6)goto _LL1D7;else{_tmp403=_tmp402->f1;if(_tmp402->f2 != Cyc_Absyn_Short_sz)goto _LL1D7;}}_LL1D6:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp403,0),0);goto _LL1D0;_LL1D7: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp404=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3FE;if(_tmp404->tag != 13)goto _LL1D9;}_LL1D8:
 goto _LL1DA;_LL1D9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3FE;if(_tmp405->tag != 14)goto _LL1DB;}_LL1DA:
 _tmp407=Cyc_Absyn_Unsigned;goto _LL1DC;_LL1DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE;if(_tmp406->tag != 6)goto _LL1DD;else{_tmp407=_tmp406->f1;if(_tmp406->f2 != Cyc_Absyn_Int_sz)goto _LL1DD;}}_LL1DC:
 _tmp409=_tmp407;goto _LL1DE;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp408=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE;if(_tmp408->tag != 6)goto _LL1DF;else{_tmp409=_tmp408->f1;if(_tmp408->f2 != Cyc_Absyn_Long_sz)goto _LL1DF;}}_LL1DE:
# 2062
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp409,0),0);goto _LL1D0;_LL1DF: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp40A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3FE;if(_tmp40A->tag != 6)goto _LL1E1;else{_tmp40B=_tmp40A->f1;if(_tmp40A->f2 != Cyc_Absyn_LongLong_sz)goto _LL1E1;}}_LL1E0:
# 2064
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp40B,(long long)0),0);goto _LL1D0;_LL1E1: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FE;if(_tmp40C->tag != 7)goto _LL1E3;else{if(_tmp40C->f1 != 0)goto _LL1E3;}}_LL1E2:
{const char*_tmpB62;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB62="0.0F",_tag_dyneither(_tmpB62,sizeof(char),5))),0),0);}goto _LL1D0;_LL1E3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FE;if(_tmp40D->tag != 7)goto _LL1E5;else{if(_tmp40D->f1 != 1)goto _LL1E5;}}_LL1E4:
{const char*_tmpB63;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB63="0.0",_tag_dyneither(_tmpB63,sizeof(char),4))),1),0);}goto _LL1D0;_LL1E5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3FE;if(_tmp40E->tag != 7)goto _LL1E7;else{_tmp40F=_tmp40E->f1;}}_LL1E6:
{const char*_tmpB64;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB64="0.0L",_tag_dyneither(_tmpB64,sizeof(char),5))),_tmp40F),0);}goto _LL1D0;_LL1E7:;_LL1E8: {
# 2069
const char*_tmpB68;void*_tmpB67[1];struct Cyc_String_pa_PrintArg_struct _tmpB66;(_tmpB66.tag=0,((_tmpB66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB67[0]=& _tmpB66,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB68="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB68,sizeof(char),40))),_tag_dyneither(_tmpB67,sizeof(void*),1)))))));}_LL1D0:;}
# 2071
res->topt=t;
return res;}
# 2078
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2091 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp416=Cyc_Toc_typ_to_c(elt_type);
void*_tmp417=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp418=Cyc_Absyn_cstar_typ(_tmp416,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB69;struct Cyc_Core_Opt*_tmp419=(_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->v=_tmp418,_tmpB69)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp41A=e1->r;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;_LL1EA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp41B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp41A;if(_tmp41B->tag != 19)goto _LL1EC;else{_tmp41C=_tmp41B->f1;}}_LL1EB:
# 2099
 if(!is_dyneither){
_tmp41C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41C,0,Cyc_Absyn_Other_coercion,0);
_tmp41C->topt=fat_ptr_type;}
# 2103
Cyc_Toc_exp_to_c(nv,_tmp41C);xinit=_tmp41C;goto _LL1E9;_LL1EC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp41D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp41A;if(_tmp41D->tag != 22)goto _LL1EE;else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL1ED:
# 2105
 if(!is_dyneither){
_tmp41E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp41E,0,Cyc_Absyn_Other_coercion,0);
_tmp41E->topt=fat_ptr_type;}
# 2109
Cyc_Toc_exp_to_c(nv,_tmp41E);Cyc_Toc_exp_to_c(nv,_tmp41F);
{struct Cyc_Absyn_Exp*_tmpB6A[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB6A[2]=_tmp41F,((_tmpB6A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB6A[0]=_tmp41E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E9;_LL1EE:;_LL1EF: {
const char*_tmpB6D;void*_tmpB6C;(_tmpB6C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6D="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB6D,sizeof(char),53))),_tag_dyneither(_tmpB6C,sizeof(void*),0)));}_LL1E9:;}{
# 2115
struct _tuple0*_tmp423=Cyc_Toc_temp_var();
struct _RegionHandle _tmp424=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp424;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp425=Cyc_Toc_add_varmap(rgn2,nv,_tmp423,Cyc_Absyn_var_exp(_tmp423,0));
struct Cyc_Absyn_Vardecl*_tmpB6E;struct Cyc_Absyn_Vardecl*_tmp426=(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E->sc=Cyc_Absyn_Public,((_tmpB6E->name=_tmp423,((_tmpB6E->tq=Cyc_Toc_mt_tq,((_tmpB6E->type=_tmp417,((_tmpB6E->initializer=xinit,((_tmpB6E->rgn=0,((_tmpB6E->attributes=0,((_tmpB6E->escapes=0,_tmpB6E)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB71;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB70;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp427=(_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70[0]=((_tmpB71.tag=4,((_tmpB71.f1=_tmp426,_tmpB71)))),_tmpB70)));
struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_varb_exp(_tmp423,(void*)_tmp427,0);
_tmp428->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_deref_exp(_tmp428,0);
_tmp429->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp425,_tmp429);{
struct _tuple0*_tmp42A=Cyc_Toc_temp_var();
_tmp425=Cyc_Toc_add_varmap(rgn2,_tmp425,_tmp42A,Cyc_Absyn_var_exp(_tmp42A,0));{
struct Cyc_Absyn_Vardecl*_tmpB72;struct Cyc_Absyn_Vardecl*_tmp42B=(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->sc=Cyc_Absyn_Public,((_tmpB72->name=_tmp42A,((_tmpB72->tq=Cyc_Toc_mt_tq,((_tmpB72->type=_tmp416,((_tmpB72->initializer=_tmp429,((_tmpB72->rgn=0,((_tmpB72->attributes=0,((_tmpB72->escapes=0,_tmpB72)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB75;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB74;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp42C=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74[0]=((_tmpB75.tag=4,((_tmpB75.f1=_tmp42B,_tmpB75)))),_tmpB74)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_varb_exp(_tmp42A,(void*)_tmp42C,0);
_tmp42D->topt=_tmp429->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp42D,e2,0);
z_init->topt=_tmp42D->topt;}
# 2136
Cyc_Toc_exp_to_c(_tmp425,z_init);{
struct _tuple0*_tmp42E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB76;struct Cyc_Absyn_Vardecl*_tmp42F=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76->sc=Cyc_Absyn_Public,((_tmpB76->name=_tmp42E,((_tmpB76->tq=Cyc_Toc_mt_tq,((_tmpB76->type=_tmp416,((_tmpB76->initializer=z_init,((_tmpB76->rgn=0,((_tmpB76->attributes=0,((_tmpB76->escapes=0,_tmpB76)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB79;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB78;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp430=(_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78[0]=((_tmpB79.tag=4,((_tmpB79.f1=_tmp42F,_tmpB79)))),_tmpB78)));
_tmp425=Cyc_Toc_add_varmap(rgn2,_tmp425,_tmp42E,Cyc_Absyn_var_exp(_tmp42E,0));{
# 2143
struct Cyc_Absyn_Exp*_tmp431=Cyc_Absyn_varb_exp(_tmp42A,(void*)_tmp42C,0);_tmp431->topt=_tmp429->topt;{
struct Cyc_Absyn_Exp*_tmp432=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp433=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp431,_tmp432,0);
_tmp433->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp425,_tmp433);{
# 2149
struct Cyc_Absyn_Exp*_tmp434=Cyc_Absyn_varb_exp(_tmp42E,(void*)_tmp430,0);_tmp434->topt=_tmp429->topt;{
struct Cyc_Absyn_Exp*_tmp435=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp434,_tmp435,0);
_tmp436->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp425,_tmp436);{
# 2155
struct Cyc_Absyn_Exp*_tmpB7A[2];struct Cyc_List_List*_tmp437=(_tmpB7A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB7A[0]=
# 2155
Cyc_Absyn_varb_exp(_tmp423,(void*)_tmp427,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB7A,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2157
struct Cyc_Absyn_Exp*_tmp438=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp437,0),_tmp438,0);{
# 2162
struct Cyc_Absyn_Exp*_tmp439=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp433,_tmp436,0),0);
struct Cyc_Absyn_Stmt*_tmp43A=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp43B=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp423,(void*)_tmp427,0),Cyc_Toc_curr_sp,0);
_tmp43B=Cyc_Toc_cast_it(_tmp418,_tmp43B);{
struct Cyc_Absyn_Exp*_tmp43C=Cyc_Absyn_deref_exp(_tmp43B,0);
struct Cyc_Absyn_Exp*_tmp43D=Cyc_Absyn_assign_exp(_tmp43C,Cyc_Absyn_var_exp(_tmp42E,0),0);
struct Cyc_Absyn_Stmt*_tmp43E=Cyc_Absyn_exp_stmt(_tmp43D,0);
_tmp43E=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp439,_tmp43A,Cyc_Absyn_skip_stmt(0),0),_tmp43E,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB80;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB7F;struct Cyc_Absyn_Decl*_tmpB7E;_tmp43E=Cyc_Absyn_decl_stmt(((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->r=(void*)((_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80[0]=((_tmpB7F.tag=0,((_tmpB7F.f1=_tmp42F,_tmpB7F)))),_tmpB80)))),((_tmpB7E->loc=0,_tmpB7E)))))),_tmp43E,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB86;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB85;struct Cyc_Absyn_Decl*_tmpB84;_tmp43E=Cyc_Absyn_decl_stmt(((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->r=(void*)((_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86[0]=((_tmpB85.tag=0,((_tmpB85.f1=_tmp42B,_tmpB85)))),_tmpB86)))),((_tmpB84->loc=0,_tmpB84)))))),_tmp43E,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB8C;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB8B;struct Cyc_Absyn_Decl*_tmpB8A;_tmp43E=Cyc_Absyn_decl_stmt(((_tmpB8A=_cycalloc(sizeof(*_tmpB8A)),((_tmpB8A->r=(void*)((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C[0]=((_tmpB8B.tag=0,((_tmpB8B.f1=_tmp426,_tmpB8B)))),_tmpB8C)))),((_tmpB8A->loc=0,_tmpB8A)))))),_tmp43E,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp43E);};};};};};};};};};};};}
# 2117
;_pop_region(rgn2);};}
# 2188 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2192
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp453=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp455;_LL1F1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp453;if(_tmp454->tag != 11)goto _LL1F3;else{_tmp455=(_tmp454->f1).aggr_info;}}_LL1F2:
# 2195
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp455);goto _LL1F0;_LL1F3:;_LL1F4: {
struct Cyc_String_pa_PrintArg_struct _tmpB94;void*_tmpB93[1];const char*_tmpB92;void*_tmpB91;(_tmpB91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB94.tag=0,((_tmpB94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB93[0]=& _tmpB94,Cyc_aprintf(((_tmpB92="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB92,sizeof(char),51))),_tag_dyneither(_tmpB93,sizeof(void*),1)))))))),_tag_dyneither(_tmpB91,sizeof(void*),0)));}_LL1F0:;}{
# 2199
struct _tuple0*_tmp45A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp45B=Cyc_Absyn_var_exp(_tmp45A,0);
struct Cyc_Absyn_Exp*_tmp45C=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp45D=Cyc_Absyn_aggrarrow_exp(_tmp45B,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp45E=Cyc_Absyn_neq_exp(_tmp45D,_tmp45C,0);
struct Cyc_Absyn_Exp*_tmp45F=Cyc_Absyn_aggrarrow_exp(_tmp45B,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp460=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp45F,0),0);
struct Cyc_Absyn_Stmt*_tmp461=Cyc_Absyn_ifthenelse_stmt(_tmp45E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp462=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp463=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp464=Cyc_Absyn_declare_stmt(_tmp45A,_tmp462,_tmp463,Cyc_Absyn_seq_stmt(_tmp461,_tmp460,0),0);
return Cyc_Toc_stmt_exp_r(_tmp464);}else{
# 2213
struct Cyc_Absyn_Exp*_tmp465=Cyc_Toc_member_exp(aggrproj(_tmp45B,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_neq_exp(_tmp465,_tmp45C,0);
struct Cyc_Absyn_Exp*_tmp467=Cyc_Toc_member_exp(aggrproj(_tmp45B,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp468=Cyc_Absyn_exp_stmt(_tmp467,0);
struct Cyc_Absyn_Stmt*_tmp469=Cyc_Absyn_ifthenelse_stmt(_tmp466,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp46A=Cyc_Absyn_declare_stmt(_tmp45A,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp469,_tmp468,0),0);
return Cyc_Toc_stmt_exp_r(_tmp46A);}};}
# 2226
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2229
void*_tmp46B=e->r;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46F;struct _dyneither_ptr*_tmp470;int*_tmp471;struct Cyc_Absyn_Exp*_tmp473;struct _dyneither_ptr*_tmp474;int*_tmp475;_LL1F6: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp46C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp46B;if(_tmp46C->tag != 13)goto _LL1F8;else{_tmp46D=_tmp46C->f2;}}_LL1F7: {
const char*_tmpB97;void*_tmpB96;(_tmpB96=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB97="cast on lhs!",_tag_dyneither(_tmpB97,sizeof(char),13))),_tag_dyneither(_tmpB96,sizeof(void*),0)));}_LL1F8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp46E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp46B;if(_tmp46E->tag != 20)goto _LL1FA;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46E->f2;_tmp471=(int*)& _tmp46E->f4;}}_LL1F9: {
# 2232
void*_tmp478=Cyc_Tcutil_compress((void*)_check_null(_tmp46F->topt));union Cyc_Absyn_AggrInfoU _tmp47A;_LL1FF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp479=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp478;if(_tmp479->tag != 11)goto _LL201;else{_tmp47A=(_tmp479->f1).aggr_info;}}_LL200: {
# 2234
struct Cyc_Absyn_Aggrdecl*_tmp47B=Cyc_Absyn_get_known_aggrdecl(_tmp47A);
*f_tag=Cyc_Toc_get_member_offset(_tmp47B,_tmp470);{
# 2237
const char*_tmpB9C;void*_tmpB9B[2];struct Cyc_String_pa_PrintArg_struct _tmpB9A;struct Cyc_String_pa_PrintArg_struct _tmpB99;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB99.tag=0,((_tmpB99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp470),((_tmpB9A.tag=0,((_tmpB9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp47B->name).f2),((_tmpB9B[0]=& _tmpB9A,((_tmpB9B[1]=& _tmpB99,Cyc_aprintf(((_tmpB9C="_union_%s_%s",_tag_dyneither(_tmpB9C,sizeof(char),13))),_tag_dyneither(_tmpB9B,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB9D;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D[0]=str,_tmpB9D)))));}
if(clear_read)*_tmp471=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp47B->impl))->tagged;};}_LL201:;_LL202:
 return 0;_LL1FE:;}_LL1FA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp472=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp46B;if(_tmp472->tag != 21)goto _LL1FC;else{_tmp473=_tmp472->f1;_tmp474=_tmp472->f2;_tmp475=(int*)& _tmp472->f4;}}_LL1FB: {
# 2244
void*_tmp481=Cyc_Tcutil_compress((void*)_check_null(_tmp473->topt));void*_tmp483;_LL204: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp481;if(_tmp482->tag != 5)goto _LL206;else{_tmp483=(_tmp482->f1).elt_typ;}}_LL205: {
# 2246
void*_tmp484=Cyc_Tcutil_compress(_tmp483);union Cyc_Absyn_AggrInfoU _tmp486;_LL209: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp484;if(_tmp485->tag != 11)goto _LL20B;else{_tmp486=(_tmp485->f1).aggr_info;}}_LL20A: {
# 2248
struct Cyc_Absyn_Aggrdecl*_tmp487=Cyc_Absyn_get_known_aggrdecl(_tmp486);
*f_tag=Cyc_Toc_get_member_offset(_tmp487,_tmp474);{
# 2251
const char*_tmpBA2;void*_tmpBA1[2];struct Cyc_String_pa_PrintArg_struct _tmpBA0;struct Cyc_String_pa_PrintArg_struct _tmpB9F;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB9F.tag=0,((_tmpB9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp474),((_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp487->name).f2),((_tmpBA1[0]=& _tmpBA0,((_tmpBA1[1]=& _tmpB9F,Cyc_aprintf(((_tmpBA2="_union_%s_%s",_tag_dyneither(_tmpBA2,sizeof(char),13))),_tag_dyneither(_tmpBA1,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpBA3;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3[0]=str,_tmpBA3)))));}
if(clear_read)*_tmp475=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp487->impl))->tagged;};}_LL20B:;_LL20C:
 return 0;_LL208:;}_LL206:;_LL207:
# 2257
 return 0;_LL203:;}_LL1FC:;_LL1FD:
# 2259
 return 0;_LL1F5:;}
# 2271 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp48D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48D,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp48E=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp48E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48D,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2295 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2299
struct _tuple0*_tmp48F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp48F,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2309
struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp490,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2312
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp48F,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{void*f1;void*f2;};struct _tuple20{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBF2(unsigned int*_tmpBF1,unsigned int*_tmpBF0,struct _tuple0***_tmpBEE){for(*_tmpBF1=0;*_tmpBF1 < *_tmpBF0;(*_tmpBF1)++){(*_tmpBEE)[*_tmpBF1]=(struct _tuple0*)
# 2689 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2318 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp491=e->r;
if(e->topt == 0){
const char*_tmpBA7;void*_tmpBA6[1];struct Cyc_String_pa_PrintArg_struct _tmpBA5;(_tmpBA5.tag=0,((_tmpBA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBA6[0]=& _tmpBA5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA7="exp_to_c: no type for %s",_tag_dyneither(_tmpBA7,sizeof(char),25))),_tag_dyneither(_tmpBA6,sizeof(void*),1)))))));}
# 2324
if((nv->struct_info).lhs_exp != 0){
void*_tmp495=_tmp491;_LL20E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp496=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp495;if(_tmp496->tag != 28)goto _LL210;}_LL20F:
 goto _LL20D;_LL210:;_LL211:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL20D:;}{
# 2332
void*old_typ=(void*)_check_null(e->topt);
void*_tmp497=_tmp491;struct _tuple0*_tmp49B;void*_tmp49C;enum Cyc_Absyn_Primop _tmp49E;struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A1;enum Cyc_Absyn_Incrementor _tmp4A2;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Core_Opt*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B5;struct Cyc_List_List*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B8;struct Cyc_List_List*_tmp4B9;int _tmp4BA;struct Cyc_List_List*_tmp4BB;struct Cyc_Absyn_VarargInfo*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_List_List*_tmp4C3;void**_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C6;int _tmp4C7;enum Cyc_Absyn_Coercion _tmp4C8;struct Cyc_Absyn_Exp*_tmp4CA;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CF;void*_tmp4D1;void*_tmp4D3;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct _dyneither_ptr*_tmp4D9;int _tmp4DA;int _tmp4DB;struct Cyc_Absyn_Exp*_tmp4DD;struct _dyneither_ptr*_tmp4DE;int _tmp4DF;int _tmp4E0;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_List_List*_tmp4E5;struct Cyc_List_List*_tmp4E7;struct Cyc_Absyn_Vardecl*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EB;int _tmp4EC;struct Cyc_Absyn_Exp*_tmp4EE;void*_tmp4EF;int _tmp4F0;struct _tuple0*_tmp4F2;struct Cyc_List_List*_tmp4F3;struct Cyc_List_List*_tmp4F4;struct Cyc_Absyn_Aggrdecl*_tmp4F5;void*_tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_List_List*_tmp4FA;struct Cyc_Absyn_Datatypedecl*_tmp4FB;struct Cyc_Absyn_Datatypefield*_tmp4FC;int _tmp500;struct Cyc_Absyn_Exp*_tmp501;void**_tmp502;struct Cyc_Absyn_Exp*_tmp503;int _tmp504;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp509;struct _dyneither_ptr*_tmp50A;struct Cyc_Absyn_Stmt*_tmp50C;_LL213: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp498=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp498->tag != 0)goto _LL215;else{if(((_tmp498->f1).Null_c).tag != 1)goto _LL215;}}_LL214: {
# 2339
struct Cyc_Absyn_Exp*_tmp511=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp511,_tmp511))->r;else{
# 2344
struct Cyc_Absyn_Exp*_tmpBA8[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpBA8[2]=_tmp511,((_tmpBA8[1]=_tmp511,((_tmpBA8[0]=_tmp511,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA8,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2346
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2348
goto _LL212;}_LL215: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp499=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp499->tag != 0)goto _LL217;}_LL216:
 goto _LL212;_LL217: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp49A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp49A->tag != 1)goto _LL219;else{_tmp49B=_tmp49A->f1;_tmp49C=(void*)_tmp49A->f2;}}_LL218:
# 2351
{struct _handler_cons _tmp513;_push_handler(& _tmp513);{int _tmp515=0;if(setjmp(_tmp513.handler))_tmp515=1;if(!_tmp515){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp49B))->r;;_pop_handler();}else{void*_tmp514=(void*)_exn_thrown;void*_tmp517=_tmp514;void*_tmp519;_LL268: {struct Cyc_Dict_Absent_exn_struct*_tmp518=(struct Cyc_Dict_Absent_exn_struct*)_tmp517;if(_tmp518->tag != Cyc_Dict_Absent)goto _LL26A;}_LL269: {
# 2353
const char*_tmpBAC;void*_tmpBAB[1];struct Cyc_String_pa_PrintArg_struct _tmpBAA;(_tmpBAA.tag=0,((_tmpBAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp49B)),((_tmpBAB[0]=& _tmpBAA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAC="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBAC,sizeof(char),32))),_tag_dyneither(_tmpBAB,sizeof(void*),1)))))));}_LL26A: _tmp519=_tmp517;_LL26B:(void)_throw(_tmp519);_LL267:;}};}
# 2355
goto _LL212;_LL219: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp49D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp49D->tag != 2)goto _LL21B;else{_tmp49E=_tmp49D->f1;_tmp49F=_tmp49D->f2;}}_LL21A: {
# 2358
struct Cyc_List_List*_tmp51D=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp49F);
# 2360
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp49F);
switch(_tmp49E){case Cyc_Absyn_Numelts: _LL26C: {
# 2363
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F))->hd;
{void*_tmp51E=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp520;void*_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;union Cyc_Absyn_Constraint*_tmp525;_LL26F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51E;if(_tmp51F->tag != 8)goto _LL271;else{_tmp520=(_tmp51F->f1).num_elts;}}_LL270:
# 2367
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp520))){
const char*_tmpBAF;void*_tmpBAE;(_tmpBAE=0,Cyc_Tcutil_terr(e->loc,((_tmpBAF="can't calculate numelts",_tag_dyneither(_tmpBAF,sizeof(char),24))),_tag_dyneither(_tmpBAE,sizeof(void*),0)));}
e->r=_tmp520->r;goto _LL26E;_LL271: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51E;if(_tmp521->tag != 5)goto _LL273;else{_tmp522=(_tmp521->f1).elt_typ;_tmp523=((_tmp521->f1).ptr_atts).nullable;_tmp524=((_tmp521->f1).ptr_atts).bounds;_tmp525=((_tmp521->f1).ptr_atts).zero_term;}}_LL272:
# 2371
{void*_tmp528=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp524);struct Cyc_Absyn_Exp*_tmp52B;_LL276: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp529=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp528;if(_tmp529->tag != 0)goto _LL278;}_LL277:
# 2373
{struct Cyc_Absyn_Exp*_tmpBB0[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpBB0[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp522),0),((_tmpBB0[0]=(struct Cyc_Absyn_Exp*)_tmp49F->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB0,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL275;_LL278: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp52A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp528;if(_tmp52A->tag != 1)goto _LL275;else{_tmp52B=_tmp52A->f1;}}_LL279:
# 2379
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp525)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp49F->hd);
# 2383
struct Cyc_Absyn_Exp*_tmpBB1[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpBB1[1]=_tmp52B,((_tmpBB1[0]=(struct Cyc_Absyn_Exp*)_tmp49F->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB1,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp523)){
if(!Cyc_Evexp_c_can_eval(_tmp52B)){
const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,Cyc_Tcutil_terr(e->loc,((_tmpBB4="can't calculate numelts",_tag_dyneither(_tmpBB4,sizeof(char),24))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}
# 2388
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp52B,Cyc_Absyn_uint_exp(0,0));}else{
# 2390
e->r=_tmp52B->r;goto _LL275;}}
# 2392
goto _LL275;_LL275:;}
# 2394
goto _LL26E;_LL273:;_LL274: {
# 2396
const char*_tmpBB9;void*_tmpBB8[2];struct Cyc_String_pa_PrintArg_struct _tmpBB7;struct Cyc_String_pa_PrintArg_struct _tmpBB6;(_tmpBB6.tag=0,((_tmpBB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB7.tag=0,((_tmpBB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB8[0]=& _tmpBB7,((_tmpBB8[1]=& _tmpBB6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB9="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBB9,sizeof(char),41))),_tag_dyneither(_tmpBB8,sizeof(void*),2)))))))))))));}_LL26E:;}
# 2399
break;}case Cyc_Absyn_Plus: _LL26D:
# 2404
{void*_tmp534=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp51D))->hd);void*_tmp536;union Cyc_Absyn_Constraint*_tmp537;union Cyc_Absyn_Constraint*_tmp538;_LL27C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp534;if(_tmp535->tag != 5)goto _LL27E;else{_tmp536=(_tmp535->f1).elt_typ;_tmp537=((_tmp535->f1).ptr_atts).bounds;_tmp538=((_tmp535->f1).ptr_atts).zero_term;}}_LL27D:
# 2406
{void*_tmp539=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp537);struct Cyc_Absyn_Exp*_tmp53C;_LL281: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp53A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp539;if(_tmp53A->tag != 0)goto _LL283;}_LL282: {
# 2408
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpBBA[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBBA[2]=e2,((_tmpBBA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp536),0),((_tmpBBA[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBA,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL280;}_LL283: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp53B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp539;if(_tmp53B->tag != 1)goto _LL280;else{_tmp53C=_tmp53B->f1;}}_LL284:
# 2414
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp538)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;
struct Cyc_Absyn_Exp*_tmpBBB[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpBBB[2]=e2,((_tmpBBB[1]=_tmp53C,((_tmpBBB[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBB,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2419
goto _LL280;_LL280:;}
# 2421
goto _LL27B;_LL27E:;_LL27F:
# 2423
 goto _LL27B;_LL27B:;}
# 2425
break;case Cyc_Absyn_Minus: _LL27A: {
# 2430
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp51D))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp51D->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2440
struct Cyc_Absyn_Exp*_tmpBBC[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBBC[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpBBC[1]=
# 2441
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBBC[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2445
break;}case Cyc_Absyn_Eq: _LL285:
 goto _LL286;case Cyc_Absyn_Neq: _LL286:
 goto _LL287;case Cyc_Absyn_Gt: _LL287:
 goto _LL288;case Cyc_Absyn_Gte: _LL288:
 goto _LL289;case Cyc_Absyn_Lt: _LL289:
 goto _LL28A;case Cyc_Absyn_Lte: _LL28A: {
# 2453
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp49F->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp51D))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp51D->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL28B:
 break;}
# 2465
goto _LL212;}_LL21B: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4A0=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4A0->tag != 4)goto _LL21D;else{_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;}}_LL21C: {
# 2467
void*e2_cyc_typ=(void*)_check_null(_tmp4A1->topt);
# 2476 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBBD;struct _dyneither_ptr incr_str=(_tmpBBD="increment",_tag_dyneither(_tmpBBD,sizeof(char),10));
if(_tmp4A2 == Cyc_Absyn_PreDec  || _tmp4A2 == Cyc_Absyn_PostDec){const char*_tmpBBE;incr_str=((_tmpBBE="decrement",_tag_dyneither(_tmpBBE,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4A1,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBC2;void*_tmpBC1[1];struct Cyc_String_pa_PrintArg_struct _tmpBC0;(_tmpBC0.tag=0,((_tmpBC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBC1[0]=& _tmpBC0,Cyc_Tcutil_terr(e->loc,((_tmpBC2="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBC2,sizeof(char),74))),_tag_dyneither(_tmpBC1,sizeof(void*),1)))))));}{
const char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="in-place inc/dec on zero-term",_tag_dyneither(_tmpBC5,sizeof(char),30))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));};}{
# 2485
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4A1,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp546=Cyc_Absyn_signed_int_exp(1,0);
_tmp546->topt=Cyc_Absyn_sint_typ;
switch(_tmp4A2){case Cyc_Absyn_PreInc: _LL28D:
# 2492
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBCB;struct Cyc_Core_Opt*_tmpBCA;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBC9;e->r=(void*)((_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9[0]=((_tmpBCB.tag=3,((_tmpBCB.f1=_tmp4A1,((_tmpBCB.f2=((_tmpBCA=_cycalloc_atomic(sizeof(*_tmpBCA)),((_tmpBCA->v=(void*)Cyc_Absyn_Plus,_tmpBCA)))),((_tmpBCB.f3=_tmp546,_tmpBCB)))))))),_tmpBC9))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL28E:
# 2496
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBD1;struct Cyc_Core_Opt*_tmpBD0;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBCF;e->r=(void*)((_tmpBCF=_cycalloc(sizeof(*_tmpBCF)),((_tmpBCF[0]=((_tmpBD1.tag=3,((_tmpBD1.f1=_tmp4A1,((_tmpBD1.f2=((_tmpBD0=_cycalloc_atomic(sizeof(*_tmpBD0)),((_tmpBD0->v=(void*)Cyc_Absyn_Minus,_tmpBD0)))),((_tmpBD1.f3=_tmp546,_tmpBD1)))))))),_tmpBCF))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL28F:
# 2500
{const char*_tmpBD5;void*_tmpBD4[1];struct Cyc_String_pa_PrintArg_struct _tmpBD3;(_tmpBD3.tag=0,((_tmpBD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD4[0]=& _tmpBD3,Cyc_Tcutil_terr(e->loc,((_tmpBD5="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBD5,sizeof(char),59))),_tag_dyneither(_tmpBD4,sizeof(void*),1)))))));}{
# 2502
const char*_tmpBD8;void*_tmpBD7;(_tmpBD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD8="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBD8,sizeof(char),34))),_tag_dyneither(_tmpBD7,sizeof(void*),0)));};}}
# 2505
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A1);
Cyc_Toc_set_lhs(nv,0);{
# 2510
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4A2 == Cyc_Absyn_PostInc  || _tmp4A2 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2516
if(_tmp4A2 == Cyc_Absyn_PreDec  || _tmp4A2 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpBD9[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD9[2]=
# 2520
Cyc_Absyn_signed_int_exp(change,0),((_tmpBD9[1]=
# 2519
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBD9[0]=
# 2518
Cyc_Toc_push_address_exp(_tmp4A1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD9,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2521
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2525
struct Cyc_Toc_functionSet*_tmp553=_tmp4A2 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2527
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp553,_tmp4A1);
struct Cyc_Absyn_Exp*_tmpBDA[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBDA[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBDA[0]=
# 2528
Cyc_Toc_push_address_exp(_tmp4A1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2530
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4A1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4A1,0,Cyc_Toc_incr_lvalue,_tmp4A2);
e->r=_tmp4A1->r;}}}
# 2534
goto _LL212;};};}_LL21D: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4A3=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4A3->tag != 3)goto _LL21F;else{_tmp4A4=_tmp4A3->f1;_tmp4A5=_tmp4A3->f2;_tmp4A6=_tmp4A3->f3;}}_LL21E: {
# 2553 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4A4->topt);
void*e2_old_typ=(void*)_check_null(_tmp4A6->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4A4,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4A6);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4A4,e1_old_typ,_tmp4A5,_tmp4A6,e2_old_typ,f_tag,tagged_member_struct_type);
# 2564
return;}{
# 2566
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4A4,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4A4,_tmp4A5,_tmp4A6,ptr_type,is_dyneither,elt_type);
# 2572
return;}{
# 2576
int e1_poly=Cyc_Toc_is_poly_project(_tmp4A4);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4A4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4A6);{
# 2582
int done=0;
if(_tmp4A5 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp4A5->v){case Cyc_Absyn_Plus: _LL291:
 change=_tmp4A6;break;case Cyc_Absyn_Minus: _LL292:
# 2590
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4A6,0);break;default: _LL293: {
const char*_tmpBDD;void*_tmpBDC;(_tmpBDC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDD="bad t ? pointer arithmetic",_tag_dyneither(_tmpBDD,sizeof(char),27))),_tag_dyneither(_tmpBDC,sizeof(void*),0)));}}
# 2593
done=1;{
# 2595
struct Cyc_Absyn_Exp*_tmp558=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBDE[3];e->r=Cyc_Toc_fncall_exp_r(_tmp558,((_tmpBDE[2]=change,((_tmpBDE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBDE[0]=
# 2596
Cyc_Toc_push_address_exp(_tmp4A4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2599
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2602
switch((enum Cyc_Absyn_Primop)_tmp4A5->v){case Cyc_Absyn_Plus: _LL295:
# 2604
 done=1;
{struct Cyc_Absyn_Exp*_tmpBDF[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4A4),((_tmpBDF[1]=_tmp4A6,((_tmpBDF[0]=_tmp4A4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL296: {
const char*_tmpBE2;void*_tmpBE1;(_tmpBE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE2="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBE2,sizeof(char),39))),_tag_dyneither(_tmpBE1,sizeof(void*),0)));}}}}
# 2611
if(!done){
# 2613
if(e1_poly)
_tmp4A6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4A6->r,0));
# 2619
if(!Cyc_Absyn_is_lvalue(_tmp4A4)){
{struct _tuple18 _tmpBE5;struct _tuple18*_tmpBE4;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))Cyc_Toc_lvalue_assign)(_tmp4A4,0,Cyc_Toc_assignop_lvalue,((_tmpBE4=_cycalloc(sizeof(struct _tuple18)* 1),((_tmpBE4[0]=(struct _tuple18)((_tmpBE5.f1=_tmp4A5,((_tmpBE5.f2=_tmp4A6,_tmpBE5)))),_tmpBE4)))));}
e->r=_tmp4A4->r;}}
# 2624
goto _LL212;};};};}_LL21F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4A7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4A7->tag != 5)goto _LL221;else{_tmp4A8=_tmp4A7->f1;_tmp4A9=_tmp4A7->f2;_tmp4AA=_tmp4A7->f3;}}_LL220:
# 2626
 Cyc_Toc_exp_to_c(nv,_tmp4A8);
Cyc_Toc_exp_to_c(nv,_tmp4A9);
Cyc_Toc_exp_to_c(nv,_tmp4AA);
goto _LL212;_LL221: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4AB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4AB->tag != 6)goto _LL223;else{_tmp4AC=_tmp4AB->f1;_tmp4AD=_tmp4AB->f2;}}_LL222:
# 2631
 Cyc_Toc_exp_to_c(nv,_tmp4AC);
Cyc_Toc_exp_to_c(nv,_tmp4AD);
goto _LL212;_LL223: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4AE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4AE->tag != 7)goto _LL225;else{_tmp4AF=_tmp4AE->f1;_tmp4B0=_tmp4AE->f2;}}_LL224:
# 2635
 Cyc_Toc_exp_to_c(nv,_tmp4AF);
Cyc_Toc_exp_to_c(nv,_tmp4B0);
goto _LL212;_LL225: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4B1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4B1->tag != 8)goto _LL227;else{_tmp4B2=_tmp4B1->f1;_tmp4B3=_tmp4B1->f2;}}_LL226:
# 2639
 Cyc_Toc_exp_to_c(nv,_tmp4B2);
Cyc_Toc_exp_to_c(nv,_tmp4B3);
goto _LL212;_LL227: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4B4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4B4->tag != 9)goto _LL229;else{_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;if(_tmp4B4->f3 != 0)goto _LL229;}}_LL228:
# 2643
 Cyc_Toc_exp_to_c(nv,_tmp4B5);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4B6);
goto _LL212;_LL229: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4B7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4B7->tag != 9)goto _LL22B;else{_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;if(_tmp4B7->f3 == 0)goto _LL22B;_tmp4BA=(_tmp4B7->f3)->num_varargs;_tmp4BB=(_tmp4B7->f3)->injectors;_tmp4BC=(_tmp4B7->f3)->vai;}}_LL22A: {
# 2655 "toc.cyc"
struct _RegionHandle _tmp55F=_new_region("r");struct _RegionHandle*r=& _tmp55F;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4BA,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4BC->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2663
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B9);
int num_normargs=num_args - _tmp4BA;
# 2667
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4B9=_tmp4B9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4B9))->hd);{
struct Cyc_List_List*_tmpBE6;new_args=((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6->hd=(struct Cyc_Absyn_Exp*)_tmp4B9->hd,((_tmpBE6->tl=new_args,_tmpBE6))))));};}}
# 2672
{struct Cyc_Absyn_Exp*_tmpBE9[3];struct Cyc_List_List*_tmpBE8;new_args=((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBE9[2]=num_varargs_exp,((_tmpBE9[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBE9[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBE8->tl=new_args,_tmpBE8))))));}
# 2677
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2679
Cyc_Toc_exp_to_c(nv,_tmp4B8);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4B8,new_args,0),0);
# 2683
if(_tmp4BC->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp563=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4BC->type));struct Cyc_Absyn_Datatypedecl*_tmp565;_LL299: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp563;if(_tmp564->tag != 3)goto _LL29B;else{if((((_tmp564->f1).datatype_info).KnownDatatype).tag != 2)goto _LL29B;_tmp565=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp564->f1).datatype_info).KnownDatatype).val);}}_LL29A:
 tud=_tmp565;goto _LL298;_LL29B:;_LL29C: {
const char*_tmpBEC;void*_tmpBEB;(_tmpBEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEC="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBEC,sizeof(char),44))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}_LL298:;}{
# 2689
unsigned int _tmpBF1;unsigned int _tmpBF0;struct _dyneither_ptr _tmpBEF;struct _tuple0**_tmpBEE;unsigned int _tmpBED;struct _dyneither_ptr vs=(_tmpBED=(unsigned int)_tmp4BA,((_tmpBEE=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpBED)),((_tmpBEF=_tag_dyneither(_tmpBEE,sizeof(struct _tuple0*),_tmpBED),((((_tmpBF0=_tmpBED,_tmpBF2(& _tmpBF1,& _tmpBF0,& _tmpBEE))),_tmpBEF)))))));
# 2691
if(_tmp4BA != 0){
# 2693
struct Cyc_List_List*_tmp568=0;
{int i=_tmp4BA - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBF3;_tmp568=((_tmpBF3=_cycalloc(sizeof(*_tmpBF3)),((_tmpBF3->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpBF3->tl=_tmp568,_tmpBF3))))));}}
# 2697
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp568,0),s,0);{
# 2700
int i=0;for(0;_tmp4B9 != 0;(((_tmp4B9=_tmp4B9->tl,_tmp4BB=_tmp4BB->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4B9->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp56B;struct Cyc_List_List*_tmp56C;struct Cyc_Absyn_Datatypefield*_tmp56A=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4BB))->hd;_tmp56B=_tmp56A->name;_tmp56C=_tmp56A->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp56C))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2715
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2718
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp56B),0),s,0);
# 2721
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp56B,tud->name)),0,s,0);};}};}else{
# 2728
struct _tuple15*_tmpBF4[3];struct Cyc_List_List*_tmp56D=(_tmpBF4[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF4[1]=
# 2728
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF4[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF4,sizeof(struct _tuple15*),3)))))));
# 2730
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2740
{int i=0;for(0;_tmp4B9 != 0;(_tmp4B9=_tmp4B9->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4B9->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4B9->hd,0),s,0);}}
# 2746
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2759 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2761
_npop_handler(0);goto _LL212;
# 2655 "toc.cyc"
;_pop_region(r);}_LL22B: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4BD=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4BD->tag != 10)goto _LL22D;else{_tmp4BE=_tmp4BD->f1;}}_LL22C:
# 2764 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4BE);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4BE),0))->r;
goto _LL212;_LL22D: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4BF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4BF->tag != 11)goto _LL22F;else{_tmp4C0=_tmp4BF->f1;}}_LL22E:
 Cyc_Toc_exp_to_c(nv,_tmp4C0);goto _LL212;_LL22F: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4C1=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4C1->tag != 12)goto _LL231;else{_tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;}}_LL230:
# 2769
 Cyc_Toc_exp_to_c(nv,_tmp4C2);
# 2778 "toc.cyc"
for(0;_tmp4C3 != 0;_tmp4C3=_tmp4C3->tl){
enum Cyc_Absyn_KindQual _tmp573=(Cyc_Tcutil_typ_kind((void*)_tmp4C3->hd))->kind;
if(_tmp573 != Cyc_Absyn_EffKind  && _tmp573 != Cyc_Absyn_RgnKind){
{void*_tmp574=Cyc_Tcutil_compress((void*)_tmp4C3->hd);_LL29E: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp574;if(_tmp575->tag != 2)goto _LL2A0;}_LL29F:
 goto _LL2A1;_LL2A0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp574;if(_tmp576->tag != 3)goto _LL2A2;}_LL2A1:
 continue;_LL2A2:;_LL2A3:
# 2785
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4C2,0))->r;
goto _LL29D;_LL29D:;}
# 2788
break;}}
# 2791
goto _LL212;_LL231: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4C4->tag != 13)goto _LL233;else{_tmp4C5=(void**)((void**)& _tmp4C4->f1);_tmp4C6=_tmp4C4->f2;_tmp4C7=_tmp4C4->f3;_tmp4C8=_tmp4C4->f4;}}_LL232: {
# 2793
void*old_t2=(void*)_check_null(_tmp4C6->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4C5;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4C5=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4C6);
# 2800
{struct _tuple19 _tmpBF5;struct _tuple19 _tmp578=(_tmpBF5.f1=Cyc_Tcutil_compress(old_t2),((_tmpBF5.f2=Cyc_Tcutil_compress(new_typ),_tmpBF5)));struct Cyc_Absyn_PtrInfo _tmp57A;struct Cyc_Absyn_PtrInfo _tmp57C;struct Cyc_Absyn_PtrInfo _tmp57E;_LL2A5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp578.f1;if(_tmp579->tag != 5)goto _LL2A7;else{_tmp57A=_tmp579->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp578.f2;if(_tmp57B->tag != 5)goto _LL2A7;else{_tmp57C=_tmp57B->f1;}};_LL2A6: {
# 2802
int _tmp580=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57A.ptr_atts).nullable);
int _tmp581=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57C.ptr_atts).nullable);
void*_tmp582=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57A.ptr_atts).bounds);
void*_tmp583=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57C.ptr_atts).bounds);
int _tmp584=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57A.ptr_atts).zero_term);
int _tmp585=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp57C.ptr_atts).zero_term);
{struct _tuple19 _tmpBF6;struct _tuple19 _tmp587=(_tmpBF6.f1=_tmp582,((_tmpBF6.f2=_tmp583,_tmpBF6)));struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Exp*_tmp58B;struct Cyc_Absyn_Exp*_tmp58D;struct Cyc_Absyn_Exp*_tmp591;_LL2AC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp588=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp587.f1;if(_tmp588->tag != 1)goto _LL2AE;else{_tmp589=_tmp588->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp587.f2;if(_tmp58A->tag != 1)goto _LL2AE;else{_tmp58B=_tmp58A->f1;}};_LL2AD:
# 2810
 if((!Cyc_Evexp_c_can_eval(_tmp589) || !Cyc_Evexp_c_can_eval(_tmp58B)) && !
Cyc_Evexp_same_const_exp(_tmp589,_tmp58B)){
const char*_tmpBF9;void*_tmpBF8;(_tmpBF8=0,Cyc_Tcutil_terr(e->loc,((_tmpBF9="can't validate cast due to potential size differences",_tag_dyneither(_tmpBF9,sizeof(char),54))),_tag_dyneither(_tmpBF8,sizeof(void*),0)));}
if(_tmp580  && !_tmp581){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFC;void*_tmpBFB;(_tmpBFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFC="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBFC,sizeof(char),44))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}
# 2819
if(_tmp4C8 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC00;void*_tmpBFF[1];struct Cyc_String_pa_PrintArg_struct _tmpBFE;(_tmpBFE.tag=0,((_tmpBFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBFF[0]=& _tmpBFE,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC00="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC00,sizeof(char),41))),_tag_dyneither(_tmpBFF,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4C6);
if(do_null_check){
if(!_tmp4C7){
const char*_tmpC03;void*_tmpC02;(_tmpC02=0,Cyc_Tcutil_warn(e->loc,((_tmpC03="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC03,sizeof(char),58))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}{
# 2826
struct Cyc_List_List*_tmpC04;e->r=Cyc_Toc_cast_it_r(*_tmp4C5,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->hd=_tmp4C6,((_tmpC04->tl=0,_tmpC04)))))),0));};}else{
# 2829
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C6->r;}};}else{
# 2834
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C6->r;}
# 2841
goto _LL2AB;_LL2AE:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp587.f1;if(_tmp58C->tag != 1)goto _LL2B0;else{_tmp58D=_tmp58C->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp587.f2;if(_tmp58E->tag != 0)goto _LL2B0;};_LL2AF:
# 2843
 if(!Cyc_Evexp_c_can_eval(_tmp58D)){
const char*_tmpC07;void*_tmpC06;(_tmpC06=0,Cyc_Tcutil_terr(e->loc,((_tmpC07="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC07,sizeof(char),71))),_tag_dyneither(_tmpC06,sizeof(void*),0)));}
# 2846
if(_tmp4C8 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC0B;void*_tmpC0A[1];struct Cyc_String_pa_PrintArg_struct _tmpC09;(_tmpC09.tag=0,((_tmpC09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC0A[0]=& _tmpC09,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC0B="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC0B,sizeof(char),44))),_tag_dyneither(_tmpC0A,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2850
if((_tmp584  && !(_tmp57C.elt_tq).real_const) && !_tmp585)
# 2853
_tmp58D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp58D,Cyc_Absyn_uint_exp(1,0),0);
# 2855
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp58D,_tmp4C6))->r;}else{
# 2857
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Toc__tag_dyneither_e;
# 2859
if(_tmp584){
# 2864
struct _tuple0*_tmp5A4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5A5=Cyc_Absyn_var_exp(_tmp5A4,0);
struct Cyc_Absyn_Exp*arg3;
# 2869
{void*_tmp5A6=_tmp4C6->r;_LL2B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A6;if(_tmp5A7->tag != 0)goto _LL2B7;else{if(((_tmp5A7->f1).String_c).tag != 8)goto _LL2B7;}}_LL2B6:
# 2871
 arg3=_tmp58D;goto _LL2B4;_LL2B7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5A8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5A6;if(_tmp5A8->tag != 0)goto _LL2B9;else{if(((_tmp5A8->f1).Wstring_c).tag != 9)goto _LL2B9;}}_LL2B8:
# 2873
 arg3=_tmp58D;goto _LL2B4;_LL2B9:;_LL2BA:
# 2875
{struct Cyc_Absyn_Exp*_tmpC0C[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4C6),(
# 2877
(_tmpC0C[1]=_tmp58D,((_tmpC0C[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5A5),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B4;_LL2B4:;}
# 2880
if(!_tmp585  && !(_tmp57C.elt_tq).real_const)
# 2883
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2885
struct Cyc_Absyn_Exp*_tmp5AA=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57C.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC0D[3];struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Absyn_fncall_exp(_tmp5A3,((_tmpC0D[2]=arg3,((_tmpC0D[1]=_tmp5AA,((_tmpC0D[0]=_tmp5A5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5AC=Cyc_Absyn_exp_stmt(_tmp5AB,0);
_tmp5AC=Cyc_Absyn_declare_stmt(_tmp5A4,Cyc_Toc_typ_to_c(old_t2),_tmp4C6,_tmp5AC,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5AC);};}else{
# 2892
struct Cyc_Absyn_Exp*_tmpC0E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5A3,(
(_tmpC0E[2]=_tmp58D,((_tmpC0E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57C.elt_typ),0),((_tmpC0E[0]=_tmp4C6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2898
goto _LL2AB;_LL2B0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp587.f1;if(_tmp58F->tag != 0)goto _LL2B2;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp590=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp587.f2;if(_tmp590->tag != 1)goto _LL2B2;else{_tmp591=_tmp590->f1;}};_LL2B1:
# 2900
 if(!Cyc_Evexp_c_can_eval(_tmp591)){
const char*_tmpC11;void*_tmpC10;(_tmpC10=0,Cyc_Tcutil_terr(e->loc,((_tmpC11="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC11,sizeof(char),71))),_tag_dyneither(_tmpC10,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC14;void*_tmpC13;(_tmpC13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC14="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC14,sizeof(char),45))),_tag_dyneither(_tmpC13,sizeof(void*),0)));}{
# 2912 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5B3=_tmp591;
if(_tmp584  && !_tmp585)
_tmp5B3=Cyc_Absyn_add_exp(_tmp591,Cyc_Absyn_uint_exp(1,0),0);{
# 2919
struct Cyc_Absyn_Exp*_tmp5B4=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC15[3];struct Cyc_Absyn_Exp*_tmp5B5=Cyc_Absyn_fncall_exp(_tmp5B4,(
(_tmpC15[2]=_tmp5B3,((_tmpC15[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57A.elt_typ),0),((_tmpC15[0]=_tmp4C6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC15,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2924
if(_tmp581){
struct Cyc_List_List*_tmpC16;_tmp5B5->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16->hd=Cyc_Absyn_copy_exp(_tmp5B5),((_tmpC16->tl=0,_tmpC16)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp4C5,_tmp5B5);
goto _LL2AB;};};_LL2B2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp592=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp587.f1;if(_tmp592->tag != 0)goto _LL2AB;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp593=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp587.f2;if(_tmp593->tag != 0)goto _LL2AB;};_LL2B3:
# 2932
 DynCast:
 if((_tmp584  && !_tmp585) && !(_tmp57C.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC19;void*_tmpC18;(_tmpC18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC19="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC19,sizeof(char),70))),_tag_dyneither(_tmpC18,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5BA=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC1A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5BA,(
(_tmpC1A[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC1A[1]=
# 2938
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp57A.elt_typ),0),((_tmpC1A[0]=_tmp4C6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2941
goto _LL2AB;_LL2AB:;}
# 2943
goto _LL2A4;}_LL2A7:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp578.f1;if(_tmp57D->tag != 5)goto _LL2A9;else{_tmp57E=_tmp57D->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp57F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp578.f2;if(_tmp57F->tag != 6)goto _LL2A9;};_LL2A8:
# 2945
{void*_tmp5BC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp57E.ptr_atts).bounds);_LL2BC: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5BC;if(_tmp5BD->tag != 0)goto _LL2BE;}_LL2BD:
# 2947
 _tmp4C6->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4C6->r,_tmp4C6->loc),Cyc_Toc_curr_sp);goto _LL2BB;_LL2BE:;_LL2BF:
 goto _LL2BB;_LL2BB:;}
# 2950
goto _LL2A4;_LL2A9:;_LL2AA:
# 2952
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4C6->r;
goto _LL2A4;_LL2A4:;}
# 2956
goto _LL212;}_LL233: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp4C9=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4C9->tag != 14)goto _LL235;else{_tmp4CA=_tmp4C9->f1;}}_LL234:
# 2960
{void*_tmp5BE=_tmp4CA->r;struct _tuple0*_tmp5C0;struct Cyc_List_List*_tmp5C1;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*_tmp5C4;_LL2C1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5BF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5BF->tag != 28)goto _LL2C3;else{_tmp5C0=_tmp5BF->f1;_tmp5C1=_tmp5BF->f2;_tmp5C2=_tmp5BF->f3;}}_LL2C2:
# 2962
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC1E;void*_tmpC1D[1];struct Cyc_String_pa_PrintArg_struct _tmpC1C;(_tmpC1C.tag=0,((_tmpC1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4CA->loc)),((_tmpC1D[0]=& _tmpC1C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1E="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC1E,sizeof(char),42))),_tag_dyneither(_tmpC1D,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp5C8=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4CA->topt),_tmp5C1,1,0,_tmp5C2,_tmp5C0);
e->r=_tmp5C8->r;
e->topt=_tmp5C8->topt;
goto _LL2C0;};_LL2C3: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5C3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5BE;if(_tmp5C3->tag != 23)goto _LL2C5;else{_tmp5C4=_tmp5C3->f1;}}_LL2C4:
# 2970
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC22;void*_tmpC21[1];struct Cyc_String_pa_PrintArg_struct _tmpC20;(_tmpC20.tag=0,((_tmpC20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4CA->loc)),((_tmpC21[0]=& _tmpC20,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC22="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC22,sizeof(char),42))),_tag_dyneither(_tmpC21,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp5C4))->r;
goto _LL2C0;_LL2C5:;_LL2C6:
# 2976
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4CA);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4CA)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4CA,0,Cyc_Toc_address_lvalue,1);
# 2982
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp4CA);}
# 2984
goto _LL2C0;_LL2C0:;}
# 2986
goto _LL212;_LL235: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp4CB=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4CB->tag != 15)goto _LL237;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;}}_LL236:
# 2989
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC26;void*_tmpC25[1];struct Cyc_String_pa_PrintArg_struct _tmpC24;(_tmpC24.tag=0,((_tmpC24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4CD->loc)),((_tmpC25[0]=& _tmpC24,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC26="%s: new at top-level",_tag_dyneither(_tmpC26,sizeof(char),21))),_tag_dyneither(_tmpC25,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp4CD->topt);
{void*_tmp5CF=_tmp4CD->r;struct Cyc_List_List*_tmp5D1;struct Cyc_Absyn_Vardecl*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D5;int _tmp5D6;struct Cyc_Absyn_Exp*_tmp5D8;void*_tmp5D9;int _tmp5DA;struct _tuple0*_tmp5DC;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DE;struct Cyc_Absyn_Aggrdecl*_tmp5DF;struct Cyc_List_List*_tmp5E1;_LL2C8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5D0=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5CF;if(_tmp5D0->tag != 25)goto _LL2CA;else{_tmp5D1=_tmp5D0->f1;}}_LL2C9: {
# 2997
struct _tuple0*_tmp5E2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5E3=Cyc_Absyn_var_exp(_tmp5E2,0);
struct Cyc_Absyn_Stmt*_tmp5E4=Cyc_Toc_init_array(nv,new_e_type,_tmp5E3,_tmp5D1,Cyc_Absyn_exp_stmt(_tmp5E3,0));
void*old_elt_typ;
{void*_tmp5E5=Cyc_Tcutil_compress(old_typ);void*_tmp5E7;struct Cyc_Absyn_Tqual _tmp5E8;union Cyc_Absyn_Constraint*_tmp5E9;_LL2D5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E5;if(_tmp5E6->tag != 5)goto _LL2D7;else{_tmp5E7=(_tmp5E6->f1).elt_typ;_tmp5E8=(_tmp5E6->f1).elt_tq;_tmp5E9=((_tmp5E6->f1).ptr_atts).zero_term;}}_LL2D6:
# 3003
 old_elt_typ=_tmp5E7;goto _LL2D4;_LL2D7:;_LL2D8: {
# 3005
const char*_tmpC29;void*_tmpC28;old_elt_typ=(
(_tmpC28=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC29="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC29,sizeof(char),52))),_tag_dyneither(_tmpC28,sizeof(void*),0))));}_LL2D4:;}{
# 3008
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp5EC=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp5ED=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5D1),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp4CC == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp5ED);else{
# 3016
struct Cyc_Absyn_Exp*r=_tmp4CC;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp5ED);}
# 3021
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp5E2,_tmp5EC,e1,_tmp5E4,0));
goto _LL2C7;};}_LL2CA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5D2=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5CF;if(_tmp5D2->tag != 26)goto _LL2CC;else{_tmp5D3=_tmp5D2->f1;_tmp5D4=_tmp5D2->f2;_tmp5D5=_tmp5D2->f3;_tmp5D6=_tmp5D2->f4;}}_LL2CB: {
# 3034 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp5EE=Cyc_Tcutil_compress(old_typ);void*_tmp5F0;struct Cyc_Absyn_Tqual _tmp5F1;union Cyc_Absyn_Constraint*_tmp5F2;union Cyc_Absyn_Constraint*_tmp5F3;_LL2DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EE;if(_tmp5EF->tag != 5)goto _LL2DC;else{_tmp5F0=(_tmp5EF->f1).elt_typ;_tmp5F1=(_tmp5EF->f1).elt_tq;_tmp5F2=((_tmp5EF->f1).ptr_atts).bounds;_tmp5F3=((_tmp5EF->f1).ptr_atts).zero_term;}}_LL2DB:
# 3037
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5F2)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2D9;_LL2DC:;_LL2DD: {
const char*_tmpC2C;void*_tmpC2B;(_tmpC2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2C="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC2C,sizeof(char),42))),_tag_dyneither(_tmpC2B,sizeof(void*),0)));}_LL2D9:;}{
# 3041
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp5D5->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5D4);{
struct Cyc_Absyn_Exp*_tmp5F6=Cyc_Absyn_var_exp(max,0);
# 3049
if(_tmp5D6)
_tmp5F6=Cyc_Absyn_add_exp(_tmp5F6,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),_tmp5D3,Cyc_Absyn_var_exp(max,0),_tmp5D5,_tmp5D6,
Cyc_Toc_skip_stmt_dl(),1);
# 3055
struct _RegionHandle _tmp5F7=_new_region("r");struct _RegionHandle*r=& _tmp5F7;_push_region(r);{
struct _tuple20*_tmpC2F;struct Cyc_List_List*_tmpC2E;struct Cyc_List_List*decls=
(_tmpC2E=_region_malloc(r,sizeof(*_tmpC2E)),((_tmpC2E->hd=((_tmpC2F=_region_malloc(r,sizeof(*_tmpC2F)),((_tmpC2F->f1=max,((_tmpC2F->f2=Cyc_Absyn_uint_typ,((_tmpC2F->f3=_tmp5D4,_tmpC2F)))))))),((_tmpC2E->tl=0,_tmpC2E)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4CC == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC30[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC30[1]=_tmp5F6,((_tmpC30[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC30,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3065
struct Cyc_Absyn_Exp*r=_tmp4CC;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC31[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC31[1]=_tmp5F6,((_tmpC31[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC31,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3072
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple20*_tmpC34;struct Cyc_List_List*_tmpC33;decls=((_tmpC33=_region_malloc(r,sizeof(*_tmpC33)),((_tmpC33->hd=((_tmpC34=_region_malloc(r,sizeof(*_tmpC34)),((_tmpC34->f1=a,((_tmpC34->f2=ptr_typ,((_tmpC34->f3=ainit,_tmpC34)))))))),((_tmpC33->tl=decls,_tmpC33))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp5FC=Cyc_Toc_temp_var();
void*_tmp5FD=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp5FE=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC35[3];struct Cyc_Absyn_Exp*_tmp5FF=
Cyc_Absyn_fncall_exp(_tmp5FE,(
(_tmpC35[2]=_tmp5F6,((_tmpC35[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC35[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC35,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3082
{struct _tuple20*_tmpC38;struct Cyc_List_List*_tmpC37;decls=((_tmpC37=_region_malloc(r,sizeof(*_tmpC37)),((_tmpC37->hd=((_tmpC38=_region_malloc(r,sizeof(*_tmpC38)),((_tmpC38->f1=_tmp5FC,((_tmpC38->f2=_tmp5FD,((_tmpC38->f3=_tmp5FF,_tmpC38)))))))),((_tmpC37->tl=decls,_tmpC37))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp5FC,0),0),0);}else{
# 3085
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp603=decls;for(0;_tmp603 != 0;_tmp603=_tmp603->tl){
struct _tuple0*_tmp605;void*_tmp606;struct Cyc_Absyn_Exp*_tmp607;struct _tuple20 _tmp604=*((struct _tuple20*)_tmp603->hd);_tmp605=_tmp604.f1;_tmp606=_tmp604.f2;_tmp607=_tmp604.f3;
s=Cyc_Absyn_declare_stmt(_tmp605,_tmp606,_tmp607,s,0);}}
# 3090
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3092
_npop_handler(0);goto _LL2C7;
# 3055
;_pop_region(r);};};};}_LL2CC: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5D7=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5CF;if(_tmp5D7->tag != 27)goto _LL2CE;else{_tmp5D8=_tmp5D7->f1;_tmp5D9=(void*)_tmp5D7->f2;_tmp5DA=_tmp5D7->f3;}}_LL2CD: {
# 3096
int is_dyneither_ptr=0;
{void*_tmp60A=Cyc_Tcutil_compress(old_typ);void*_tmp60C;struct Cyc_Absyn_Tqual _tmp60D;union Cyc_Absyn_Constraint*_tmp60E;union Cyc_Absyn_Constraint*_tmp60F;_LL2DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60A;if(_tmp60B->tag != 5)goto _LL2E1;else{_tmp60C=(_tmp60B->f1).elt_typ;_tmp60D=(_tmp60B->f1).elt_tq;_tmp60E=((_tmp60B->f1).ptr_atts).bounds;_tmp60F=((_tmp60B->f1).ptr_atts).zero_term;}}_LL2E0:
# 3099
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp60E)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL2DE;_LL2E1:;_LL2E2: {
const char*_tmpC3B;void*_tmpC3A;(_tmpC3A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3B="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC3B,sizeof(char),42))),_tag_dyneither(_tmpC3A,sizeof(void*),0)));}_LL2DE:;}{
# 3103
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=_tmp5D9;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp5D8);{
struct Cyc_Absyn_Exp*_tmp612=Cyc_Absyn_var_exp(max,0);
# 3111
struct Cyc_Absyn_Stmt*s;
if(_tmp5DA){
_tmp612=Cyc_Absyn_add_exp(_tmp612,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0);
s=Cyc_Absyn_exp_stmt(ex,0);};}else{
# 3119
s=Cyc_Absyn_skip_stmt(0);}{
# 3121
struct _RegionHandle _tmp613=_new_region("r");struct _RegionHandle*r=& _tmp613;_push_region(r);{
struct _tuple20*_tmpC3E;struct Cyc_List_List*_tmpC3D;struct Cyc_List_List*decls=
(_tmpC3D=_region_malloc(r,sizeof(*_tmpC3D)),((_tmpC3D->hd=((_tmpC3E=_region_malloc(r,sizeof(*_tmpC3E)),((_tmpC3E->f1=max,((_tmpC3E->f2=Cyc_Absyn_uint_typ,((_tmpC3E->f3=_tmp5D8,_tmpC3E)))))))),((_tmpC3D->tl=0,_tmpC3D)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp4CC == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC3F[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC3F[1]=_tmp612,((_tmpC3F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3131
struct Cyc_Absyn_Exp*r=_tmp4CC;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC40[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC40[1]=_tmp612,((_tmpC40[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC40,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3138
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple20*_tmpC43;struct Cyc_List_List*_tmpC42;decls=((_tmpC42=_region_malloc(r,sizeof(*_tmpC42)),((_tmpC42->hd=((_tmpC43=_region_malloc(r,sizeof(*_tmpC43)),((_tmpC43->f1=a,((_tmpC43->f2=ptr_typ,((_tmpC43->f3=ainit,_tmpC43)))))))),((_tmpC42->tl=decls,_tmpC42))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp618=Cyc_Toc_temp_var();
void*_tmp619=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp61A=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC44[3];struct Cyc_Absyn_Exp*_tmp61B=
Cyc_Absyn_fncall_exp(_tmp61A,(
(_tmpC44[2]=_tmp612,((_tmpC44[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC44[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3148
{struct _tuple20*_tmpC47;struct Cyc_List_List*_tmpC46;decls=((_tmpC46=_region_malloc(r,sizeof(*_tmpC46)),((_tmpC46->hd=((_tmpC47=_region_malloc(r,sizeof(*_tmpC47)),((_tmpC47->f1=_tmp618,((_tmpC47->f2=_tmp619,((_tmpC47->f3=_tmp61B,_tmpC47)))))))),((_tmpC46->tl=decls,_tmpC46))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp618,0),0),0);}else{
# 3151
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp61F=decls;for(0;_tmp61F != 0;_tmp61F=_tmp61F->tl){
struct _tuple0*_tmp621;void*_tmp622;struct Cyc_Absyn_Exp*_tmp623;struct _tuple20 _tmp620=*((struct _tuple20*)_tmp61F->hd);_tmp621=_tmp620.f1;_tmp622=_tmp620.f2;_tmp623=_tmp620.f3;
s=Cyc_Absyn_declare_stmt(_tmp621,_tmp622,_tmp623,s,0);}}
# 3156
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3158
_npop_handler(0);goto _LL2C7;
# 3121
;_pop_region(r);};};};}_LL2CE: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5DB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5CF;if(_tmp5DB->tag != 28)goto _LL2D0;else{_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;_tmp5DE=_tmp5DB->f3;_tmp5DF=_tmp5DB->f4;}}_LL2CF: {
# 3162
struct Cyc_Absyn_Exp*_tmp626=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp4CD->topt),_tmp5DD,1,_tmp4CC,_tmp5DE,_tmp5DC);
e->r=_tmp626->r;
e->topt=_tmp626->topt;
goto _LL2C7;}_LL2D0: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5E0=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5CF;if(_tmp5E0->tag != 23)goto _LL2D2;else{_tmp5E1=_tmp5E0->f1;}}_LL2D1:
# 3168
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp4CC,_tmp5E1))->r;
goto _LL2C7;_LL2D2:;_LL2D3: {
# 3175
void*old_elt_typ=(void*)_check_null(_tmp4CD->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3178
struct _tuple0*_tmp627=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp628=Cyc_Absyn_var_exp(_tmp627,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp628,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp4CC == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3185
struct Cyc_Absyn_Exp*r=_tmp4CC;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3193
int done=0;
{void*_tmp629=_tmp4CD->r;void*_tmp62B;struct Cyc_Absyn_Exp*_tmp62C;_LL2E4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp62A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp629;if(_tmp62A->tag != 13)goto _LL2E6;else{_tmp62B=(void*)_tmp62A->f1;_tmp62C=_tmp62A->f2;}}_LL2E5:
# 3196
{struct _tuple19 _tmpC48;struct _tuple19 _tmp62E=(_tmpC48.f1=Cyc_Tcutil_compress(_tmp62B),((_tmpC48.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp62C->topt)),_tmpC48)));void*_tmp630;union Cyc_Absyn_Constraint*_tmp631;union Cyc_Absyn_Constraint*_tmp633;_LL2E9:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp62F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62E.f1;if(_tmp62F->tag != 5)goto _LL2EB;else{_tmp630=(_tmp62F->f1).elt_typ;_tmp631=((_tmp62F->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp632=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62E.f2;if(_tmp632->tag != 5)goto _LL2EB;else{_tmp633=((_tmp632->f1).ptr_atts).bounds;}};_LL2EA:
# 3199
{struct _tuple19 _tmpC49;struct _tuple19 _tmp635=(_tmpC49.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp631),((_tmpC49.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp633),_tmpC49)));struct Cyc_Absyn_Exp*_tmp638;_LL2EE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp636=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp635.f1;if(_tmp636->tag != 0)goto _LL2F0;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp637=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp635.f2;if(_tmp637->tag != 1)goto _LL2F0;else{_tmp638=_tmp637->f1;}};_LL2EF:
# 3201
 Cyc_Toc_exp_to_c(nv,_tmp62C);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp639=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC4A[4];e->r=Cyc_Toc_fncall_exp_r(_tmp639,(
(_tmpC4A[3]=_tmp638,((_tmpC4A[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp630),0),((_tmpC4A[1]=_tmp62C,((_tmpC4A[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4A,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3209
goto _LL2ED;};_LL2F0:;_LL2F1:
 goto _LL2ED;_LL2ED:;}
# 3212
goto _LL2E8;_LL2EB:;_LL2EC:
 goto _LL2E8;_LL2E8:;}
# 3215
goto _LL2E3;_LL2E6:;_LL2E7:
 goto _LL2E3;_LL2E3:;}
# 3218
if(!done){
struct Cyc_Absyn_Stmt*_tmp63B=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp628),0);
struct Cyc_Absyn_Exp*_tmp63C=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp4CD);
_tmp63B=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp628,_tmp63C,0),_tmp4CD,0),_tmp63B,0);{
# 3224
void*_tmp63D=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp627,_tmp63D,mexp,_tmp63B,0));};}
# 3227
goto _LL2C7;};}_LL2C7:;}
# 3229
goto _LL212;};_LL237: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp4CE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4CE->tag != 17)goto _LL239;else{_tmp4CF=_tmp4CE->f1;}}_LL238: {
# 3232
int _tmp63E=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4CF);
Cyc_Toc_set_in_sizeof(nv,_tmp63E);
goto _LL212;}_LL239: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4D0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4D0->tag != 16)goto _LL23B;else{_tmp4D1=(void*)_tmp4D0->f1;}}_LL23A:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC4D;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC4C;e->r=(void*)((_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C[0]=((_tmpC4D.tag=16,((_tmpC4D.f1=Cyc_Toc_typ_to_c(_tmp4D1),_tmpC4D)))),_tmpC4C))));}goto _LL212;_LL23B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp4D2=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4D2->tag != 18)goto _LL23D;else{_tmp4D3=(void*)_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;}}_LL23C: {
# 3238
void*_tmp641=_tmp4D3;
struct Cyc_List_List*_tmp642=_tmp4D4;
for(0;_tmp642 != 0;_tmp642=_tmp642->tl){
void*_tmp643=(void*)_tmp642->hd;struct _dyneither_ptr*_tmp645;unsigned int _tmp647;_LL2F3: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp644=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp643;if(_tmp644->tag != 0)goto _LL2F5;else{_tmp645=_tmp644->f1;}}_LL2F4:
 goto _LL2F2;_LL2F5: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp646=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp643;if(_tmp646->tag != 1)goto _LL2F2;else{_tmp647=_tmp646->f1;}}_LL2F6:
# 3244
{void*_tmp648=Cyc_Tcutil_compress(_tmp641);union Cyc_Absyn_AggrInfoU _tmp64A;struct Cyc_List_List*_tmp64C;struct Cyc_List_List*_tmp64E;struct Cyc_Absyn_Datatypefield*_tmp650;_LL2F8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp649=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp648;if(_tmp649->tag != 11)goto _LL2FA;else{_tmp64A=(_tmp649->f1).aggr_info;}}_LL2F9: {
# 3246
struct Cyc_Absyn_Aggrdecl*_tmp651=Cyc_Absyn_get_known_aggrdecl(_tmp64A);
if(_tmp651->impl == 0){
const char*_tmpC50;void*_tmpC4F;(_tmpC4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC50="struct fields must be known",_tag_dyneither(_tmpC50,sizeof(char),28))),_tag_dyneither(_tmpC4F,sizeof(void*),0)));}
_tmp64C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp651->impl))->fields;goto _LL2FB;}_LL2FA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp64B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp648;if(_tmp64B->tag != 12)goto _LL2FC;else{_tmp64C=_tmp64B->f2;}}_LL2FB: {
# 3251
struct Cyc_Absyn_Aggrfield*_tmp654=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64C,(int)_tmp647);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC53;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC52;_tmp642->hd=(void*)((void*)((_tmpC52=_cycalloc(sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC53.tag=0,((_tmpC53.f1=_tmp654->name,_tmpC53)))),_tmpC52)))));}
_tmp641=_tmp654->type;
goto _LL2F7;}_LL2FC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp64D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp648;if(_tmp64D->tag != 10)goto _LL2FE;else{_tmp64E=_tmp64D->f1;}}_LL2FD:
# 3256
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC56;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC55;_tmp642->hd=(void*)((void*)((_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=0,((_tmpC56.f1=Cyc_Absyn_fieldname((int)(_tmp647 + 1)),_tmpC56)))),_tmpC55)))));}
_tmp641=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp64E,(int)_tmp647)).f2;
goto _LL2F7;_LL2FE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp648;if(_tmp64F->tag != 4)goto _LL300;else{if((((_tmp64F->f1).field_info).KnownDatatypefield).tag != 2)goto _LL300;_tmp650=((struct _tuple1)(((_tmp64F->f1).field_info).KnownDatatypefield).val).f2;}}_LL2FF:
# 3260
 if(_tmp647 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC59;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC58;_tmp642->hd=(void*)((void*)((_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=0,((_tmpC59.f1=Cyc_Toc_tag_sp,_tmpC59)))),_tmpC58)))));}else{
# 3263
_tmp641=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp650->typs,(int)(_tmp647 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC5C;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC5B;_tmp642->hd=(void*)((void*)((_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B[0]=((_tmpC5C.tag=0,((_tmpC5C.f1=Cyc_Absyn_fieldname((int)_tmp647),_tmpC5C)))),_tmpC5B)))));};}
# 3266
goto _LL2F7;_LL300:;_LL301:
{const char*_tmpC5F;void*_tmpC5E;(_tmpC5E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5F="impossible type for offsetof tuple index",_tag_dyneither(_tmpC5F,sizeof(char),41))),_tag_dyneither(_tmpC5E,sizeof(void*),0)));}
goto _LL2F7;_LL2F7:;}
# 3270
goto _LL2F2;_LL2F2:;}
# 3273
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC62;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC61;e->r=(void*)((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC62.tag=18,((_tmpC62.f1=Cyc_Toc_typ_to_c(_tmp4D3),((_tmpC62.f2=_tmp4D4,_tmpC62)))))),_tmpC61))));}
goto _LL212;}_LL23D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4D5->tag != 19)goto _LL23F;else{_tmp4D6=_tmp4D5->f1;}}_LL23E: {
# 3276
int _tmp661=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp662=Cyc_Tcutil_compress((void*)_check_null(_tmp4D6->topt));
{void*_tmp663=_tmp662;void*_tmp665;struct Cyc_Absyn_Tqual _tmp666;void*_tmp667;union Cyc_Absyn_Constraint*_tmp668;union Cyc_Absyn_Constraint*_tmp669;union Cyc_Absyn_Constraint*_tmp66A;_LL303: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp664=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663;if(_tmp664->tag != 5)goto _LL305;else{_tmp665=(_tmp664->f1).elt_typ;_tmp666=(_tmp664->f1).elt_tq;_tmp667=((_tmp664->f1).ptr_atts).rgn;_tmp668=((_tmp664->f1).ptr_atts).nullable;_tmp669=((_tmp664->f1).ptr_atts).bounds;_tmp66A=((_tmp664->f1).ptr_atts).zero_term;}}_LL304:
# 3281
{void*_tmp66B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp669);struct Cyc_Absyn_Exp*_tmp66D;_LL308: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp66C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp66B;if(_tmp66C->tag != 1)goto _LL30A;else{_tmp66D=_tmp66C->f1;}}_LL309: {
# 3283
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4D6);
Cyc_Toc_exp_to_c(nv,_tmp4D6);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC65;void*_tmpC64;(_tmpC64=0,Cyc_Tcutil_warn(e->loc,((_tmpC65="inserted null check due to dereference",_tag_dyneither(_tmpC65,sizeof(char),39))),_tag_dyneither(_tmpC64,sizeof(void*),0)));}{
# 3289
struct Cyc_List_List*_tmpC66;_tmp4D6->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp662),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC66=_cycalloc(sizeof(*_tmpC66)),((_tmpC66->hd=Cyc_Absyn_copy_exp(_tmp4D6),((_tmpC66->tl=0,_tmpC66)))))),0));};}
# 3297
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp66A)){
unsigned int _tmp673;int _tmp674;struct _tuple11 _tmp672=Cyc_Evexp_eval_const_uint_exp(_tmp66D);_tmp673=_tmp672.f1;_tmp674=_tmp672.f2;
# 3303
if(!_tmp674  || _tmp673 <= 0){
const char*_tmpC69;void*_tmpC68;(_tmpC68=0,Cyc_Tcutil_terr(e->loc,((_tmpC69="cannot determine dereference is in bounds",_tag_dyneither(_tmpC69,sizeof(char),42))),_tag_dyneither(_tmpC68,sizeof(void*),0)));}}
# 3306
goto _LL307;}_LL30A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp66E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp66B;if(_tmp66E->tag != 0)goto _LL307;}_LL30B: {
# 3309
struct Cyc_Absyn_Exp*_tmp677=Cyc_Absyn_uint_exp(0,0);
_tmp677->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp4D6,_tmp677);
Cyc_Toc_exp_to_c(nv,e);
goto _LL307;}_LL307:;}
# 3315
goto _LL302;_LL305:;_LL306: {
const char*_tmpC6C;void*_tmpC6B;(_tmpC6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6C="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC6C,sizeof(char),29))),_tag_dyneither(_tmpC6B,sizeof(void*),0)));}_LL302:;}
# 3318
Cyc_Toc_set_lhs(nv,_tmp661);
goto _LL212;};}_LL23F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4D7->tag != 20)goto _LL241;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;_tmp4DA=_tmp4D7->f3;_tmp4DB=_tmp4D7->f4;}}_LL240: {
# 3321
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4D8->topt);
Cyc_Toc_exp_to_c(nv,_tmp4D8);
if(_tmp4DA  && _tmp4DB)
e->r=Cyc_Toc_check_tagged_union(_tmp4D8,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp4D9,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3329
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL212;}_LL241: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4DC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4DC->tag != 21)goto _LL243;else{_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;_tmp4DF=_tmp4DC->f3;_tmp4E0=_tmp4DC->f4;}}_LL242: {
# 3333
int _tmp67A=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp4DD->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp4DD);
Cyc_Toc_exp_to_c(nv,_tmp4DD);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp67C;struct Cyc_Absyn_Tqual _tmp67D;void*_tmp67E;union Cyc_Absyn_Constraint*_tmp67F;union Cyc_Absyn_Constraint*_tmp680;union Cyc_Absyn_Constraint*_tmp681;struct Cyc_Absyn_PtrInfo _tmp67B=Cyc_Toc_get_ptr_type(e1typ);_tmp67C=_tmp67B.elt_typ;_tmp67D=_tmp67B.elt_tq;_tmp67E=(_tmp67B.ptr_atts).rgn;_tmp67F=(_tmp67B.ptr_atts).nullable;_tmp680=(_tmp67B.ptr_atts).bounds;_tmp681=(_tmp67B.ptr_atts).zero_term;
{void*_tmp682=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp680);struct Cyc_Absyn_Exp*_tmp684;_LL30D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp683=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp682;if(_tmp683->tag != 1)goto _LL30F;else{_tmp684=_tmp683->f1;}}_LL30E: {
# 3344
unsigned int _tmp687;int _tmp688;struct _tuple11 _tmp686=Cyc_Evexp_eval_const_uint_exp(_tmp684);_tmp687=_tmp686.f1;_tmp688=_tmp686.f2;
if(_tmp688){
if(_tmp687 < 1){
const char*_tmpC6F;void*_tmpC6E;(_tmpC6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC6F="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC6F,sizeof(char),44))),_tag_dyneither(_tmpC6E,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC72;void*_tmpC71;(_tmpC71=0,Cyc_Tcutil_warn(e->loc,((_tmpC72="inserted null check due to dereference",_tag_dyneither(_tmpC72,sizeof(char),39))),_tag_dyneither(_tmpC71,sizeof(void*),0)));}{
# 3352
struct Cyc_Absyn_Exp*_tmpC73[1];_tmp4DD->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC73[0]=Cyc_Absyn_new_exp(_tmp4DD->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC73,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3357
if(!Cyc_Evexp_c_can_eval(_tmp684)){
const char*_tmpC76;void*_tmpC75;(_tmpC75=0,Cyc_Tcutil_terr(e->loc,((_tmpC76="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC76,sizeof(char),47))),_tag_dyneither(_tmpC75,sizeof(void*),0)));}{
# 3361
struct Cyc_Absyn_Exp*_tmpC77[4];_tmp4DD->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC77[3]=
# 3365
Cyc_Absyn_uint_exp(0,0),((_tmpC77[2]=
# 3364
Cyc_Absyn_sizeoftyp_exp(_tmp67C,0),((_tmpC77[1]=_tmp684,((_tmpC77[0]=
# 3363
Cyc_Absyn_new_exp(_tmp4DD->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC77,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3367
goto _LL30C;}_LL30F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp685=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp682;if(_tmp685->tag != 0)goto _LL30C;}_LL310: {
# 3370
void*ta1=Cyc_Toc_typ_to_c(_tmp67C);
{struct Cyc_Absyn_Exp*_tmpC78[3];_tmp4DD->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp67D),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC78[2]=
# 3375
Cyc_Absyn_uint_exp(0,0),((_tmpC78[1]=
# 3374
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC78[0]=
# 3373
Cyc_Absyn_new_exp(_tmp4DD->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC78,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3378
goto _LL30C;}_LL30C:;}
# 3380
if(_tmp4DF  && _tmp4E0)
e->r=Cyc_Toc_check_tagged_union(_tmp4DD,Cyc_Toc_typ_to_c(e1typ),_tmp67C,_tmp4DE,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3383
if(is_poly  && _tmp4E0)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp67A);
goto _LL212;};};}_LL243: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4E1->tag != 22)goto _LL245;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E1->f2;}}_LL244: {
# 3388
int _tmp692=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp693=Cyc_Tcutil_compress((void*)_check_null(_tmp4E2->topt));
# 3392
{void*_tmp694=_tmp693;struct Cyc_List_List*_tmp696;void*_tmp698;struct Cyc_Absyn_Tqual _tmp699;void*_tmp69A;union Cyc_Absyn_Constraint*_tmp69B;union Cyc_Absyn_Constraint*_tmp69C;union Cyc_Absyn_Constraint*_tmp69D;_LL312: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp694;if(_tmp695->tag != 10)goto _LL314;else{_tmp696=_tmp695->f1;}}_LL313:
# 3395
 Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_exp_to_c(nv,_tmp4E3);{
unsigned int _tmp69F;int _tmp6A0;struct _tuple11 _tmp69E=Cyc_Evexp_eval_const_uint_exp(_tmp4E3);_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;
if(!_tmp6A0){
const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC7B="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC7B,sizeof(char),44))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4E2,Cyc_Absyn_fieldname((int)(_tmp69F + 1)));
goto _LL311;};_LL314: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp697=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp694;if(_tmp697->tag != 5)goto _LL316;else{_tmp698=(_tmp697->f1).elt_typ;_tmp699=(_tmp697->f1).elt_tq;_tmp69A=((_tmp697->f1).ptr_atts).rgn;_tmp69B=((_tmp697->f1).ptr_atts).nullable;_tmp69C=((_tmp697->f1).ptr_atts).bounds;_tmp69D=((_tmp697->f1).ptr_atts).zero_term;}}_LL315: {
# 3403
struct Cyc_List_List*_tmp6A3=Cyc_Toc_get_relns(_tmp4E2);
int _tmp6A4=Cyc_Toc_need_null_check(_tmp4E2);
int _tmp6A5=Cyc_Toc_in_sizeof(nv);
# 3411
int in_bnds=_tmp6A5;
{void*_tmp6A6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69C);_LL319:;_LL31A:
# 3414
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp693,_tmp6A3,_tmp4E2,_tmp4E3);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC7F;void*_tmpC7E[1];struct Cyc_String_pa_PrintArg_struct _tmpC7D;(_tmpC7D.tag=0,((_tmpC7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC7E[0]=& _tmpC7D,Cyc_Tcutil_warn(e->loc,((_tmpC7F="bounds check necessary for %s",_tag_dyneither(_tmpC7F,sizeof(char),30))),_tag_dyneither(_tmpC7E,sizeof(void*),1)))))));}_LL318:;}
# 3421
Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_exp_to_c(nv,_tmp4E3);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6AA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69C);struct Cyc_Absyn_Exp*_tmp6AC;_LL31C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6AB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6AA;if(_tmp6AB->tag != 1)goto _LL31E;else{_tmp6AC=_tmp6AB->f1;}}_LL31D: {
# 3426
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp69B) && (unsigned int)Cyc_Toc_need_null_check;
void*ta1=Cyc_Toc_typ_to_c(_tmp698);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp699);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC82;void*_tmpC81;(_tmpC81=0,Cyc_Tcutil_warn(e->loc,((_tmpC82="inserted null check due to dereference",_tag_dyneither(_tmpC82,sizeof(char),39))),_tag_dyneither(_tmpC81,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC83[1];_tmp4E2->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC83[0]=Cyc_Absyn_copy_exp(_tmp4E2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC83,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3437
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp69D)){
# 3439
if(!Cyc_Evexp_c_can_eval(_tmp6AC)){
const char*_tmpC86;void*_tmpC85;(_tmpC85=0,Cyc_Tcutil_terr(e->loc,((_tmpC86="cannot determine subscript is in bounds",_tag_dyneither(_tmpC86,sizeof(char),40))),_tag_dyneither(_tmpC85,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp4E2);
struct Cyc_Absyn_Exp*_tmpC87[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC87[2]=_tmp4E3,((_tmpC87[1]=_tmp6AC,((_tmpC87[0]=_tmp4E2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC87,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6AC)){
const char*_tmpC8A;void*_tmpC89;(_tmpC89=0,Cyc_Tcutil_terr(e->loc,((_tmpC8A="cannot determine subscript is in bounds",_tag_dyneither(_tmpC8A,sizeof(char),40))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC8D;void*_tmpC8C;(_tmpC8C=0,Cyc_Tcutil_warn(e->loc,((_tmpC8D="inserted null check due to dereference",_tag_dyneither(_tmpC8D,sizeof(char),39))),_tag_dyneither(_tmpC8C,sizeof(void*),0)));}{
# 3451
struct Cyc_Absyn_Exp*_tmpC8E[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC8E[3]=_tmp4E3,((_tmpC8E[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC8E[1]=_tmp6AC,((_tmpC8E[0]=_tmp4E2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8E,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3457
if(!Cyc_Evexp_c_can_eval(_tmp6AC)){
const char*_tmpC91;void*_tmpC90;(_tmpC90=0,Cyc_Tcutil_terr(e->loc,((_tmpC91="cannot determine subscript is in bounds",_tag_dyneither(_tmpC91,sizeof(char),40))),_tag_dyneither(_tmpC90,sizeof(void*),0)));}{
# 3460
struct Cyc_Absyn_Exp*_tmpC92[2];_tmp4E3->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC92[1]=Cyc_Absyn_copy_exp(_tmp4E3),((_tmpC92[0]=_tmp6AC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC92,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3463
goto _LL31B;}_LL31E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6AD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6AA;if(_tmp6AD->tag != 0)goto _LL31B;}_LL31F: {
# 3465
void*ta1=Cyc_Toc_typ_to_c(_tmp698);
if(in_bnds){
# 3469
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp699),
Cyc_Toc_member_exp(_tmp4E2,Cyc_Toc_curr_sp,0)),_tmp4E3);}else{
# 3474
struct Cyc_Absyn_Exp*_tmp6BC=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC93[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp699),
Cyc_Absyn_fncall_exp(_tmp6BC,(
(_tmpC93[2]=_tmp4E3,((_tmpC93[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC93[0]=_tmp4E2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3480
goto _LL31B;}_LL31B:;}
# 3482
goto _LL311;}_LL316:;_LL317: {
const char*_tmpC96;void*_tmpC95;(_tmpC95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC96="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC96,sizeof(char),49))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}_LL311:;}
# 3485
Cyc_Toc_set_lhs(nv,_tmp692);
goto _LL212;};}_LL245: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4E4->tag != 23)goto _LL247;else{_tmp4E5=_tmp4E4->f1;}}_LL246:
# 3488
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E5))->r;else{
# 3493
struct Cyc_List_List*_tmp6C0=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E5);
void*_tmp6C1=Cyc_Toc_add_tuple_type(_tmp6C0);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp4E5 != 0;(_tmp4E5=_tmp4E5->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E5->hd);{
struct _tuple15*_tmpC99;struct Cyc_List_List*_tmpC98;dles=((_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=((_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->f1=0,((_tmpC99->f2=(struct Cyc_Absyn_Exp*)_tmp4E5->hd,_tmpC99)))))),((_tmpC98->tl=dles,_tmpC98))))));};}}
# 3500
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3503
goto _LL212;_LL247: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp4E6=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4E6->tag != 25)goto _LL249;else{_tmp4E7=_tmp4E6->f1;}}_LL248:
# 3507
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E7);
{struct Cyc_List_List*_tmp6C4=_tmp4E7;for(0;_tmp6C4 != 0;_tmp6C4=_tmp6C4->tl){
struct Cyc_Absyn_Exp*_tmp6C6;struct _tuple15*_tmp6C5=(struct _tuple15*)_tmp6C4->hd;_tmp6C6=_tmp6C5->f2;
Cyc_Toc_exp_to_c(nv,_tmp6C6);}}
# 3512
goto _LL212;_LL249: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4E8=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4E8->tag != 26)goto _LL24B;else{_tmp4E9=_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;_tmp4EB=_tmp4E8->f3;_tmp4EC=_tmp4E8->f4;}}_LL24A: {
# 3516
unsigned int _tmp6C8;int _tmp6C9;struct _tuple11 _tmp6C7=Cyc_Evexp_eval_const_uint_exp(_tmp4EA);_tmp6C8=_tmp6C7.f1;_tmp6C9=_tmp6C7.f2;{
void*_tmp6CA=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4EB->topt));
Cyc_Toc_exp_to_c(nv,_tmp4EB);{
struct Cyc_List_List*es=0;
# 3521
if(!Cyc_Toc_is_zero(_tmp4EB)){
if(!_tmp6C9){
const char*_tmpC9C;void*_tmpC9B;(_tmpC9B=0,Cyc_Tcutil_terr(_tmp4EA->loc,((_tmpC9C="cannot determine value of constant",_tag_dyneither(_tmpC9C,sizeof(char),35))),_tag_dyneither(_tmpC9B,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6C8;++ i){
struct _tuple15*_tmpC9F;struct Cyc_List_List*_tmpC9E;es=((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->hd=((_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F->f1=0,((_tmpC9F->f2=_tmp4EB,_tmpC9F)))))),((_tmpC9E->tl=es,_tmpC9E))))));}}
# 3527
if(_tmp4EC){
struct Cyc_Absyn_Exp*_tmp6CF=Cyc_Toc_cast_it(_tmp6CA,Cyc_Absyn_uint_exp(0,0));
struct _tuple15*_tmpCA2;struct Cyc_List_List*_tmpCA1;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->hd=((_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->f1=0,((_tmpCA2->f2=_tmp6CF,_tmpCA2)))))),((_tmpCA1->tl=0,_tmpCA1)))))));}}
# 3532
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL212;};};}_LL24B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp4ED=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4ED->tag != 27)goto _LL24D;else{_tmp4EE=_tmp4ED->f1;_tmp4EF=(void*)_tmp4ED->f2;_tmp4F0=_tmp4ED->f3;}}_LL24C:
# 3537
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL212;_LL24D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4F1->tag != 28)goto _LL24F;else{_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;_tmp4F4=_tmp4F1->f3;_tmp4F5=_tmp4F1->f4;}}_LL24E:
# 3542
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp6D2=Cyc_Toc_init_struct(nv,old_typ,_tmp4F3,0,0,_tmp4F4,_tmp4F2);
e->r=_tmp6D2->r;
e->topt=_tmp6D2->topt;}else{
# 3553
if(_tmp4F5 == 0){
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCA5,sizeof(char),38))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp4F5;
# 3557
struct _RegionHandle _tmp6D5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6D5;_push_region(rgn);
{struct Cyc_List_List*_tmp6D6=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4F4,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3561
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F5->impl))->tagged){
# 3563
struct Cyc_Absyn_Aggrfield*_tmp6D8;struct Cyc_Absyn_Exp*_tmp6D9;struct _tuple21*_tmp6D7=(struct _tuple21*)((struct Cyc_List_List*)_check_null(_tmp6D6))->hd;_tmp6D8=_tmp6D7->f1;_tmp6D9=_tmp6D7->f2;{
void*_tmp6DA=(void*)_check_null(_tmp6D9->topt);
void*_tmp6DB=_tmp6D8->type;
Cyc_Toc_exp_to_c(nv,_tmp6D9);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6DB) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6DA))
_tmp6D9->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6D9->r,0));{
# 3572
int i=Cyc_Toc_get_member_offset(_tmp4F5,_tmp6D8->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple15*_tmpCAA;struct _tuple15*_tmpCA9;struct _tuple15*_tmpCA8[2];struct Cyc_List_List*_tmp6DC=(_tmpCA8[1]=((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9->f1=0,((_tmpCA9->f2=_tmp6D9,_tmpCA9)))))),((_tmpCA8[0]=((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->f1=0,((_tmpCAA->f2=field_tag_exp,_tmpCAA)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA8,sizeof(struct _tuple15*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6DC,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCB0;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpCAF;void*_tmpCAE[1];struct Cyc_List_List*ds=(_tmpCAE[0]=(void*)((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0[0]=((_tmpCAF.tag=1,((_tmpCAF.f1=_tmp6D8->name,_tmpCAF)))),_tmpCB0)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAE,sizeof(void*),1)));
struct _tuple15*_tmpCB3;struct _tuple15*_tmpCB2[1];struct Cyc_List_List*dles=(_tmpCB2[0]=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->f1=ds,((_tmpCB3->f2=umem,_tmpCB3)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB2,sizeof(struct _tuple15*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3581
struct Cyc_List_List*_tmp6E5=0;
struct Cyc_List_List*_tmp6E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6E6 != 0;_tmp6E6=_tmp6E6->tl){
struct Cyc_List_List*_tmp6E7=_tmp6D6;for(0;_tmp6E7 != 0;_tmp6E7=_tmp6E7->tl){
if((*((struct _tuple21*)_tmp6E7->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6E6->hd){
struct Cyc_Absyn_Aggrfield*_tmp6E9;struct Cyc_Absyn_Exp*_tmp6EA;struct _tuple21*_tmp6E8=(struct _tuple21*)_tmp6E7->hd;_tmp6E9=_tmp6E8->f1;_tmp6EA=_tmp6E8->f2;{
void*_tmp6EB=Cyc_Toc_typ_to_c(_tmp6E9->type);
void*_tmp6EC=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6EA->topt));
Cyc_Toc_exp_to_c(nv,_tmp6EA);
# 3591
if(!Cyc_Tcutil_unify(_tmp6EB,_tmp6EC)){
# 3593
if(!Cyc_Tcutil_is_arithmetic_type(_tmp6EB) || !
Cyc_Tcutil_is_arithmetic_type(_tmp6EC))
_tmp6EA=Cyc_Toc_cast_it(_tmp6EB,Cyc_Absyn_copy_exp(_tmp6EA));}
{struct _tuple15*_tmpCB6;struct Cyc_List_List*_tmpCB5;_tmp6E5=((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5->hd=((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6->f1=0,((_tmpCB6->f2=_tmp6EA,_tmpCB6)))))),((_tmpCB5->tl=_tmp6E5,_tmpCB5))))));}
break;};}}}
# 3600
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E5));}}
# 3558
;_pop_region(rgn);};}
# 3604
goto _LL212;_LL24F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp4F6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4F6->tag != 29)goto _LL251;else{_tmp4F7=(void*)_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;}}_LL250: {
# 3606
struct Cyc_List_List*fs;
{void*_tmp6EF=Cyc_Tcutil_compress(_tmp4F7);struct Cyc_List_List*_tmp6F1;_LL321: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6F0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6EF;if(_tmp6F0->tag != 12)goto _LL323;else{_tmp6F1=_tmp6F0->f2;}}_LL322:
 fs=_tmp6F1;goto _LL320;_LL323:;_LL324: {
const char*_tmpCBA;void*_tmpCB9[1];struct Cyc_String_pa_PrintArg_struct _tmpCB8;(_tmpCB8.tag=0,((_tmpCB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4F7)),((_tmpCB9[0]=& _tmpCB8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBA="anon struct has type %s",_tag_dyneither(_tmpCBA,sizeof(char),24))),_tag_dyneither(_tmpCB9,sizeof(void*),1)))))));}_LL320:;}{
# 3611
struct _RegionHandle _tmp6F5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6F5;_push_region(rgn);{
struct Cyc_List_List*_tmp6F6=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp4F8,Cyc_Absyn_StructA,fs);
for(0;_tmp6F6 != 0;_tmp6F6=_tmp6F6->tl){
struct Cyc_Absyn_Aggrfield*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F9;struct _tuple21*_tmp6F7=(struct _tuple21*)_tmp6F6->hd;_tmp6F8=_tmp6F7->f1;_tmp6F9=_tmp6F7->f2;{
void*_tmp6FA=(void*)_check_null(_tmp6F9->topt);
void*_tmp6FB=_tmp6F8->type;
Cyc_Toc_exp_to_c(nv,_tmp6F9);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6FB) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6FA))
_tmp6F9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6F9->r,0));};}
# 3625
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4F8);}
# 3627
_npop_handler(0);goto _LL212;
# 3611
;_pop_region(rgn);};}_LL251: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4F9->tag != 30)goto _LL253;else{_tmp4FA=_tmp4F9->f1;_tmp4FB=_tmp4F9->f2;_tmp4FC=_tmp4F9->f3;}}_LL252: {
# 3630
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp6FC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp6FD=Cyc_Absyn_var_exp(_tmp6FC,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp4FC->name,_tmp4FB->name));
tag_exp=_tmp4FB->is_extensible?Cyc_Absyn_var_exp(_tmp4FC->name,0):
 Cyc_Toc_datatype_tag(_tmp4FB,_tmp4FC->name);
mem_exp=_tmp6FD;{
struct Cyc_List_List*_tmp6FE=_tmp4FC->typs;
# 3641
if(Cyc_Toc_is_toplevel(nv)){
# 3643
struct Cyc_List_List*dles=0;
for(0;_tmp4FA != 0;(_tmp4FA=_tmp4FA->tl,_tmp6FE=_tmp6FE->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4FA->hd;
void*_tmp6FF=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6FE))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp6FF))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple15*_tmpCBD;struct Cyc_List_List*_tmpCBC;dles=((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC->hd=((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->f1=0,((_tmpCBD->f2=cur_e,_tmpCBD)))))),((_tmpCBC->tl=dles,_tmpCBC))))));};}
# 3654
{struct _tuple15*_tmpCC0;struct Cyc_List_List*_tmpCBF;dles=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->hd=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->f1=0,((_tmpCC0->f2=tag_exp,_tmpCC0)))))),((_tmpCBF->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpCBF))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3660
struct Cyc_List_List*_tmpCC1;struct Cyc_List_List*_tmp704=
(_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpCC1->tl=0,_tmpCC1)))));
# 3663
{int i=1;for(0;_tmp4FA != 0;(((_tmp4FA=_tmp4FA->tl,i ++)),_tmp6FE=_tmp6FE->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4FA->hd;
void*_tmp705=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6FE))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp705))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp706=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3673
struct Cyc_List_List*_tmpCC2;_tmp704=((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->hd=_tmp706,((_tmpCC2->tl=_tmp704,_tmpCC2))))));};}}{
# 3675
struct Cyc_Absyn_Stmt*_tmp708=Cyc_Absyn_exp_stmt(_tmp6FD,0);
struct Cyc_List_List*_tmpCC3;struct Cyc_Absyn_Stmt*_tmp709=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->hd=_tmp708,((_tmpCC3->tl=_tmp704,_tmpCC3))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6FC,datatype_ctype,0,_tmp709,0));};}
# 3679
goto _LL212;};}_LL253: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4FD->tag != 31)goto _LL255;}_LL254:
# 3681
 goto _LL256;_LL255: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp4FE=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4FE->tag != 32)goto _LL257;}_LL256:
 goto _LL212;_LL257: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp4FF->tag != 33)goto _LL259;else{_tmp500=(_tmp4FF->f1).is_calloc;_tmp501=(_tmp4FF->f1).rgn;_tmp502=(_tmp4FF->f1).elt_type;_tmp503=(_tmp4FF->f1).num_elts;_tmp504=(_tmp4FF->f1).fat_result;}}_LL258: {
# 3685
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp502)));
Cyc_Toc_exp_to_c(nv,_tmp503);
# 3689
if(_tmp504){
struct _tuple0*_tmp70C=Cyc_Toc_temp_var();
struct _tuple0*_tmp70D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp500){
xexp=_tmp503;
if(_tmp501 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp501;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp70C,0));}else{
# 3700
pexp=Cyc_Toc_calloc_exp(*_tmp502,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp70C,0));}{
# 3702
struct Cyc_Absyn_Exp*_tmpCC4[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCC4[2]=
Cyc_Absyn_var_exp(_tmp70C,0),((_tmpCC4[1]=
# 3703
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCC4[0]=Cyc_Absyn_var_exp(_tmp70D,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3706
if(_tmp501 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp501;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp70C,0));}else{
# 3711
pexp=Cyc_Toc_malloc_exp(*_tmp502,Cyc_Absyn_var_exp(_tmp70C,0));}{
# 3713
struct Cyc_Absyn_Exp*_tmpCC5[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpCC5[2]=
Cyc_Absyn_var_exp(_tmp70C,0),((_tmpCC5[1]=
# 3713
Cyc_Absyn_uint_exp(1,0),((_tmpCC5[0]=Cyc_Absyn_var_exp(_tmp70D,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3716
struct Cyc_Absyn_Stmt*_tmp710=Cyc_Absyn_declare_stmt(_tmp70C,Cyc_Absyn_uint_typ,_tmp503,
Cyc_Absyn_declare_stmt(_tmp70D,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp710);};}else{
# 3721
if(_tmp500){
if(_tmp501 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp501;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp503))->r;}else{
# 3727
e->r=(Cyc_Toc_calloc_exp(*_tmp502,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp503))->r;}}else{
# 3730
if(_tmp501 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp501;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp503))->r;}else{
# 3735
e->r=(Cyc_Toc_malloc_exp(*_tmp502,_tmp503))->r;}}}
# 3739
goto _LL212;}_LL259: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp505->tag != 34)goto _LL25B;else{_tmp506=_tmp505->f1;_tmp507=_tmp505->f2;}}_LL25A: {
# 3748
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp506->topt);
void*e2_old_typ=(void*)_check_null(_tmp507->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpCC8;void*_tmpCC7;(_tmpCC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC8="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpCC8,sizeof(char),57))),_tag_dyneither(_tmpCC7,sizeof(void*),0)));}{
# 3755
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3759
swap_fn=Cyc_Toc__swap_word_e;}{
# 3763
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp506,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp507,& f2_tag,& tagged_mem_type2,1);
# 3770
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp506);
Cyc_Toc_exp_to_c(nv,_tmp507);
Cyc_Toc_set_lhs(nv,0);
# 3776
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp506,f1_tag,tagged_mem_type1);else{
# 3780
_tmp506=Cyc_Toc_push_address_exp(_tmp506);}
# 3782
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp507,f2_tag,tagged_mem_type2);else{
# 3786
_tmp507=Cyc_Toc_push_address_exp(_tmp507);}
# 3788
{struct Cyc_Absyn_Exp*_tmpCC9[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCC9[1]=_tmp507,((_tmpCC9[0]=_tmp506,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC9,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3790
goto _LL212;};};}_LL25B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp508->tag != 37)goto _LL25D;else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;}}_LL25C: {
# 3793
void*_tmp714=Cyc_Tcutil_compress((void*)_check_null(_tmp509->topt));
Cyc_Toc_exp_to_c(nv,_tmp509);
{void*_tmp715=_tmp714;struct Cyc_Absyn_Aggrdecl*_tmp717;_LL326: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp715;if(_tmp716->tag != 11)goto _LL328;else{if((((_tmp716->f1).aggr_info).KnownAggr).tag != 2)goto _LL328;_tmp717=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp716->f1).aggr_info).KnownAggr).val);}}_LL327: {
# 3797
struct Cyc_Absyn_Exp*_tmp718=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp717,_tmp50A),0);
struct Cyc_Absyn_Exp*_tmp719=Cyc_Toc_member_exp(_tmp509,_tmp50A,0);
struct Cyc_Absyn_Exp*_tmp71A=Cyc_Toc_member_exp(_tmp719,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp71A,_tmp718,0))->r;
goto _LL325;}_LL328:;_LL329: {
const char*_tmpCCC;void*_tmpCCB;(_tmpCCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCC="non-aggregate type in tagcheck",_tag_dyneither(_tmpCCC,sizeof(char),31))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}_LL325:;}
# 3804
goto _LL212;}_LL25D: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp50B->tag != 36)goto _LL25F;else{_tmp50C=_tmp50B->f1;}}_LL25E:
 Cyc_Toc_stmt_to_c(nv,_tmp50C);goto _LL212;_LL25F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp50D=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp50D->tag != 35)goto _LL261;}_LL260: {
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCF="UnresolvedMem",_tag_dyneither(_tmpCCF,sizeof(char),14))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}_LL261: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp50E->tag != 24)goto _LL263;}_LL262: {
const char*_tmpCD2;void*_tmpCD1;(_tmpCD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCD2="compoundlit",_tag_dyneither(_tmpCD2,sizeof(char),12))),_tag_dyneither(_tmpCD1,sizeof(void*),0)));}_LL263: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp50F->tag != 38)goto _LL265;}_LL264: {
const char*_tmpCD5;void*_tmpCD4;(_tmpCD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD5="valueof(-)",_tag_dyneither(_tmpCD5,sizeof(char),11))),_tag_dyneither(_tmpCD4,sizeof(void*),0)));}_LL265: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp497;if(_tmp510->tag != 39)goto _LL212;}_LL266: {
const char*_tmpCD8;void*_tmpCD7;(_tmpCD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD8="__asm__",_tag_dyneither(_tmpCD8,sizeof(char),8))),_tag_dyneither(_tmpCD7,sizeof(void*),0)));}_LL212:;};}
# 3841 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3844
Cyc_Toc_skip_stmt_dl(),0);}
# 3847
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp725=e->r;struct Cyc_Absyn_Exp*_tmp727;struct Cyc_Absyn_Exp*_tmp729;struct Cyc_Absyn_Exp*_tmp72B;_LL32B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp726=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp725;if(_tmp726->tag != 13)goto _LL32D;else{_tmp727=_tmp726->f2;}}_LL32C:
 return Cyc_Toc_path_length(_tmp727);_LL32D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp728=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp725;if(_tmp728->tag != 19)goto _LL32F;else{_tmp729=_tmp728->f1;}}_LL32E:
 return 1 + Cyc_Toc_path_length(_tmp729);_LL32F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp72A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp725;if(_tmp72A->tag != 20)goto _LL331;else{_tmp72B=_tmp72A->f1;}}_LL330:
 return Cyc_Toc_path_length(_tmp72B);_LL331:;_LL332:
 return 0;_LL32A:;}struct _tuple22{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple23{struct _tuple0*f1;void*f2;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3856
static struct _tuple22 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3861
struct Cyc_Absyn_Stmt*s;
{void*_tmp72C=p->r;struct _tuple0*_tmp72E;struct Cyc_Absyn_Pat*_tmp72F;struct _tuple0*_tmp731;struct Cyc_Absyn_Vardecl*_tmp734;struct Cyc_Absyn_Pat*_tmp735;enum Cyc_Absyn_Sign _tmp738;int _tmp739;char _tmp73B;struct _dyneither_ptr _tmp73D;int _tmp73E;struct Cyc_Absyn_Enumdecl*_tmp740;struct Cyc_Absyn_Enumfield*_tmp741;void*_tmp743;struct Cyc_Absyn_Enumfield*_tmp744;struct Cyc_Absyn_Datatypedecl*_tmp747;struct Cyc_Absyn_Datatypefield*_tmp748;struct Cyc_List_List*_tmp749;struct Cyc_List_List*_tmp74B;struct Cyc_List_List*_tmp74D;union Cyc_Absyn_AggrInfoU _tmp750;struct Cyc_List_List*_tmp751;struct Cyc_Absyn_Pat*_tmp753;_LL334: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp72D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp72D->tag != 1)goto _LL336;else{_tmp72E=(_tmp72D->f1)->name;_tmp72F=_tmp72D->f2;}}_LL335: {
# 3865
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp757=(void*)_check_null(_tmp72F->topt);
{struct _tuple23*_tmpCDB;struct Cyc_List_List*_tmpCDA;decls=((_tmpCDA=_region_malloc(rgn,sizeof(*_tmpCDA)),((_tmpCDA->hd=((_tmpCDB=_region_malloc(rgn,sizeof(*_tmpCDB)),((_tmpCDB->f1=v,((_tmpCDB->f2=Cyc_Toc_typ_to_c_array(_tmp757),_tmpCDB)))))),((_tmpCDA->tl=decls,_tmpCDA))))));}{
struct _tuple22 _tmp75A=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp72E,Cyc_Absyn_var_exp(v,0)),rgn,_tmp757,
Cyc_Absyn_var_exp(v,0),path,_tmp72F,fail_stmt,decls);
nv=_tmp75A.f1;
decls=_tmp75A.f2;{
struct Cyc_Absyn_Stmt*_tmp75B=_tmp75A.f3;
struct Cyc_Absyn_Stmt*_tmp75C=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp75C,_tmp75B,0);
goto _LL333;};};}_LL336: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp730=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp730->tag != 3)goto _LL338;else{_tmp731=(_tmp730->f2)->name;}}_LL337: {
# 3878
struct _tuple0*_tmp75D=Cyc_Toc_temp_var();
void*_tmp75E=(void*)_check_null(p->topt);
{struct _tuple23*_tmpCDE;struct Cyc_List_List*_tmpCDD;decls=((_tmpCDD=_region_malloc(rgn,sizeof(*_tmpCDD)),((_tmpCDD->hd=((_tmpCDE=_region_malloc(rgn,sizeof(*_tmpCDE)),((_tmpCDE->f1=_tmp75D,((_tmpCDE->f2=Cyc_Toc_typ_to_c_array(_tmp75E),_tmpCDE)))))),((_tmpCDD->tl=decls,_tmpCDD))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp731,Cyc_Absyn_var_exp(_tmp75D,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp75D,0),r,0);
goto _LL333;}_LL338: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp732=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp732->tag != 0)goto _LL33A;}_LL339:
 s=Cyc_Toc_skip_stmt_dl();goto _LL333;_LL33A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp733=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp733->tag != 2)goto _LL33C;else{_tmp734=_tmp733->f1;_tmp735=_tmp733->f2;}}_LL33B: {
# 3887
struct _tuple0*_tmp761=Cyc_Toc_temp_var();
{struct _tuple23*_tmpCE1;struct Cyc_List_List*_tmpCE0;decls=((_tmpCE0=_region_malloc(rgn,sizeof(*_tmpCE0)),((_tmpCE0->hd=((_tmpCE1=_region_malloc(rgn,sizeof(*_tmpCE1)),((_tmpCE1->f1=_tmp761,((_tmpCE1->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCE1)))))),((_tmpCE0->tl=decls,_tmpCE0))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp734->name,Cyc_Absyn_var_exp(_tmp761,0));
# 3891
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp761,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple22 _tmp764=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp735,fail_stmt,decls);
_tmp764.f3=Cyc_Absyn_seq_stmt(s,_tmp764.f3,0);
return _tmp764;};}_LL33C: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp736=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp736->tag != 8)goto _LL33E;}_LL33D:
# 3898
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL333;_LL33E: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp737=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp737->tag != 9)goto _LL340;else{_tmp738=_tmp737->f1;_tmp739=_tmp737->f2;}}_LL33F:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp738,_tmp739,0),fail_stmt);goto _LL333;_LL340: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp73A=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp73A->tag != 10)goto _LL342;else{_tmp73B=_tmp73A->f1;}}_LL341:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp73B,0),fail_stmt);goto _LL333;_LL342: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp73C=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp73C->tag != 11)goto _LL344;else{_tmp73D=_tmp73C->f1;_tmp73E=_tmp73C->f2;}}_LL343:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp73D,_tmp73E,0),fail_stmt);goto _LL333;_LL344: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp73F=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp73F->tag != 12)goto _LL346;else{_tmp740=_tmp73F->f1;_tmp741=_tmp73F->f2;}}_LL345:
# 3903
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCE4;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCE3;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE4.tag=31,((_tmpCE4.f1=_tmp741->name,((_tmpCE4.f2=_tmp740,((_tmpCE4.f3=_tmp741,_tmpCE4)))))))),_tmpCE3)))),0),fail_stmt);}
goto _LL333;_LL346: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp742=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp742->tag != 13)goto _LL348;else{_tmp743=(void*)_tmp742->f1;_tmp744=_tmp742->f2;}}_LL347:
# 3906
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCE7;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCE6;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE7.tag=32,((_tmpCE7.f1=_tmp744->name,((_tmpCE7.f2=_tmp743,((_tmpCE7.f3=_tmp744,_tmpCE7)))))))),_tmpCE6)))),0),fail_stmt);}
goto _LL333;_LL348: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp745=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp745->tag != 5)goto _LL34A;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp746=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp745->f1)->r;if(_tmp746->tag != 7)goto _LL34A;else{_tmp747=_tmp746->f1;_tmp748=_tmp746->f2;_tmp749=_tmp746->f3;}}}_LL349:
# 3916
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp769=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp748->name,_tmp747->name);
void*_tmp76A=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp76A,r);
struct Cyc_List_List*_tmp76B=_tmp748->typs;
for(0;_tmp749 != 0;(((_tmp749=_tmp749->tl,_tmp76B=((struct Cyc_List_List*)_check_null(_tmp76B))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp76C=(struct Cyc_Absyn_Pat*)_tmp749->hd;
if(_tmp76C->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp76D=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp76B))->hd)).f2;
void*_tmp76E=(void*)_check_null(_tmp76C->topt);
void*_tmp76F=Cyc_Toc_typ_to_c_array(_tmp76E);
struct Cyc_Absyn_Exp*_tmp770=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp769,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp76D)))
_tmp770=Cyc_Toc_cast_it(_tmp76F,_tmp770);{
# 3934
struct _tuple22 _tmp771=Cyc_Toc_xlate_pat(nv,rgn,_tmp76E,_tmp770,_tmp770,_tmp76C,fail_stmt,decls);
# 3936
nv=_tmp771.f1;
decls=_tmp771.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp771.f3,0);};};}{
# 3940
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp769,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp747->is_extensible?Cyc_Absyn_var_exp(_tmp748->name,0): Cyc_Toc_datatype_tag(_tmp747,_tmp748->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3948
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp769,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3951
s=Cyc_Absyn_declare_stmt(_tmp769,_tmp76A,rcast,s,0);
goto _LL333;};};_LL34A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp74A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp74A->tag != 7)goto _LL34C;else{_tmp74B=_tmp74A->f3;}}_LL34B:
# 3954
 _tmp74D=_tmp74B;goto _LL34D;_LL34C: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp74C=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp74C->tag != 4)goto _LL34E;else{_tmp74D=_tmp74C->f1;}}_LL34D:
# 3962
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp74D != 0;(_tmp74D=_tmp74D->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp772=(struct Cyc_Absyn_Pat*)_tmp74D->hd;
if(_tmp772->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp773=(void*)_check_null(_tmp772->topt);
struct _dyneither_ptr*_tmp774=Cyc_Absyn_fieldname(cnt);
struct _tuple22 _tmp775=Cyc_Toc_xlate_pat(nv,rgn,_tmp773,Cyc_Toc_member_exp(r,_tmp774,0),
Cyc_Toc_member_exp(path,_tmp774,0),_tmp772,fail_stmt,decls);
nv=_tmp775.f1;
decls=_tmp775.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp775.f3,0);};}
# 3976
goto _LL333;};_LL34E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp74E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp74E->tag != 6)goto _LL350;else{if(_tmp74E->f1 != 0)goto _LL350;}}_LL34F: {
# 3978
const char*_tmpCEA;void*_tmpCE9;(_tmpCE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEA="unresolved aggregate pattern!",_tag_dyneither(_tmpCEA,sizeof(char),30))),_tag_dyneither(_tmpCE9,sizeof(void*),0)));}_LL350: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp74F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp74F->tag != 6)goto _LL352;else{if(_tmp74F->f1 == 0)goto _LL352;_tmp750=(_tmp74F->f1)->aggr_info;_tmp751=_tmp74F->f3;}}_LL351: {
# 3980
struct Cyc_Absyn_Aggrdecl*_tmp778=Cyc_Absyn_get_known_aggrdecl(_tmp750);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp778->impl))->tagged){
# 3984
struct Cyc_List_List*_tmp77A;struct Cyc_Absyn_Pat*_tmp77B;struct _tuple24*_tmp779=(struct _tuple24*)((struct Cyc_List_List*)_check_null(_tmp751))->hd;_tmp77A=_tmp779->f1;_tmp77B=_tmp779->f2;{
struct _dyneither_ptr*f;
{void*_tmp77C=(void*)((struct Cyc_List_List*)_check_null(_tmp77A))->hd;struct _dyneither_ptr*_tmp77E;_LL35B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp77D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp77C;if(_tmp77D->tag != 1)goto _LL35D;else{_tmp77E=_tmp77D->f1;}}_LL35C:
 f=_tmp77E;goto _LL35A;_LL35D:;_LL35E: {
const char*_tmpCED;void*_tmpCEC;(_tmpCEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCED="no field name in tagged union pattern",_tag_dyneither(_tmpCED,sizeof(char),38))),_tag_dyneither(_tmpCEC,sizeof(void*),0)));}_LL35A:;}{
# 3991
void*_tmp781=(void*)_check_null(_tmp77B->topt);
void*_tmp782=Cyc_Toc_typ_to_c_array(_tmp781);
# 3994
struct Cyc_Absyn_Exp*_tmp783=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp784=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp784=Cyc_Toc_cast_it(_tmp782,_tmp784);
_tmp783=Cyc_Toc_cast_it(_tmp782,_tmp783);{
struct _tuple22 _tmp785=Cyc_Toc_xlate_pat(nv,rgn,_tmp781,_tmp784,_tmp783,_tmp77B,fail_stmt,decls);
# 4002
nv=_tmp785.f1;
decls=_tmp785.f2;{
struct Cyc_Absyn_Stmt*_tmp786=_tmp785.f3;
struct Cyc_Absyn_Stmt*_tmp787=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp778,f),0),fail_stmt);
# 4010
s=Cyc_Absyn_seq_stmt(_tmp787,_tmp786,0);};};};};}else{
# 4013
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp751 != 0;_tmp751=_tmp751->tl){
struct _tuple24*_tmp788=(struct _tuple24*)_tmp751->hd;
struct Cyc_Absyn_Pat*_tmp789=(*_tmp788).f2;
if(_tmp789->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp78A=(void*)((struct Cyc_List_List*)_check_null((*_tmp788).f1))->hd;struct _dyneither_ptr*_tmp78C;_LL360: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp78B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp78A;if(_tmp78B->tag != 1)goto _LL362;else{_tmp78C=_tmp78B->f1;}}_LL361:
 f=_tmp78C;goto _LL35F;_LL362:;_LL363: {
struct Cyc_Toc_Match_error_exn_struct _tmpCF0;struct Cyc_Toc_Match_error_exn_struct*_tmpCEF;(int)_throw((void*)((_tmpCEF=_cycalloc_atomic(sizeof(*_tmpCEF)),((_tmpCEF[0]=((_tmpCF0.tag=Cyc_Toc_Match_error,_tmpCF0)),_tmpCEF)))));}_LL35F:;}{
# 4024
void*_tmp78F=(void*)_check_null(_tmp789->topt);
void*_tmp790=Cyc_Toc_typ_to_c_array(_tmp78F);
struct Cyc_Absyn_Exp*_tmp791=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp778->impl))->fields,f)))->type))
_tmp791=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp790),_tmp791);{
struct _tuple22 _tmp792=Cyc_Toc_xlate_pat(nv,rgn,_tmp78F,_tmp791,
Cyc_Toc_member_exp(path,f,0),_tmp789,fail_stmt,decls);
nv=_tmp792.f1;
decls=_tmp792.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp792.f3,0);};};};}}
# 4036
goto _LL333;}_LL352: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp752=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp752->tag != 5)goto _LL354;else{_tmp753=_tmp752->f1;}}_LL353: {
# 4039
void*_tmp793=(void*)_check_null(_tmp753->topt);
# 4042
struct _tuple22 _tmp794=Cyc_Toc_xlate_pat(nv,rgn,_tmp793,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp753,fail_stmt,decls);
# 4044
nv=_tmp794.f1;
decls=_tmp794.f2;{
struct Cyc_Absyn_Stmt*_tmp795=_tmp794.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4050
Cyc_Toc_skip_stmt_dl(),0),_tmp795,0);else{
# 4052
s=_tmp795;}
goto _LL333;};}_LL354: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp754=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp754->tag != 14)goto _LL356;}_LL355: {
const char*_tmpCF3;void*_tmpCF2;(_tmpCF2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF3="unknownid pat",_tag_dyneither(_tmpCF3,sizeof(char),14))),_tag_dyneither(_tmpCF2,sizeof(void*),0)));}_LL356: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp755=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp755->tag != 15)goto _LL358;}_LL357: {
const char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF6="unknowncall pat",_tag_dyneither(_tmpCF6,sizeof(char),16))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}_LL358: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp756=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp72C;if(_tmp756->tag != 16)goto _LL333;}_LL359: {
const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF9="exp pat",_tag_dyneither(_tmpCF9,sizeof(char),8))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}_LL333:;}{
# 4058
struct _tuple22 _tmpCFA;return(_tmpCFA.f1=nv,((_tmpCFA.f2=decls,((_tmpCFA.f3=s,_tmpCFA)))));};}struct _tuple25{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4095 "toc.cyc"
static struct _tuple25*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4097
struct _tuple25*_tmpCFB;return(_tmpCFB=_region_malloc(r,sizeof(*_tmpCFB)),((_tmpCFB->f1=Cyc_Toc_fresh_label(),((_tmpCFB->f2=Cyc_Toc_fresh_label(),((_tmpCFB->f3=sc,_tmpCFB)))))));}
# 4100
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4102
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp79E=(void*)_check_null(e->topt);
# 4107
int leave_as_switch;
{void*_tmp79F=Cyc_Tcutil_compress(_tmp79E);_LL365: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7A0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp79F;if(_tmp7A0->tag != 6)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7A1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp79F;if(_tmp7A1->tag != 13)goto _LL369;}_LL368:
 leave_as_switch=1;goto _LL364;_LL369:;_LL36A:
 leave_as_switch=0;goto _LL364;_LL364:;}
# 4113
{struct Cyc_List_List*_tmp7A2=scs;for(0;_tmp7A2 != 0;_tmp7A2=_tmp7A2->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7A2->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7A2->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4118
if(leave_as_switch){
# 4120
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7A3=scs;for(0;_tmp7A3 != 0;_tmp7A3=_tmp7A3->tl){
struct Cyc_Absyn_Stmt*_tmp7A4=((struct Cyc_Absyn_Switch_clause*)_tmp7A3->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7A3->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7A4,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7A5=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7A5;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7A4);;_pop_region(rgn);};}}
# 4129
return;}{
# 4132
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4137
struct _RegionHandle _tmp7A6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7A6;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7A7=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple25*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4148
{struct Cyc_List_List*_tmp7A8=lscs;for(0;_tmp7A8 != 0;_tmp7A8=_tmp7A8->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple25*)_tmp7A8->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7A8->tl == 0?end_l:(*((struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7A8->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp7AA;struct Cyc_List_List*_tmp7AB;struct Cyc_Absyn_Stmt*_tmp7AC;struct _tuple22 _tmp7A9=Cyc_Toc_xlate_pat(_tmp7A7,rgn,_tmp79E,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp7AA=_tmp7A9.f1;_tmp7AB=_tmp7A9.f2;_tmp7AC=_tmp7A9.f3;
# 4154
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7AD=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7AA,_tmp7AD);
_tmp7AC=Cyc_Absyn_seq_stmt(_tmp7AC,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7AD,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4161
decls=_tmp7AB;
{struct Cyc_List_List*_tmpCFC;nvs=((_tmpCFC=_region_malloc(rgn,sizeof(*_tmpCFC)),((_tmpCFC->hd=_tmp7AA,((_tmpCFC->tl=nvs,_tmpCFC))))));}{
struct Cyc_List_List*_tmpCFD;test_stmts=((_tmpCFD=_region_malloc(rgn,sizeof(*_tmpCFD)),((_tmpCFD->hd=_tmp7AC,((_tmpCFD->tl=test_stmts,_tmpCFD))))));};}}
# 4165
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4170
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp7B1;struct _dyneither_ptr*_tmp7B2;struct Cyc_Absyn_Switch_clause*_tmp7B3;struct _tuple25*_tmp7B0=(struct _tuple25*)lscs->hd;_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;_tmp7B3=_tmp7B0->f3;{
struct Cyc_Toc_Env*_tmp7B4=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7B3->body;
struct _RegionHandle _tmp7B5=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7B5;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp7B7;struct Cyc_Absyn_Switch_clause*_tmp7B8;struct _tuple25*_tmp7B6=(struct _tuple25*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7B7=_tmp7B6->f2;_tmp7B8=_tmp7B6->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7B4,end_l,_tmp7B7,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7B8->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4183
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7B4,end_l),s);}
# 4185
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7B1,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7B2,s,0),0);{
struct Cyc_List_List*_tmpCFE;stmts=((_tmpCFE=_region_malloc(rgn,sizeof(*_tmpCFE)),((_tmpCFE->hd=s,((_tmpCFE->tl=stmts,_tmpCFE))))));};
# 4175
;_pop_region(rgn2);};}{
# 4189
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4192
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp7BB;void*_tmp7BC;struct _tuple23*_tmp7BA=(struct _tuple23*)decls->hd;_tmp7BB=_tmp7BA->f1;_tmp7BC=_tmp7BA->f2;
res=Cyc_Absyn_declare_stmt(_tmp7BB,_tmp7BC,0,res,0);}
# 4197
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4138
;_pop_region(rgn);};};}
# 4202
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4208
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4210
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpCFF;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpCFF->tl=0,_tmpCFF)))))),0),0);}
# 4214
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4219
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4221
while(1){
void*_tmp7BE=s->r;struct Cyc_Absyn_Exp*_tmp7C1;struct Cyc_Absyn_Stmt*_tmp7C3;struct Cyc_Absyn_Stmt*_tmp7C4;struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Exp*_tmp7C8;struct Cyc_Absyn_Stmt*_tmp7C9;struct Cyc_Absyn_Stmt*_tmp7CA;struct Cyc_Absyn_Exp*_tmp7CC;struct Cyc_Absyn_Stmt*_tmp7CD;struct Cyc_Absyn_Stmt*_tmp7CF;struct Cyc_Absyn_Stmt*_tmp7D1;struct Cyc_Absyn_Stmt*_tmp7D3;struct Cyc_Absyn_Exp*_tmp7D5;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Exp*_tmp7D7;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7DA;struct Cyc_List_List*_tmp7DB;struct Cyc_List_List*_tmp7DD;struct Cyc_Absyn_Switch_clause**_tmp7DE;struct Cyc_Absyn_Decl*_tmp7E0;struct Cyc_Absyn_Stmt*_tmp7E1;struct _dyneither_ptr*_tmp7E3;struct Cyc_Absyn_Stmt*_tmp7E4;struct Cyc_Absyn_Stmt*_tmp7E6;struct Cyc_Absyn_Exp*_tmp7E7;struct Cyc_Absyn_Stmt*_tmp7E9;struct Cyc_List_List*_tmp7EA;struct Cyc_Absyn_Exp*_tmp7EC;_LL36C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7BF=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7BF->tag != 0)goto _LL36E;}_LL36D:
# 4224
 return;_LL36E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7C0=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C0->tag != 1)goto _LL370;else{_tmp7C1=_tmp7C0->f1;}}_LL36F:
# 4226
 Cyc_Toc_exp_to_c(nv,_tmp7C1);
return;_LL370: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7C2=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C2->tag != 2)goto _LL372;else{_tmp7C3=_tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}_LL371:
# 4229
 Cyc_Toc_stmt_to_c(nv,_tmp7C3);
s=_tmp7C4;
continue;_LL372: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7C5=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C5->tag != 3)goto _LL374;else{_tmp7C6=_tmp7C5->f1;}}_LL373: {
# 4233
void*topt=0;
if(_tmp7C6 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7C6->topt));
Cyc_Toc_exp_to_c(nv,_tmp7C6);}
# 4239
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp7ED=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp7EE=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp7ED,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp7ED,topt,_tmp7C6,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp7EE,0),0))->r;}else{
# 4247
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4249
return;}_LL374: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7C7=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7C7->tag != 4)goto _LL376;else{_tmp7C8=_tmp7C7->f1;_tmp7C9=_tmp7C7->f2;_tmp7CA=_tmp7C7->f3;}}_LL375:
# 4251
 Cyc_Toc_exp_to_c(nv,_tmp7C8);
Cyc_Toc_stmt_to_c(nv,_tmp7C9);
s=_tmp7CA;
continue;_LL376: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7CB=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7CB->tag != 5)goto _LL378;else{_tmp7CC=(_tmp7CB->f1).f1;_tmp7CD=_tmp7CB->f2;}}_LL377:
# 4256
 Cyc_Toc_exp_to_c(nv,_tmp7CC);{
struct _RegionHandle _tmp7EF=_new_region("temp");struct _RegionHandle*temp=& _tmp7EF;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7CD);
# 4260
_npop_handler(0);return;
# 4257
;_pop_region(temp);};_LL378: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7CE=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7CE->tag != 6)goto _LL37A;else{_tmp7CF=_tmp7CE->f1;}}_LL379: {
# 4262
struct _dyneither_ptr**_tmp7F1;struct Cyc_Toc_Env*_tmp7F0=nv;_tmp7F1=_tmp7F0->break_lab;
if(_tmp7F1 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F1,0);{
# 4266
int dest_depth=_tmp7CF == 0?0: _tmp7CF->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL37A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7D0=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D0->tag != 7)goto _LL37C;else{_tmp7D1=_tmp7D0->f1;}}_LL37B: {
# 4270
struct _dyneither_ptr**_tmp7F3;struct Cyc_Toc_Env*_tmp7F2=nv;_tmp7F3=_tmp7F2->continue_lab;
if(_tmp7F3 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp7F3,0);
_tmp7D3=_tmp7D1;goto _LL37D;}_LL37C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7D2=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D2->tag != 8)goto _LL37E;else{_tmp7D3=_tmp7D2->f2;}}_LL37D:
# 4276
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7D3))->try_depth,s);
return;_LL37E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7D4=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D4->tag != 9)goto _LL380;else{_tmp7D5=_tmp7D4->f1;_tmp7D6=(_tmp7D4->f2).f1;_tmp7D7=(_tmp7D4->f3).f1;_tmp7D8=_tmp7D4->f4;}}_LL37F:
# 4280
 Cyc_Toc_exp_to_c(nv,_tmp7D5);Cyc_Toc_exp_to_c(nv,_tmp7D6);Cyc_Toc_exp_to_c(nv,_tmp7D7);{
struct _RegionHandle _tmp7F4=_new_region("temp");struct _RegionHandle*temp=& _tmp7F4;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7D8);
# 4284
_npop_handler(0);return;
# 4281
;_pop_region(temp);};_LL380: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7D9=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7D9->tag != 10)goto _LL382;else{_tmp7DA=_tmp7D9->f1;_tmp7DB=_tmp7D9->f2;}}_LL381:
# 4286
 Cyc_Toc_xlate_switch(nv,s,_tmp7DA,_tmp7DB);
return;_LL382: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7DC=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7DC->tag != 11)goto _LL384;else{_tmp7DD=_tmp7DC->f1;_tmp7DE=_tmp7DC->f2;}}_LL383: {
# 4289
struct Cyc_Toc_FallthruInfo*_tmp7F6;struct Cyc_Toc_Env*_tmp7F5=nv;_tmp7F6=_tmp7F5->fallthru_info;
if(_tmp7F6 == 0){
const char*_tmpD02;void*_tmpD01;(_tmpD01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD02="fallthru in unexpected place",_tag_dyneither(_tmpD02,sizeof(char),29))),_tag_dyneither(_tmpD01,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp7FA;struct Cyc_List_List*_tmp7FB;struct Cyc_Dict_Dict _tmp7FC;struct Cyc_Toc_FallthruInfo _tmp7F9=*_tmp7F6;_tmp7FA=_tmp7F9.label;_tmp7FB=_tmp7F9.binders;_tmp7FC=_tmp7F9.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp7FA,0);
# 4295
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7DE)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp7FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7FB);
struct Cyc_List_List*_tmp7FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7DD);
for(0;_tmp7FD != 0;(_tmp7FD=_tmp7FD->tl,_tmp7FE=_tmp7FE->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7FE))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp7FC,(struct _tuple0*)_tmp7FD->hd),(struct Cyc_Absyn_Exp*)_tmp7FE->hd,0),s2,0);}
# 4303
s->r=s2->r;
return;};};};}_LL384: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7DF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7DF->tag != 12)goto _LL386;else{_tmp7E0=_tmp7DF->f1;_tmp7E1=_tmp7DF->f2;}}_LL385:
# 4309
{void*_tmp7FF=_tmp7E0->r;struct Cyc_Absyn_Vardecl*_tmp801;struct Cyc_Absyn_Pat*_tmp803;struct Cyc_Absyn_Exp*_tmp804;struct Cyc_List_List*_tmp806;struct Cyc_Absyn_Fndecl*_tmp808;struct Cyc_Absyn_Tvar*_tmp80A;struct Cyc_Absyn_Vardecl*_tmp80B;int _tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Tvar*_tmp80F;struct Cyc_Absyn_Vardecl*_tmp810;_LL38F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp800=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp800->tag != 0)goto _LL391;else{_tmp801=_tmp800->f1;}}_LL390: {
# 4311
struct _RegionHandle _tmp811=_new_region("temp");struct _RegionHandle*temp=& _tmp811;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD05;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD04;struct Cyc_Toc_Env*_tmp812=Cyc_Toc_add_varmap(temp,nv,_tmp801->name,
Cyc_Absyn_varb_exp(_tmp801->name,(void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=4,((_tmpD05.f1=_tmp801,_tmpD05)))),_tmpD04)))),0));
Cyc_Toc_local_decl_to_c(_tmp812,_tmp812,_tmp801,_tmp7E1);}
# 4316
_npop_handler(0);goto _LL38E;
# 4311
;_pop_region(temp);}_LL391: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp802=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp802->tag != 2)goto _LL393;else{_tmp803=_tmp802->f1;_tmp804=_tmp802->f3;}}_LL392:
# 4320
{void*_tmp815=_tmp803->r;struct Cyc_Absyn_Vardecl*_tmp817;_LL39E: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp816=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp815;if(_tmp816->tag != 1)goto _LL3A0;else{_tmp817=_tmp816->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp818=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp816->f2)->r;if(_tmp818->tag != 0)goto _LL3A0;};}}_LL39F: {
# 4322
struct _tuple0*old_name=_tmp817->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp817->name=new_name;
_tmp817->initializer=_tmp804;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD08;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD07;_tmp7E0->r=(void*)((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=((_tmpD08.tag=0,((_tmpD08.f1=_tmp817,_tmpD08)))),_tmpD07))));}{
struct _RegionHandle _tmp81B=_new_region("temp");struct _RegionHandle*temp=& _tmp81B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD0B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD0A;struct Cyc_Toc_Env*_tmp81C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=4,((_tmpD0B.f1=_tmp817,_tmpD0B)))),_tmpD0A)))),0));
Cyc_Toc_local_decl_to_c(_tmp81C,nv,_tmp817,_tmp7E1);}
# 4333
_npop_handler(0);goto _LL39D;
# 4327
;_pop_region(temp);};}_LL3A0:;_LL3A1:
# 4338
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp803,(void*)_check_null(_tmp804->topt),_tmp804,_tmp7E1))->r;
goto _LL39D;_LL39D:;}
# 4341
goto _LL38E;_LL393: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp805=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp805->tag != 3)goto _LL395;else{_tmp806=_tmp805->f1;}}_LL394: {
# 4354 "toc.cyc"
struct Cyc_List_List*_tmp81F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp806);
if(_tmp81F == 0){
const char*_tmpD0E;void*_tmpD0D;(_tmpD0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD0E="empty Letv_d",_tag_dyneither(_tmpD0E,sizeof(char),13))),_tag_dyneither(_tmpD0D,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD11;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD10;_tmp7E0->r=(void*)((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10[0]=((_tmpD11.tag=0,((_tmpD11.f1=(struct Cyc_Absyn_Vardecl*)_tmp81F->hd,_tmpD11)))),_tmpD10))));}
_tmp81F=_tmp81F->tl;
for(0;_tmp81F != 0;_tmp81F=_tmp81F->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD14;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD13;struct Cyc_Absyn_Decl*_tmp824=Cyc_Absyn_new_decl((void*)((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13[0]=((_tmpD14.tag=0,((_tmpD14.f1=(struct Cyc_Absyn_Vardecl*)_tmp81F->hd,_tmpD14)))),_tmpD13)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp824,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4363
Cyc_Toc_stmt_to_c(nv,s);
goto _LL38E;}_LL395: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp807=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp807->tag != 1)goto _LL397;else{_tmp808=_tmp807->f1;}}_LL396: {
# 4366
struct _tuple0*_tmp827=_tmp808->name;
struct _RegionHandle _tmp828=_new_region("temp");struct _RegionHandle*temp=& _tmp828;_push_region(temp);{
struct Cyc_Toc_Env*_tmp829=Cyc_Toc_add_varmap(temp,nv,_tmp808->name,Cyc_Absyn_var_exp(_tmp827,0));
Cyc_Toc_fndecl_to_c(_tmp829,_tmp808,0);
Cyc_Toc_stmt_to_c(_tmp829,_tmp7E1);}
# 4372
_npop_handler(0);goto _LL38E;
# 4367
;_pop_region(temp);}_LL397: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp809=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp809->tag != 4)goto _LL399;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;_tmp80C=_tmp809->f3;_tmp80D=_tmp809->f4;}}_LL398: {
# 4374
struct Cyc_Absyn_Stmt*_tmp82A=_tmp7E1;
# 4390 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp80B->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4397
struct _RegionHandle _tmp82B=_new_region("temp");struct _RegionHandle*temp=& _tmp82B;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp82A);
# 4400
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp82A,0))->r;else{
if(_tmp80D == 0){
struct Cyc_Absyn_Exp*_tmpD17[1];struct Cyc_Absyn_Exp*_tmpD16[1];struct Cyc_List_List*_tmpD15;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD15->tl=0,_tmpD15)))))),0),
# 4409
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD16[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD16,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp82A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD17[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD17,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4415
Cyc_Toc_exp_to_c(nv,_tmp80D);{
struct Cyc_Absyn_Exp*_tmpD19[1];struct Cyc_Absyn_Exp*_tmpD18[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpD18[1]=_tmp80D,((_tmpD18[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD18,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4423
Cyc_Absyn_seq_stmt(_tmp82A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpD19[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD19,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4428
_npop_handler(0);return;
# 4397
;_pop_region(temp);}_LL399: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp80E=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp7FF;if(_tmp80E->tag != 5)goto _LL39B;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;}}_LL39A: {
# 4431
struct _tuple0*old_name=_tmp810->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp810->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD28;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD27;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD26;struct Cyc_Absyn_Decl*_tmpD25;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD24;s->r=(void*)((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD28.tag=12,((_tmpD28.f1=((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->r=(void*)((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27[0]=((_tmpD26.tag=0,((_tmpD26.f1=_tmp810,_tmpD26)))),_tmpD27)))),((_tmpD25->loc=0,_tmpD25)))))),((_tmpD28.f2=_tmp7E1,_tmpD28)))))),_tmpD24))));}{
struct _RegionHandle _tmp836=_new_region("temp");struct _RegionHandle*temp=& _tmp836;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD2B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD2A;struct Cyc_Toc_Env*_tmp837=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A[0]=((_tmpD2B.tag=4,((_tmpD2B.f1=_tmp810,_tmpD2B)))),_tmpD2A)))),0));
Cyc_Toc_local_decl_to_c(_tmp837,nv,_tmp810,_tmp7E1);}
# 4441
_npop_handler(0);return;
# 4435
;_pop_region(temp);};}_LL39B:;_LL39C: {
# 4443
const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD2E="bad nested declaration within function",_tag_dyneither(_tmpD2E,sizeof(char),39))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}_LL38E:;}
# 4445
return;_LL386: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7E2=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7E2->tag != 13)goto _LL388;else{_tmp7E3=_tmp7E2->f1;_tmp7E4=_tmp7E2->f2;}}_LL387:
# 4447
 s=_tmp7E4;continue;_LL388: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7E5=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7E5->tag != 14)goto _LL38A;else{_tmp7E6=_tmp7E5->f1;_tmp7E7=(_tmp7E5->f2).f1;}}_LL389: {
# 4449
struct _RegionHandle _tmp83C=_new_region("temp");struct _RegionHandle*temp=& _tmp83C;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E6);
Cyc_Toc_exp_to_c(nv,_tmp7E7);
# 4453
_npop_handler(0);return;
# 4449
;_pop_region(temp);}_LL38A: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7E8=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7E8->tag != 15)goto _LL38C;else{_tmp7E9=_tmp7E8->f1;_tmp7EA=_tmp7E8->f2;}}_LL38B: {
# 4470 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4481
e_exp->topt=e_typ;{
struct _RegionHandle _tmp83D=_new_region("temp");struct _RegionHandle*temp=& _tmp83D;_push_region(temp);{
struct Cyc_Toc_Env*_tmp83E=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4485
Cyc_Toc_stmt_to_c(_tmp83E,_tmp7E9);{
struct Cyc_Absyn_Stmt*_tmp83F=Cyc_Absyn_seq_stmt(_tmp7E9,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4491
struct _tuple0*_tmp840=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp841=Cyc_Absyn_var_exp(_tmp840,0);
struct Cyc_Absyn_Vardecl*_tmp842=Cyc_Absyn_new_vardecl(_tmp840,Cyc_Absyn_exn_typ(),0);
_tmp841->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD38;struct Cyc_Absyn_Pat*_tmpD37;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD36;struct Cyc_Absyn_Pat*_tmpD35;struct Cyc_Absyn_Pat*_tmp843=(_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->r=(void*)((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD36.tag=1,((_tmpD36.f1=_tmp842,((_tmpD36.f2=((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD37->topt=Cyc_Absyn_exn_typ(),((_tmpD37->loc=0,_tmpD37)))))))),_tmpD36)))))),_tmpD38)))),((_tmpD35->topt=Cyc_Absyn_exn_typ(),((_tmpD35->loc=0,_tmpD35)))))));
struct Cyc_Absyn_Exp*_tmp844=Cyc_Absyn_throw_exp(_tmp841,0);
_tmp844->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp845=Cyc_Absyn_exp_stmt(_tmp844,0);
struct Cyc_Core_Opt*_tmpD3E;struct Cyc_List_List*_tmpD3D;struct Cyc_Absyn_Switch_clause*_tmpD3C;struct Cyc_Absyn_Switch_clause*_tmp846=
(_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->pattern=_tmp843,((_tmpD3C->pat_vars=((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->v=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->hd=_tmp842,((_tmpD3D->tl=0,_tmpD3D)))))),_tmpD3E)))),((_tmpD3C->where_clause=0,((_tmpD3C->body=_tmp845,((_tmpD3C->loc=0,_tmpD3C)))))))))));
# 4502
struct Cyc_List_List*_tmpD3F;struct Cyc_Absyn_Stmt*_tmp847=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7EA,(
(_tmpD3F=_cycalloc(sizeof(*_tmpD3F)),((_tmpD3F->hd=_tmp846,((_tmpD3F->tl=0,_tmpD3F))))))),0);
# 4506
Cyc_Toc_stmt_to_c(_tmp83E,_tmp847);{
# 4509
struct Cyc_List_List*_tmpD40;struct Cyc_Absyn_Exp*_tmp848=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD40->tl=0,_tmpD40)))))),0);
# 4513
struct Cyc_List_List*_tmpD41;struct Cyc_Absyn_Stmt*_tmp849=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD41->tl=0,_tmpD41)))))),0),0);
# 4517
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
# 4528
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp847,0),0),0),0),0),0))->r;};};};};}
# 4532
_npop_handler(0);return;
# 4482
;_pop_region(temp);};}_LL38C: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp7EB=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7BE;if(_tmp7EB->tag != 16)goto _LL36B;else{_tmp7EC=_tmp7EB->f1;}}_LL38D:
# 4534
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4537
Cyc_Toc_exp_to_c(nv,_tmp7EC);{
struct Cyc_List_List*_tmpD42;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->hd=_tmp7EC,((_tmpD42->tl=0,_tmpD42)))))),0));};}
# 4540
return;_LL36B:;}}
# 4549
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp857=_new_region("frgn");struct _RegionHandle*frgn=& _tmp857;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp858=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp859=f->args;for(0;_tmp859 != 0;_tmp859=_tmp859->tl){
struct _tuple0*_tmpD43;struct _tuple0*_tmp85A=(_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->f1=Cyc_Absyn_Loc_n,((_tmpD43->f2=(*((struct _tuple8*)_tmp859->hd)).f1,_tmpD43)))));
(*((struct _tuple8*)_tmp859->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp859->hd)).f3);
_tmp858=Cyc_Toc_add_varmap(frgn,_tmp858,_tmp85A,Cyc_Absyn_var_exp(_tmp85A,0));}}
# 4564
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4568
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp85D;struct Cyc_Absyn_Tqual _tmp85E;void*_tmp85F;int _tmp860;struct Cyc_Absyn_VarargInfo _tmp85C=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp85D=_tmp85C.name;_tmp85E=_tmp85C.tq;_tmp85F=_tmp85C.type;_tmp860=_tmp85C.inject;{
void*_tmp861=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp85F,(void*)& Cyc_Absyn_HeapRgn_val,_tmp85E,Cyc_Absyn_false_conref));
struct _tuple0*_tmpD44;struct _tuple0*_tmp862=(_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->f1=Cyc_Absyn_Loc_n,((_tmpD44->f2=(struct _dyneither_ptr*)_check_null(_tmp85D),_tmpD44)))));
{struct _tuple8*_tmpD47;struct Cyc_List_List*_tmpD46;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->f1=_tmp85D,((_tmpD47->f2=_tmp85E,((_tmpD47->f3=_tmp861,_tmpD47)))))))),((_tmpD46->tl=0,_tmpD46)))))));}
_tmp858=Cyc_Toc_add_varmap(frgn,_tmp858,_tmp862,Cyc_Absyn_var_exp(_tmp862,0));
f->cyc_varargs=0;};}
# 4577
{struct Cyc_List_List*_tmp866=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp866 != 0;_tmp866=_tmp866->tl){
((struct Cyc_Absyn_Vardecl*)_tmp866->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp866->hd)->type);}}
# 4580
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp858),f->body);}
# 4556
;_pop_region(frgn);};}
# 4585
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3A2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3A3:
 return Cyc_Absyn_Extern;default: _LL3A4:
 return s;}}
# 4601 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp867=ad->name;
struct _DynRegionHandle*_tmp869;struct Cyc_Dict_Dict*_tmp86A;struct Cyc_Toc_TocState _tmp868=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp869=_tmp868.dyn;_tmp86A=_tmp868.aggrs_so_far;{
struct _DynRegionFrame _tmp86B;struct _RegionHandle*d=_open_dynregion(& _tmp86B,_tmp869);
{int seen_defn_before;
struct _tuple14**_tmp86C=((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp86A,_tmp867);
if(_tmp86C == 0){
seen_defn_before=0;{
struct _tuple14*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple14*_tmpD48;v=((_tmpD48=_region_malloc(d,sizeof(*_tmpD48)),((_tmpD48->f1=ad,((_tmpD48->f2=Cyc_Absyn_strctq(_tmp867),_tmpD48))))));}else{
# 4613
struct _tuple14*_tmpD49;v=((_tmpD49=_region_malloc(d,sizeof(*_tmpD49)),((_tmpD49->f1=ad,((_tmpD49->f2=Cyc_Absyn_unionq_typ(_tmp867),_tmpD49))))));}
*_tmp86A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp86A,_tmp867,v);};}else{
# 4616
struct Cyc_Absyn_Aggrdecl*_tmp870;void*_tmp871;struct _tuple14*_tmp86F=*_tmp86C;_tmp870=_tmp86F->f1;_tmp871=_tmp86F->f2;
if(_tmp870->impl == 0){
{struct _tuple14*_tmpD4A;*_tmp86A=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(*_tmp86A,_tmp867,((_tmpD4A=_region_malloc(d,sizeof(*_tmpD4A)),((_tmpD4A->f1=ad,((_tmpD4A->f2=_tmp871,_tmpD4A)))))));}
seen_defn_before=0;}else{
# 4621
seen_defn_before=1;}}{
# 4623
struct Cyc_Absyn_Aggrdecl*_tmpD4B;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->kind=ad->kind,((_tmpD4B->sc=Cyc_Absyn_Public,((_tmpD4B->name=ad->name,((_tmpD4B->tvs=0,((_tmpD4B->impl=0,((_tmpD4B->attributes=ad->attributes,_tmpD4B)))))))))))));
# 4629
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD4C;new_ad->impl=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->exist_vars=0,((_tmpD4C->rgn_po=0,((_tmpD4C->fields=0,((_tmpD4C->tagged=0,_tmpD4C))))))))));}{
# 4634
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp874=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp874 != 0;_tmp874=_tmp874->tl){
# 4638
struct Cyc_Absyn_Aggrfield*_tmp875=(struct Cyc_Absyn_Aggrfield*)_tmp874->hd;
void*_tmp876=_tmp875->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp876)) && (
ad->kind == Cyc_Absyn_StructA  && _tmp874->tl == 0  || ad->kind == Cyc_Absyn_UnionA)){
# 4648
void*_tmp877=Cyc_Tcutil_compress(_tmp876);void*_tmp879;struct Cyc_Absyn_Tqual _tmp87A;union Cyc_Absyn_Constraint*_tmp87B;unsigned int _tmp87C;_LL3A7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp878=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp877;if(_tmp878->tag != 8)goto _LL3A9;else{_tmp879=(_tmp878->f1).elt_type;_tmp87A=(_tmp878->f1).tq;_tmp87B=(_tmp878->f1).zero_term;_tmp87C=(_tmp878->f1).zt_loc;}}_LL3A8:
# 4650
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD52;struct Cyc_Absyn_ArrayInfo _tmpD51;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD50;_tmp876=(void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD52.tag=8,((_tmpD52.f1=((_tmpD51.elt_type=_tmp879,((_tmpD51.tq=_tmp87A,((_tmpD51.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD51.zero_term=_tmp87B,((_tmpD51.zt_loc=_tmp87C,_tmpD51)))))))))),_tmpD52)))),_tmpD50))));}
goto _LL3A6;_LL3A9:;_LL3AA:
# 4653
 if(Cyc_Toc_array_or_aggr_with_array(_tmp876))goto _LL3A6;else{
if(ad->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpD55;void*_tmpD54;(_tmpD54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD55="union with non-array fields of kind TA",_tag_dyneither(_tmpD55,sizeof(char),39))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}}_LL3A6:;}{
# 4658
struct Cyc_Absyn_Aggrfield*_tmpD56;struct Cyc_Absyn_Aggrfield*_tmp882=(_tmpD56=_cycalloc(sizeof(*_tmpD56)),((_tmpD56->name=_tmp875->name,((_tmpD56->tq=Cyc_Toc_mt_tq,((_tmpD56->type=
# 4660
Cyc_Toc_typ_to_c(_tmp876),((_tmpD56->width=_tmp875->width,((_tmpD56->attributes=_tmp875->attributes,_tmpD56)))))))))));
# 4666
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp883=_tmp882->type;
struct _dyneither_ptr*_tmp884=_tmp882->name;
const char*_tmpD5B;void*_tmpD5A[2];struct Cyc_String_pa_PrintArg_struct _tmpD59;struct Cyc_String_pa_PrintArg_struct _tmpD58;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD58.tag=0,((_tmpD58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp884),((_tmpD59.tag=0,((_tmpD59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD5A[0]=& _tmpD59,((_tmpD5A[1]=& _tmpD58,Cyc_aprintf(((_tmpD5B="_union_%s_%s",_tag_dyneither(_tmpD5B,sizeof(char),13))),_tag_dyneither(_tmpD5A,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD5C;struct _dyneither_ptr*str=(_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C[0]=s,_tmpD5C)));
struct Cyc_Absyn_Aggrfield*_tmpD5D;struct Cyc_Absyn_Aggrfield*_tmp885=(_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D->name=Cyc_Toc_val_sp,((_tmpD5D->tq=Cyc_Toc_mt_tq,((_tmpD5D->type=_tmp883,((_tmpD5D->width=0,((_tmpD5D->attributes=0,_tmpD5D)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD5E;struct Cyc_Absyn_Aggrfield*_tmp886=(_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->name=Cyc_Toc_tag_sp,((_tmpD5E->tq=Cyc_Toc_mt_tq,((_tmpD5E->type=Cyc_Absyn_sint_typ,((_tmpD5E->width=0,((_tmpD5E->attributes=0,_tmpD5E)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD5F[2];struct Cyc_List_List*_tmp887=(_tmpD5F[1]=_tmp885,((_tmpD5F[0]=_tmp886,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD5F,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD64;struct _tuple0*_tmpD63;struct Cyc_Absyn_Aggrdecl*_tmpD62;struct Cyc_Absyn_Aggrdecl*_tmp888=(_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->kind=Cyc_Absyn_StructA,((_tmpD62->sc=Cyc_Absyn_Public,((_tmpD62->name=(
(_tmpD63=_cycalloc(sizeof(*_tmpD63)),((_tmpD63->f1=Cyc_Absyn_Loc_n,((_tmpD63->f2=str,_tmpD63)))))),((_tmpD62->tvs=0,((_tmpD62->impl=(
(_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->exist_vars=0,((_tmpD64->rgn_po=0,((_tmpD64->fields=_tmp887,((_tmpD64->tagged=0,_tmpD64)))))))))),((_tmpD62->attributes=0,_tmpD62)))))))))))));
# 4678
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD6A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD69;struct Cyc_List_List*_tmpD68;Cyc_Toc_result_decls=((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((_tmpD68->hd=Cyc_Absyn_new_decl((void*)((_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A[0]=((_tmpD69.tag=6,((_tmpD69.f1=_tmp888,_tmpD69)))),_tmpD6A)))),0),((_tmpD68->tl=Cyc_Toc_result_decls,_tmpD68))))));}
_tmp882->type=Cyc_Absyn_strct(str);}{
# 4681
struct Cyc_List_List*_tmpD6B;new_fields=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=_tmp882,((_tmpD6B->tl=new_fields,_tmpD6B))))));};};}}
# 4683
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4685
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD75;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD74;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD73;struct Cyc_List_List*_tmpD72;Cyc_Toc_result_decls=((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->hd=((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->r=(void*)((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73[0]=((_tmpD74.tag=6,((_tmpD74.f1=new_ad,_tmpD74)))),_tmpD73)))),((_tmpD75->loc=0,_tmpD75)))))),((_tmpD72->tl=Cyc_Toc_result_decls,_tmpD72))))));}};}
# 4605
;_pop_dynregion(d);};}
# 4714 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp89F;struct Cyc_Set_Set**_tmp8A0;struct Cyc_Toc_TocState _tmp89E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp89F=_tmp89E.dyn;_tmp8A0=_tmp89E.datatypes_so_far;{
struct _DynRegionFrame _tmp8A1;struct _RegionHandle*d=_open_dynregion(& _tmp8A1,_tmp89F);{
struct _tuple0*_tmp8A2=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp8A0,_tmp8A2)){
_npop_handler(0);return;}
*_tmp8A0=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp8A0,_tmp8A2);}{
# 4722
struct Cyc_List_List*_tmp8A3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8A3 != 0;_tmp8A3=_tmp8A3->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8A3->hd;
# 4725
struct Cyc_List_List*_tmp8A4=0;
int i=1;
{struct Cyc_List_List*_tmp8A5=f->typs;for(0;_tmp8A5 != 0;(_tmp8A5=_tmp8A5->tl,i ++)){
struct _dyneither_ptr*_tmp8A6=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD76;struct Cyc_Absyn_Aggrfield*_tmp8A7=(_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->name=_tmp8A6,((_tmpD76->tq=(*((struct _tuple10*)_tmp8A5->hd)).f1,((_tmpD76->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8A5->hd)).f2),((_tmpD76->width=0,((_tmpD76->attributes=0,_tmpD76)))))))))));
struct Cyc_List_List*_tmpD77;_tmp8A4=((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->hd=_tmp8A7,((_tmpD77->tl=_tmp8A4,_tmpD77))))));}}
# 4733
{struct Cyc_Absyn_Aggrfield*_tmpD7A;struct Cyc_List_List*_tmpD79;_tmp8A4=((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((_tmpD79->hd=((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->name=Cyc_Toc_tag_sp,((_tmpD7A->tq=Cyc_Toc_mt_tq,((_tmpD7A->type=Cyc_Absyn_sint_typ,((_tmpD7A->width=0,((_tmpD7A->attributes=0,_tmpD7A)))))))))))),((_tmpD79->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8A4),_tmpD79))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD7D;struct Cyc_Absyn_Aggrdecl*_tmpD7C;struct Cyc_Absyn_Aggrdecl*_tmp8AC=
(_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C->kind=Cyc_Absyn_StructA,((_tmpD7C->sc=Cyc_Absyn_Public,((_tmpD7C->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD7C->tvs=0,((_tmpD7C->impl=(
# 4739
(_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->exist_vars=0,((_tmpD7D->rgn_po=0,((_tmpD7D->fields=_tmp8A4,((_tmpD7D->tagged=0,_tmpD7D)))))))))),((_tmpD7C->attributes=0,_tmpD7C)))))))))))));
# 4741
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD83;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD82;struct Cyc_List_List*_tmpD81;Cyc_Toc_result_decls=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->hd=Cyc_Absyn_new_decl((void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD82.tag=6,((_tmpD82.f1=_tmp8AC,_tmpD82)))),_tmpD83)))),0),((_tmpD81->tl=Cyc_Toc_result_decls,_tmpD81))))));};}};
# 4716
;_pop_dynregion(d);};}
# 4762 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp8B3;struct Cyc_Dict_Dict*_tmp8B4;struct Cyc_Toc_TocState _tmp8B2=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B3=_tmp8B2.dyn;_tmp8B4=_tmp8B2.xdatatypes_so_far;{
struct _DynRegionFrame _tmp8B5;struct _RegionHandle*d=_open_dynregion(& _tmp8B5,_tmp8B3);
{struct _tuple0*_tmp8B6=xd->name;
struct Cyc_List_List*_tmp8B7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp8B7 != 0;_tmp8B7=_tmp8B7->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8B7->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8B9=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8B8,Cyc_Absyn_false_conref,0);
# 4774
int*_tmp8BA=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8B4,f->name);_LL3AC: if(_tmp8BA != 0)goto _LL3AE;_LL3AD: {
# 4776
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4780
struct Cyc_Absyn_Vardecl*_tmp8BB=Cyc_Absyn_new_vardecl(f->name,_tmp8B9,initopt);
_tmp8BB->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD89;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD88;struct Cyc_List_List*_tmpD87;Cyc_Toc_result_decls=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->hd=Cyc_Absyn_new_decl((void*)((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89[0]=((_tmpD88.tag=0,((_tmpD88.f1=_tmp8BB,_tmpD88)))),_tmpD89)))),0),((_tmpD87->tl=Cyc_Toc_result_decls,_tmpD87))))));}
*_tmp8B4=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8B4,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8BF=f->typs;for(0;_tmp8BF != 0;(_tmp8BF=_tmp8BF->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD91;void*_tmpD90[1];const char*_tmpD8F;struct _dyneither_ptr*_tmpD8E;struct _dyneither_ptr*_tmp8C0=(_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[0]=(struct _dyneither_ptr)((_tmpD91.tag=1,((_tmpD91.f1=(unsigned long)i,((_tmpD90[0]=& _tmpD91,Cyc_aprintf(((_tmpD8F="f%d",_tag_dyneither(_tmpD8F,sizeof(char),4))),_tag_dyneither(_tmpD90,sizeof(void*),1)))))))),_tmpD8E)));
struct Cyc_Absyn_Aggrfield*_tmpD92;struct Cyc_Absyn_Aggrfield*_tmp8C1=(_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->name=_tmp8C0,((_tmpD92->tq=(*((struct _tuple10*)_tmp8BF->hd)).f1,((_tmpD92->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8BF->hd)).f2),((_tmpD92->width=0,((_tmpD92->attributes=0,_tmpD92)))))))))));
struct Cyc_List_List*_tmpD93;fields=((_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->hd=_tmp8C1,((_tmpD93->tl=fields,_tmpD93))))));}}
# 4793
{struct Cyc_Absyn_Aggrfield*_tmpD96;struct Cyc_List_List*_tmpD95;fields=((_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95->hd=((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96->name=Cyc_Toc_tag_sp,((_tmpD96->tq=Cyc_Toc_mt_tq,((_tmpD96->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD96->width=0,((_tmpD96->attributes=0,_tmpD96)))))))))))),((_tmpD95->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD95))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD99;struct Cyc_Absyn_Aggrdecl*_tmpD98;struct Cyc_Absyn_Aggrdecl*_tmp8CA=
(_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98->kind=Cyc_Absyn_StructA,((_tmpD98->sc=Cyc_Absyn_Public,((_tmpD98->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD98->tvs=0,((_tmpD98->impl=(
# 4800
(_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->exist_vars=0,((_tmpD99->rgn_po=0,((_tmpD99->fields=fields,((_tmpD99->tagged=0,_tmpD99)))))))))),((_tmpD98->attributes=0,_tmpD98)))))))))))));
# 4802
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD9F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD9E;struct Cyc_List_List*_tmpD9D;Cyc_Toc_result_decls=((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->hd=Cyc_Absyn_new_decl((void*)((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=((_tmpD9E.tag=6,((_tmpD9E.f1=_tmp8CA,_tmpD9E)))),_tmpD9F)))),0),((_tmpD9D->tl=Cyc_Toc_result_decls,_tmpD9D))))));}
# 4804
goto _LL3AB;};};};}_LL3AE: if(_tmp8BA == 0)goto _LL3B0;if(*_tmp8BA != 0)goto _LL3B0;_LL3AF:
# 4806
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8D0=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8D1=Cyc_Absyn_new_vardecl(f->name,_tmp8B9,_tmp8D0);
_tmp8D1->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDA5;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDA4;struct Cyc_List_List*_tmpDA3;Cyc_Toc_result_decls=((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->hd=Cyc_Absyn_new_decl((void*)((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5[0]=((_tmpDA4.tag=0,((_tmpDA4.f1=_tmp8D1,_tmpDA4)))),_tmpDA5)))),0),((_tmpDA3->tl=Cyc_Toc_result_decls,_tmpDA3))))));}
*_tmp8B4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8B4,f->name,1);}
# 4813
goto _LL3AB;_LL3B0:;_LL3B1:
 goto _LL3AB;_LL3AB:;}}
# 4767
;_pop_dynregion(d);};};}
# 4820
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4828
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4832
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8D5=init->r;struct Cyc_Absyn_Vardecl*_tmp8D7;struct Cyc_Absyn_Exp*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D9;int _tmp8DA;_LL3B3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8D6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8D5;if(_tmp8D6->tag != 26)goto _LL3B5;else{_tmp8D7=_tmp8D6->f1;_tmp8D8=_tmp8D6->f2;_tmp8D9=_tmp8D6->f3;_tmp8DA=_tmp8D6->f4;}}_LL3B4:
# 4839
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8D7,_tmp8D8,_tmp8D9,_tmp8DA,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3B2;_LL3B5:;_LL3B6:
# 4844
 if(vd->sc == Cyc_Absyn_Static){
# 4848
struct _RegionHandle _tmp8DB=_new_region("temp");struct _RegionHandle*temp=& _tmp8DB;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8DC=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8DC,init);}
# 4849
;_pop_region(temp);}else{
# 4853
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3B2;_LL3B2:;}else{
# 4858
void*_tmp8DD=Cyc_Tcutil_compress(old_typ);void*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8E0;union Cyc_Absyn_Constraint*_tmp8E1;_LL3B8:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8DE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8DD;if(_tmp8DE->tag != 8)goto _LL3BA;else{_tmp8DF=(_tmp8DE->f1).elt_type;_tmp8E0=(_tmp8DE->f1).num_elts;_tmp8E1=(_tmp8DE->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8E1))goto _LL3BA;_LL3B9:
 if(_tmp8E0 == 0){
const char*_tmpDA8;void*_tmpDA7;(_tmpDA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA8="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpDA8,sizeof(char),55))),_tag_dyneither(_tmpDA7,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8E0;
struct Cyc_Absyn_Exp*_tmp8E4=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4866
struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8E4,_tmp8E5,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B7;};_LL3BA:;_LL3BB:
 goto _LL3B7;_LL3B7:;}}
# 4880
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8E6=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8E6;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8E7=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8E8=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8EA;struct Cyc_List_List*_tmp8EB;struct Cyc_Absyn_Stmt*_tmp8EC;struct _tuple22 _tmp8E9=
Cyc_Toc_xlate_pat(_tmp8E8,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8EA=_tmp8E9.f1;_tmp8EB=_tmp8E9.f2;_tmp8EC=_tmp8E9.f3;
Cyc_Toc_stmt_to_c(_tmp8EA,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8EC,s,0),0);
for(0;_tmp8EB != 0;_tmp8EB=_tmp8EB->tl){
struct _tuple0*_tmp8EE;void*_tmp8EF;struct _tuple23*_tmp8ED=(struct _tuple23*)_tmp8EB->hd;_tmp8EE=_tmp8ED->f1;_tmp8EF=_tmp8ED->f2;
s=Cyc_Absyn_declare_stmt(_tmp8EE,_tmp8EF,0,s,0);}}{
# 4896
struct Cyc_Absyn_Stmt*_tmp8F0=s;_npop_handler(0);return _tmp8F0;};
# 4883
;_pop_region(prgn);};}
# 4902
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp8F1=e->r;struct Cyc_Absyn_Exp*_tmp8F3;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_Absyn_Exp*_tmp8F7;struct Cyc_Absyn_Exp*_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FB;struct Cyc_Absyn_Exp*_tmp8FD;struct Cyc_Absyn_Exp*_tmp8FF;struct Cyc_Absyn_Exp*_tmp901;struct Cyc_List_List*_tmp903;struct Cyc_Absyn_Exp*_tmp905;struct Cyc_Absyn_Exp*_tmp906;struct Cyc_Absyn_Exp*_tmp908;struct Cyc_Absyn_Exp*_tmp909;struct Cyc_Absyn_Exp*_tmp90B;struct Cyc_Absyn_Exp*_tmp90C;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp918;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_List_List*_tmp91C;void**_tmp91E;struct Cyc_Absyn_Exp*_tmp91F;void**_tmp921;struct Cyc_List_List*_tmp922;struct Cyc_List_List*_tmp924;struct Cyc_List_List*_tmp926;void**_tmp928;void**_tmp92A;struct Cyc_Absyn_Stmt*_tmp92C;struct Cyc_Absyn_MallocInfo*_tmp92E;_LL3BD: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp8F2=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8F2->tag != 19)goto _LL3BF;else{_tmp8F3=_tmp8F2->f1;}}_LL3BE:
 _tmp8F5=_tmp8F3;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp8F4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8F4->tag != 20)goto _LL3C1;else{_tmp8F5=_tmp8F4->f1;}}_LL3C0:
 _tmp8F7=_tmp8F5;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp8F6=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8F6->tag != 21)goto _LL3C3;else{_tmp8F7=_tmp8F6->f1;}}_LL3C2:
 _tmp8F9=_tmp8F7;goto _LL3C4;_LL3C3: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp8F8=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8F8->tag != 14)goto _LL3C5;else{_tmp8F9=_tmp8F8->f1;}}_LL3C4:
 _tmp8FB=_tmp8F9;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp8FA=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8FA->tag != 10)goto _LL3C7;else{_tmp8FB=_tmp8FA->f1;}}_LL3C6:
 _tmp8FD=_tmp8FB;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp8FC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8FC->tag != 11)goto _LL3C9;else{_tmp8FD=_tmp8FC->f1;}}_LL3C8:
 _tmp8FF=_tmp8FD;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp8FE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp8FE->tag != 17)goto _LL3CB;else{_tmp8FF=_tmp8FE->f1;}}_LL3CA:
 _tmp901=_tmp8FF;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp900=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp900->tag != 4)goto _LL3CD;else{_tmp901=_tmp900->f1;}}_LL3CC:
 Cyc_Toc_exptypes_to_c(_tmp901);goto _LL3BC;_LL3CD: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp902=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp902->tag != 2)goto _LL3CF;else{_tmp903=_tmp902->f2;}}_LL3CE:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp903);goto _LL3BC;_LL3CF: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp904=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp904->tag != 6)goto _LL3D1;else{_tmp905=_tmp904->f1;_tmp906=_tmp904->f2;}}_LL3D0:
 _tmp908=_tmp905;_tmp909=_tmp906;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp907=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp907->tag != 7)goto _LL3D3;else{_tmp908=_tmp907->f1;_tmp909=_tmp907->f2;}}_LL3D2:
 _tmp90B=_tmp908;_tmp90C=_tmp909;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp90A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp90A->tag != 8)goto _LL3D5;else{_tmp90B=_tmp90A->f1;_tmp90C=_tmp90A->f2;}}_LL3D4:
 _tmp90E=_tmp90B;_tmp90F=_tmp90C;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp90D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp90D->tag != 22)goto _LL3D7;else{_tmp90E=_tmp90D->f1;_tmp90F=_tmp90D->f2;}}_LL3D6:
 _tmp911=_tmp90E;_tmp912=_tmp90F;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp910=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp910->tag != 34)goto _LL3D9;else{_tmp911=_tmp910->f1;_tmp912=_tmp910->f2;}}_LL3D8:
 _tmp914=_tmp911;_tmp915=_tmp912;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp913=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp913->tag != 3)goto _LL3DB;else{_tmp914=_tmp913->f1;_tmp915=_tmp913->f3;}}_LL3DA:
 Cyc_Toc_exptypes_to_c(_tmp914);Cyc_Toc_exptypes_to_c(_tmp915);goto _LL3BC;_LL3DB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp916=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp916->tag != 5)goto _LL3DD;else{_tmp917=_tmp916->f1;_tmp918=_tmp916->f2;_tmp919=_tmp916->f3;}}_LL3DC:
# 4920
 Cyc_Toc_exptypes_to_c(_tmp917);Cyc_Toc_exptypes_to_c(_tmp918);Cyc_Toc_exptypes_to_c(_tmp919);goto _LL3BC;_LL3DD: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp91A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp91A->tag != 9)goto _LL3DF;else{_tmp91B=_tmp91A->f1;_tmp91C=_tmp91A->f2;}}_LL3DE:
# 4922
 Cyc_Toc_exptypes_to_c(_tmp91B);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp91C);goto _LL3BC;_LL3DF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp91D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp91D->tag != 13)goto _LL3E1;else{_tmp91E=(void**)((void**)& _tmp91D->f1);_tmp91F=_tmp91D->f2;}}_LL3E0:
*_tmp91E=Cyc_Toc_typ_to_c(*_tmp91E);Cyc_Toc_exptypes_to_c(_tmp91F);goto _LL3BC;_LL3E1: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp920=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp920->tag != 24)goto _LL3E3;else{_tmp921=(void**)&(_tmp920->f1)->f3;_tmp922=_tmp920->f2;}}_LL3E2:
# 4925
*_tmp921=Cyc_Toc_typ_to_c(*_tmp921);
_tmp924=_tmp922;goto _LL3E4;_LL3E3: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp923=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp923->tag != 35)goto _LL3E5;else{_tmp924=_tmp923->f2;}}_LL3E4:
 _tmp926=_tmp924;goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp925=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp925->tag != 25)goto _LL3E7;else{_tmp926=_tmp925->f1;}}_LL3E6:
# 4929
 for(0;_tmp926 != 0;_tmp926=_tmp926->tl){
struct Cyc_Absyn_Exp*_tmp93F;struct _tuple15 _tmp93E=*((struct _tuple15*)_tmp926->hd);_tmp93F=_tmp93E.f2;
Cyc_Toc_exptypes_to_c(_tmp93F);}
# 4933
goto _LL3BC;_LL3E7: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp927=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp927->tag != 18)goto _LL3E9;else{_tmp928=(void**)((void**)& _tmp927->f1);}}_LL3E8:
 _tmp92A=_tmp928;goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp929=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp929->tag != 16)goto _LL3EB;else{_tmp92A=(void**)((void**)& _tmp929->f1);}}_LL3EA:
*_tmp92A=Cyc_Toc_typ_to_c(*_tmp92A);goto _LL3BC;_LL3EB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp92B=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp92B->tag != 36)goto _LL3ED;else{_tmp92C=_tmp92B->f1;}}_LL3EC:
 Cyc_Toc_stmttypes_to_c(_tmp92C);goto _LL3BC;_LL3ED: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp92D=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp92D->tag != 33)goto _LL3EF;else{_tmp92E=(struct Cyc_Absyn_MallocInfo*)& _tmp92D->f1;}}_LL3EE:
# 4938
 if(_tmp92E->elt_type != 0){
void**_tmpDA9;_tmp92E->elt_type=((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp92E->elt_type))),_tmpDA9))));}
Cyc_Toc_exptypes_to_c(_tmp92E->num_elts);
goto _LL3BC;_LL3EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp92F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp92F->tag != 0)goto _LL3F1;}_LL3F0:
 goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp930=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp930->tag != 1)goto _LL3F3;}_LL3F2:
 goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp931=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp931->tag != 31)goto _LL3F5;}_LL3F4:
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp932=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp932->tag != 39)goto _LL3F7;}_LL3F6:
 goto _LL3F8;_LL3F7: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp933=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp933->tag != 32)goto _LL3F9;}_LL3F8:
 goto _LL3BC;_LL3F9: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp934=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp934->tag != 29)goto _LL3FB;}_LL3FA:
# 4948
 goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp935=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp935->tag != 30)goto _LL3FD;}_LL3FC:
 goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp936=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp936->tag != 28)goto _LL3FF;}_LL3FE:
 goto _LL400;_LL3FF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp937=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp937->tag != 26)goto _LL401;}_LL400:
 goto _LL402;_LL401: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp938=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp938->tag != 27)goto _LL403;}_LL402:
 goto _LL404;_LL403: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp939=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp939->tag != 23)goto _LL405;}_LL404:
 goto _LL406;_LL405: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp93A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp93A->tag != 12)goto _LL407;}_LL406:
 goto _LL408;_LL407: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp93B=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp93B->tag != 15)goto _LL409;}_LL408:
 goto _LL40A;_LL409: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp93C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp93C->tag != 38)goto _LL40B;}_LL40A:
 goto _LL40C;_LL40B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp93D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp8F1;if(_tmp93D->tag != 37)goto _LL3BC;}_LL40C:
# 4958
{const char*_tmpDAC;void*_tmpDAB;(_tmpDAB=0,Cyc_Tcutil_terr(e->loc,((_tmpDAC="Cyclone expression within C code",_tag_dyneither(_tmpDAC,sizeof(char),33))),_tag_dyneither(_tmpDAB,sizeof(void*),0)));}
goto _LL3BC;_LL3BC:;}
# 4963
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp943=d->r;struct Cyc_Absyn_Vardecl*_tmp945;struct Cyc_Absyn_Fndecl*_tmp947;struct Cyc_Absyn_Aggrdecl*_tmp949;struct Cyc_Absyn_Enumdecl*_tmp94B;struct Cyc_Absyn_Typedefdecl*_tmp94D;_LL40E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp944=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp944->tag != 0)goto _LL410;else{_tmp945=_tmp944->f1;}}_LL40F:
# 4966
 _tmp945->type=Cyc_Toc_typ_to_c(_tmp945->type);
if(_tmp945->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp945->initializer));
goto _LL40D;_LL410: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp946=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp946->tag != 1)goto _LL412;else{_tmp947=_tmp946->f1;}}_LL411:
# 4970
 _tmp947->ret_type=Cyc_Toc_typ_to_c(_tmp947->ret_type);
{struct Cyc_List_List*_tmp959=_tmp947->args;for(0;_tmp959 != 0;_tmp959=_tmp959->tl){
(*((struct _tuple8*)_tmp959->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp959->hd)).f3);}}
# 4974
goto _LL40D;_LL412: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp948=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp948->tag != 6)goto _LL414;else{_tmp949=_tmp948->f1;}}_LL413:
 Cyc_Toc_aggrdecl_to_c(_tmp949,1);goto _LL40D;_LL414: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp94A=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp94A->tag != 8)goto _LL416;else{_tmp94B=_tmp94A->f1;}}_LL415:
# 4977
 if(_tmp94B->fields != 0){
struct Cyc_List_List*_tmp95A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp94B->fields))->v;for(0;_tmp95A != 0;_tmp95A=_tmp95A->tl){
struct Cyc_Absyn_Enumfield*_tmp95B=(struct Cyc_Absyn_Enumfield*)_tmp95A->hd;
if(_tmp95B->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp95B->tag));}}
# 4982
goto _LL40D;_LL416: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp94C=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp94C->tag != 9)goto _LL418;else{_tmp94D=_tmp94C->f1;}}_LL417:
 _tmp94D->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp94D->defn));goto _LL40D;_LL418: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp94E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp94E->tag != 2)goto _LL41A;}_LL419:
 goto _LL41B;_LL41A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp94F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp94F->tag != 3)goto _LL41C;}_LL41B:
 goto _LL41D;_LL41C: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp950=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp950->tag != 7)goto _LL41E;}_LL41D:
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp951=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp951->tag != 10)goto _LL420;}_LL41F:
 goto _LL421;_LL420: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp952=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp952->tag != 11)goto _LL422;}_LL421:
 goto _LL423;_LL422: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp953=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp953->tag != 12)goto _LL424;}_LL423:
 goto _LL425;_LL424: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp954=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp954->tag != 13)goto _LL426;}_LL425:
 goto _LL427;_LL426: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp955=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp955->tag != 4)goto _LL428;}_LL427:
 goto _LL429;_LL428: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp956=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp956->tag != 5)goto _LL42A;}_LL429:
# 4993
{const char*_tmpDAF;void*_tmpDAE;(_tmpDAE=0,Cyc_Tcutil_terr(d->loc,((_tmpDAF="Cyclone declaration within C code",_tag_dyneither(_tmpDAF,sizeof(char),34))),_tag_dyneither(_tmpDAE,sizeof(void*),0)));}
goto _LL40D;_LL42A: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp957=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp957->tag != 14)goto _LL42C;}_LL42B:
 goto _LL42D;_LL42C: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp958=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp943;if(_tmp958->tag != 15)goto _LL40D;}_LL42D:
# 4997
 goto _LL40D;_LL40D:;}
# 5001
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp95E=s->r;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Stmt*_tmp962;struct Cyc_Absyn_Stmt*_tmp963;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Stmt*_tmp968;struct Cyc_Absyn_Stmt*_tmp969;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Stmt*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Stmt*_tmp971;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_List_List*_tmp974;struct Cyc_Absyn_Decl*_tmp976;struct Cyc_Absyn_Stmt*_tmp977;struct Cyc_Absyn_Stmt*_tmp979;struct Cyc_Absyn_Exp*_tmp97A;_LL42F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp95F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp95F->tag != 1)goto _LL431;else{_tmp960=_tmp95F->f1;}}_LL430:
 Cyc_Toc_exptypes_to_c(_tmp960);goto _LL42E;_LL431: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp961=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp961->tag != 2)goto _LL433;else{_tmp962=_tmp961->f1;_tmp963=_tmp961->f2;}}_LL432:
 Cyc_Toc_stmttypes_to_c(_tmp962);Cyc_Toc_stmttypes_to_c(_tmp963);goto _LL42E;_LL433: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp964=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp964->tag != 3)goto _LL435;else{_tmp965=_tmp964->f1;}}_LL434:
 if(_tmp965 != 0)Cyc_Toc_exptypes_to_c(_tmp965);goto _LL42E;_LL435: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp966=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp966->tag != 4)goto _LL437;else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f2;_tmp969=_tmp966->f3;}}_LL436:
# 5007
 Cyc_Toc_exptypes_to_c(_tmp967);Cyc_Toc_stmttypes_to_c(_tmp968);Cyc_Toc_stmttypes_to_c(_tmp969);goto _LL42E;_LL437: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp96A=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp96A->tag != 5)goto _LL439;else{_tmp96B=(_tmp96A->f1).f1;_tmp96C=_tmp96A->f2;}}_LL438:
# 5009
 Cyc_Toc_exptypes_to_c(_tmp96B);Cyc_Toc_stmttypes_to_c(_tmp96C);goto _LL42E;_LL439: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp96D=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp96D->tag != 9)goto _LL43B;else{_tmp96E=_tmp96D->f1;_tmp96F=(_tmp96D->f2).f1;_tmp970=(_tmp96D->f3).f1;_tmp971=_tmp96D->f4;}}_LL43A:
# 5011
 Cyc_Toc_exptypes_to_c(_tmp96E);Cyc_Toc_exptypes_to_c(_tmp96F);Cyc_Toc_exptypes_to_c(_tmp970);
Cyc_Toc_stmttypes_to_c(_tmp971);goto _LL42E;_LL43B: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp972=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp972->tag != 10)goto _LL43D;else{_tmp973=_tmp972->f1;_tmp974=_tmp972->f2;}}_LL43C:
# 5014
 Cyc_Toc_exptypes_to_c(_tmp973);
for(0;_tmp974 != 0;_tmp974=_tmp974->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp974->hd)->body);}
goto _LL42E;_LL43D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp975=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp975->tag != 12)goto _LL43F;else{_tmp976=_tmp975->f1;_tmp977=_tmp975->f2;}}_LL43E:
 Cyc_Toc_decltypes_to_c(_tmp976);Cyc_Toc_stmttypes_to_c(_tmp977);goto _LL42E;_LL43F: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp978=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp978->tag != 14)goto _LL441;else{_tmp979=_tmp978->f1;_tmp97A=(_tmp978->f2).f1;}}_LL440:
 Cyc_Toc_stmttypes_to_c(_tmp979);Cyc_Toc_exptypes_to_c(_tmp97A);goto _LL42E;_LL441: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp97B=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp97B->tag != 0)goto _LL443;}_LL442:
 goto _LL444;_LL443: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp97C=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp97C->tag != 6)goto _LL445;}_LL444:
 goto _LL446;_LL445: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp97D=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp97D->tag != 7)goto _LL447;}_LL446:
 goto _LL448;_LL447: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp97E=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp97E->tag != 8)goto _LL449;}_LL448:
 goto _LL42E;_LL449: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp97F=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp97F->tag != 11)goto _LL44B;}_LL44A:
 goto _LL44C;_LL44B: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp980=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp980->tag != 13)goto _LL44D;}_LL44C:
 goto _LL44E;_LL44D: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp981=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp981->tag != 15)goto _LL44F;}_LL44E:
 goto _LL450;_LL44F: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp982=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp95E;if(_tmp982->tag != 16)goto _LL42E;}_LL450:
{const char*_tmpDB2;void*_tmpDB1;(_tmpDB1=0,Cyc_Tcutil_terr(s->loc,((_tmpDB2="Cyclone statement in C code",_tag_dyneither(_tmpDB2,sizeof(char),28))),_tag_dyneither(_tmpDB1,sizeof(void*),0)));}
goto _LL42E;_LL42E:;}
# 5035
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpDB5;void*_tmpDB4;(_tmpDB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDB5="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDB5,sizeof(char),29))),_tag_dyneither(_tmpDB4,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp987=d->r;struct Cyc_Absyn_Vardecl*_tmp989;struct Cyc_Absyn_Fndecl*_tmp98B;struct Cyc_Absyn_Aggrdecl*_tmp991;struct Cyc_Absyn_Datatypedecl*_tmp993;struct Cyc_Absyn_Enumdecl*_tmp995;struct Cyc_Absyn_Typedefdecl*_tmp997;struct Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99F;struct Cyc_List_List*_tmp9A1;_LL452: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp988=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp988->tag != 0)goto _LL454;else{_tmp989=_tmp988->f1;}}_LL453: {
# 5042
struct _tuple0*_tmp9A2=_tmp989->name;
# 5044
if(_tmp989->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpDB6;_tmp9A2=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->f1=Cyc_Absyn_Rel_n(0),((_tmpDB6->f2=(*_tmp9A2).f2,_tmpDB6))))));}
if(_tmp989->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp989->initializer));else{
# 5050
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp989->initializer));}}
# 5052
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpDB9;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDB8;nv=Cyc_Toc_add_varmap(r,nv,_tmp989->name,Cyc_Absyn_varb_exp(_tmp9A2,(void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDB9.tag=1,((_tmpDB9.f1=_tmp989,_tmpDB9)))),_tmpDB8)))),0));}
_tmp989->name=_tmp9A2;
_tmp989->sc=Cyc_Toc_scope_to_c(_tmp989->sc);
_tmp989->type=Cyc_Toc_typ_to_c(_tmp989->type);
{struct Cyc_List_List*_tmpDBA;Cyc_Toc_result_decls=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=d,((_tmpDBA->tl=Cyc_Toc_result_decls,_tmpDBA))))));}
goto _LL451;}_LL454: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp98A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp98A->tag != 1)goto _LL456;else{_tmp98B=_tmp98A->f1;}}_LL455: {
# 5059
struct _tuple0*_tmp9A7=_tmp98B->name;
# 5061
if(_tmp98B->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpDBB;_tmp9A7=((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->f1=Cyc_Absyn_Abs_n(0,1),((_tmpDBB->f2=(*_tmp9A7).f2,_tmpDBB))))));}
_tmp98B->sc=Cyc_Absyn_Public;}
# 5065
nv=Cyc_Toc_add_varmap(r,nv,_tmp98B->name,Cyc_Absyn_var_exp(_tmp9A7,0));
_tmp98B->name=_tmp9A7;
Cyc_Toc_fndecl_to_c(nv,_tmp98B,cinclude);
{struct Cyc_List_List*_tmpDBC;Cyc_Toc_result_decls=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->hd=d,((_tmpDBC->tl=Cyc_Toc_result_decls,_tmpDBC))))));}
goto _LL451;}_LL456: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp98C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp98C->tag != 2)goto _LL458;}_LL457:
 goto _LL459;_LL458: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp98D=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp98D->tag != 3)goto _LL45A;}_LL459: {
# 5072
const char*_tmpDBF;void*_tmpDBE;(_tmpDBE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDBF="letdecl at toplevel",_tag_dyneither(_tmpDBF,sizeof(char),20))),_tag_dyneither(_tmpDBE,sizeof(void*),0)));}_LL45A: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp98E=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp98E->tag != 4)goto _LL45C;}_LL45B: {
# 5074
const char*_tmpDC2;void*_tmpDC1;(_tmpDC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDC2="region decl at toplevel",_tag_dyneither(_tmpDC2,sizeof(char),24))),_tag_dyneither(_tmpDC1,sizeof(void*),0)));}_LL45C: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp98F=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp98F->tag != 5)goto _LL45E;}_LL45D: {
# 5076
const char*_tmpDC5;void*_tmpDC4;(_tmpDC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDC5="alias decl at toplevel",_tag_dyneither(_tmpDC5,sizeof(char),23))),_tag_dyneither(_tmpDC4,sizeof(void*),0)));}_LL45E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp990=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp990->tag != 6)goto _LL460;else{_tmp991=_tmp990->f1;}}_LL45F:
# 5078
 Cyc_Toc_aggrdecl_to_c(_tmp991,1);
goto _LL451;_LL460: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp992=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp992->tag != 7)goto _LL462;else{_tmp993=_tmp992->f1;}}_LL461:
# 5081
 if(_tmp993->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp993);else{
# 5084
Cyc_Toc_datatypedecl_to_c(_tmp993);}
goto _LL451;_LL462: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp994=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp994->tag != 8)goto _LL464;else{_tmp995=_tmp994->f1;}}_LL463:
# 5087
 Cyc_Toc_enumdecl_to_c(_tmp995);
{struct Cyc_List_List*_tmpDC6;Cyc_Toc_result_decls=((_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->hd=d,((_tmpDC6->tl=Cyc_Toc_result_decls,_tmpDC6))))));}
goto _LL451;_LL464: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp996=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp996->tag != 9)goto _LL466;else{_tmp997=_tmp996->f1;}}_LL465:
# 5091
 _tmp997->name=_tmp997->name;
_tmp997->tvs=0;
if(_tmp997->defn != 0)
_tmp997->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp997->defn));else{
# 5096
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp997->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL472:
 _tmp997->defn=Cyc_Absyn_void_star_typ();break;default: _LL473:
 _tmp997->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5101
{struct Cyc_List_List*_tmpDC7;Cyc_Toc_result_decls=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->hd=d,((_tmpDC7->tl=Cyc_Toc_result_decls,_tmpDC7))))));}
goto _LL451;_LL466: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp998=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp998->tag != 14)goto _LL468;}_LL467:
 goto _LL469;_LL468: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp999=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp999->tag != 15)goto _LL46A;}_LL469:
# 5105
 goto _LL451;_LL46A: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp99A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp99A->tag != 10)goto _LL46C;else{_tmp99B=_tmp99A->f2;}}_LL46B:
 _tmp99D=_tmp99B;goto _LL46D;_LL46C: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp99C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp99C->tag != 11)goto _LL46E;else{_tmp99D=_tmp99C->f2;}}_LL46D:
 _tmp99F=_tmp99D;goto _LL46F;_LL46E: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp99E=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp99E->tag != 12)goto _LL470;else{_tmp99F=_tmp99E->f1;}}_LL46F:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp99F,top,cinclude);goto _LL451;_LL470: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9A0=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp987;if(_tmp9A0->tag != 13)goto _LL451;else{_tmp9A1=_tmp9A0->f1;}}_LL471:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9A1,top,1);goto _LL451;_LL451:;};}
# 5112
return nv;}
# 5116
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp9B3;struct Cyc_Core_NewRegion _tmp9B2=Cyc_Core__new_dynregion("internal-error",0);_tmp9B3=_tmp9B2.dynregion;{
struct _DynRegionFrame _tmp9B4;struct _RegionHandle*d=_open_dynregion(& _tmp9B4,_tmp9B3);{
struct Cyc_Dict_Dict*_tmpDD4;struct Cyc_Dict_Dict*_tmpDD3;struct Cyc_Set_Set**_tmpDD2;struct Cyc_List_List**_tmpDD1;struct Cyc_Dict_Dict*_tmpDD0;struct Cyc_List_List**_tmpDCF;struct Cyc_Toc_TocState*_tmpDCE;Cyc_Toc_toc_state=(
(_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->dyn=(struct _DynRegionHandle*)_tmp9B3,((_tmpDCE->tuple_types=(struct Cyc_List_List**)(
(_tmpDCF=_region_malloc(d,sizeof(*_tmpDCF)),((_tmpDCF[0]=0,_tmpDCF)))),((_tmpDCE->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDD0=_region_malloc(d,sizeof(*_tmpDD0)),((_tmpDD0[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDD0)))),((_tmpDCE->abs_struct_types=(struct Cyc_List_List**)(
(_tmpDD1=_region_malloc(d,sizeof(*_tmpDD1)),((_tmpDD1[0]=0,_tmpDD1)))),((_tmpDCE->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpDD2=_region_malloc(d,sizeof(*_tmpDD2)),((_tmpDD2[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDD2)))),((_tmpDCE->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDD3=_region_malloc(d,sizeof(*_tmpDD3)),((_tmpDD3[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDD3)))),((_tmpDCE->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpDD4=_region_malloc(d,sizeof(*_tmpDD4)),((_tmpDD4[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple13*,struct _tuple13*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpDD4)))),((_tmpDCE->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDCE))))))))))))))))));}
# 5130
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDD5;Cyc_Toc_globals=_tag_dyneither(((_tmpDD5=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDD5[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpDD5[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpDD5[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpDD5[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpDD5[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpDD5[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpDD5[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpDD5[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpDD5[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDD5[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpDD5[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpDD5[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpDD5[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpDD5[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDD5[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpDD5[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpDD5[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDD5[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpDD5[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDD5[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDD5[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDD5[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDD5[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpDD5[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpDD5[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpDD5[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpDD5[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpDD5[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpDD5[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpDD5[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpDD5[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpDD5[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpDD5[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpDD5[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpDD5[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpDD5[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpDD5[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpDD5[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDD5[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpDD5[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpDD5[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpDD5)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5118
;_pop_dynregion(d);};}
# 5183
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9BD=_new_region("start");struct _RegionHandle*start=& _tmp9BD;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5192
struct _DynRegionHandle*_tmp9BF;struct Cyc_Xarray_Xarray*_tmp9C0;struct Cyc_Toc_TocState _tmp9BE=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp9BF=_tmp9BE.dyn;_tmp9C0=_tmp9BE.temp_labels;
{struct _DynRegionFrame _tmp9C1;struct _RegionHandle*d=_open_dynregion(& _tmp9C1,_tmp9BF);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9C0);;_pop_dynregion(d);}
# 5196
Cyc_Core_free_dynregion(_tmp9BF);};{
# 5198
struct Cyc_List_List*_tmp9C2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9C2;};
# 5185
;_pop_region(start);};}
