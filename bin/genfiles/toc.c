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
# 344
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
# 76 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 85
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};
# 106
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{struct _tuple0*f1;struct _tuple0*f2;};
# 108
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple0*_tmp1;struct _tuple0*_tmp2;struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
struct _tuple0*_tmp4;struct _tuple0*_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 116
void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
if(v == 0){
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 123
void*_tmpC;struct _tuple15*_tmpB=*v;_tmpC=_tmpB->f2;{
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
{const char*_tmp9DE;void*_tmp9DD;(_tmp9DD=0,Cyc_fprintf(Cyc_stderr,((_tmp9DE="\n",_tag_dyneither(_tmp9DE,sizeof(char),2))),_tag_dyneither(_tmp9DD,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9E1;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9E0;(int)_throw((void*)((_tmp9E0=_cycalloc_atomic(sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=Cyc_Toc_Toc_Unimplemented,_tmp9E1)),_tmp9E0)))));};}
# 144
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 147
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9E4;void*_tmp9E3;(_tmp9E3=0,Cyc_fprintf(Cyc_stderr,((_tmp9E4="\n",_tag_dyneither(_tmp9E4,sizeof(char),2))),_tag_dyneither(_tmp9E3,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9E7;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9E6;(int)_throw((void*)((_tmp9E6=_cycalloc_atomic(sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=Cyc_Toc_Toc_Impossible,_tmp9E7)),_tmp9E6)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmp9E8;skip_stmt_opt=((_tmp9E8=_cycalloc(sizeof(*_tmp9E8)),((_tmp9E8[0]=Cyc_Absyn_skip_stmt(0),_tmp9E8))));}
return*skip_stmt_opt;}
# 273
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 276
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9EB;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9EA;return(void*)((_tmp9EA=_cycalloc(sizeof(*_tmp9EA)),((_tmp9EA[0]=((_tmp9EB.tag=13,((_tmp9EB.f1=t,((_tmp9EB.f2=e,((_tmp9EB.f3=0,((_tmp9EB.f4=Cyc_Absyn_No_coercion,_tmp9EB)))))))))),_tmp9EA))));}
# 279
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9EE;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9ED;return(void*)((_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EE.tag=19,((_tmp9EE.f1=e,_tmp9EE)))),_tmp9ED))));}
# 282
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9F1;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9F0;return(void*)((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0[0]=((_tmp9F1.tag=22,((_tmp9F1.f1=e1,((_tmp9F1.f2=e2,_tmp9F1)))))),_tmp9F0))));}
# 285
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9F4;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9F3;return(void*)((_tmp9F3=_cycalloc(sizeof(*_tmp9F3)),((_tmp9F3[0]=((_tmp9F4.tag=35,((_tmp9F4.f1=s,_tmp9F4)))),_tmp9F3))));}
# 288
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9F7;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9F6;return(void*)((_tmp9F6=_cycalloc(sizeof(*_tmp9F6)),((_tmp9F6[0]=((_tmp9F7.tag=16,((_tmp9F7.f1=t,_tmp9F7)))),_tmp9F6))));}
# 291
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9FA;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9F9;return(void*)((_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9[0]=((_tmp9FA.tag=9,((_tmp9FA.f1=e,((_tmp9FA.f2=es,((_tmp9FA.f3=0,((_tmp9FA.f4=1,_tmp9FA)))))))))),_tmp9F9))));}
# 294
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9FD;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9FC;return(void*)((_tmp9FC=_cycalloc(sizeof(*_tmp9FC)),((_tmp9FC[0]=((_tmp9FD.tag=1,((_tmp9FD.f1=e,_tmp9FD)))),_tmp9FC))));}
# 297
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA00;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9FF;return(void*)((_tmp9FF=_cycalloc(sizeof(*_tmp9FF)),((_tmp9FF[0]=((_tmpA00.tag=2,((_tmpA00.f1=s1,((_tmpA00.f2=s2,_tmpA00)))))),_tmp9FF))));}
# 300
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA03;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA02;return(void*)((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02[0]=((_tmpA03.tag=5,((_tmpA03.f1=e1,((_tmpA03.f2=e2,((_tmpA03.f3=e3,_tmpA03)))))))),_tmpA02))));}
# 303
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA06;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA05;return(void*)((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05[0]=((_tmpA06.tag=20,((_tmpA06.f1=e,((_tmpA06.f2=n,((_tmpA06.f3=0,((_tmpA06.f4=0,_tmpA06)))))))))),_tmpA05))));}
# 306
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA09;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA08;return(void*)((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA09.tag=21,((_tmpA09.f1=e,((_tmpA09.f2=n,((_tmpA09.f3=0,((_tmpA09.f4=0,_tmpA09)))))))))),_tmpA08))));}
# 309
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 312
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA0C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA0B;return(void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=34,((_tmpA0C.f1=tdopt,((_tmpA0C.f2=ds,_tmpA0C)))))),_tmpA0B))));}
# 314
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA0F;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA0E;return(void*)((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E[0]=((_tmpA0F.tag=8,((_tmpA0F.f1=v,((_tmpA0F.f2=s,_tmpA0F)))))),_tmpA0E))));}
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
struct Cyc_Core_Impossible_exn_struct _tmpA15;const char*_tmpA14;struct Cyc_Core_Impossible_exn_struct*_tmpA13;(int)_throw((void*)((_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA15.tag=Cyc_Core_Impossible,((_tmpA15.f1=((_tmpA14="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA14,sizeof(char),44))),_tmpA15)))),_tmpA13)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA22;const char*_tmpA21;void*_tmpA20[1];struct Cyc_String_pa_PrintArg_struct _tmpA1F;struct Cyc_Core_Impossible_exn_struct*_tmpA1E;(int)_throw((void*)((_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA22.tag=Cyc_Core_Impossible,((_tmpA22.f1=(struct _dyneither_ptr)((_tmpA1F.tag=0,((_tmpA1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA20[0]=& _tmpA1F,Cyc_aprintf(((_tmpA21="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA21,sizeof(char),67))),_tag_dyneither(_tmpA20,sizeof(void*),1)))))))),_tmpA22)))),_tmpA1E)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA28;const char*_tmpA27;struct Cyc_Core_Impossible_exn_struct*_tmpA26;(int)_throw((void*)((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26[0]=((_tmpA28.tag=Cyc_Core_Impossible,((_tmpA28.f1=((_tmpA27="impossible type (not pointer)",_tag_dyneither(_tmpA27,sizeof(char),30))),_tmpA28)))),_tmpA26)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
 _tmpF0=_tmpEE;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpEF->tag != 27)goto _LL31;else{_tmpF0=_tmpEF->f3;}}_LL30:
 _tmpF2=_tmpF0;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF1->tag != 24)goto _LL33;else{_tmpF2=_tmpF1->f2;}}_LL32:
 _tmpF4=_tmpF2;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDD;if(_tmpF3->tag != 34)goto _LL35;else{_tmpF4=_tmpF3->f2;}}_LL34:
# 443
 for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpF4->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 451
static int Cyc_Toc_is_nullable(void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpF8;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 5)goto _LL3A;else{_tmpF8=((_tmpF7->f1).ptr_atts).nullable;}}_LL39:
# 454
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8);_LL3A:;_LL3B: {
const char*_tmpA2B;void*_tmpA2A;(_tmpA2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA2B="is_nullable",_tag_dyneither(_tmpA2B,sizeof(char),12))),_tag_dyneither(_tmpA2A,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA2D;unsigned int _tmpA2C;struct _dyneither_ptr buf=(_tmpA2C=len,((_tmpA2D=_cyccalloc_atomic(sizeof(char),_tmpA2C),_tag_dyneither(_tmpA2D,sizeof(char),_tmpA2C))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmpA30;char _tmpA2F;struct _dyneither_ptr _tmpA2E;(_tmpA2E=p,((_tmpA2F=*((char*)_check_dyneither_subscript(_tmpA2E,sizeof(char),0)),((_tmpA30='_',((_get_dyneither_size(_tmpA2E,sizeof(char))== 1  && (_tmpA2F == '\000'  && _tmpA30 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA2E.curr)=_tmpA30)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 484
{char _tmpA33;char _tmpA32;struct _dyneither_ptr _tmpA31;(_tmpA31=p,((_tmpA32=*((char*)_check_dyneither_subscript(_tmpA31,sizeof(char),0)),((_tmpA33='_',((_get_dyneither_size(_tmpA31,sizeof(char))== 1  && (_tmpA32 == '\000'  && _tmpA33 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA31.curr)=_tmpA33)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA34;Cyc_strcpy(p,((_tmpA34="_struct",_tag_dyneither(_tmpA34,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 496
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmpA35;pair=((_tmpA35.f1=fieldname,((_tmpA35.f2=dtname,_tmpA35))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmpA36;struct _tuple14*_tmp115=(_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->f1=fieldname,((_tmpA36->f2=dtname,_tmpA36)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA39;struct _tuple0*_tmpA38;struct _tuple0*res=(_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38->f1=_tmp117,((_tmpA38->f2=((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=newvar,_tmpA39)))),_tmpA38)))));
*_tmp110=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp110,_tmp115,res);{
struct _tuple0*_tmp119=res;_npop_handler(0);return _tmp119;};};};}
# 503
;_pop_dynregion(d);};};}
# 514
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 518
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
struct _DynRegionHandle*_tmp11F;struct Cyc_List_List**_tmp120;struct Cyc_Toc_TocState _tmp11E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp11F=_tmp11E.dyn;_tmp120=_tmp11E.tuple_types;{
struct _DynRegionFrame _tmp121;struct _RegionHandle*d=_open_dynregion(& _tmp121,_tmp11F);
# 522
{struct Cyc_List_List*_tmp122=*_tmp120;for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
void*_tmp124;struct Cyc_List_List*_tmp125;struct _tuple17*_tmp123=(struct _tuple17*)_tmp122->hd;_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;{
struct Cyc_List_List*_tmp126=tqs0;
for(0;_tmp126 != 0  && _tmp125 != 0;(_tmp126=_tmp126->tl,_tmp125=_tmp125->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp126->hd)).f2,(void*)_tmp125->hd))
break;}
if(_tmp126 == 0  && _tmp125 == 0){
void*_tmp127=_tmp124;_npop_handler(0);return _tmp127;}};}}{
# 532
struct Cyc_Int_pa_PrintArg_struct _tmpA41;void*_tmpA40[1];const char*_tmpA3F;struct _dyneither_ptr*_tmpA3E;struct _dyneither_ptr*xname=(_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=(struct _dyneither_ptr)((_tmpA41.tag=1,((_tmpA41.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA40[0]=& _tmpA41,Cyc_aprintf(((_tmpA3F="_tuple%d",_tag_dyneither(_tmpA3F,sizeof(char),9))),_tag_dyneither(_tmpA40,sizeof(void*),1)))))))),_tmpA3E)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA44;struct Cyc_List_List*_tmpA43;_tmp128=((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43->hd=((_tmpA44=_cycalloc(sizeof(*_tmpA44)),((_tmpA44->name=Cyc_Absyn_fieldname(i),((_tmpA44->tq=Cyc_Toc_mt_tq,((_tmpA44->type=(void*)ts2->hd,((_tmpA44->width=0,((_tmpA44->attributes=0,_tmpA44)))))))))))),((_tmpA43->tl=_tmp128,_tmpA43))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA49;struct _tuple0*_tmpA48;struct Cyc_Absyn_Aggrdecl*_tmpA47;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA47=_cycalloc(sizeof(*_tmpA47)),((_tmpA47->kind=Cyc_Absyn_StructA,((_tmpA47->sc=Cyc_Absyn_Public,((_tmpA47->name=(
(_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48->f1=Cyc_Absyn_Rel_n(0),((_tmpA48->f2=xname,_tmpA48)))))),((_tmpA47->tvs=0,((_tmpA47->impl=(
(_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->exist_vars=0,((_tmpA49->rgn_po=0,((_tmpA49->fields=_tmp128,((_tmpA49->tagged=0,_tmpA49)))))))))),((_tmpA47->attributes=0,_tmpA47)))))))))))));
# 547
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA4F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA4E;struct Cyc_List_List*_tmpA4D;Cyc_Toc_result_decls=((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D->hd=Cyc_Absyn_new_decl((void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA4E.tag=6,((_tmpA4E.f1=_tmp12B,_tmpA4E)))),_tmpA4F)))),0),((_tmpA4D->tl=Cyc_Toc_result_decls,_tmpA4D))))));}
{struct _tuple17*_tmpA52;struct Cyc_List_List*_tmpA51;*_tmp120=((_tmpA51=_region_malloc(d,sizeof(*_tmpA51)),((_tmpA51->hd=((_tmpA52=_region_malloc(d,sizeof(*_tmpA52)),((_tmpA52->f1=x,((_tmpA52->f2=ts,_tmpA52)))))),((_tmpA51->tl=*_tmp120,_tmpA51))))));}{
void*_tmp131=x;_npop_handler(0);return _tmp131;};};};
# 522
;_pop_dynregion(d);};}struct _tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 558
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 564
struct _DynRegionHandle*_tmp13A;struct Cyc_List_List**_tmp13B;struct Cyc_Toc_TocState _tmp139=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp13A=_tmp139.dyn;_tmp13B=_tmp139.abs_struct_types;{
# 567
struct _DynRegionFrame _tmp13C;struct _RegionHandle*d=_open_dynregion(& _tmp13C,_tmp13A);
# 569
{struct Cyc_List_List*_tmp13D=*_tmp13B;for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){
struct _tuple0*_tmp13F;struct Cyc_List_List*_tmp140;void*_tmp141;struct _tuple18*_tmp13E=(struct _tuple18*)_tmp13D->hd;_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;_tmp141=_tmp13E->f3;
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
struct Cyc_Int_pa_PrintArg_struct _tmpA5A;void*_tmpA59[1];const char*_tmpA58;struct _dyneither_ptr*_tmpA57;struct _dyneither_ptr*xname=(_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=(struct _dyneither_ptr)((_tmpA5A.tag=1,((_tmpA5A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA59[0]=& _tmpA5A,Cyc_aprintf(((_tmpA58="_tuple%d",_tag_dyneither(_tmpA58,sizeof(char),9))),_tag_dyneither(_tmpA59,sizeof(void*),1)))))))),_tmpA57)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 611
{struct _tuple18*_tmpA5D;struct Cyc_List_List*_tmpA5C;*_tmp13B=((_tmpA5C=_region_malloc(d,sizeof(*_tmpA5C)),((_tmpA5C->hd=((_tmpA5D=_region_malloc(d,sizeof(*_tmpA5D)),((_tmpA5D->f1=struct_name,((_tmpA5D->f2=type_args,((_tmpA5D->f3=x,_tmpA5D)))))))),((_tmpA5C->tl=*_tmp13B,_tmpA5C))))));}{
# 614
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 621
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA60;struct Cyc_List_List*_tmpA5F;_tmp147=((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->hd=((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->name=_tmp14C->name,((_tmpA60->tq=Cyc_Toc_mt_tq,((_tmpA60->type=t,((_tmpA60->width=_tmp14C->width,((_tmpA60->attributes=_tmp14C->attributes,_tmpA60)))))))))))),((_tmpA5F->tl=_tmp147,_tmpA5F))))));};}
# 625
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA65;struct _tuple0*_tmpA64;struct Cyc_Absyn_Aggrdecl*_tmpA63;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63->kind=Cyc_Absyn_StructA,((_tmpA63->sc=Cyc_Absyn_Public,((_tmpA63->name=(
(_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->f1=Cyc_Absyn_Rel_n(0),((_tmpA64->f2=xname,_tmpA64)))))),((_tmpA63->tvs=0,((_tmpA63->impl=(
(_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->exist_vars=0,((_tmpA65->rgn_po=0,((_tmpA65->fields=_tmp147,((_tmpA65->tagged=0,_tmpA65)))))))))),((_tmpA63->attributes=0,_tmpA63)))))))))))));
# 633
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA6B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA6A;struct Cyc_List_List*_tmpA69;Cyc_Toc_result_decls=((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->hd=Cyc_Absyn_new_decl((void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6A.tag=6,((_tmpA6A.f1=_tmp14F,_tmpA6A)))),_tmpA6B)))),0),((_tmpA69->tl=Cyc_Toc_result_decls,_tmpA69))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 615
;_pop_region(r);};};
# 569
;_pop_dynregion(d);};}
# 642
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA78;const char*_tmpA77;void*_tmpA76[1];struct Cyc_Int_pa_PrintArg_struct _tmpA75;struct _tuple0*_tmpA74;struct _tuple0*res=(_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->f1=Cyc_Absyn_Loc_n,((_tmpA74->f2=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=(struct _dyneither_ptr)((_tmpA75.tag=1,((_tmpA75.f1=(unsigned int)_tmp15B,((_tmpA76[0]=& _tmpA75,Cyc_aprintf(((_tmpA77="_tmp%X",_tag_dyneither(_tmpA77,sizeof(char),7))),_tag_dyneither(_tmpA76,sizeof(void*),1)))))))),_tmpA78)))),_tmpA74)))));
return res;}
# 650
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA80;void*_tmpA7F[1];const char*_tmpA7E;struct _dyneither_ptr*_tmpA7D;struct _dyneither_ptr*res=(_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=(struct _dyneither_ptr)((_tmpA80.tag=1,((_tmpA80.f1=(unsigned int)_tmp165,((_tmpA7F[0]=& _tmpA80,Cyc_aprintf(((_tmpA7E="_LL%X",_tag_dyneither(_tmpA7E,sizeof(char),6))),_tag_dyneither(_tmpA7F,sizeof(void*),1)))))))),_tmpA7D)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA83;void*_tmpA82;(_tmpA82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA83="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA83,sizeof(char),43))),_tag_dyneither(_tmpA82,sizeof(void*),0)));}{
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
struct _tuple8*_tmpA84;return(_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->f1=_tmp170,((_tmpA84->f2=_tmp171,((_tmpA84->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA84)))))));};}
# 687
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA85;return(_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85->f1=_tmp175,((_tmpA85->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA85)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpA86;return(_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->name=f->name,((_tmpA86->tq=Cyc_Toc_mt_tq,((_tmpA86->type=
# 719
Cyc_Toc_typ_to_c(f->type),((_tmpA86->width=f->width,((_tmpA86->attributes=f->attributes,_tmpA86)))))))))));}
# 723
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 725
return;}
# 728
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA87;cs=((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA87))));}
# 733
return*cs;}
# 735
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA88;r=((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA88))));}
# 740
return*r;}
# 742
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA89;r=((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA89))));}
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
const char*_tmpA8C;void*_tmpA8B;(_tmpA8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA8C="unresolved DatatypeFieldType",_tag_dyneither(_tmpA8C,sizeof(char),29))),_tag_dyneither(_tmpA8B,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
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
{struct Cyc_List_List*_tmpA8D;_tmp1D1=((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->hd=(void*)_tmp1A5->hd,((_tmpA8D->tl=_tmp1D1,_tmpA8D))))));}goto _LL9F;_LL9F:;}{
# 812
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 815
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA8E;struct _tuple8*_tmp1DC=(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->f1=_tmp1A4->name,((_tmpA8E->f2=_tmp1A4->tq,((_tmpA8E->f3=_tmp1DB,_tmpA8E)))))));
struct Cyc_List_List*_tmpA8F;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=_tmp1DC,((_tmpA8F->tl=0,_tmpA8F)))))));}{
# 819
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA95;struct Cyc_Absyn_FnInfo _tmpA94;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA93;return(void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA95.tag=9,((_tmpA95.f1=((_tmpA94.tvars=0,((_tmpA94.effect=0,((_tmpA94.ret_tqual=_tmp1A0,((_tmpA94.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA94.args=_tmp1DA,((_tmpA94.c_varargs=_tmp1A3,((_tmpA94.cyc_varargs=0,((_tmpA94.rgn_po=0,((_tmpA94.attributes=_tmp1D1,_tmpA94)))))))))))))))))),_tmpA95)))),_tmpA93))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 824
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 829
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA98;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA97;return(void*)((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA98.tag=12,((_tmpA98.f1=_tmp1A9,((_tmpA98.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA98)))))),_tmpA97))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA9B;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA9A;return(void*)((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A[0]=((_tmpA9B.tag=17,((_tmpA9B.f1=_tmp1B3,((_tmpA9B.f2=0,((_tmpA9B.f3=_tmp1B5,((_tmpA9B.f4=0,_tmpA9B)))))))))),_tmpA9A))));}else{
return t;}}else{
# 866
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA9E;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA9D;return(void*)((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=((_tmpA9E.tag=17,((_tmpA9E.f1=_tmp1B3,((_tmpA9E.f2=0,((_tmpA9E.f3=_tmp1B5,((_tmpA9E.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA9E)))))))))),_tmpA9D))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
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
const char*_tmpAA2;void*_tmpAA1[1];struct Cyc_String_pa_PrintArg_struct _tmpAA0;(_tmpAA0.tag=0,((_tmpAA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAA1[0]=& _tmpAA0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA2="atomic_typ:  bad type %s",_tag_dyneither(_tmpAA2,sizeof(char),25))),_tag_dyneither(_tmpAA1,sizeof(void*),1)))))));}_LLB8:;}
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
const char*_tmpAA5;void*_tmpAA4;(_tmpAA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA5="is_poly_field: type missing fields",_tag_dyneither(_tmpAA5,sizeof(char),35))),_tag_dyneither(_tmpAA4,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 982
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpAA9;void*_tmpAA8[1];struct Cyc_String_pa_PrintArg_struct _tmpAA7;(_tmpAA7.tag=0,((_tmpAA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAA8[0]=& _tmpAA7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA9="is_poly_field: bad field %s",_tag_dyneither(_tmpAA9,sizeof(char),28))),_tag_dyneither(_tmpAA8,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpAAD;void*_tmpAAC[1];struct Cyc_String_pa_PrintArg_struct _tmpAAB;(_tmpAAB.tag=0,((_tmpAAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAAC[0]=& _tmpAAB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAD="is_poly_field: bad type %s",_tag_dyneither(_tmpAAD,sizeof(char),27))),_tag_dyneither(_tmpAAC,sizeof(void*),1)))))));}_LLEC:;}
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
const char*_tmpAB1;void*_tmpAB0[1];struct Cyc_String_pa_PrintArg_struct _tmpAAF;(_tmpAAF.tag=0,((_tmpAAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpAB0[0]=& _tmpAAF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB1="is_poly_project: bad type %s",_tag_dyneither(_tmpAB1,sizeof(char),29))),_tag_dyneither(_tmpAB0,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1004
 return 0;_LLF3:;}
# 1009
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAB2;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->hd=s,((_tmpAB2->tl=0,_tmpAB2)))))),0);}
# 1013
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAB3;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->hd=s,((_tmpAB3->tl=0,_tmpAB3)))))),0);}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpAB4[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAB4[1]=s,((_tmpAB4[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB4,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpAB5[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAB5[1]=n,((_tmpAB5[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1031
struct Cyc_Absyn_Exp*_tmpAB6[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAB6[1]=n,((_tmpAB6[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpAB7[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpAB7[2]=n,((_tmpAB7[1]=s,((_tmpAB7[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpAB8;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->hd=e,((_tmpAB8->tl=0,_tmpAB8)))))),0);}
# 1043
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpAB9[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB9,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpABF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpABE;struct Cyc_List_List*_tmpABD;Cyc_Toc_result_decls=((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->hd=Cyc_Absyn_new_decl((void*)((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF[0]=((_tmpABE.tag=0,((_tmpABE.f1=vd,_tmpABE)))),_tmpABF)))),0),((_tmpABD->tl=Cyc_Toc_result_decls,_tmpABD))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1073
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1075
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1077
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpAC6;struct _tuple16*_tmpAC5;struct _tuple16*_tmpAC4;struct _tuple16*_tmpAC3[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpAC3[2]=(
# 1080
(_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->f1=0,((_tmpAC4->f2=xplussz,_tmpAC4)))))),((_tmpAC3[1]=(
# 1079
(_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->f1=0,((_tmpAC5->f2=xexp,_tmpAC5)))))),((_tmpAC3[0]=(
# 1078
(_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->f1=0,((_tmpAC6->f2=xexp,_tmpAC6)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAC3,sizeof(struct _tuple16*),3)))))))),0);}
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
int*_tmpACE;struct Cyc_Absyn_Exp**_tmpACD;int*_tmpACC;struct Cyc_Toc_Env*_tmpACB;return(_tmpACB=_region_malloc(r,sizeof(*_tmpACB)),((_tmpACB->break_lab=(struct _dyneither_ptr**)0,((_tmpACB->continue_lab=(struct _dyneither_ptr**)0,((_tmpACB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpACB->varmap=(struct Cyc_Dict_Dict)
# 1145
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpACB->toplevel=(int)1,((_tmpACB->in_lhs=(int*)(
# 1147
(_tmpACC=_region_malloc(r,sizeof(*_tmpACC)),((_tmpACC[0]=0,_tmpACC)))),((_tmpACB->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpACB->struct_info).varsizeexp=((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=0,_tmpACD)))),(((_tmpACB->struct_info).lhs_exp=0,_tmpACB->struct_info)))),((_tmpACB->in_sizeof=(int*)(
(_tmpACE=_region_malloc(r,sizeof(*_tmpACE)),((_tmpACE[0]=0,_tmpACE)))),((_tmpACB->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpACB)))))))))))))))))));}
# 1153
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpACF;return(_tmpACF=_region_malloc(r,sizeof(*_tmpACF)),((_tmpACF->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpACF->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpACF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpACF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpACF->toplevel=(int)_tmp255,((_tmpACF->in_lhs=(int*)_tmp256,((_tmpACF->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpACF->in_sizeof=(int*)_tmp258,((_tmpACF->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpACF)))))))))))))))))));};}
# 1158
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpAD0;return(_tmpAD0=_region_malloc(r,sizeof(*_tmpAD0)),((_tmpAD0->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpAD0->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpAD0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpAD0->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpAD0->toplevel=(int)0,((_tmpAD0->in_lhs=(int*)_tmp260,((_tmpAD0->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpAD0->in_sizeof=(int*)_tmp262,((_tmpAD0->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD0)))))))))))))))))));};}
# 1162
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpAD1;return(_tmpAD1=_region_malloc(r,sizeof(*_tmpAD1)),((_tmpAD1->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpAD1->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpAD1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpAD1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpAD1->toplevel=(int)1,((_tmpAD1->in_lhs=(int*)_tmp26A,((_tmpAD1->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpAD1->in_sizeof=(int*)_tmp26C,((_tmpAD1->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD1)))))))))))))))))));};}
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
struct Cyc_Toc_Env*_tmpAD2;return(_tmpAD2=_region_malloc(r,sizeof(*_tmpAD2)),((_tmpAD2->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpAD2->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpAD2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpAD2->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpAD2->toplevel=(int)_tmp27A,((_tmpAD2->in_lhs=(int*)_tmp27B,((_tmpAD2->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpAD2->struct_info).varsizeexp=_tmp27C,(((_tmpAD2->struct_info).lhs_exp=exp,_tmpAD2->struct_info)))),((_tmpAD2->in_sizeof=(int*)_tmp27D,((_tmpAD2->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD2)))))))))))))))))));};}
# 1189
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp27F=(*x).f1;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1192
const char*_tmpAD6;void*_tmpAD5[1];struct Cyc_String_pa_PrintArg_struct _tmpAD4;(_tmpAD4.tag=0,((_tmpAD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpAD5[0]=& _tmpAD4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD6="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpAD6,sizeof(char),30))),_tag_dyneither(_tmpAD5,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1195
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpAD7;return(_tmpAD7=_region_malloc(r,sizeof(*_tmpAD7)),((_tmpAD7->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpAD7->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpAD7->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpAD7->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpAD7->toplevel=(int)_tmp288,((_tmpAD7->in_lhs=(int*)_tmp289,((_tmpAD7->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpAD7->in_sizeof=(int*)_tmp28B,((_tmpAD7->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD7)))))))))))))))))));};};}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpAD8;return(_tmpAD8=_region_malloc(r,sizeof(*_tmpAD8)),((_tmpAD8->break_lab=(struct _dyneither_ptr**)0,((_tmpAD8->continue_lab=(struct _dyneither_ptr**)0,((_tmpAD8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpAD8->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpAD8->toplevel=(int)_tmp293,((_tmpAD8->in_lhs=(int*)_tmp294,((_tmpAD8->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpAD8->in_sizeof=(int*)_tmp296,((_tmpAD8->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD8)))))))))))))))))));};}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1213
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpAD9;fallthru_vars=((_tmpAD9=_region_malloc(r,sizeof(*_tmpAD9)),((_tmpAD9->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpAD9->tl=fallthru_vars,_tmpAD9))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpADA;struct Cyc_Toc_FallthruInfo*fi=
(_tmpADA=_region_malloc(r,sizeof(*_tmpADA)),((_tmpADA->label=fallthru_l,((_tmpADA->binders=fallthru_vars,((_tmpADA->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpADA)))))));
struct _dyneither_ptr**_tmpADD;struct Cyc_Toc_Env*_tmpADC;return(_tmpADC=_region_malloc(r,sizeof(*_tmpADC)),((_tmpADC->break_lab=(struct _dyneither_ptr**)((_tmpADD=_region_malloc(r,sizeof(*_tmpADD)),((_tmpADD[0]=break_l,_tmpADD)))),((_tmpADC->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpADC->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpADC->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpADC->toplevel=(int)_tmp29E,((_tmpADC->in_lhs=(int*)_tmp29F,((_tmpADC->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpADC->in_sizeof=(int*)_tmp2A1,((_tmpADC->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpADC)))))))))))))))))));};};};}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1228
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpAE0;struct Cyc_Toc_Env*_tmpADF;return(_tmpADF=_region_malloc(r,sizeof(*_tmpADF)),((_tmpADF->break_lab=(struct _dyneither_ptr**)((_tmpAE0=_region_malloc(r,sizeof(*_tmpAE0)),((_tmpAE0[0]=break_l,_tmpAE0)))),((_tmpADF->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpADF->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpADF->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpADF->toplevel=(int)_tmp2AC,((_tmpADF->in_lhs=(int*)_tmp2AD,((_tmpADF->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpADF->in_sizeof=(int*)_tmp2AF,((_tmpADF->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpADF)))))))))))))))))));};}
# 1235
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1238
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpAE3;struct Cyc_Toc_Env*_tmpAE2;return(_tmpAE2=_region_malloc(r,sizeof(*_tmpAE2)),((_tmpAE2->break_lab=(struct _dyneither_ptr**)0,((_tmpAE2->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpAE2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpAE3=_region_malloc(r,sizeof(*_tmpAE3)),((_tmpAE3->label=next_l,((_tmpAE3->binders=0,((_tmpAE3->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpAE3)))))))),((_tmpAE2->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpAE2->toplevel=(int)_tmp2B7,((_tmpAE2->in_lhs=(int*)_tmp2B8,((_tmpAE2->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpAE2->in_sizeof=(int*)_tmp2BA,((_tmpAE2->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAE2)))))))))))))))))));};}
# 1253 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1256
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2BD=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1261
{const char*_tmpAE6;void*_tmpAE5;(_tmpAE5=0,Cyc_Tcutil_terr(e->loc,((_tmpAE6="dereference of NULL pointer",_tag_dyneither(_tmpAE6,sizeof(char),28))),_tag_dyneither(_tmpAE5,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1266
 return 0;_LL114:;_LL115: {
const char*_tmpAE9;void*_tmpAE8;(_tmpAE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE9="need_null_check",_tag_dyneither(_tmpAE9,sizeof(char),16))),_tag_dyneither(_tmpAE8,sizeof(void*),0)));}_LL10B:;}
# 1271
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2C6=e->annot;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1276
{const char*_tmpAEC;void*_tmpAEB;(_tmpAEB=0,Cyc_Tcutil_terr(e->loc,((_tmpAEC="dereference of NULL pointer",_tag_dyneither(_tmpAEC,sizeof(char),28))),_tag_dyneither(_tmpAEB,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAEF;void*_tmpAEE;(_tmpAEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEF="get_relns",_tag_dyneither(_tmpAEF,sizeof(char),10))),_tag_dyneither(_tmpAEE,sizeof(void*),0)));}_LL116:;}
# 1284
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp2D1=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmp2D3;union Cyc_Absyn_Constraint*_tmp2D4;struct Cyc_Absyn_Exp*_tmp2D6;_LL122: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1;if(_tmp2D2->tag != 5)goto _LL124;else{_tmp2D3=((_tmp2D2->f1).ptr_atts).bounds;_tmp2D4=((_tmp2D2->f1).ptr_atts).zero_term;}}_LL123: {
# 1287
void*_tmp2D7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2D3);struct Cyc_Absyn_Exp*_tmp2DA;_LL129: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2D8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2D7;if(_tmp2D8->tag != 0)goto _LL12B;}_LL12A:
 return 0;_LL12B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2D9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D7;if(_tmp2D9->tag != 1)goto _LL128;else{_tmp2DA=_tmp2D9->f1;}}_LL12C: {
# 1290
unsigned int _tmp2DC;int _tmp2DD;struct _tuple11 _tmp2DB=Cyc_Evexp_eval_const_uint_exp(_tmp2DA);_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;
return _tmp2DD  && i <= _tmp2DC;}_LL128:;}_LL124: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2D5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D1;if(_tmp2D5->tag != 8)goto _LL126;else{_tmp2D6=(_tmp2D5->f1).num_elts;}}_LL125:
# 1294
 if(_tmp2D6 == 0)return 0;{
unsigned int _tmp2DF;int _tmp2E0;struct _tuple11 _tmp2DE=Cyc_Evexp_eval_const_uint_exp(_tmp2D6);_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;
return _tmp2E0  && i <= _tmp2DF;};_LL126:;_LL127:
 return 0;_LL121:;}
# 1302
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1304
for(0;relns != 0;relns=relns->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2E1=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp2E1->vd != y)continue;{
union Cyc_CfFlowInfo_RelnOp _tmp2E2=_tmp2E1->rop;struct Cyc_Absyn_Vardecl*_tmp2E3;struct Cyc_Absyn_Vardecl*_tmp2E4;_LL12E: if((_tmp2E2.LessNumelts).tag != 3)goto _LL130;_tmp2E3=(struct Cyc_Absyn_Vardecl*)(_tmp2E2.LessNumelts).val;_LL12F:
 _tmp2E4=_tmp2E3;goto _LL131;_LL130: if((_tmp2E2.LessEqNumelts).tag != 5)goto _LL132;_tmp2E4=(struct Cyc_Absyn_Vardecl*)(_tmp2E2.LessEqNumelts).val;_LL131:
 if(_tmp2E4 == v)return 1;else{goto _LL12D;}_LL132:;_LL133:
 continue;_LL12D:;};}
# 1313
return 0;}
# 1317
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1319
{void*_tmp2E5=e->r;struct Cyc_Absyn_Vardecl*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2F1;int _tmp2F3;int _tmp2F5;int _tmp2F7;struct Cyc_Absyn_Exp*_tmp2F9;_LL135: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2E6->tag != 1)goto _LL137;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2E7=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2E6->f2);if(_tmp2E7->tag != 5)goto _LL137;else{_tmp2E8=_tmp2E7->f1;}}}_LL136:
 _tmp2EB=_tmp2E8;goto _LL138;_LL137: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2E9->tag != 1)goto _LL139;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EA=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2E9->f2);if(_tmp2EA->tag != 4)goto _LL139;else{_tmp2EB=_tmp2EA->f1;}}}_LL138:
 _tmp2EE=_tmp2EB;goto _LL13A;_LL139: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2EC->tag != 1)goto _LL13B;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp2ED=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp2EC->f2);if(_tmp2ED->tag != 1)goto _LL13B;else{_tmp2EE=_tmp2ED->f1;}}}_LL13A:
 _tmp2F1=_tmp2EE;goto _LL13C;_LL13B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2EF->tag != 1)goto _LL13D;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp2F0=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp2EF->f2);if(_tmp2F0->tag != 3)goto _LL13D;else{_tmp2F1=_tmp2F0->f1;}}}_LL13C:
# 1324
 if(_tmp2F1->escapes)return 0;
# 1326
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2F1))return 1;
goto _LL134;_LL13D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2F2->tag != 0)goto _LL13F;else{if(((_tmp2F2->f1).Int_c).tag != 5)goto _LL13F;if(((struct _tuple5)((_tmp2F2->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL13F;_tmp2F3=((struct _tuple5)((_tmp2F2->f1).Int_c).val).f2;}}_LL13E:
 _tmp2F5=_tmp2F3;goto _LL140;_LL13F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2F4->tag != 0)goto _LL141;else{if(((_tmp2F4->f1).Int_c).tag != 5)goto _LL141;if(((struct _tuple5)((_tmp2F4->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL141;_tmp2F5=((struct _tuple5)((_tmp2F4->f1).Int_c).val).f2;}}_LL140:
# 1330
 return _tmp2F5 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp2F5,vtype);_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2F6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2F6->tag != 0)goto _LL143;else{if(((_tmp2F6->f1).Int_c).tag != 5)goto _LL143;if(((struct _tuple5)((_tmp2F6->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL143;_tmp2F7=((struct _tuple5)((_tmp2F6->f1).Int_c).val).f2;}}_LL142:
# 1332
 return Cyc_Toc_check_const_array((unsigned int)_tmp2F7,vtype);_LL143: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2F8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E5;if(_tmp2F8->tag != 2)goto _LL145;else{if(_tmp2F8->f1 != Cyc_Absyn_Numelts)goto _LL145;if(_tmp2F8->f2 == 0)goto _LL145;_tmp2F9=(struct Cyc_Absyn_Exp*)(_tmp2F8->f2)->hd;}}_LL144:
# 1335
{void*_tmp2FA=_tmp2F9->r;struct Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Vardecl*_tmp300;struct Cyc_Absyn_Vardecl*_tmp303;struct Cyc_Absyn_Vardecl*_tmp306;_LL148: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FA;if(_tmp2FB->tag != 1)goto _LL14A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp2FC=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp2FB->f2);if(_tmp2FC->tag != 5)goto _LL14A;else{_tmp2FD=_tmp2FC->f1;}}}_LL149:
 _tmp300=_tmp2FD;goto _LL14B;_LL14A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2FE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FA;if(_tmp2FE->tag != 1)goto _LL14C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2FF=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp2FE->f2);if(_tmp2FF->tag != 4)goto _LL14C;else{_tmp300=_tmp2FF->f1;}}}_LL14B:
 _tmp303=_tmp300;goto _LL14D;_LL14C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp301=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FA;if(_tmp301->tag != 1)goto _LL14E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp302=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp301->f2);if(_tmp302->tag != 1)goto _LL14E;else{_tmp303=_tmp302->f1;}}}_LL14D:
 _tmp306=_tmp303;goto _LL14F;_LL14E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp304=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2FA;if(_tmp304->tag != 1)goto _LL150;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp305=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp304->f2);if(_tmp305->tag != 3)goto _LL150;else{_tmp306=_tmp305->f1;}}}_LL14F:
# 1340
 return _tmp306 == v;_LL150:;_LL151:
 goto _LL147;_LL147:;}
# 1343
goto _LL134;_LL145:;_LL146:
 goto _LL134;_LL134:;}
# 1346
return 0;}static char _tmp314[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1352
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1362 "toc.cyc"
struct Cyc_Absyn_Vardecl*x;
# 1364
{void*_tmp307=a->r;struct Cyc_Absyn_Vardecl*_tmp30A;struct Cyc_Absyn_Vardecl*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp310;struct Cyc_Absyn_Vardecl*_tmp313;_LL153: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp308->tag != 1)goto _LL155;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp309=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp308->f2);if(_tmp309->tag != 5)goto _LL155;else{_tmp30A=_tmp309->f1;}}}_LL154:
 _tmp30D=_tmp30A;goto _LL156;_LL155: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp30B->tag != 1)goto _LL157;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp30B->f2);if(_tmp30C->tag != 4)goto _LL157;else{_tmp30D=_tmp30C->f1;}}}_LL156:
 _tmp310=_tmp30D;goto _LL158;_LL157: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp30E->tag != 1)goto _LL159;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp30F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp30E->f2);if(_tmp30F->tag != 1)goto _LL159;else{_tmp310=_tmp30F->f1;}}}_LL158:
 _tmp313=_tmp310;goto _LL15A;_LL159: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp311=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp311->tag != 1)goto _LL15B;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp312=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp311->f2);if(_tmp312->tag != 3)goto _LL15B;else{_tmp313=_tmp312->f1;}}}_LL15A:
# 1369
 if(_tmp313->escapes)goto _LL15C;
x=_tmp313;
goto _LL152;_LL15B:;_LL15C: {
# 1374
static struct _dyneither_ptr bogus_string={_tmp314,_tmp314,_tmp314 + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1377
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1383
x=& bogus_vardecl;
x->type=a_typ;}_LL152:;}{
# 1386
void*_tmp315=a_typ;
# 1388
inner_loop: {
void*_tmp316=i->r;void*_tmp318;struct Cyc_Absyn_Exp*_tmp319;int _tmp31B;int _tmp31D;int _tmp31F;struct Cyc_Absyn_Exp*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Vardecl*_tmp325;struct Cyc_Absyn_Vardecl*_tmp328;struct Cyc_Absyn_Vardecl*_tmp32B;struct Cyc_Absyn_Vardecl*_tmp32E;_LL15E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp317=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp317->tag != 13)goto _LL160;else{_tmp318=(void*)_tmp317->f1;_tmp319=_tmp317->f2;}}_LL15F:
 i=_tmp319;goto inner_loop;_LL160: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp31A->tag != 0)goto _LL162;else{if(((_tmp31A->f1).Int_c).tag != 5)goto _LL162;if(((struct _tuple5)((_tmp31A->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LL162;_tmp31B=((struct _tuple5)((_tmp31A->f1).Int_c).val).f2;}}_LL161:
 _tmp31D=_tmp31B;goto _LL163;_LL162: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp31C->tag != 0)goto _LL164;else{if(((_tmp31C->f1).Int_c).tag != 5)goto _LL164;if(((struct _tuple5)((_tmp31C->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL164;_tmp31D=((struct _tuple5)((_tmp31C->f1).Int_c).val).f2;}}_LL163:
# 1393
 return _tmp31D >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp31D + 1),_tmp315);_LL164: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp31E->tag != 0)goto _LL166;else{if(((_tmp31E->f1).Int_c).tag != 5)goto _LL166;if(((struct _tuple5)((_tmp31E->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL166;_tmp31F=((struct _tuple5)((_tmp31E->f1).Int_c).val).f2;}}_LL165:
# 1395
 return Cyc_Toc_check_const_array((unsigned int)(_tmp31F + 1),_tmp315);_LL166: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp320=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp320->tag != 2)goto _LL168;else{if(_tmp320->f1 != Cyc_Absyn_Mod)goto _LL168;if(_tmp320->f2 == 0)goto _LL168;_tmp321=(struct Cyc_Absyn_Exp*)(_tmp320->f2)->hd;if((_tmp320->f2)->tl == 0)goto _LL168;_tmp322=(struct Cyc_Absyn_Exp*)((_tmp320->f2)->tl)->hd;}}_LL167:
# 1399
 return Cyc_Toc_check_leq_size(relns,x,_tmp322,_tmp315);_LL168: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp323=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp323->tag != 1)goto _LL16A;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp324=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp323->f2);if(_tmp324->tag != 5)goto _LL16A;else{_tmp325=_tmp324->f1;}}}_LL169:
 _tmp328=_tmp325;goto _LL16B;_LL16A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp326=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp326->tag != 1)goto _LL16C;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp327=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp326->f2);if(_tmp327->tag != 4)goto _LL16C;else{_tmp328=_tmp327->f1;}}}_LL16B:
 _tmp32B=_tmp328;goto _LL16D;_LL16C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp329=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp329->tag != 1)goto _LL16E;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp32A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp329->f2);if(_tmp32A->tag != 1)goto _LL16E;else{_tmp32B=_tmp32A->f1;}}}_LL16D:
 _tmp32E=_tmp32B;goto _LL16F;_LL16E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp32C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp316;if(_tmp32C->tag != 1)goto _LL170;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp32D=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp32C->f2);if(_tmp32D->tag != 3)goto _LL170;else{_tmp32E=_tmp32D->f1;}}}_LL16F:
# 1404
 if(_tmp32E->escapes)return 0;
# 1407
{struct Cyc_List_List*_tmp32F=relns;for(0;_tmp32F != 0;_tmp32F=_tmp32F->tl){
struct Cyc_CfFlowInfo_Reln*_tmp330=(struct Cyc_CfFlowInfo_Reln*)_tmp32F->hd;
if(_tmp330->vd == _tmp32E){
union Cyc_CfFlowInfo_RelnOp _tmp331=_tmp330->rop;struct Cyc_Absyn_Vardecl*_tmp332;struct Cyc_Absyn_Vardecl*_tmp333;void*_tmp334;unsigned int _tmp335;unsigned int _tmp336;_LL173: if((_tmp331.LessNumelts).tag != 3)goto _LL175;_tmp332=(struct Cyc_Absyn_Vardecl*)(_tmp331.LessNumelts).val;_LL174:
# 1412
 if(x == _tmp332)return 1;else{goto _LL172;}_LL175: if((_tmp331.LessVar).tag != 2)goto _LL177;_tmp333=((struct _tuple12)(_tmp331.LessVar).val).f1;_tmp334=((struct _tuple12)(_tmp331.LessVar).val).f2;_LL176:
# 1426 "toc.cyc"
{struct _tuple19 _tmpAF0;struct _tuple19 _tmp338=(_tmpAF0.f1=Cyc_Tcutil_compress(_tmp334),((_tmpAF0.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpAF0)));void*_tmp33A;union Cyc_Absyn_Constraint*_tmp33C;_LL17E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp338.f1;if(_tmp339->tag != 19)goto _LL180;else{_tmp33A=(void*)_tmp339->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp338.f2;if(_tmp33B->tag != 5)goto _LL180;else{_tmp33C=((_tmp33B->f1).ptr_atts).bounds;}};_LL17F:
# 1428
{void*_tmp33D=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp33C);struct Cyc_Absyn_Exp*_tmp33F;_LL183: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp33E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp33D;if(_tmp33E->tag != 1)goto _LL185;else{_tmp33F=_tmp33E->f1;}}_LL184: {
# 1430
struct Cyc_Absyn_Exp*_tmp340=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp33A,0),0,Cyc_Absyn_No_coercion,0);
# 1436
if(Cyc_Evexp_lte_const_exp(_tmp340,_tmp33F))
return 1;
goto _LL182;}_LL185:;_LL186:
 goto _LL182;_LL182:;}
# 1441
goto _LL17D;_LL180:;_LL181:
 goto _LL17D;_LL17D:;}
# 1445
{struct Cyc_List_List*_tmp341=relns;for(0;_tmp341 != 0;_tmp341=_tmp341->tl){
struct Cyc_CfFlowInfo_Reln*_tmp342=(struct Cyc_CfFlowInfo_Reln*)_tmp341->hd;
if(_tmp342->vd == _tmp333){
union Cyc_CfFlowInfo_RelnOp _tmp343=_tmp342->rop;struct Cyc_Absyn_Vardecl*_tmp344;struct Cyc_Absyn_Vardecl*_tmp345;unsigned int _tmp346;unsigned int _tmp347;struct Cyc_Absyn_Vardecl*_tmp348;_LL188: if((_tmp343.LessEqNumelts).tag != 5)goto _LL18A;_tmp344=(struct Cyc_Absyn_Vardecl*)(_tmp343.LessEqNumelts).val;_LL189:
 _tmp345=_tmp344;goto _LL18B;_LL18A: if((_tmp343.LessNumelts).tag != 3)goto _LL18C;_tmp345=(struct Cyc_Absyn_Vardecl*)(_tmp343.LessNumelts).val;_LL18B:
# 1451
 if(x == _tmp345)return 1;
goto _LL187;_LL18C: if((_tmp343.EqualConst).tag != 1)goto _LL18E;_tmp346=(unsigned int)(_tmp343.EqualConst).val;_LL18D:
# 1454
 return Cyc_Toc_check_const_array(_tmp346,_tmp315);_LL18E: if((_tmp343.LessEqConst).tag != 6)goto _LL190;_tmp347=(unsigned int)(_tmp343.LessEqConst).val;if(!(_tmp347 > 0))goto _LL190;_LL18F:
# 1456
 return Cyc_Toc_check_const_array(_tmp347,_tmp315);_LL190: if((_tmp343.LessVar).tag != 2)goto _LL192;_tmp348=((struct _tuple12)(_tmp343.LessVar).val).f1;_LL191:
# 1458
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp348))return 1;
goto _LL187;_LL192:;_LL193:
 goto _LL187;_LL187:;}}}
# 1463
goto _LL172;_LL177: if((_tmp331.LessConst).tag != 4)goto _LL179;_tmp335=(unsigned int)(_tmp331.LessConst).val;_LL178:
# 1466
 return Cyc_Toc_check_const_array(_tmp335,_tmp315);_LL179: if((_tmp331.LessEqConst).tag != 6)goto _LL17B;_tmp336=(unsigned int)(_tmp331.LessEqConst).val;_LL17A:
# 1469
 return Cyc_Toc_check_const_array(_tmp336 + 1,_tmp315);_LL17B:;_LL17C:
 goto _LL172;_LL172:;}}}
# 1474
goto _LL15D;_LL170:;_LL171:
 goto _LL15D;_LL15D:;}
# 1477
return 0;};}
# 1480
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAF3;void*_tmpAF2;(_tmpAF2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF3="Missing type in primop ",_tag_dyneither(_tmpAF3,sizeof(char),24))),_tag_dyneither(_tmpAF2,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1484
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAF6;void*_tmpAF5;(_tmpAF5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF6="Missing type in primop ",_tag_dyneither(_tmpAF6,sizeof(char),24))),_tag_dyneither(_tmpAF5,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1488
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpAF7;return(_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->f1=Cyc_Toc_mt_tq,((_tmpAF7->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpAF7)))));}
# 1491
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpAF8;return(_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->f1=0,((_tmpAF8->f2=e,_tmpAF8)))));};}
# 1496
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1499
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp34F=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp34F=Cyc_Absyn_add_exp(_tmp34F,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1507
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp34F): Cyc_Toc_malloc_ptr(_tmp34F);else{
# 1511
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp34F);}}else{
# 1516
t=struct_typ;
eo=0;}
# 1519
if(do_declare){
void*_tmp350=x->r;struct _tuple0*_tmp352;_LL195: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp351->tag != 1)goto _LL197;else{_tmp352=_tmp351->f1;}}_LL196:
# 1522
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp352,t,eo,s,0),0);_LL197:;_LL198: {
# 1524
const char*_tmpAFB;void*_tmpAFA;(_tmpAFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFB="bogus x to make_struct",_tag_dyneither(_tmpAFB,sizeof(char),23))),_tag_dyneither(_tmpAFA,sizeof(void*),0)));}_LL194:;}else{
# 1528
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1531
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1535
static int Cyc_Toc_array_or_aggr_with_array(void*typ){
void*_tmp355=Cyc_Tcutil_compress(typ);union Cyc_Absyn_AggrInfoU _tmp358;struct Cyc_List_List*_tmp359;struct Cyc_List_List*_tmp35B;_LL19A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp356=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp355;if(_tmp356->tag != 8)goto _LL19C;}_LL19B:
 return 1;_LL19C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp355;if(_tmp357->tag != 11)goto _LL19E;else{_tmp358=(_tmp357->f1).aggr_info;_tmp359=(_tmp357->f1).targs;}}_LL19D: {
# 1539
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp358);
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp35B=aggrfields;goto _LL19F;}_LL19E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp35A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp355;if(_tmp35A->tag != 12)goto _LL1A0;else{_tmp35B=_tmp35A->f2;}}_LL19F:
# 1543
 if(_tmp35B == 0)return 0;{
struct Cyc_List_List*fields;
for(fields=_tmp35B;fields->tl != 0;fields=fields->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp35C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
return Cyc_Toc_array_or_aggr_with_array(_tmp35C->type);};};_LL1A0:;_LL1A1:
# 1549
 return 0;_LL199:;}
# 1553
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp35F;_LL1A3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35D;if(_tmp35E->tag != 8)goto _LL1A5;else{_tmp35F=(_tmp35E->f1).elt_type;}}_LL1A4:
 return _tmp35F;_LL1A5:;_LL1A6: {
const char*_tmpAFF;void*_tmpAFE[1];struct Cyc_String_pa_PrintArg_struct _tmpAFD;(_tmpAFD.tag=0,((_tmpAFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAFE[0]=& _tmpAFD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFF="array_elt_type called on %s",_tag_dyneither(_tmpAFF,sizeof(char),28))),_tag_dyneither(_tmpAFE,sizeof(void*),1)))))));}_LL1A2:;}
# 1560
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1566
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1571
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1576
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp363=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp363 != 0;_tmp363=_tmp363->tl){
# 1580
struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct _tuple16*_tmp364=(struct _tuple16*)_tmp363->hd;_tmp365=_tmp364->f1;_tmp366=_tmp364->f2;{
# 1584
struct Cyc_Absyn_Exp*e_index;
if(_tmp365 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1588
if(_tmp365->tl != 0){const char*_tmpB02;void*_tmpB01;(_tmpB01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB02="multiple designators in array",_tag_dyneither(_tmpB02,sizeof(char),30))),_tag_dyneither(_tmpB01,sizeof(void*),0)));}{
void*_tmp369=(void*)_tmp365->hd;
void*_tmp36A=_tmp369;struct Cyc_Absyn_Exp*_tmp36C;_LL1A8: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp36B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp36A;if(_tmp36B->tag != 0)goto _LL1AA;else{_tmp36C=_tmp36B->f1;}}_LL1A9:
# 1592
 Cyc_Toc_exp_to_c(nv,_tmp36C);
e_index=_tmp36C;
goto _LL1A7;_LL1AA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp36D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp36A;if(_tmp36D->tag != 1)goto _LL1A7;}_LL1AB: {
const char*_tmpB05;void*_tmpB04;(_tmpB04=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB05="field name designators in array",_tag_dyneither(_tmpB05,sizeof(char),32))),_tag_dyneither(_tmpB04,sizeof(void*),0)));}_LL1A7:;};}{
# 1598
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp370=_tmp366->r;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp376;int _tmp377;void*_tmp379;struct Cyc_List_List*_tmp37A;_LL1AD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp371=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp371->tag != 25)goto _LL1AF;else{_tmp372=_tmp371->f1;}}_LL1AE:
# 1601
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp372,s);
goto _LL1AC;_LL1AF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp373->tag != 26)goto _LL1B1;else{_tmp374=_tmp373->f1;_tmp375=_tmp373->f2;_tmp376=_tmp373->f3;_tmp377=_tmp373->f4;}}_LL1B0:
# 1604
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp374,_tmp375,_tmp376,_tmp377,s,0);
# 1606
goto _LL1AC;_LL1B1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp378->tag != 28)goto _LL1B3;else{_tmp379=(void*)_tmp378->f1;_tmp37A=_tmp378->f2;}}_LL1B2:
# 1608
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp379,_tmp37A,s);
goto _LL1AC;_LL1B3:;_LL1B4:
# 1611
 Cyc_Toc_exp_to_c(nv,_tmp366);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp366),0),s,0);
goto _LL1AC;_LL1AC:;};};}}
# 1617
return s;}
# 1622
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1627
struct _tuple0*_tmp37B=vd->name;
void*expected_elt_type;
{void*_tmp37C=Cyc_Tcutil_compress(comprehension_type);void*_tmp37E;void*_tmp380;_LL1B6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37C;if(_tmp37D->tag != 8)goto _LL1B8;else{_tmp37E=(_tmp37D->f1).elt_type;}}_LL1B7:
# 1631
 expected_elt_type=_tmp37E;goto _LL1B5;_LL1B8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37C;if(_tmp37F->tag != 5)goto _LL1BA;else{_tmp380=(_tmp37F->f1).elt_typ;}}_LL1B9:
# 1633
 expected_elt_type=_tmp380;goto _LL1B5;_LL1BA:;_LL1BB: {
const char*_tmpB08;void*_tmpB07;(_tmpB07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB08="init_comprehension passed a bad type!",_tag_dyneither(_tmpB08,sizeof(char),38))),_tag_dyneither(_tmpB07,sizeof(void*),0)));}_LL1B5:;}{
# 1636
void*_tmp383=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp384=_new_region("r2");struct _RegionHandle*r2=& _tmp384;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB0B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB0A;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp37B,Cyc_Absyn_varb_exp(_tmp37B,(void*)((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=((_tmpB0B.tag=4,((_tmpB0B.f1=vd,_tmpB0B)))),_tmpB0A)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp37B,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp37B,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp37B,0),0);
# 1647
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp37B,0),0);
struct Cyc_Absyn_Stmt*body;
# 1651
{void*_tmp385=e2->r;struct Cyc_List_List*_tmp387;struct Cyc_Absyn_Vardecl*_tmp389;struct Cyc_Absyn_Exp*_tmp38A;struct Cyc_Absyn_Exp*_tmp38B;int _tmp38C;void*_tmp38E;struct Cyc_List_List*_tmp38F;_LL1BD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp386=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp385;if(_tmp386->tag != 25)goto _LL1BF;else{_tmp387=_tmp386->f1;}}_LL1BE:
# 1653
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp387,Cyc_Toc_skip_stmt_dl());
goto _LL1BC;_LL1BF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp388=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp385;if(_tmp388->tag != 26)goto _LL1C1;else{_tmp389=_tmp388->f1;_tmp38A=_tmp388->f2;_tmp38B=_tmp388->f3;_tmp38C=_tmp388->f4;}}_LL1C0:
# 1656
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp389,_tmp38A,_tmp38B,_tmp38C,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1BC;_LL1C1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp38D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp385;if(_tmp38D->tag != 28)goto _LL1C3;else{_tmp38E=(void*)_tmp38D->f1;_tmp38F=_tmp38D->f2;}}_LL1C2:
# 1659
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp38E,_tmp38F,Cyc_Toc_skip_stmt_dl());
goto _LL1BC;_LL1C3:;_LL1C4:
# 1662
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1BC;_LL1BC:;}{
# 1666
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1668
if(zero_term){
# 1673
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp383,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1678
struct Cyc_Absyn_Stmt*_tmp390=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp37B,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp390;};};}
# 1640
;_pop_region(r2);};};}
# 1684
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1687
{struct Cyc_List_List*_tmp393=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp393 != 0;_tmp393=_tmp393->tl){
struct Cyc_List_List*_tmp395;struct Cyc_Absyn_Exp*_tmp396;struct _tuple16*_tmp394=(struct _tuple16*)_tmp393->hd;_tmp395=_tmp394->f1;_tmp396=_tmp394->f2;
if(_tmp395 == 0){
const char*_tmpB0E;void*_tmpB0D;(_tmpB0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0E="empty designator list",_tag_dyneither(_tmpB0E,sizeof(char),22))),_tag_dyneither(_tmpB0D,sizeof(void*),0)));}
if(_tmp395->tl != 0){
const char*_tmpB11;void*_tmpB10;(_tmpB10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB11="too many designators in anonymous struct",_tag_dyneither(_tmpB11,sizeof(char),41))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}{
void*_tmp39B=(void*)_tmp395->hd;struct _dyneither_ptr*_tmp39D;_LL1C6: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp39C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp39B;if(_tmp39C->tag != 1)goto _LL1C8;else{_tmp39D=_tmp39C->f1;}}_LL1C7: {
# 1695
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp39D,0);
{void*_tmp39E=_tmp396->r;struct Cyc_List_List*_tmp3A0;struct Cyc_Absyn_Vardecl*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;int _tmp3A5;void*_tmp3A7;struct Cyc_List_List*_tmp3A8;_LL1CB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp39F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp39E;if(_tmp39F->tag != 25)goto _LL1CD;else{_tmp3A0=_tmp39F->f1;}}_LL1CC:
# 1698
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp396->topt),lval,_tmp3A0,s);goto _LL1CA;_LL1CD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3A1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp39E;if(_tmp3A1->tag != 26)goto _LL1CF;else{_tmp3A2=_tmp3A1->f1;_tmp3A3=_tmp3A1->f2;_tmp3A4=_tmp3A1->f3;_tmp3A5=_tmp3A1->f4;}}_LL1CE:
# 1700
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp396->topt),lval,_tmp3A2,_tmp3A3,_tmp3A4,_tmp3A5,s,0);
goto _LL1CA;_LL1CF: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3A6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp39E;if(_tmp3A6->tag != 28)goto _LL1D1;else{_tmp3A7=(void*)_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;}}_LL1D0:
# 1703
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3A7,_tmp3A8,s);goto _LL1CA;_LL1D1:;_LL1D2:
# 1705
 Cyc_Toc_exp_to_c(nv,_tmp396);
# 1707
if(Cyc_Toc_is_poly_field(struct_type,_tmp39D))
_tmp396=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp396);
# 1710
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp396,0),0),s,0);
goto _LL1CA;_LL1CA:;}
# 1713
goto _LL1C5;}_LL1C8:;_LL1C9: {
const char*_tmpB14;void*_tmpB13;(_tmpB13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB14="array designator in struct",_tag_dyneither(_tmpB14,sizeof(char),27))),_tag_dyneither(_tmpB13,sizeof(void*),0)));}_LL1C5:;};}}
# 1717
return s;}
# 1722
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1726
struct _RegionHandle _tmp3AB=_new_region("r");struct _RegionHandle*r=& _tmp3AB;_push_region(r);
{struct Cyc_List_List*_tmp3AC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp3AD=Cyc_Toc_add_tuple_type(_tmp3AC);
# 1730
struct _tuple0*_tmp3AE=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3AF=Cyc_Absyn_var_exp(_tmp3AE,0);
struct Cyc_Absyn_Stmt*_tmp3B0=Cyc_Absyn_exp_stmt(_tmp3AF,0);
# 1734
struct Cyc_Absyn_Exp*(*_tmp3B1)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1736
int is_atomic=1;
struct Cyc_List_List*_tmp3B2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3B2);for(0;_tmp3B2 != 0;(_tmp3B2=_tmp3B2->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3B2->hd;
struct Cyc_Absyn_Exp*lval=_tmp3B1(_tmp3AF,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp3B3=e->r;struct Cyc_List_List*_tmp3B5;struct Cyc_Absyn_Vardecl*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;int _tmp3BA;_LL1D4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3B4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3B4->tag != 25)goto _LL1D6;else{_tmp3B5=_tmp3B4->f1;}}_LL1D5:
# 1744
 _tmp3B0=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp3B5,_tmp3B0);
goto _LL1D3;_LL1D6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3B6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B3;if(_tmp3B6->tag != 26)goto _LL1D8;else{_tmp3B7=_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;_tmp3B9=_tmp3B6->f3;_tmp3BA=_tmp3B6->f4;}}_LL1D7:
# 1747
 _tmp3B0=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp3B7,_tmp3B8,_tmp3B9,_tmp3BA,_tmp3B0,0);
# 1749
goto _LL1D3;_LL1D8:;_LL1D9:
# 1752
 Cyc_Toc_exp_to_c(nv,e);
_tmp3B0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B1(_tmp3AF,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3B0,0);
# 1755
goto _LL1D3;_LL1D3:;};}}{
# 1758
struct Cyc_Absyn_Exp*_tmp3BB=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp3AE,0),_tmp3AD,_tmp3B0,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp3BB;};}
# 1727
;_pop_region(r);}
# 1762
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3BC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3BC != 0;_tmp3BC=_tmp3BC->tl){
struct Cyc_Absyn_Aggrfield*_tmp3BD=(struct Cyc_Absyn_Aggrfield*)_tmp3BC->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3BD->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1769
struct Cyc_String_pa_PrintArg_struct _tmpB1C;void*_tmpB1B[1];const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB1C.tag=0,((_tmpB1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB1B[0]=& _tmpB1C,Cyc_aprintf(((_tmpB1A="get_member_offset %s failed",_tag_dyneither(_tmpB1A,sizeof(char),28))),_tag_dyneither(_tmpB1B,sizeof(void*),1)))))))),_tag_dyneither(_tmpB19,sizeof(void*),0)));};}
# 1772
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3C2=Cyc_Tcutil_compress(*typ);void*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;union Cyc_Absyn_Constraint*_tmp3C7;unsigned int _tmp3C8;_LL1DB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 8)goto _LL1DD;else{_tmp3C4=(_tmp3C3->f1).elt_type;_tmp3C5=(_tmp3C3->f1).tq;_tmp3C6=(_tmp3C3->f1).num_elts;_tmp3C7=(_tmp3C3->f1).zero_term;_tmp3C8=(_tmp3C3->f1).zt_loc;}}_LL1DC:
# 1777
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3C6))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB22;struct Cyc_Absyn_ArrayInfo _tmpB21;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB20;*typ=(void*)((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB22.tag=8,((_tmpB22.f1=((_tmpB21.elt_type=_tmp3C4,((_tmpB21.tq=_tmp3C5,((_tmpB21.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB21.zero_term=_tmp3C7,((_tmpB21.zt_loc=_tmp3C8,_tmpB21)))))))))),_tmpB22)))),_tmpB20))));}
return 1;}
# 1781
goto _LL1DA;_LL1DD:;_LL1DE:
 goto _LL1DA;_LL1DA:;}
# 1784
return 0;}
# 1787
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1793
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple0*_tmp3CC=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3CC,0);
do_declare=1;}else{
# 1801
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1804
struct Cyc_Absyn_Stmt*_tmp3CD=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1806
struct Cyc_Absyn_Exp*(*_tmp3CE)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*_tmp3CF=Cyc_Toc_typ_to_c(struct_type);
# 1809
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3D0=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp3D2;struct Cyc_List_List*_tmp3D3;_LL1E0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D1=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D0;if(_tmp3D1->tag != 11)goto _LL1E2;else{_tmp3D2=(_tmp3D1->f1).aggr_info;_tmp3D3=(_tmp3D1->f1).targs;}}_LL1E1:
# 1816
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3D2);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3D3;
goto _LL1DF;_LL1E2:;_LL1E3: {
const char*_tmpB25;void*_tmpB24;(_tmpB24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB25="init_struct: bad struct type",_tag_dyneither(_tmpB25,sizeof(char),29))),_tag_dyneither(_tmpB24,sizeof(void*),0)));}_LL1DF:;}{
# 1822
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3D6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3D6->tl != 0;_tmp3D6=_tmp3D6->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3D7=(struct Cyc_Absyn_Aggrfield*)_tmp3D6->hd;
struct _RegionHandle _tmp3D8=_new_region("temp");struct _RegionHandle*temp=& _tmp3D8;_push_region(temp);
{void*cast_type;
if(pointer)
cast_type=Cyc_Absyn_cstar_typ(aggr_typ,Cyc_Toc_mt_tq);else{
# 1832
cast_type=aggr_typ;}
_tmp3CD=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(xexp)),0);{
struct Cyc_List_List*_tmp3D9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1838
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3D9,_tmp3D7->type))){
struct Cyc_List_List*_tmp3DA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3DB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3D9,_tmp3DA);
# 1842
struct Cyc_List_List*new_fields=0;
for(_tmp3D6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3D6 != 0;_tmp3D6=_tmp3D6->tl){
struct Cyc_Absyn_Aggrfield*_tmp3DC=(struct Cyc_Absyn_Aggrfield*)_tmp3D6->hd;
struct Cyc_Absyn_Aggrfield*_tmpB26;struct Cyc_Absyn_Aggrfield*_tmp3DD=(_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26->name=_tmp3DC->name,((_tmpB26->tq=Cyc_Toc_mt_tq,((_tmpB26->type=
# 1847
Cyc_Tcutil_rsubstitute(temp,_tmp3DB,_tmp3DC->type),((_tmpB26->width=_tmp3DC->width,((_tmpB26->attributes=_tmp3DC->attributes,_tmpB26)))))))))));
# 1854
if(_tmp3D6->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3DD->type)){
struct _dyneither_ptr**_tmpB27;index=((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=_tmp3DD->name,_tmpB27))));}}{
# 1861
struct Cyc_List_List*_tmpB28;new_fields=((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28->hd=_tmp3DD,((_tmpB28->tl=new_fields,_tmpB28))))));};}
# 1863
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB30;void*_tmpB2F[1];const char*_tmpB2E;struct _dyneither_ptr*_tmpB2D;struct _dyneither_ptr*name=
(_tmpB2D=_cycalloc(sizeof(*_tmpB2D)),((_tmpB2D[0]=(struct _dyneither_ptr)((_tmpB30.tag=1,((_tmpB30.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB2F[0]=& _tmpB30,Cyc_aprintf(((_tmpB2E="_genStruct%d",_tag_dyneither(_tmpB2E,sizeof(char),13))),_tag_dyneither(_tmpB2F,sizeof(void*),1)))))))),_tmpB2D)));
struct _tuple0*_tmpB31;struct _tuple0*qv=(_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB31->f2=name,_tmpB31)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB32;struct Cyc_Absyn_AggrdeclImpl*_tmp3E1=(_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->exist_vars=0,((_tmpB32->rgn_po=0,((_tmpB32->fields=aggrfields,((_tmpB32->tagged=0,_tmpB32)))))))));
# 1872
struct Cyc_Absyn_Aggrdecl*_tmpB33;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33->kind=Cyc_Absyn_StructA,((_tmpB33->sc=Cyc_Absyn_Public,((_tmpB33->name=qv,((_tmpB33->tvs=0,((_tmpB33->impl=_tmp3E1,((_tmpB33->attributes=0,_tmpB33)))))))))))));
# 1878
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB3D;struct Cyc_Absyn_Aggrdecl**_tmpB3C;struct Cyc_Absyn_AggrInfo _tmpB3B;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB3A;struct_type=(void*)((_tmpB3A=_cycalloc(sizeof(*_tmpB3A)),((_tmpB3A[0]=((_tmpB3D.tag=11,((_tmpB3D.f1=((_tmpB3B.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB3C=_cycalloc(sizeof(*_tmpB3C)),((_tmpB3C[0]=new_ad,_tmpB3C))))),((_tmpB3B.targs=0,_tmpB3B)))),_tmpB3D)))),_tmpB3A))));}
# 1882
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}};}
# 1828
;_pop_region(temp);};}{
# 1887
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3ED=_new_region("r");struct _RegionHandle*r=& _tmp3ED;_push_region(r);
{struct Cyc_List_List*_tmp3EE=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3EE != 0;_tmp3EE=_tmp3EE->tl){
struct Cyc_List_List*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;struct _tuple16*_tmp3EF=(struct _tuple16*)_tmp3EE->hd;_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3F1->topt));
if(_tmp3F0 == 0){
const char*_tmpB40;void*_tmpB3F;(_tmpB3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB40="empty designator list",_tag_dyneither(_tmpB40,sizeof(char),22))),_tag_dyneither(_tmpB3F,sizeof(void*),0)));}
if(_tmp3F0->tl != 0){
# 1897
struct _tuple0*_tmp3F4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_var_exp(_tmp3F4,0);
for(0;_tmp3F0 != 0;_tmp3F0=_tmp3F0->tl){
void*_tmp3F6=(void*)_tmp3F0->hd;struct _dyneither_ptr*_tmp3F8;_LL1E5: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3F6;if(_tmp3F7->tag != 1)goto _LL1E7;else{_tmp3F8=_tmp3F7->f1;}}_LL1E6:
# 1903
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3F8))
_tmp3F5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3F5);
_tmp3CD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3CE(xexp,_tmp3F8,0),_tmp3F5,0),0),_tmp3CD,0);
# 1907
goto _LL1E4;_LL1E7:;_LL1E8: {
const char*_tmpB43;void*_tmpB42;(_tmpB42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="array designator in struct",_tag_dyneither(_tmpB43,sizeof(char),27))),_tag_dyneither(_tmpB42,sizeof(void*),0)));}_LL1E4:;}
# 1911
Cyc_Toc_exp_to_c(nv,_tmp3F1);
_tmp3CD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F5,_tmp3F1,0),0),_tmp3CD,0);}else{
# 1914
void*_tmp3FB=(void*)_tmp3F0->hd;struct _dyneither_ptr*_tmp3FD;_LL1EA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3FC=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3FB;if(_tmp3FC->tag != 1)goto _LL1EC;else{_tmp3FD=_tmp3FC->f1;}}_LL1EB: {
# 1916
struct Cyc_Absyn_Exp*lval=_tmp3CE(xexp,_tmp3FD,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3FD);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3CD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3CD,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1925
{void*_tmp3FE=_tmp3F1->r;struct Cyc_List_List*_tmp400;struct Cyc_Absyn_Vardecl*_tmp402;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp404;int _tmp405;void*_tmp407;struct Cyc_List_List*_tmp408;_LL1EF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3FF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3FE;if(_tmp3FF->tag != 25)goto _LL1F1;else{_tmp400=_tmp3FF->f1;}}_LL1F0:
# 1927
 _tmp3CD=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3FD)))->type,lval,_tmp400,_tmp3CD);
# 1929
goto _LL1EE;_LL1F1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp401=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3FE;if(_tmp401->tag != 26)goto _LL1F3;else{_tmp402=_tmp401->f1;_tmp403=_tmp401->f2;_tmp404=_tmp401->f3;_tmp405=_tmp401->f4;}}_LL1F2:
# 1933
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3FD)){
struct _tuple5 _tmpB46;union Cyc_Absyn_Cnst _tmpB45;*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_times_exp(_tmp403,Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_check_null(_tmp404->topt)),0),0),
# 1937
Cyc_Absyn_const_exp((((_tmpB45.Int_c).val=((_tmpB46.f1=Cyc_Absyn_Unsigned,((_tmpB46.f2=(int)sizeof(double),_tmpB46)))),(((_tmpB45.Int_c).tag=5,_tmpB45)))),0),0);}
# 1940
_tmp3CD=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3FD)))->type,lval,_tmp402,_tmp403,_tmp404,_tmp405,_tmp3CD,0);
# 1943
goto _LL1EE;_LL1F3: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp406=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3FE;if(_tmp406->tag != 28)goto _LL1F5;else{_tmp407=(void*)_tmp406->f1;_tmp408=_tmp406->f2;}}_LL1F4:
# 1945
 _tmp3CD=Cyc_Toc_init_anon_struct(nv,lval,_tmp407,_tmp408,_tmp3CD);goto _LL1EE;_LL1F5:;_LL1F6: {
# 1947
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3F1->topt));
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3F1);{
struct Cyc_Absyn_Aggrfield*_tmp40B=Cyc_Absyn_lookup_field(aggrfields,_tmp3FD);
# 1951
if(Cyc_Toc_is_poly_field(struct_type,_tmp3FD) && !was_ptr_type)
_tmp3F1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3F1);
# 1954
if(exist_types != 0)
_tmp3F1=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp40B))->type),_tmp3F1);
# 1957
_tmp3CD=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3F1,0),0),_tmp3CD,0);
goto _LL1EE;};}_LL1EE:;}
# 1960
goto _LL1E9;}_LL1EC:;_LL1ED: {
const char*_tmpB49;void*_tmpB48;(_tmpB48=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB49="array designator in struct",_tag_dyneither(_tmpB49,sizeof(char),27))),_tag_dyneither(_tmpB48,sizeof(void*),0)));}_LL1E9:;}}}{
# 1966
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3CD,pointer,rgnopt,is_atomic,do_declare);_npop_handler(0);return _tmp40E;};
# 1889
;_pop_region(r);};};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1971
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1974
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1977
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB4C;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB4B;return Cyc_Absyn_new_exp((void*)((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B[0]=((_tmpB4C.tag=4,((_tmpB4C.f1=e1,((_tmpB4C.f2=incr,_tmpB4C)))))),_tmpB4B)))),0);}
# 1981
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1990
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1992
void*_tmp411=e1->r;struct Cyc_Absyn_Stmt*_tmp413;void*_tmp415;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp418;struct _dyneither_ptr*_tmp419;int _tmp41A;int _tmp41B;_LL1F8: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp412=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp412->tag != 35)goto _LL1FA;else{_tmp413=_tmp412->f1;}}_LL1F9:
 Cyc_Toc_lvalue_assign_stmt(_tmp413,fs,f,f_env);goto _LL1F7;_LL1FA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp414=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp414->tag != 13)goto _LL1FC;else{_tmp415=(void*)_tmp414->f1;_tmp416=_tmp414->f2;}}_LL1FB:
 Cyc_Toc_lvalue_assign(_tmp416,fs,f,f_env);goto _LL1F7;_LL1FC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp417=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp411;if(_tmp417->tag != 20)goto _LL1FE;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;_tmp41A=_tmp417->f3;_tmp41B=_tmp417->f4;}}_LL1FD:
# 1997
 e1->r=_tmp418->r;
{struct Cyc_List_List*_tmpB4D;Cyc_Toc_lvalue_assign(e1,((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D->hd=_tmp419,((_tmpB4D->tl=fs,_tmpB4D)))))),f,f_env);}
goto _LL1F7;_LL1FE:;_LL1FF: {
# 2005
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2007
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1F7;}_LL1F7:;}
# 2013
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2015
void*_tmp41D=s->r;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Decl*_tmp421;struct Cyc_Absyn_Stmt*_tmp422;struct Cyc_Absyn_Stmt*_tmp424;_LL201: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp41E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp41D;if(_tmp41E->tag != 1)goto _LL203;else{_tmp41F=_tmp41E->f1;}}_LL202:
 Cyc_Toc_lvalue_assign(_tmp41F,fs,f,f_env);goto _LL200;_LL203: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp420=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp41D;if(_tmp420->tag != 12)goto _LL205;else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;}}_LL204:
# 2018
 Cyc_Toc_lvalue_assign_stmt(_tmp422,fs,f,f_env);goto _LL200;_LL205: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp423=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp41D;if(_tmp423->tag != 2)goto _LL207;else{_tmp424=_tmp423->f2;}}_LL206:
 Cyc_Toc_lvalue_assign_stmt(_tmp424,fs,f,f_env);goto _LL200;_LL207:;_LL208: {
const char*_tmpB51;void*_tmpB50[1];struct Cyc_String_pa_PrintArg_struct _tmpB4F;(_tmpB4F.tag=0,((_tmpB4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB50[0]=& _tmpB4F,Cyc_Toc_toc_impos(((_tmpB51="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB51,sizeof(char),23))),_tag_dyneither(_tmpB50,sizeof(void*),1)))))));}_LL200:;}
# 2024
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2028
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp428=e->r;void**_tmp42A;struct Cyc_Absyn_Exp**_tmp42B;struct Cyc_Absyn_Exp*_tmp42D;struct Cyc_Absyn_Stmt*_tmp42F;_LL20A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp429=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp428;if(_tmp429->tag != 13)goto _LL20C;else{_tmp42A=(void**)((void**)& _tmp429->f1);_tmp42B=(struct Cyc_Absyn_Exp**)& _tmp429->f2;}}_LL20B:
# 2031
*_tmp42B=Cyc_Toc_push_address_exp(*_tmp42B);
*_tmp42A=Cyc_Absyn_cstar_typ(*_tmp42A,Cyc_Toc_mt_tq);
return e;_LL20C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp42C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp428;if(_tmp42C->tag != 19)goto _LL20E;else{_tmp42D=_tmp42C->f1;}}_LL20D:
# 2035
 return _tmp42D;_LL20E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp42E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp428;if(_tmp42E->tag != 35)goto _LL210;else{_tmp42F=_tmp42E->f1;}}_LL20F:
# 2039
 Cyc_Toc_push_address_stmt(_tmp42F);
return e;_LL210:;_LL211:
# 2042
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB55;void*_tmpB54[1];struct Cyc_String_pa_PrintArg_struct _tmpB53;(_tmpB53.tag=0,((_tmpB53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB54[0]=& _tmpB53,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="can't take & of exp %s",_tag_dyneither(_tmpB55,sizeof(char),23))),_tag_dyneither(_tmpB54,sizeof(void*),1)))))));};_LL209:;}
# 2047
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp433=s->r;struct Cyc_Absyn_Stmt*_tmp435;struct Cyc_Absyn_Stmt*_tmp437;struct Cyc_Absyn_Exp**_tmp439;_LL213: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp434=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp433;if(_tmp434->tag != 2)goto _LL215;else{_tmp435=_tmp434->f2;}}_LL214:
 _tmp437=_tmp435;goto _LL216;_LL215: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp436=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp433;if(_tmp436->tag != 12)goto _LL217;else{_tmp437=_tmp436->f2;}}_LL216:
 Cyc_Toc_push_address_stmt(_tmp437);goto _LL212;_LL217: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp438=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp433;if(_tmp438->tag != 1)goto _LL219;else{_tmp439=(struct Cyc_Absyn_Exp**)& _tmp438->f1;}}_LL218:
*_tmp439=Cyc_Toc_push_address_exp(*_tmp439);goto _LL212;_LL219:;_LL21A: {
# 2053
const char*_tmpB59;void*_tmpB58[1];struct Cyc_String_pa_PrintArg_struct _tmpB57;(_tmpB57.tag=0,((_tmpB57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB58[0]=& _tmpB57,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB59="can't take & of stmt %s",_tag_dyneither(_tmpB59,sizeof(char),24))),_tag_dyneither(_tmpB58,sizeof(void*),1)))))));}_LL212:;}
# 2058
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2060
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2062
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB5A;result=((_tmpB5A=_region_malloc(r2,sizeof(*_tmpB5A)),((_tmpB5A->hd=f(x->hd,env),((_tmpB5A->tl=0,_tmpB5A))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB5B;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB5B=_region_malloc(r2,sizeof(*_tmpB5B)),((_tmpB5B->hd=f(x->hd,env),((_tmpB5B->tl=0,_tmpB5B))))));}
prev=prev->tl;}
# 2069
return result;}
# 2071
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2075
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpB5C;return(_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->f1=0,((_tmpB5C->f2=e,_tmpB5C)))));}
# 2079
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp440=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp442;_LL21C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp441=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp440;if(_tmp441->tag != 5)goto _LL21E;else{_tmp442=_tmp441->f1;}}_LL21D:
 return _tmp442;_LL21E:;_LL21F: {
const char*_tmpB5F;void*_tmpB5E;(_tmpB5E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5F="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB5F,sizeof(char),28))),_tag_dyneither(_tmpB5E,sizeof(void*),0)));}_LL21B:;}
# 2089
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp445=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp448;enum Cyc_Absyn_Sign _tmp44A;enum Cyc_Absyn_Sign _tmp44E;enum Cyc_Absyn_Sign _tmp450;enum Cyc_Absyn_Sign _tmp452;int _tmp456;_LL221: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445;if(_tmp446->tag != 5)goto _LL223;}_LL222:
 res=Cyc_Absyn_null_exp(0);goto _LL220;_LL223: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp445;if(_tmp447->tag != 6)goto _LL225;else{_tmp448=_tmp447->f1;if(_tmp447->f2 != Cyc_Absyn_Char_sz)goto _LL225;}}_LL224:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp448,'\000'),0);goto _LL220;_LL225: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp445;if(_tmp449->tag != 6)goto _LL227;else{_tmp44A=_tmp449->f1;if(_tmp449->f2 != Cyc_Absyn_Short_sz)goto _LL227;}}_LL226:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp44A,0),0);goto _LL220;_LL227: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp445;if(_tmp44B->tag != 13)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp445;if(_tmp44C->tag != 14)goto _LL22B;}_LL22A:
 _tmp44E=Cyc_Absyn_Unsigned;goto _LL22C;_LL22B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp445;if(_tmp44D->tag != 6)goto _LL22D;else{_tmp44E=_tmp44D->f1;if(_tmp44D->f2 != Cyc_Absyn_Int_sz)goto _LL22D;}}_LL22C:
 _tmp450=_tmp44E;goto _LL22E;_LL22D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp445;if(_tmp44F->tag != 6)goto _LL22F;else{_tmp450=_tmp44F->f1;if(_tmp44F->f2 != Cyc_Absyn_Long_sz)goto _LL22F;}}_LL22E:
# 2099
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp450,0),0);goto _LL220;_LL22F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp445;if(_tmp451->tag != 6)goto _LL231;else{_tmp452=_tmp451->f1;if(_tmp451->f2 != Cyc_Absyn_LongLong_sz)goto _LL231;}}_LL230:
# 2101
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp452,(long long)0),0);goto _LL220;_LL231: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp445;if(_tmp453->tag != 7)goto _LL233;else{if(_tmp453->f1 != 0)goto _LL233;}}_LL232:
{const char*_tmpB60;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB60="0.0F",_tag_dyneither(_tmpB60,sizeof(char),5))),0),0);}goto _LL220;_LL233: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp445;if(_tmp454->tag != 7)goto _LL235;else{if(_tmp454->f1 != 1)goto _LL235;}}_LL234:
{const char*_tmpB61;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB61="0.0",_tag_dyneither(_tmpB61,sizeof(char),4))),1),0);}goto _LL220;_LL235: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp445;if(_tmp455->tag != 7)goto _LL237;else{_tmp456=_tmp455->f1;}}_LL236:
{const char*_tmpB62;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB62="0.0L",_tag_dyneither(_tmpB62,sizeof(char),5))),_tmp456),0);}goto _LL220;_LL237:;_LL238: {
# 2106
const char*_tmpB66;void*_tmpB65[1];struct Cyc_String_pa_PrintArg_struct _tmpB64;(_tmpB64.tag=0,((_tmpB64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB65[0]=& _tmpB64,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB66,sizeof(char),40))),_tag_dyneither(_tmpB65,sizeof(void*),1)))))));}_LL220:;}
# 2108
res->topt=t;
return res;}
# 2115
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2128 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp45D=Cyc_Toc_typ_to_c(elt_type);
void*_tmp45E=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp45F=Cyc_Absyn_cstar_typ(_tmp45D,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB67;struct Cyc_Core_Opt*_tmp460=(_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67->v=_tmp45F,_tmpB67)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp461=e1->r;struct Cyc_Absyn_Exp*_tmp463;struct Cyc_Absyn_Exp*_tmp465;struct Cyc_Absyn_Exp*_tmp466;_LL23A: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp462=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp461;if(_tmp462->tag != 19)goto _LL23C;else{_tmp463=_tmp462->f1;}}_LL23B:
# 2136
 if(!is_dyneither){
_tmp463=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp463,0,Cyc_Absyn_Other_coercion,0);
_tmp463->topt=fat_ptr_type;}
# 2140
Cyc_Toc_exp_to_c(nv,_tmp463);xinit=_tmp463;goto _LL239;_LL23C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp464=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp461;if(_tmp464->tag != 22)goto _LL23E;else{_tmp465=_tmp464->f1;_tmp466=_tmp464->f2;}}_LL23D:
# 2142
 if(!is_dyneither){
_tmp465=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp465,0,Cyc_Absyn_Other_coercion,0);
_tmp465->topt=fat_ptr_type;}
# 2146
Cyc_Toc_exp_to_c(nv,_tmp465);Cyc_Toc_exp_to_c(nv,_tmp466);
{struct Cyc_Absyn_Exp*_tmpB68[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB68[2]=_tmp466,((_tmpB68[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB68[0]=_tmp465,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB68,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL239;_LL23E:;_LL23F: {
const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6B="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB6B,sizeof(char),53))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}_LL239:;}{
# 2152
struct _tuple0*_tmp46A=Cyc_Toc_temp_var();
struct _RegionHandle _tmp46B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp46B;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp46C=Cyc_Toc_add_varmap(rgn2,nv,_tmp46A,Cyc_Absyn_var_exp(_tmp46A,0));
struct Cyc_Absyn_Vardecl*_tmpB6C;struct Cyc_Absyn_Vardecl*_tmp46D=(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C->sc=Cyc_Absyn_Public,((_tmpB6C->name=_tmp46A,((_tmpB6C->tq=Cyc_Toc_mt_tq,((_tmpB6C->type=_tmp45E,((_tmpB6C->initializer=xinit,((_tmpB6C->rgn=0,((_tmpB6C->attributes=0,((_tmpB6C->escapes=0,_tmpB6C)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB6F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB6E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp46E=(_tmpB6E=_cycalloc(sizeof(*_tmpB6E)),((_tmpB6E[0]=((_tmpB6F.tag=4,((_tmpB6F.f1=_tmp46D,_tmpB6F)))),_tmpB6E)));
struct Cyc_Absyn_Exp*_tmp46F=Cyc_Absyn_varb_exp(_tmp46A,(void*)_tmp46E,0);
_tmp46F->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_deref_exp(_tmp46F,0);
_tmp470->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp46C,_tmp470);{
struct _tuple0*_tmp471=Cyc_Toc_temp_var();
_tmp46C=Cyc_Toc_add_varmap(rgn2,_tmp46C,_tmp471,Cyc_Absyn_var_exp(_tmp471,0));{
struct Cyc_Absyn_Vardecl*_tmpB70;struct Cyc_Absyn_Vardecl*_tmp472=(_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->sc=Cyc_Absyn_Public,((_tmpB70->name=_tmp471,((_tmpB70->tq=Cyc_Toc_mt_tq,((_tmpB70->type=_tmp45D,((_tmpB70->initializer=_tmp470,((_tmpB70->rgn=0,((_tmpB70->attributes=0,((_tmpB70->escapes=0,_tmpB70)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB73;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB72;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp473=(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=4,((_tmpB73.f1=_tmp472,_tmpB73)))),_tmpB72)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp474=Cyc_Absyn_varb_exp(_tmp471,(void*)_tmp473,0);
_tmp474->topt=_tmp470->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp474,e2,0);
z_init->topt=_tmp474->topt;}
# 2173
Cyc_Toc_exp_to_c(_tmp46C,z_init);{
struct _tuple0*_tmp475=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB74;struct Cyc_Absyn_Vardecl*_tmp476=(_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->sc=Cyc_Absyn_Public,((_tmpB74->name=_tmp475,((_tmpB74->tq=Cyc_Toc_mt_tq,((_tmpB74->type=_tmp45D,((_tmpB74->initializer=z_init,((_tmpB74->rgn=0,((_tmpB74->attributes=0,((_tmpB74->escapes=0,_tmpB74)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB77;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB76;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp477=(_tmpB76=_cycalloc(sizeof(*_tmpB76)),((_tmpB76[0]=((_tmpB77.tag=4,((_tmpB77.f1=_tmp476,_tmpB77)))),_tmpB76)));
_tmp46C=Cyc_Toc_add_varmap(rgn2,_tmp46C,_tmp475,Cyc_Absyn_var_exp(_tmp475,0));{
# 2180
struct Cyc_Absyn_Exp*_tmp478=Cyc_Absyn_varb_exp(_tmp471,(void*)_tmp473,0);_tmp478->topt=_tmp470->topt;{
struct Cyc_Absyn_Exp*_tmp479=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp47A=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp478,_tmp479,0);
_tmp47A->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp46C,_tmp47A);{
# 2186
struct Cyc_Absyn_Exp*_tmp47B=Cyc_Absyn_varb_exp(_tmp475,(void*)_tmp477,0);_tmp47B->topt=_tmp470->topt;{
struct Cyc_Absyn_Exp*_tmp47C=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp47D=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp47B,_tmp47C,0);
_tmp47D->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp46C,_tmp47D);{
# 2192
struct Cyc_Absyn_Exp*_tmpB78[2];struct Cyc_List_List*_tmp47E=(_tmpB78[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB78[0]=
# 2192
Cyc_Absyn_varb_exp(_tmp46A,(void*)_tmp46E,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB78,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2194
struct Cyc_Absyn_Exp*_tmp47F=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp47E,0),_tmp47F,0);{
# 2199
struct Cyc_Absyn_Exp*_tmp480=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp47A,_tmp47D,0),0);
struct Cyc_Absyn_Stmt*_tmp481=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp482=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp46A,(void*)_tmp46E,0),Cyc_Toc_curr_sp,0);
_tmp482=Cyc_Toc_cast_it(_tmp45F,_tmp482);{
struct Cyc_Absyn_Exp*_tmp483=Cyc_Absyn_deref_exp(_tmp482,0);
struct Cyc_Absyn_Exp*_tmp484=Cyc_Absyn_assign_exp(_tmp483,Cyc_Absyn_var_exp(_tmp475,0),0);
struct Cyc_Absyn_Stmt*_tmp485=Cyc_Absyn_exp_stmt(_tmp484,0);
_tmp485=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp480,_tmp481,Cyc_Absyn_skip_stmt(0),0),_tmp485,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB7E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB7D;struct Cyc_Absyn_Decl*_tmpB7C;_tmp485=Cyc_Absyn_decl_stmt(((_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->r=(void*)((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB7D.tag=0,((_tmpB7D.f1=_tmp476,_tmpB7D)))),_tmpB7E)))),((_tmpB7C->loc=0,_tmpB7C)))))),_tmp485,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB84;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB83;struct Cyc_Absyn_Decl*_tmpB82;_tmp485=Cyc_Absyn_decl_stmt(((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->r=(void*)((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB83.tag=0,((_tmpB83.f1=_tmp472,_tmpB83)))),_tmpB84)))),((_tmpB82->loc=0,_tmpB82)))))),_tmp485,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB8A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB89;struct Cyc_Absyn_Decl*_tmpB88;_tmp485=Cyc_Absyn_decl_stmt(((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->r=(void*)((_tmpB8A=_cycalloc(sizeof(*_tmpB8A)),((_tmpB8A[0]=((_tmpB89.tag=0,((_tmpB89.f1=_tmp46D,_tmpB89)))),_tmpB8A)))),((_tmpB88->loc=0,_tmpB88)))))),_tmp485,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp485);};};};};};};};};};};};}
# 2154
;_pop_region(rgn2);};}
# 2225 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2229
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp49A=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp49C;_LL241: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp49B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49A;if(_tmp49B->tag != 11)goto _LL243;else{_tmp49C=(_tmp49B->f1).aggr_info;}}_LL242:
# 2232
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp49C);goto _LL240;_LL243:;_LL244: {
struct Cyc_String_pa_PrintArg_struct _tmpB92;void*_tmpB91[1];const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB92.tag=0,((_tmpB92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB91[0]=& _tmpB92,Cyc_aprintf(((_tmpB90="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB90,sizeof(char),51))),_tag_dyneither(_tmpB91,sizeof(void*),1)))))))),_tag_dyneither(_tmpB8F,sizeof(void*),0)));}_LL240:;}{
# 2236
struct _tuple0*_tmp4A1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_var_exp(_tmp4A1,0);
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_aggrarrow_exp(_tmp4A2,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_neq_exp(_tmp4A4,_tmp4A3,0);
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Absyn_aggrarrow_exp(_tmp4A2,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4A7=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4A6,0),0);
struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_ifthenelse_stmt(_tmp4A5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4A9=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4AB=Cyc_Absyn_declare_stmt(_tmp4A1,_tmp4A9,_tmp4AA,Cyc_Absyn_seq_stmt(_tmp4A8,_tmp4A7,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4AB);}else{
# 2250
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Toc_member_exp(aggrproj(_tmp4A2,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Absyn_neq_exp(_tmp4AC,_tmp4A3,0);
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Toc_member_exp(aggrproj(_tmp4A2,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4AF=Cyc_Absyn_exp_stmt(_tmp4AE,0);
struct Cyc_Absyn_Stmt*_tmp4B0=Cyc_Absyn_ifthenelse_stmt(_tmp4AD,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4B1=Cyc_Absyn_declare_stmt(_tmp4A1,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4B0,_tmp4AF,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4B1);}};}
# 2263
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2266
void*_tmp4B2=e->r;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B6;struct _dyneither_ptr*_tmp4B7;int*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4BA;struct _dyneither_ptr*_tmp4BB;int*_tmp4BC;_LL246: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4B3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B2;if(_tmp4B3->tag != 13)goto _LL248;else{_tmp4B4=_tmp4B3->f2;}}_LL247: {
const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB95="cast on lhs!",_tag_dyneither(_tmpB95,sizeof(char),13))),_tag_dyneither(_tmpB94,sizeof(void*),0)));}_LL248: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4B5=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B2;if(_tmp4B5->tag != 20)goto _LL24A;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;_tmp4B8=(int*)& _tmp4B5->f4;}}_LL249: {
# 2269
void*_tmp4BF=Cyc_Tcutil_compress((void*)_check_null(_tmp4B6->topt));union Cyc_Absyn_AggrInfoU _tmp4C1;_LL24F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4BF;if(_tmp4C0->tag != 11)goto _LL251;else{_tmp4C1=(_tmp4C0->f1).aggr_info;}}_LL250: {
# 2271
struct Cyc_Absyn_Aggrdecl*_tmp4C2=Cyc_Absyn_get_known_aggrdecl(_tmp4C1);
*f_tag=Cyc_Toc_get_member_offset(_tmp4C2,_tmp4B7);{
# 2274
const char*_tmpB9A;void*_tmpB99[2];struct Cyc_String_pa_PrintArg_struct _tmpB98;struct Cyc_String_pa_PrintArg_struct _tmpB97;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B7),((_tmpB98.tag=0,((_tmpB98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4C2->name).f2),((_tmpB99[0]=& _tmpB98,((_tmpB99[1]=& _tmpB97,Cyc_aprintf(((_tmpB9A="_union_%s_%s",_tag_dyneither(_tmpB9A,sizeof(char),13))),_tag_dyneither(_tmpB99,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB9B;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=str,_tmpB9B)))));}
if(clear_read)*_tmp4B8=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C2->impl))->tagged;};}_LL251:;_LL252:
 return 0;_LL24E:;}_LL24A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4B9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B2;if(_tmp4B9->tag != 21)goto _LL24C;else{_tmp4BA=_tmp4B9->f1;_tmp4BB=_tmp4B9->f2;_tmp4BC=(int*)& _tmp4B9->f4;}}_LL24B: {
# 2281
void*_tmp4C8=Cyc_Tcutil_compress((void*)_check_null(_tmp4BA->topt));void*_tmp4CA;_LL254: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C8;if(_tmp4C9->tag != 5)goto _LL256;else{_tmp4CA=(_tmp4C9->f1).elt_typ;}}_LL255: {
# 2283
void*_tmp4CB=Cyc_Tcutil_compress(_tmp4CA);union Cyc_Absyn_AggrInfoU _tmp4CD;_LL259: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CB;if(_tmp4CC->tag != 11)goto _LL25B;else{_tmp4CD=(_tmp4CC->f1).aggr_info;}}_LL25A: {
# 2285
struct Cyc_Absyn_Aggrdecl*_tmp4CE=Cyc_Absyn_get_known_aggrdecl(_tmp4CD);
*f_tag=Cyc_Toc_get_member_offset(_tmp4CE,_tmp4BB);{
# 2288
const char*_tmpBA0;void*_tmpB9F[2];struct Cyc_String_pa_PrintArg_struct _tmpB9E;struct Cyc_String_pa_PrintArg_struct _tmpB9D;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB9D.tag=0,((_tmpB9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4BB),((_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4CE->name).f2),((_tmpB9F[0]=& _tmpB9E,((_tmpB9F[1]=& _tmpB9D,Cyc_aprintf(((_tmpBA0="_union_%s_%s",_tag_dyneither(_tmpBA0,sizeof(char),13))),_tag_dyneither(_tmpB9F,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpBA1;*tagged_member_type=Cyc_Absyn_strct(((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1[0]=str,_tmpBA1)))));}
if(clear_read)*_tmp4BC=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CE->impl))->tagged;};}_LL25B:;_LL25C:
 return 0;_LL258:;}_LL256:;_LL257:
# 2294
 return 0;_LL253:;}_LL24C:;_LL24D:
# 2296
 return 0;_LL245:;}
# 2308 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp4D4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4D4,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4D5,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4D4,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2332 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2336
struct _tuple0*_tmp4D6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4D6,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2346
struct Cyc_Absyn_Exp*_tmp4D7=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4D7,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2349
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4D6,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBF0(unsigned int*_tmpBEF,unsigned int*_tmpBEE,struct _tuple0***_tmpBEC){for(*_tmpBEF=0;*_tmpBEF < *_tmpBEE;(*_tmpBEF)++){(*_tmpBEC)[*_tmpBEF]=(struct _tuple0*)
# 2726 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2355 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4D8=e->r;
if(e->topt == 0){
const char*_tmpBA5;void*_tmpBA4[1];struct Cyc_String_pa_PrintArg_struct _tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBA4[0]=& _tmpBA3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA5="exp_to_c: no type for %s",_tag_dyneither(_tmpBA5,sizeof(char),25))),_tag_dyneither(_tmpBA4,sizeof(void*),1)))))));}
# 2361
if((nv->struct_info).lhs_exp != 0){
void*_tmp4DC=_tmp4D8;_LL25E: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4DD=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4DC;if(_tmp4DD->tag != 27)goto _LL260;}_LL25F:
 goto _LL25D;_LL260:;_LL261:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL25D:;}{
# 2369
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4DE=_tmp4D8;struct _tuple0*_tmp4E2;void*_tmp4E3;enum Cyc_Absyn_Primop _tmp4E5;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;enum Cyc_Absyn_Incrementor _tmp4E9;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Core_Opt*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_List_List*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_List_List*_tmp500;int _tmp501;struct Cyc_List_List*_tmp502;struct Cyc_Absyn_VarargInfo*_tmp503;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_List_List*_tmp50A;void**_tmp50C;struct Cyc_Absyn_Exp*_tmp50D;int _tmp50E;enum Cyc_Absyn_Coercion _tmp50F;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp516;void*_tmp518;void*_tmp51A;struct Cyc_List_List*_tmp51B;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51F;struct _dyneither_ptr*_tmp520;int _tmp521;int _tmp522;struct Cyc_Absyn_Exp*_tmp524;struct _dyneither_ptr*_tmp525;int _tmp526;int _tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_List_List*_tmp52C;struct Cyc_List_List*_tmp52E;struct Cyc_Absyn_Vardecl*_tmp530;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp532;int _tmp533;struct _tuple0*_tmp535;struct Cyc_List_List*_tmp536;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_Aggrdecl*_tmp538;void*_tmp53A;struct Cyc_List_List*_tmp53B;struct Cyc_List_List*_tmp53D;struct Cyc_Absyn_Datatypedecl*_tmp53E;struct Cyc_Absyn_Datatypefield*_tmp53F;int _tmp543;struct Cyc_Absyn_Exp*_tmp544;void**_tmp545;struct Cyc_Absyn_Exp*_tmp546;int _tmp547;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54C;struct _dyneither_ptr*_tmp54D;struct Cyc_Absyn_Stmt*_tmp54F;_LL263: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4DF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4DF->tag != 0)goto _LL265;else{if(((_tmp4DF->f1).Null_c).tag != 1)goto _LL265;}}_LL264: {
# 2376
struct Cyc_Absyn_Exp*_tmp554=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp554,_tmp554))->r;else{
# 2381
struct Cyc_Absyn_Exp*_tmpBA6[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpBA6[2]=_tmp554,((_tmpBA6[1]=_tmp554,((_tmpBA6[0]=_tmp554,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA6,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2383
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2385
goto _LL262;}_LL265: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4E0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4E0->tag != 0)goto _LL267;}_LL266:
 goto _LL262;_LL267: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4E1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4E1->tag != 1)goto _LL269;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=(void*)_tmp4E1->f2;}}_LL268:
# 2388
{struct _handler_cons _tmp556;_push_handler(& _tmp556);{int _tmp558=0;if(setjmp(_tmp556.handler))_tmp558=1;if(!_tmp558){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4E2))->r;;_pop_handler();}else{void*_tmp557=(void*)_exn_thrown;void*_tmp55A=_tmp557;void*_tmp55C;_LL2B6: {struct Cyc_Dict_Absent_exn_struct*_tmp55B=(struct Cyc_Dict_Absent_exn_struct*)_tmp55A;if(_tmp55B->tag != Cyc_Dict_Absent)goto _LL2B8;}_LL2B7: {
# 2390
const char*_tmpBAA;void*_tmpBA9[1];struct Cyc_String_pa_PrintArg_struct _tmpBA8;(_tmpBA8.tag=0,((_tmpBA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4E2)),((_tmpBA9[0]=& _tmpBA8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAA="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBAA,sizeof(char),32))),_tag_dyneither(_tmpBA9,sizeof(void*),1)))))));}_LL2B8: _tmp55C=_tmp55A;_LL2B9:(void)_throw(_tmp55C);_LL2B5:;}};}
# 2392
goto _LL262;_LL269: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4E4->tag != 2)goto _LL26B;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E4->f2;}}_LL26A: {
# 2395
struct Cyc_List_List*_tmp560=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4E6);
# 2397
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4E6);
switch(_tmp4E5){case Cyc_Absyn_Numelts: _LL2BA: {
# 2400
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;
{void*_tmp561=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp563;void*_tmp565;union Cyc_Absyn_Constraint*_tmp566;union Cyc_Absyn_Constraint*_tmp567;union Cyc_Absyn_Constraint*_tmp568;_LL2BD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp561;if(_tmp562->tag != 8)goto _LL2BF;else{_tmp563=(_tmp562->f1).num_elts;}}_LL2BE:
# 2404
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp563))){
const char*_tmpBAD;void*_tmpBAC;(_tmpBAC=0,Cyc_Tcutil_terr(e->loc,((_tmpBAD="can't calculate numelts",_tag_dyneither(_tmpBAD,sizeof(char),24))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}
e->r=_tmp563->r;goto _LL2BC;_LL2BF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp561;if(_tmp564->tag != 5)goto _LL2C1;else{_tmp565=(_tmp564->f1).elt_typ;_tmp566=((_tmp564->f1).ptr_atts).nullable;_tmp567=((_tmp564->f1).ptr_atts).bounds;_tmp568=((_tmp564->f1).ptr_atts).zero_term;}}_LL2C0:
# 2408
{void*_tmp56B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp567);struct Cyc_Absyn_Exp*_tmp56E;_LL2C4: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp56C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp56B;if(_tmp56C->tag != 0)goto _LL2C6;}_LL2C5:
# 2410
{struct Cyc_Absyn_Exp*_tmpBAE[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpBAE[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp565),0),((_tmpBAE[0]=(struct Cyc_Absyn_Exp*)_tmp4E6->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2C3;_LL2C6: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp56D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp56B;if(_tmp56D->tag != 1)goto _LL2C3;else{_tmp56E=_tmp56D->f1;}}_LL2C7:
# 2416
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp568)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4E6->hd);
# 2420
struct Cyc_Absyn_Exp*_tmpBAF[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpBAF[1]=_tmp56E,((_tmpBAF[0]=(struct Cyc_Absyn_Exp*)_tmp4E6->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp566)){
if(!Cyc_Evexp_c_can_eval(_tmp56E)){
const char*_tmpBB2;void*_tmpBB1;(_tmpBB1=0,Cyc_Tcutil_terr(e->loc,((_tmpBB2="can't calculate numelts",_tag_dyneither(_tmpBB2,sizeof(char),24))),_tag_dyneither(_tmpBB1,sizeof(void*),0)));}
# 2425
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp56E,Cyc_Absyn_uint_exp(0,0));}else{
# 2427
e->r=_tmp56E->r;goto _LL2C3;}}
# 2429
goto _LL2C3;_LL2C3:;}
# 2431
goto _LL2BC;_LL2C1:;_LL2C2: {
# 2433
const char*_tmpBB7;void*_tmpBB6[2];struct Cyc_String_pa_PrintArg_struct _tmpBB5;struct Cyc_String_pa_PrintArg_struct _tmpBB4;(_tmpBB4.tag=0,((_tmpBB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB5.tag=0,((_tmpBB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBB6[0]=& _tmpBB5,((_tmpBB6[1]=& _tmpBB4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB7="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBB7,sizeof(char),41))),_tag_dyneither(_tmpBB6,sizeof(void*),2)))))))))))));}_LL2BC:;}
# 2436
break;}case Cyc_Absyn_Plus: _LL2BB:
# 2441
{void*_tmp577=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp560))->hd);void*_tmp579;union Cyc_Absyn_Constraint*_tmp57A;union Cyc_Absyn_Constraint*_tmp57B;_LL2CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp577;if(_tmp578->tag != 5)goto _LL2CC;else{_tmp579=(_tmp578->f1).elt_typ;_tmp57A=((_tmp578->f1).ptr_atts).bounds;_tmp57B=((_tmp578->f1).ptr_atts).zero_term;}}_LL2CB:
# 2443
{void*_tmp57C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp57A);struct Cyc_Absyn_Exp*_tmp57F;_LL2CF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp57D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp57C;if(_tmp57D->tag != 0)goto _LL2D1;}_LL2D0: {
# 2445
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpBB8[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBB8[2]=e2,((_tmpBB8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp579),0),((_tmpBB8[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB8,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2CE;}_LL2D1: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp57E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp57C;if(_tmp57E->tag != 1)goto _LL2CE;else{_tmp57F=_tmp57E->f1;}}_LL2D2:
# 2451
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp57B)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6->tl))->hd;
struct Cyc_Absyn_Exp*_tmpBB9[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpBB9[2]=e2,((_tmpBB9[1]=_tmp57F,((_tmpBB9[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2456
goto _LL2CE;_LL2CE:;}
# 2458
goto _LL2C9;_LL2CC:;_LL2CD:
# 2460
 goto _LL2C9;_LL2C9:;}
# 2462
break;case Cyc_Absyn_Minus: _LL2C8: {
# 2467
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp560))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp560->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2477
struct Cyc_Absyn_Exp*_tmpBBA[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBBA[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpBBA[1]=
# 2478
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBBA[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBBA,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2482
break;}case Cyc_Absyn_Eq: _LL2D3:
 goto _LL2D4;case Cyc_Absyn_Neq: _LL2D4:
 goto _LL2D5;case Cyc_Absyn_Gt: _LL2D5:
 goto _LL2D6;case Cyc_Absyn_Gte: _LL2D6:
 goto _LL2D7;case Cyc_Absyn_Lt: _LL2D7:
 goto _LL2D8;case Cyc_Absyn_Lte: _LL2D8: {
# 2490
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E6->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp560))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp560->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2D9:
 break;}
# 2502
goto _LL262;}_LL26B: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4E7->tag != 4)goto _LL26D;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;}}_LL26C: {
# 2504
void*e2_cyc_typ=(void*)_check_null(_tmp4E8->topt);
# 2513 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBBB;struct _dyneither_ptr incr_str=(_tmpBBB="increment",_tag_dyneither(_tmpBBB,sizeof(char),10));
if(_tmp4E9 == Cyc_Absyn_PreDec  || _tmp4E9 == Cyc_Absyn_PostDec){const char*_tmpBBC;incr_str=((_tmpBBC="decrement",_tag_dyneither(_tmpBBC,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E8,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBC0;void*_tmpBBF[1];struct Cyc_String_pa_PrintArg_struct _tmpBBE;(_tmpBBE.tag=0,((_tmpBBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBBF[0]=& _tmpBBE,Cyc_Tcutil_terr(e->loc,((_tmpBC0="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBC0,sizeof(char),74))),_tag_dyneither(_tmpBBF,sizeof(void*),1)))))));}{
const char*_tmpBC3;void*_tmpBC2;(_tmpBC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC3="in-place inc/dec on zero-term",_tag_dyneither(_tmpBC3,sizeof(char),30))),_tag_dyneither(_tmpBC2,sizeof(void*),0)));};}{
# 2522
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4E8,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp589=Cyc_Absyn_signed_int_exp(1,0);
_tmp589->topt=Cyc_Absyn_sint_typ;
switch(_tmp4E9){case Cyc_Absyn_PreInc: _LL2DB:
# 2529
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBC9;struct Cyc_Core_Opt*_tmpBC8;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBC7;e->r=(void*)((_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7[0]=((_tmpBC9.tag=3,((_tmpBC9.f1=_tmp4E8,((_tmpBC9.f2=((_tmpBC8=_cycalloc_atomic(sizeof(*_tmpBC8)),((_tmpBC8->v=(void*)Cyc_Absyn_Plus,_tmpBC8)))),((_tmpBC9.f3=_tmp589,_tmpBC9)))))))),_tmpBC7))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2DC:
# 2533
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBCF;struct Cyc_Core_Opt*_tmpBCE;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBCD;e->r=(void*)((_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBCF.tag=3,((_tmpBCF.f1=_tmp4E8,((_tmpBCF.f2=((_tmpBCE=_cycalloc_atomic(sizeof(*_tmpBCE)),((_tmpBCE->v=(void*)Cyc_Absyn_Minus,_tmpBCE)))),((_tmpBCF.f3=_tmp589,_tmpBCF)))))))),_tmpBCD))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2DD:
# 2537
{const char*_tmpBD3;void*_tmpBD2[1];struct Cyc_String_pa_PrintArg_struct _tmpBD1;(_tmpBD1.tag=0,((_tmpBD1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD2[0]=& _tmpBD1,Cyc_Tcutil_terr(e->loc,((_tmpBD3="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBD3,sizeof(char),59))),_tag_dyneither(_tmpBD2,sizeof(void*),1)))))));}{
# 2539
const char*_tmpBD6;void*_tmpBD5;(_tmpBD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD6="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBD6,sizeof(char),34))),_tag_dyneither(_tmpBD5,sizeof(void*),0)));};}}
# 2542
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E8);
Cyc_Toc_set_lhs(nv,0);{
# 2547
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4E9 == Cyc_Absyn_PostInc  || _tmp4E9 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2553
if(_tmp4E9 == Cyc_Absyn_PreDec  || _tmp4E9 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpBD7[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD7[2]=
# 2557
Cyc_Absyn_signed_int_exp(change,0),((_tmpBD7[1]=
# 2556
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBD7[0]=
# 2555
Cyc_Toc_push_address_exp(_tmp4E8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2558
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2562
struct Cyc_Toc_functionSet*_tmp596=_tmp4E9 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2564
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp596,_tmp4E8);
struct Cyc_Absyn_Exp*_tmpBD8[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD8[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBD8[0]=
# 2565
Cyc_Toc_push_address_exp(_tmp4E8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2567
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4E8)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4E8,0,Cyc_Toc_incr_lvalue,_tmp4E9);
e->r=_tmp4E8->r;}}}
# 2571
goto _LL262;};};}_LL26D: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4EA=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4EA->tag != 3)goto _LL26F;else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;}}_LL26E: {
# 2590 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4EB->topt);
void*e2_old_typ=(void*)_check_null(_tmp4ED->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4EB,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EB);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4ED);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4EB,e1_old_typ,_tmp4EC,_tmp4ED,e2_old_typ,f_tag,tagged_member_struct_type);
# 2601
return;}{
# 2603
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4EB,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4EB,_tmp4EC,_tmp4ED,ptr_type,is_dyneither,elt_type);
# 2609
return;}{
# 2613
int e1_poly=Cyc_Toc_is_poly_project(_tmp4EB);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4EB);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4ED);{
# 2619
int done=0;
if(_tmp4EC != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp4EC->v){case Cyc_Absyn_Plus: _LL2DF:
 change=_tmp4ED;break;case Cyc_Absyn_Minus: _LL2E0:
# 2627
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4ED,0);break;default: _LL2E1: {
const char*_tmpBDB;void*_tmpBDA;(_tmpBDA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDB="bad t ? pointer arithmetic",_tag_dyneither(_tmpBDB,sizeof(char),27))),_tag_dyneither(_tmpBDA,sizeof(void*),0)));}}
# 2630
done=1;{
# 2632
struct Cyc_Absyn_Exp*_tmp59B=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBDC[3];e->r=Cyc_Toc_fncall_exp_r(_tmp59B,((_tmpBDC[2]=change,((_tmpBDC[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBDC[0]=
# 2633
Cyc_Toc_push_address_exp(_tmp4EB),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2636
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2639
switch((enum Cyc_Absyn_Primop)_tmp4EC->v){case Cyc_Absyn_Plus: _LL2E3:
# 2641
 done=1;
{struct Cyc_Absyn_Exp*_tmpBDD[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4EB),((_tmpBDD[1]=_tmp4ED,((_tmpBDD[0]=_tmp4EB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2E4: {
const char*_tmpBE0;void*_tmpBDF;(_tmpBDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE0="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBE0,sizeof(char),39))),_tag_dyneither(_tmpBDF,sizeof(void*),0)));}}}}
# 2648
if(!done){
# 2650
if(e1_poly)
_tmp4ED->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4ED->r,0));
# 2656
if(!Cyc_Absyn_is_lvalue(_tmp4EB)){
{struct _tuple20 _tmpBE3;struct _tuple20*_tmpBE2;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp4EB,0,Cyc_Toc_assignop_lvalue,((_tmpBE2=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpBE2[0]=(struct _tuple20)((_tmpBE3.f1=_tmp4EC,((_tmpBE3.f2=_tmp4ED,_tmpBE3)))),_tmpBE2)))));}
e->r=_tmp4EB->r;}}
# 2661
goto _LL262;};};};}_LL26F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4EE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4EE->tag != 5)goto _LL271;else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;_tmp4F1=_tmp4EE->f3;}}_LL270:
# 2663
 Cyc_Toc_exp_to_c(nv,_tmp4EF);
Cyc_Toc_exp_to_c(nv,_tmp4F0);
Cyc_Toc_exp_to_c(nv,_tmp4F1);
goto _LL262;_LL271: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4F2=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4F2->tag != 6)goto _LL273;else{_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;}}_LL272:
# 2668
 Cyc_Toc_exp_to_c(nv,_tmp4F3);
Cyc_Toc_exp_to_c(nv,_tmp4F4);
goto _LL262;_LL273: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4F5=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4F5->tag != 7)goto _LL275;else{_tmp4F6=_tmp4F5->f1;_tmp4F7=_tmp4F5->f2;}}_LL274:
# 2672
 Cyc_Toc_exp_to_c(nv,_tmp4F6);
Cyc_Toc_exp_to_c(nv,_tmp4F7);
goto _LL262;_LL275: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4F8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4F8->tag != 8)goto _LL277;else{_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F8->f2;}}_LL276:
# 2676
 Cyc_Toc_exp_to_c(nv,_tmp4F9);
Cyc_Toc_exp_to_c(nv,_tmp4FA);
goto _LL262;_LL277: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4FB=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4FB->tag != 9)goto _LL279;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=_tmp4FB->f2;if(_tmp4FB->f3 != 0)goto _LL279;}}_LL278:
# 2680
 Cyc_Toc_exp_to_c(nv,_tmp4FC);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4FD);
goto _LL262;_LL279: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4FE=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp4FE->tag != 9)goto _LL27B;else{_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;if(_tmp4FE->f3 == 0)goto _LL27B;_tmp501=(_tmp4FE->f3)->num_varargs;_tmp502=(_tmp4FE->f3)->injectors;_tmp503=(_tmp4FE->f3)->vai;}}_LL27A: {
# 2692 "toc.cyc"
struct _RegionHandle _tmp5A2=_new_region("r");struct _RegionHandle*r=& _tmp5A2;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp501,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp503->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2700
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp500);
int num_normargs=num_args - _tmp501;
# 2704
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp500=_tmp500->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd);{
struct Cyc_List_List*_tmpBE4;new_args=((_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4->hd=(struct Cyc_Absyn_Exp*)_tmp500->hd,((_tmpBE4->tl=new_args,_tmpBE4))))));};}}
# 2709
{struct Cyc_Absyn_Exp*_tmpBE7[3];struct Cyc_List_List*_tmpBE6;new_args=((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBE7[2]=num_varargs_exp,((_tmpBE7[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBE7[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBE6->tl=new_args,_tmpBE6))))));}
# 2714
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2716
Cyc_Toc_exp_to_c(nv,_tmp4FF);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4FF,new_args,0),0);
# 2720
if(_tmp503->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5A6=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp503->type));struct Cyc_Absyn_Datatypedecl*_tmp5A8;_LL2E7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5A6;if(_tmp5A7->tag != 3)goto _LL2E9;else{if((((_tmp5A7->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2E9;_tmp5A8=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5A7->f1).datatype_info).KnownDatatype).val);}}_LL2E8:
 tud=_tmp5A8;goto _LL2E6;_LL2E9:;_LL2EA: {
const char*_tmpBEA;void*_tmpBE9;(_tmpBE9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEA="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBEA,sizeof(char),44))),_tag_dyneither(_tmpBE9,sizeof(void*),0)));}_LL2E6:;}{
# 2726
unsigned int _tmpBEF;unsigned int _tmpBEE;struct _dyneither_ptr _tmpBED;struct _tuple0**_tmpBEC;unsigned int _tmpBEB;struct _dyneither_ptr vs=(_tmpBEB=(unsigned int)_tmp501,((_tmpBEC=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpBEB)),((_tmpBED=_tag_dyneither(_tmpBEC,sizeof(struct _tuple0*),_tmpBEB),((((_tmpBEE=_tmpBEB,_tmpBF0(& _tmpBEF,& _tmpBEE,& _tmpBEC))),_tmpBED)))))));
# 2728
if(_tmp501 != 0){
# 2730
struct Cyc_List_List*_tmp5AB=0;
{int i=_tmp501 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBF1;_tmp5AB=((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpBF1->tl=_tmp5AB,_tmpBF1))))));}}
# 2734
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5AB,0),s,0);{
# 2737
int i=0;for(0;_tmp500 != 0;(((_tmp500=_tmp500->tl,_tmp502=_tmp502->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp500->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct Cyc_Absyn_Datatypefield*_tmp5AD=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp502))->hd;_tmp5AE=_tmp5AD->name;_tmp5AF=_tmp5AD->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp5AF))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2752
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2755
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5AE),0),s,0);
# 2758
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5AE,tud->name)),0,s,0);};}};}else{
# 2765
struct _tuple16*_tmpBF2[3];struct Cyc_List_List*_tmp5B0=(_tmpBF2[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF2[1]=
# 2765
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBF2[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF2,sizeof(struct _tuple16*),3)))))));
# 2767
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2777
{int i=0;for(0;_tmp500 != 0;(_tmp500=_tmp500->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp500->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp500->hd,0),s,0);}}
# 2783
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2796 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2798
_npop_handler(0);goto _LL262;
# 2692 "toc.cyc"
;_pop_region(r);}_LL27B: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp504=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp504->tag != 10)goto _LL27D;else{_tmp505=_tmp504->f1;}}_LL27C:
# 2801 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp505);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp505),0))->r;
goto _LL262;_LL27D: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp506=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp506->tag != 11)goto _LL27F;else{_tmp507=_tmp506->f1;}}_LL27E:
 Cyc_Toc_exp_to_c(nv,_tmp507);goto _LL262;_LL27F: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp508->tag != 12)goto _LL281;else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;}}_LL280:
# 2806
 Cyc_Toc_exp_to_c(nv,_tmp509);
# 2815 "toc.cyc"
for(0;_tmp50A != 0;_tmp50A=_tmp50A->tl){
enum Cyc_Absyn_KindQual _tmp5B6=(Cyc_Tcutil_typ_kind((void*)_tmp50A->hd))->kind;
if(_tmp5B6 != Cyc_Absyn_EffKind  && _tmp5B6 != Cyc_Absyn_RgnKind){
{void*_tmp5B7=Cyc_Tcutil_compress((void*)_tmp50A->hd);_LL2EC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B7;if(_tmp5B8->tag != 2)goto _LL2EE;}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5B7;if(_tmp5B9->tag != 3)goto _LL2F0;}_LL2EF:
 continue;_LL2F0:;_LL2F1:
# 2822
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp509,0))->r;
goto _LL2EB;_LL2EB:;}
# 2825
break;}}
# 2828
goto _LL262;_LL281: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp50B->tag != 13)goto _LL283;else{_tmp50C=(void**)((void**)& _tmp50B->f1);_tmp50D=_tmp50B->f2;_tmp50E=_tmp50B->f3;_tmp50F=_tmp50B->f4;}}_LL282: {
# 2830
void*old_t2=(void*)_check_null(_tmp50D->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp50C;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp50C=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp50D);
# 2837
{struct _tuple19 _tmpBF3;struct _tuple19 _tmp5BB=(_tmpBF3.f1=Cyc_Tcutil_compress(old_t2),((_tmpBF3.f2=Cyc_Tcutil_compress(new_typ),_tmpBF3)));struct Cyc_Absyn_PtrInfo _tmp5BD;struct Cyc_Absyn_PtrInfo _tmp5BF;struct Cyc_Absyn_PtrInfo _tmp5C1;_LL2F3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BB.f1;if(_tmp5BC->tag != 5)goto _LL2F5;else{_tmp5BD=_tmp5BC->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5BE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BB.f2;if(_tmp5BE->tag != 5)goto _LL2F5;else{_tmp5BF=_tmp5BE->f1;}};_LL2F4: {
# 2839
int _tmp5C3=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5BD.ptr_atts).nullable);
int _tmp5C4=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5BF.ptr_atts).nullable);
void*_tmp5C5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5BD.ptr_atts).bounds);
void*_tmp5C6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5BF.ptr_atts).bounds);
int _tmp5C7=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5BD.ptr_atts).zero_term);
int _tmp5C8=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5BF.ptr_atts).zero_term);
{struct _tuple19 _tmpBF4;struct _tuple19 _tmp5CA=(_tmpBF4.f1=_tmp5C5,((_tmpBF4.f2=_tmp5C6,_tmpBF4)));struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CE;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D4;_LL2FA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5CB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CA.f1;if(_tmp5CB->tag != 1)goto _LL2FC;else{_tmp5CC=_tmp5CB->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5CD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CA.f2;if(_tmp5CD->tag != 1)goto _LL2FC;else{_tmp5CE=_tmp5CD->f1;}};_LL2FB:
# 2847
 if((!Cyc_Evexp_c_can_eval(_tmp5CC) || !Cyc_Evexp_c_can_eval(_tmp5CE)) && !
Cyc_Evexp_same_const_exp(_tmp5CC,_tmp5CE)){
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,Cyc_Tcutil_terr(e->loc,((_tmpBF7="can't validate cast due to potential size differences",_tag_dyneither(_tmpBF7,sizeof(char),54))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}
if(_tmp5C3  && !_tmp5C4){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBFA;void*_tmpBF9;(_tmpBF9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBFA="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBFA,sizeof(char),44))),_tag_dyneither(_tmpBF9,sizeof(void*),0)));}
# 2856
if(_tmp50F != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBFE;void*_tmpBFD[1];struct Cyc_String_pa_PrintArg_struct _tmpBFC;(_tmpBFC.tag=0,((_tmpBFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBFD[0]=& _tmpBFC,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBFE="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBFE,sizeof(char),41))),_tag_dyneither(_tmpBFD,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp50D);
if(do_null_check){
if(!_tmp50E){
const char*_tmpC01;void*_tmpC00;(_tmpC00=0,Cyc_Tcutil_warn(e->loc,((_tmpC01="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC01,sizeof(char),58))),_tag_dyneither(_tmpC00,sizeof(void*),0)));}{
# 2863
struct Cyc_List_List*_tmpC02;e->r=Cyc_Toc_cast_it_r(*_tmp50C,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC02=_cycalloc(sizeof(*_tmpC02)),((_tmpC02->hd=_tmp50D,((_tmpC02->tl=0,_tmpC02)))))),0));};}else{
# 2866
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp50D->r;}};}else{
# 2871
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp50D->r;}
# 2878
goto _LL2F9;_LL2FC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5CF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CA.f1;if(_tmp5CF->tag != 1)goto _LL2FE;else{_tmp5D0=_tmp5CF->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5D1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5CA.f2;if(_tmp5D1->tag != 0)goto _LL2FE;};_LL2FD:
# 2880
 if(!Cyc_Evexp_c_can_eval(_tmp5D0)){
const char*_tmpC05;void*_tmpC04;(_tmpC04=0,Cyc_Tcutil_terr(e->loc,((_tmpC05="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC05,sizeof(char),71))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}
# 2883
if(_tmp50F == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC09;void*_tmpC08[1];struct Cyc_String_pa_PrintArg_struct _tmpC07;(_tmpC07.tag=0,((_tmpC07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC08[0]=& _tmpC07,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC09="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC09,sizeof(char),44))),_tag_dyneither(_tmpC08,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2887
if((_tmp5C7  && !(_tmp5BF.elt_tq).real_const) && !_tmp5C8)
# 2890
_tmp5D0=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5D0,Cyc_Absyn_uint_exp(1,0),0);
# 2892
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5D0,_tmp50D))->r;}else{
# 2894
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Toc__tag_dyneither_e;
# 2896
if(_tmp5C7){
# 2901
struct _tuple0*_tmp5E7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5E8=Cyc_Absyn_var_exp(_tmp5E7,0);
struct Cyc_Absyn_Exp*arg3;
# 2906
{void*_tmp5E9=_tmp50D->r;_LL303: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5EA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EA->tag != 0)goto _LL305;else{if(((_tmp5EA->f1).String_c).tag != 8)goto _LL305;}}_LL304:
# 2908
 arg3=_tmp5D0;goto _LL302;_LL305: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EB->tag != 0)goto _LL307;else{if(((_tmp5EB->f1).Wstring_c).tag != 9)goto _LL307;}}_LL306:
# 2910
 arg3=_tmp5D0;goto _LL302;_LL307:;_LL308:
# 2912
{struct Cyc_Absyn_Exp*_tmpC0A[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp50D),(
# 2914
(_tmpC0A[1]=_tmp5D0,((_tmpC0A[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5E8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL302;_LL302:;}
# 2917
if(!_tmp5C8  && !(_tmp5BF.elt_tq).real_const)
# 2920
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2922
struct Cyc_Absyn_Exp*_tmp5ED=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BF.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC0B[3];struct Cyc_Absyn_Exp*_tmp5EE=Cyc_Absyn_fncall_exp(_tmp5E6,((_tmpC0B[2]=arg3,((_tmpC0B[1]=_tmp5ED,((_tmpC0B[0]=_tmp5E8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5EF=Cyc_Absyn_exp_stmt(_tmp5EE,0);
_tmp5EF=Cyc_Absyn_declare_stmt(_tmp5E7,Cyc_Toc_typ_to_c(old_t2),_tmp50D,_tmp5EF,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5EF);};}else{
# 2929
struct Cyc_Absyn_Exp*_tmpC0C[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5E6,(
(_tmpC0C[2]=_tmp5D0,((_tmpC0C[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BF.elt_typ),0),((_tmpC0C[0]=_tmp50D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2935
goto _LL2F9;_LL2FE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5D2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5CA.f1;if(_tmp5D2->tag != 0)goto _LL300;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5D3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5CA.f2;if(_tmp5D3->tag != 1)goto _LL300;else{_tmp5D4=_tmp5D3->f1;}};_LL2FF:
# 2937
 if(!Cyc_Evexp_c_can_eval(_tmp5D4)){
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,Cyc_Tcutil_terr(e->loc,((_tmpC0F="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC0F,sizeof(char),71))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC12;void*_tmpC11;(_tmpC11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC12="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC12,sizeof(char),45))),_tag_dyneither(_tmpC11,sizeof(void*),0)));}{
# 2949 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5F6=_tmp5D4;
if(_tmp5C7  && !_tmp5C8)
_tmp5F6=Cyc_Absyn_add_exp(_tmp5D4,Cyc_Absyn_uint_exp(1,0),0);{
# 2956
struct Cyc_Absyn_Exp*_tmp5F7=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC13[3];struct Cyc_Absyn_Exp*_tmp5F8=Cyc_Absyn_fncall_exp(_tmp5F7,(
(_tmpC13[2]=_tmp5F6,((_tmpC13[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BD.elt_typ),0),((_tmpC13[0]=_tmp50D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC13,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2961
if(_tmp5C4){
struct Cyc_List_List*_tmpC14;_tmp5F8->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC14=_cycalloc(sizeof(*_tmpC14)),((_tmpC14->hd=Cyc_Absyn_copy_exp(_tmp5F8),((_tmpC14->tl=0,_tmpC14)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp50C,_tmp5F8);
goto _LL2F9;};};_LL300:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5D5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5CA.f1;if(_tmp5D5->tag != 0)goto _LL2F9;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5D6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5CA.f2;if(_tmp5D6->tag != 0)goto _LL2F9;};_LL301:
# 2969
 DynCast:
 if((_tmp5C7  && !_tmp5C8) && !(_tmp5BF.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC17;void*_tmpC16;(_tmpC16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC17="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC17,sizeof(char),70))),_tag_dyneither(_tmpC16,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC18[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5FD,(
(_tmpC18[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC18[1]=
# 2975
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BD.elt_typ),0),((_tmpC18[0]=_tmp50D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2978
goto _LL2F9;_LL2F9:;}
# 2980
goto _LL2F2;}_LL2F5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BB.f1;if(_tmp5C0->tag != 5)goto _LL2F7;else{_tmp5C1=_tmp5C0->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BB.f2;if(_tmp5C2->tag != 6)goto _LL2F7;};_LL2F6:
# 2982
{void*_tmp5FF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5C1.ptr_atts).bounds);_LL30A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp600=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5FF;if(_tmp600->tag != 0)goto _LL30C;}_LL30B:
# 2984
 _tmp50D->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp50D->r,_tmp50D->loc),Cyc_Toc_curr_sp);goto _LL309;_LL30C:;_LL30D:
 goto _LL309;_LL309:;}
# 2987
goto _LL2F2;_LL2F7:;_LL2F8:
# 2989
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp50D->r;
goto _LL2F2;_LL2F2:;}
# 2993
goto _LL262;}_LL283: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp510->tag != 14)goto _LL285;else{_tmp511=_tmp510->f1;}}_LL284:
# 2997
{void*_tmp601=_tmp511->r;struct _tuple0*_tmp603;struct Cyc_List_List*_tmp604;struct Cyc_List_List*_tmp605;struct Cyc_List_List*_tmp607;_LL30F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp602=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp601;if(_tmp602->tag != 27)goto _LL311;else{_tmp603=_tmp602->f1;_tmp604=_tmp602->f2;_tmp605=_tmp602->f3;}}_LL310:
# 2999
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC1C;void*_tmpC1B[1];struct Cyc_String_pa_PrintArg_struct _tmpC1A;(_tmpC1A.tag=0,((_tmpC1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp511->loc)),((_tmpC1B[0]=& _tmpC1A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1C="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC1C,sizeof(char),42))),_tag_dyneither(_tmpC1B,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp511->topt),_tmp604,1,0,_tmp605,_tmp603))->r;
goto _LL30E;_LL311: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp606=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp601;if(_tmp606->tag != 23)goto _LL313;else{_tmp607=_tmp606->f1;}}_LL312:
# 3005
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC20;void*_tmpC1F[1];struct Cyc_String_pa_PrintArg_struct _tmpC1E;(_tmpC1E.tag=0,((_tmpC1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp511->loc)),((_tmpC1F[0]=& _tmpC1E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC20="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC20,sizeof(char),42))),_tag_dyneither(_tmpC1F,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp607))->r;
goto _LL30E;_LL313:;_LL314:
# 3011
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp511);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp511)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp511,0,Cyc_Toc_address_lvalue,1);
# 3017
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp511);}
# 3019
goto _LL30E;_LL30E:;}
# 3021
goto _LL262;_LL285: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp512->tag != 15)goto _LL287;else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;}}_LL286:
# 3024
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC24;void*_tmpC23[1];struct Cyc_String_pa_PrintArg_struct _tmpC22;(_tmpC22.tag=0,((_tmpC22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp514->loc)),((_tmpC23[0]=& _tmpC22,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC24="%s: new at top-level",_tag_dyneither(_tmpC24,sizeof(char),21))),_tag_dyneither(_tmpC23,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp514->topt);
{void*_tmp611=_tmp514->r;struct Cyc_List_List*_tmp613;struct Cyc_Absyn_Vardecl*_tmp615;struct Cyc_Absyn_Exp*_tmp616;struct Cyc_Absyn_Exp*_tmp617;int _tmp618;struct _tuple0*_tmp61A;struct Cyc_List_List*_tmp61B;struct Cyc_List_List*_tmp61C;struct Cyc_Absyn_Aggrdecl*_tmp61D;struct Cyc_List_List*_tmp61F;_LL316: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp612->tag != 25)goto _LL318;else{_tmp613=_tmp612->f1;}}_LL317: {
# 3032
struct _tuple0*_tmp620=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp621=Cyc_Absyn_var_exp(_tmp620,0);
struct Cyc_Absyn_Stmt*_tmp622=Cyc_Toc_init_array(nv,new_e_type,_tmp621,_tmp613,Cyc_Absyn_exp_stmt(_tmp621,0));
void*old_elt_typ;
{void*_tmp623=Cyc_Tcutil_compress(old_typ);void*_tmp625;struct Cyc_Absyn_Tqual _tmp626;union Cyc_Absyn_Constraint*_tmp627;_LL321: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp624=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp623;if(_tmp624->tag != 5)goto _LL323;else{_tmp625=(_tmp624->f1).elt_typ;_tmp626=(_tmp624->f1).elt_tq;_tmp627=((_tmp624->f1).ptr_atts).zero_term;}}_LL322:
# 3038
 old_elt_typ=_tmp625;goto _LL320;_LL323:;_LL324: {
# 3040
const char*_tmpC27;void*_tmpC26;old_elt_typ=(
(_tmpC26=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC27="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC27,sizeof(char),52))),_tag_dyneither(_tmpC26,sizeof(void*),0))));}_LL320:;}{
# 3043
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp62A=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp62B=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp613),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp513 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp62B);else{
# 3051
struct Cyc_Absyn_Exp*r=_tmp513;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp62B);}
# 3056
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp620,_tmp62A,e1,_tmp622,0));
goto _LL315;};}_LL318: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp614=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp614->tag != 26)goto _LL31A;else{_tmp615=_tmp614->f1;_tmp616=_tmp614->f2;_tmp617=_tmp614->f3;_tmp618=_tmp614->f4;}}_LL319: {
# 3069 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp62C=Cyc_Tcutil_compress(old_typ);void*_tmp62E;struct Cyc_Absyn_Tqual _tmp62F;union Cyc_Absyn_Constraint*_tmp630;union Cyc_Absyn_Constraint*_tmp631;_LL326: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp62D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp62C;if(_tmp62D->tag != 5)goto _LL328;else{_tmp62E=(_tmp62D->f1).elt_typ;_tmp62F=(_tmp62D->f1).elt_tq;_tmp630=((_tmp62D->f1).ptr_atts).bounds;_tmp631=((_tmp62D->f1).ptr_atts).zero_term;}}_LL327:
# 3072
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp630)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL325;_LL328:;_LL329: {
const char*_tmpC2A;void*_tmpC29;(_tmpC29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2A="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC2A,sizeof(char),42))),_tag_dyneither(_tmpC29,sizeof(void*),0)));}_LL325:;}{
# 3076
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp617->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp616);{
struct Cyc_Absyn_Exp*_tmp634=Cyc_Absyn_var_exp(max,0);
# 3084
if(_tmp618)
_tmp634=Cyc_Absyn_add_exp(_tmp634,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),_tmp615,Cyc_Absyn_var_exp(max,0),_tmp617,_tmp618,
Cyc_Toc_skip_stmt_dl(),1);
# 3090
struct _RegionHandle _tmp635=_new_region("r");struct _RegionHandle*r=& _tmp635;_push_region(r);{
struct _tuple21*_tmpC2D;struct Cyc_List_List*_tmpC2C;struct Cyc_List_List*decls=
(_tmpC2C=_region_malloc(r,sizeof(*_tmpC2C)),((_tmpC2C->hd=((_tmpC2D=_region_malloc(r,sizeof(*_tmpC2D)),((_tmpC2D->f1=max,((_tmpC2D->f2=Cyc_Absyn_uint_typ,((_tmpC2D->f3=_tmp616,_tmpC2D)))))))),((_tmpC2C->tl=0,_tmpC2C)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp513 == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC2E[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC2E[1]=_tmp634,((_tmpC2E[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3100
struct Cyc_Absyn_Exp*r=_tmp513;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC2F[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC2F[1]=_tmp634,((_tmpC2F[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3107
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpC32;struct Cyc_List_List*_tmpC31;decls=((_tmpC31=_region_malloc(r,sizeof(*_tmpC31)),((_tmpC31->hd=((_tmpC32=_region_malloc(r,sizeof(*_tmpC32)),((_tmpC32->f1=a,((_tmpC32->f2=ptr_typ,((_tmpC32->f3=ainit,_tmpC32)))))))),((_tmpC31->tl=decls,_tmpC31))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp63A=Cyc_Toc_temp_var();
void*_tmp63B=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp63C=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC33[3];struct Cyc_Absyn_Exp*_tmp63D=
Cyc_Absyn_fncall_exp(_tmp63C,(
(_tmpC33[2]=_tmp634,((_tmpC33[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC33[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC33,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3117
{struct _tuple21*_tmpC36;struct Cyc_List_List*_tmpC35;decls=((_tmpC35=_region_malloc(r,sizeof(*_tmpC35)),((_tmpC35->hd=((_tmpC36=_region_malloc(r,sizeof(*_tmpC36)),((_tmpC36->f1=_tmp63A,((_tmpC36->f2=_tmp63B,((_tmpC36->f3=_tmp63D,_tmpC36)))))))),((_tmpC35->tl=decls,_tmpC35))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp63A,0),0),0);}else{
# 3120
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp641=decls;for(0;_tmp641 != 0;_tmp641=_tmp641->tl){
struct _tuple0*_tmp643;void*_tmp644;struct Cyc_Absyn_Exp*_tmp645;struct _tuple21 _tmp642=*((struct _tuple21*)_tmp641->hd);_tmp643=_tmp642.f1;_tmp644=_tmp642.f2;_tmp645=_tmp642.f3;
s=Cyc_Absyn_declare_stmt(_tmp643,_tmp644,_tmp645,s,0);}}
# 3125
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3127
_npop_handler(0);goto _LL315;
# 3090
;_pop_region(r);};};};}_LL31A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp619=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp619->tag != 27)goto _LL31C;else{_tmp61A=_tmp619->f1;_tmp61B=_tmp619->f2;_tmp61C=_tmp619->f3;_tmp61D=_tmp619->f4;}}_LL31B:
# 3130
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp514->topt),_tmp61B,1,_tmp513,_tmp61C,_tmp61A))->r;
goto _LL315;_LL31C: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp61E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp61E->tag != 23)goto _LL31E;else{_tmp61F=_tmp61E->f1;}}_LL31D:
# 3134
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp513,_tmp61F))->r;
goto _LL315;_LL31E:;_LL31F: {
# 3141
void*old_elt_typ=(void*)_check_null(_tmp514->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3144
struct _tuple0*_tmp648=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp649=Cyc_Absyn_var_exp(_tmp648,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp649,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp513 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3151
struct Cyc_Absyn_Exp*r=_tmp513;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3159
int done=0;
{void*_tmp64A=_tmp514->r;void*_tmp64C;struct Cyc_Absyn_Exp*_tmp64D;_LL32B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp64A;if(_tmp64B->tag != 13)goto _LL32D;else{_tmp64C=(void*)_tmp64B->f1;_tmp64D=_tmp64B->f2;}}_LL32C:
# 3162
{struct _tuple19 _tmpC37;struct _tuple19 _tmp64F=(_tmpC37.f1=Cyc_Tcutil_compress(_tmp64C),((_tmpC37.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp64D->topt)),_tmpC37)));void*_tmp651;union Cyc_Absyn_Constraint*_tmp652;union Cyc_Absyn_Constraint*_tmp654;_LL330:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp650=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64F.f1;if(_tmp650->tag != 5)goto _LL332;else{_tmp651=(_tmp650->f1).elt_typ;_tmp652=((_tmp650->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp653=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64F.f2;if(_tmp653->tag != 5)goto _LL332;else{_tmp654=((_tmp653->f1).ptr_atts).bounds;}};_LL331:
# 3165
{struct _tuple19 _tmpC38;struct _tuple19 _tmp656=(_tmpC38.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp652),((_tmpC38.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp654),_tmpC38)));struct Cyc_Absyn_Exp*_tmp659;_LL335:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp657=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp656.f1;if(_tmp657->tag != 0)goto _LL337;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp658=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp656.f2;if(_tmp658->tag != 1)goto _LL337;else{_tmp659=_tmp658->f1;}};_LL336:
# 3167
 Cyc_Toc_exp_to_c(nv,_tmp64D);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp65A=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC39[4];e->r=Cyc_Toc_fncall_exp_r(_tmp65A,(
(_tmpC39[3]=_tmp659,((_tmpC39[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp651),0),((_tmpC39[1]=_tmp64D,((_tmpC39[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3175
goto _LL334;};_LL337:;_LL338:
 goto _LL334;_LL334:;}
# 3178
goto _LL32F;_LL332:;_LL333:
 goto _LL32F;_LL32F:;}
# 3181
goto _LL32A;_LL32D:;_LL32E:
 goto _LL32A;_LL32A:;}
# 3184
if(!done){
struct Cyc_Absyn_Stmt*_tmp65C=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp649),0);
struct Cyc_Absyn_Exp*_tmp65D=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp514);
_tmp65C=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp649,_tmp65D,0),_tmp514,0),_tmp65C,0);{
# 3190
void*_tmp65E=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp648,_tmp65E,mexp,_tmp65C,0));};}
# 3193
goto _LL315;};}_LL315:;}
# 3195
goto _LL262;};_LL287: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp515->tag != 17)goto _LL289;else{_tmp516=_tmp515->f1;}}_LL288: {
# 3198
int _tmp65F=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp516);
Cyc_Toc_set_in_sizeof(nv,_tmp65F);
goto _LL262;}_LL289: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp517->tag != 16)goto _LL28B;else{_tmp518=(void*)_tmp517->f1;}}_LL28A:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC3C;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC3B;e->r=(void*)((_tmpC3B=_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B[0]=((_tmpC3C.tag=16,((_tmpC3C.f1=Cyc_Toc_typ_to_c(_tmp518),_tmpC3C)))),_tmpC3B))));}goto _LL262;_LL28B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp519=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp519->tag != 18)goto _LL28D;else{_tmp51A=(void*)_tmp519->f1;_tmp51B=_tmp519->f2;}}_LL28C: {
# 3204
void*_tmp662=_tmp51A;
struct Cyc_List_List*_tmp663=_tmp51B;
for(0;_tmp663 != 0;_tmp663=_tmp663->tl){
void*_tmp664=(void*)_tmp663->hd;struct _dyneither_ptr*_tmp666;unsigned int _tmp668;_LL33A: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp665=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp664;if(_tmp665->tag != 0)goto _LL33C;else{_tmp666=_tmp665->f1;}}_LL33B:
 goto _LL339;_LL33C: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp667=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp664;if(_tmp667->tag != 1)goto _LL339;else{_tmp668=_tmp667->f1;}}_LL33D:
# 3210
{void*_tmp669=Cyc_Tcutil_compress(_tmp662);union Cyc_Absyn_AggrInfoU _tmp66B;struct Cyc_List_List*_tmp66D;struct Cyc_List_List*_tmp66F;struct Cyc_Absyn_Datatypefield*_tmp671;_LL33F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp66A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp669;if(_tmp66A->tag != 11)goto _LL341;else{_tmp66B=(_tmp66A->f1).aggr_info;}}_LL340: {
# 3212
struct Cyc_Absyn_Aggrdecl*_tmp672=Cyc_Absyn_get_known_aggrdecl(_tmp66B);
if(_tmp672->impl == 0){
const char*_tmpC3F;void*_tmpC3E;(_tmpC3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3F="struct fields must be known",_tag_dyneither(_tmpC3F,sizeof(char),28))),_tag_dyneither(_tmpC3E,sizeof(void*),0)));}
_tmp66D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp672->impl))->fields;goto _LL342;}_LL341: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp669;if(_tmp66C->tag != 12)goto _LL343;else{_tmp66D=_tmp66C->f2;}}_LL342: {
# 3217
struct Cyc_Absyn_Aggrfield*_tmp675=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp66D,(int)_tmp668);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC42;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC41;_tmp663->hd=(void*)((void*)((_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41[0]=((_tmpC42.tag=0,((_tmpC42.f1=_tmp675->name,_tmpC42)))),_tmpC41)))));}
_tmp662=_tmp675->type;
goto _LL33E;}_LL343: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp66E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp669;if(_tmp66E->tag != 10)goto _LL345;else{_tmp66F=_tmp66E->f1;}}_LL344:
# 3222
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC45;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC44;_tmp663->hd=(void*)((void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC45.tag=0,((_tmpC45.f1=Cyc_Absyn_fieldname((int)(_tmp668 + 1)),_tmpC45)))),_tmpC44)))));}
_tmp662=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp66F,(int)_tmp668)).f2;
goto _LL33E;_LL345: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp670=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp669;if(_tmp670->tag != 4)goto _LL347;else{if((((_tmp670->f1).field_info).KnownDatatypefield).tag != 2)goto _LL347;_tmp671=((struct _tuple1)(((_tmp670->f1).field_info).KnownDatatypefield).val).f2;}}_LL346:
# 3226
 if(_tmp668 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC48;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC47;_tmp663->hd=(void*)((void*)((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47[0]=((_tmpC48.tag=0,((_tmpC48.f1=Cyc_Toc_tag_sp,_tmpC48)))),_tmpC47)))));}else{
# 3229
_tmp662=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp671->typs,(int)(_tmp668 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC4B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC4A;_tmp663->hd=(void*)((void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC4B.tag=0,((_tmpC4B.f1=Cyc_Absyn_fieldname((int)_tmp668),_tmpC4B)))),_tmpC4A)))));};}
# 3232
goto _LL33E;_LL347:;_LL348:
{const char*_tmpC4E;void*_tmpC4D;(_tmpC4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4E="impossible type for offsetof tuple index",_tag_dyneither(_tmpC4E,sizeof(char),41))),_tag_dyneither(_tmpC4D,sizeof(void*),0)));}
goto _LL33E;_LL33E:;}
# 3236
goto _LL339;_LL339:;}
# 3239
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC51;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC50;e->r=(void*)((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50[0]=((_tmpC51.tag=18,((_tmpC51.f1=Cyc_Toc_typ_to_c(_tmp51A),((_tmpC51.f2=_tmp51B,_tmpC51)))))),_tmpC50))));}
goto _LL262;}_LL28D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp51C->tag != 19)goto _LL28F;else{_tmp51D=_tmp51C->f1;}}_LL28E: {
# 3242
int _tmp682=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp683=Cyc_Tcutil_compress((void*)_check_null(_tmp51D->topt));
{void*_tmp684=_tmp683;void*_tmp686;struct Cyc_Absyn_Tqual _tmp687;void*_tmp688;union Cyc_Absyn_Constraint*_tmp689;union Cyc_Absyn_Constraint*_tmp68A;union Cyc_Absyn_Constraint*_tmp68B;_LL34A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp685=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp684;if(_tmp685->tag != 5)goto _LL34C;else{_tmp686=(_tmp685->f1).elt_typ;_tmp687=(_tmp685->f1).elt_tq;_tmp688=((_tmp685->f1).ptr_atts).rgn;_tmp689=((_tmp685->f1).ptr_atts).nullable;_tmp68A=((_tmp685->f1).ptr_atts).bounds;_tmp68B=((_tmp685->f1).ptr_atts).zero_term;}}_LL34B:
# 3247
{void*_tmp68C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp68A);struct Cyc_Absyn_Exp*_tmp68E;_LL34F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp68D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp68C;if(_tmp68D->tag != 1)goto _LL351;else{_tmp68E=_tmp68D->f1;}}_LL350: {
# 3249
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp51D);
Cyc_Toc_exp_to_c(nv,_tmp51D);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC54;void*_tmpC53;(_tmpC53=0,Cyc_Tcutil_warn(e->loc,((_tmpC54="inserted null check due to dereference",_tag_dyneither(_tmpC54,sizeof(char),39))),_tag_dyneither(_tmpC53,sizeof(void*),0)));}{
# 3255
struct Cyc_List_List*_tmpC55;_tmp51D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp683),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC55=_cycalloc(sizeof(*_tmpC55)),((_tmpC55->hd=Cyc_Absyn_copy_exp(_tmp51D),((_tmpC55->tl=0,_tmpC55)))))),0));};}
# 3263
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp68B)){
unsigned int _tmp694;int _tmp695;struct _tuple11 _tmp693=Cyc_Evexp_eval_const_uint_exp(_tmp68E);_tmp694=_tmp693.f1;_tmp695=_tmp693.f2;
# 3269
if(!_tmp695  || _tmp694 <= 0){
const char*_tmpC58;void*_tmpC57;(_tmpC57=0,Cyc_Tcutil_terr(e->loc,((_tmpC58="cannot determine dereference is in bounds",_tag_dyneither(_tmpC58,sizeof(char),42))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}}
# 3272
goto _LL34E;}_LL351: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp68F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp68C;if(_tmp68F->tag != 0)goto _LL34E;}_LL352: {
# 3275
struct Cyc_Absyn_Exp*_tmp698=Cyc_Absyn_uint_exp(0,0);
_tmp698->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp51D,_tmp698);
Cyc_Toc_exp_to_c(nv,e);
goto _LL34E;}_LL34E:;}
# 3281
goto _LL349;_LL34C:;_LL34D: {
const char*_tmpC5B;void*_tmpC5A;(_tmpC5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5B="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC5B,sizeof(char),29))),_tag_dyneither(_tmpC5A,sizeof(void*),0)));}_LL349:;}
# 3284
Cyc_Toc_set_lhs(nv,_tmp682);
goto _LL262;};}_LL28F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp51E->tag != 20)goto _LL291;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;_tmp521=_tmp51E->f3;_tmp522=_tmp51E->f4;}}_LL290: {
# 3287
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp51F->topt);
Cyc_Toc_exp_to_c(nv,_tmp51F);
if(_tmp521  && _tmp522)
e->r=Cyc_Toc_check_tagged_union(_tmp51F,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp520,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3295
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL262;}_LL291: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp523=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp523->tag != 21)goto _LL293;else{_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;_tmp526=_tmp523->f3;_tmp527=_tmp523->f4;}}_LL292: {
# 3299
int _tmp69B=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp524->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp524);
Cyc_Toc_exp_to_c(nv,_tmp524);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp69D;struct Cyc_Absyn_Tqual _tmp69E;void*_tmp69F;union Cyc_Absyn_Constraint*_tmp6A0;union Cyc_Absyn_Constraint*_tmp6A1;union Cyc_Absyn_Constraint*_tmp6A2;struct Cyc_Absyn_PtrInfo _tmp69C=Cyc_Toc_get_ptr_type(e1typ);_tmp69D=_tmp69C.elt_typ;_tmp69E=_tmp69C.elt_tq;_tmp69F=(_tmp69C.ptr_atts).rgn;_tmp6A0=(_tmp69C.ptr_atts).nullable;_tmp6A1=(_tmp69C.ptr_atts).bounds;_tmp6A2=(_tmp69C.ptr_atts).zero_term;
{void*_tmp6A3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6A1);struct Cyc_Absyn_Exp*_tmp6A5;_LL354: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6A3;if(_tmp6A4->tag != 1)goto _LL356;else{_tmp6A5=_tmp6A4->f1;}}_LL355: {
# 3310
unsigned int _tmp6A8;int _tmp6A9;struct _tuple11 _tmp6A7=Cyc_Evexp_eval_const_uint_exp(_tmp6A5);_tmp6A8=_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;
if(_tmp6A9){
if(_tmp6A8 < 1){
const char*_tmpC5E;void*_tmpC5D;(_tmpC5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC5E="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC5E,sizeof(char),44))),_tag_dyneither(_tmpC5D,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC61;void*_tmpC60;(_tmpC60=0,Cyc_Tcutil_warn(e->loc,((_tmpC61="inserted null check due to dereference",_tag_dyneither(_tmpC61,sizeof(char),39))),_tag_dyneither(_tmpC60,sizeof(void*),0)));}{
# 3318
struct Cyc_Absyn_Exp*_tmpC62[1];_tmp524->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC62[0]=Cyc_Absyn_new_exp(_tmp524->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC62,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3323
if(!Cyc_Evexp_c_can_eval(_tmp6A5)){
const char*_tmpC65;void*_tmpC64;(_tmpC64=0,Cyc_Tcutil_terr(e->loc,((_tmpC65="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC65,sizeof(char),47))),_tag_dyneither(_tmpC64,sizeof(void*),0)));}{
# 3327
struct Cyc_Absyn_Exp*_tmpC66[4];_tmp524->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC66[3]=
# 3331
Cyc_Absyn_uint_exp(0,0),((_tmpC66[2]=
# 3330
Cyc_Absyn_sizeoftyp_exp(_tmp69D,0),((_tmpC66[1]=_tmp6A5,((_tmpC66[0]=
# 3329
Cyc_Absyn_new_exp(_tmp524->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC66,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3333
goto _LL353;}_LL356: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6A6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6A3;if(_tmp6A6->tag != 0)goto _LL353;}_LL357: {
# 3336
void*ta1=Cyc_Toc_typ_to_c(_tmp69D);
{struct Cyc_Absyn_Exp*_tmpC67[3];_tmp524->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp69E),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC67[2]=
# 3341
Cyc_Absyn_uint_exp(0,0),((_tmpC67[1]=
# 3340
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC67[0]=
# 3339
Cyc_Absyn_new_exp(_tmp524->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC67,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3344
goto _LL353;}_LL353:;}
# 3346
if(_tmp526  && _tmp527)
e->r=Cyc_Toc_check_tagged_union(_tmp524,Cyc_Toc_typ_to_c(e1typ),_tmp69D,_tmp525,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3349
if(is_poly  && _tmp527)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp69B);
goto _LL262;};};}_LL293: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp528->tag != 22)goto _LL295;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}}_LL294: {
# 3354
int _tmp6B3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6B4=Cyc_Tcutil_compress((void*)_check_null(_tmp529->topt));
# 3358
{void*_tmp6B5=_tmp6B4;struct Cyc_List_List*_tmp6B7;void*_tmp6B9;struct Cyc_Absyn_Tqual _tmp6BA;void*_tmp6BB;union Cyc_Absyn_Constraint*_tmp6BC;union Cyc_Absyn_Constraint*_tmp6BD;union Cyc_Absyn_Constraint*_tmp6BE;_LL359: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6B6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6B5;if(_tmp6B6->tag != 10)goto _LL35B;else{_tmp6B7=_tmp6B6->f1;}}_LL35A:
# 3361
 Cyc_Toc_exp_to_c(nv,_tmp529);
Cyc_Toc_exp_to_c(nv,_tmp52A);{
unsigned int _tmp6C0;int _tmp6C1;struct _tuple11 _tmp6BF=Cyc_Evexp_eval_const_uint_exp(_tmp52A);_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;
if(!_tmp6C1){
const char*_tmpC6A;void*_tmpC69;(_tmpC69=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC6A="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC6A,sizeof(char),44))),_tag_dyneither(_tmpC69,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp529,Cyc_Absyn_fieldname((int)(_tmp6C0 + 1)));
goto _LL358;};_LL35B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B5;if(_tmp6B8->tag != 5)goto _LL35D;else{_tmp6B9=(_tmp6B8->f1).elt_typ;_tmp6BA=(_tmp6B8->f1).elt_tq;_tmp6BB=((_tmp6B8->f1).ptr_atts).rgn;_tmp6BC=((_tmp6B8->f1).ptr_atts).nullable;_tmp6BD=((_tmp6B8->f1).ptr_atts).bounds;_tmp6BE=((_tmp6B8->f1).ptr_atts).zero_term;}}_LL35C: {
# 3369
struct Cyc_List_List*_tmp6C4=Cyc_Toc_get_relns(_tmp529);
int _tmp6C5=Cyc_Toc_in_sizeof(nv);
# 3376
int in_bnds=_tmp6C5;
{void*_tmp6C6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6BD);_LL360:;_LL361:
# 3379
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6B4,_tmp6C4,_tmp529,_tmp52A);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC6E;void*_tmpC6D[1];struct Cyc_String_pa_PrintArg_struct _tmpC6C;(_tmpC6C.tag=0,((_tmpC6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC6D[0]=& _tmpC6C,Cyc_Tcutil_warn(e->loc,((_tmpC6E="bounds check necessary for %s",_tag_dyneither(_tmpC6E,sizeof(char),30))),_tag_dyneither(_tmpC6D,sizeof(void*),1)))))));}_LL35F:;}
# 3386
Cyc_Toc_exp_to_c(nv,_tmp529);
Cyc_Toc_exp_to_c(nv,_tmp52A);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6CA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6BD);struct Cyc_Absyn_Exp*_tmp6CC;_LL363: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6CB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6CA;if(_tmp6CB->tag != 1)goto _LL365;else{_tmp6CC=_tmp6CB->f1;}}_LL364: {
# 3391
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BC);
void*ta1=Cyc_Toc_typ_to_c(_tmp6B9);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6BA);
if(in_bnds)
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3397
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BE)){
# 3399
if(!Cyc_Evexp_c_can_eval(_tmp6CC)){
const char*_tmpC71;void*_tmpC70;(_tmpC70=0,Cyc_Tcutil_terr(e->loc,((_tmpC71="cannot determine subscript is in bounds",_tag_dyneither(_tmpC71,sizeof(char),40))),_tag_dyneither(_tmpC70,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp529);
struct Cyc_Absyn_Exp*_tmpC72[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC72[2]=_tmp52A,((_tmpC72[1]=_tmp6CC,((_tmpC72[0]=_tmp529,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC72,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6CC)){
const char*_tmpC75;void*_tmpC74;(_tmpC74=0,Cyc_Tcutil_terr(e->loc,((_tmpC75="cannot determine subscript is in bounds",_tag_dyneither(_tmpC75,sizeof(char),40))),_tag_dyneither(_tmpC74,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC78;void*_tmpC77;(_tmpC77=0,Cyc_Tcutil_warn(e->loc,((_tmpC78="inserted null check due to dereference",_tag_dyneither(_tmpC78,sizeof(char),39))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}{
# 3411
struct Cyc_Absyn_Exp*_tmpC79[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC79[3]=_tmp52A,((_tmpC79[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC79[1]=_tmp6CC,((_tmpC79[0]=_tmp529,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC79,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3417
if(!Cyc_Evexp_c_can_eval(_tmp6CC)){
const char*_tmpC7C;void*_tmpC7B;(_tmpC7B=0,Cyc_Tcutil_terr(e->loc,((_tmpC7C="cannot determine subscript is in bounds",_tag_dyneither(_tmpC7C,sizeof(char),40))),_tag_dyneither(_tmpC7B,sizeof(void*),0)));}{
# 3420
struct Cyc_Absyn_Exp*_tmpC7D[2];_tmp52A->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC7D[1]=Cyc_Absyn_copy_exp(_tmp52A),((_tmpC7D[0]=_tmp6CC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7D,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3423
goto _LL362;}_LL365: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6CD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6CA;if(_tmp6CD->tag != 0)goto _LL362;}_LL366: {
# 3425
void*ta1=Cyc_Toc_typ_to_c(_tmp6B9);
if(in_bnds){
# 3429
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BA),
Cyc_Toc_member_exp(_tmp529,Cyc_Toc_curr_sp,0)),_tmp52A);}else{
# 3434
struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC7E[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6BA),
Cyc_Absyn_fncall_exp(_tmp6D9,(
(_tmpC7E[2]=_tmp52A,((_tmpC7E[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC7E[0]=_tmp529,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3440
goto _LL362;}_LL362:;}
# 3442
goto _LL358;}_LL35D:;_LL35E: {
const char*_tmpC81;void*_tmpC80;(_tmpC80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC81="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC81,sizeof(char),49))),_tag_dyneither(_tmpC80,sizeof(void*),0)));}_LL358:;}
# 3445
Cyc_Toc_set_lhs(nv,_tmp6B3);
goto _LL262;};}_LL295: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp52B->tag != 23)goto _LL297;else{_tmp52C=_tmp52B->f1;}}_LL296:
# 3448
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp52C))->r;else{
# 3453
struct Cyc_List_List*_tmp6DD=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp52C);
void*_tmp6DE=Cyc_Toc_add_tuple_type(_tmp6DD);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp52C != 0;(_tmp52C=_tmp52C->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp52C->hd);{
struct _tuple16*_tmpC84;struct Cyc_List_List*_tmpC83;dles=((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->hd=((_tmpC84=_cycalloc(sizeof(*_tmpC84)),((_tmpC84->f1=0,((_tmpC84->f2=(struct Cyc_Absyn_Exp*)_tmp52C->hd,_tmpC84)))))),((_tmpC83->tl=dles,_tmpC83))))));};}}
# 3460
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3463
goto _LL262;_LL297: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp52D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp52D->tag != 25)goto _LL299;else{_tmp52E=_tmp52D->f1;}}_LL298:
# 3467
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp52E);
{struct Cyc_List_List*_tmp6E1=_tmp52E;for(0;_tmp6E1 != 0;_tmp6E1=_tmp6E1->tl){
struct Cyc_Absyn_Exp*_tmp6E3;struct _tuple16*_tmp6E2=(struct _tuple16*)_tmp6E1->hd;_tmp6E3=_tmp6E2->f2;
Cyc_Toc_exp_to_c(nv,_tmp6E3);}}
# 3472
goto _LL262;_LL299: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp52F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp52F->tag != 26)goto _LL29B;else{_tmp530=_tmp52F->f1;_tmp531=_tmp52F->f2;_tmp532=_tmp52F->f3;_tmp533=_tmp52F->f4;}}_LL29A: {
# 3476
unsigned int _tmp6E5;int _tmp6E6;struct _tuple11 _tmp6E4=Cyc_Evexp_eval_const_uint_exp(_tmp531);_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{
void*_tmp6E7=Cyc_Toc_typ_to_c((void*)_check_null(_tmp532->topt));
Cyc_Toc_exp_to_c(nv,_tmp532);{
struct Cyc_List_List*es=0;
# 3481
if(!Cyc_Toc_is_zero(_tmp532)){
if(!_tmp6E6){
const char*_tmpC87;void*_tmpC86;(_tmpC86=0,Cyc_Tcutil_terr(_tmp531->loc,((_tmpC87="cannot determine value of constant",_tag_dyneither(_tmpC87,sizeof(char),35))),_tag_dyneither(_tmpC86,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6E5;++ i){
struct _tuple16*_tmpC8A;struct Cyc_List_List*_tmpC89;es=((_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->hd=((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->f1=0,((_tmpC8A->f2=_tmp532,_tmpC8A)))))),((_tmpC89->tl=es,_tmpC89))))));}}
# 3487
if(_tmp533){
struct Cyc_Absyn_Exp*_tmp6EC=Cyc_Toc_cast_it(_tmp6E7,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC8D;struct Cyc_List_List*_tmpC8C;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->hd=((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D->f1=0,((_tmpC8D->f2=_tmp6EC,_tmpC8D)))))),((_tmpC8C->tl=0,_tmpC8C)))))));}}
# 3492
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL262;};};}_LL29B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp534=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp534->tag != 27)goto _LL29D;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;_tmp537=_tmp534->f3;_tmp538=_tmp534->f4;}}_LL29C:
# 3497
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp536,0,0,_tmp537,_tmp535))->r;else{
# 3505
if(_tmp538 == 0){
const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC90="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC90,sizeof(char),38))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp538;
# 3509
struct _RegionHandle _tmp6F1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6F1;_push_region(rgn);
{struct Cyc_List_List*_tmp6F2=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp537,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3513
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp538->impl))->tagged){
# 3515
struct Cyc_Absyn_Aggrfield*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F5;struct _tuple22*_tmp6F3=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6F2))->hd;_tmp6F4=_tmp6F3->f1;_tmp6F5=_tmp6F3->f2;{
void*_tmp6F6=(void*)_check_null(_tmp6F5->topt);
void*_tmp6F7=_tmp6F4->type;
Cyc_Toc_exp_to_c(nv,_tmp6F5);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6F7) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6F6))
_tmp6F5->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6F5->r,0));{
# 3524
int i=Cyc_Toc_get_member_offset(_tmp538,_tmp6F4->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC95;struct _tuple16*_tmpC94;struct _tuple16*_tmpC93[2];struct Cyc_List_List*_tmp6F8=(_tmpC93[1]=((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->f1=0,((_tmpC94->f2=_tmp6F5,_tmpC94)))))),((_tmpC93[0]=((_tmpC95=_cycalloc(sizeof(*_tmpC95)),((_tmpC95->f1=0,((_tmpC95->f2=field_tag_exp,_tmpC95)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6F8,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC9B;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC9A;void*_tmpC99[1];struct Cyc_List_List*ds=(_tmpC99[0]=(void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9A.tag=1,((_tmpC9A.f1=_tmp6F4->name,_tmpC9A)))),_tmpC9B)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC99,sizeof(void*),1)));
struct _tuple16*_tmpC9E;struct _tuple16*_tmpC9D[1];struct Cyc_List_List*dles=(_tmpC9D[0]=((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->f1=ds,((_tmpC9E->f2=umem,_tmpC9E)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9D,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3533
struct Cyc_List_List*_tmp701=0;
struct Cyc_List_List*_tmp702=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp702 != 0;_tmp702=_tmp702->tl){
struct Cyc_List_List*_tmp703=_tmp6F2;for(0;_tmp703 != 0;_tmp703=_tmp703->tl){
if((*((struct _tuple22*)_tmp703->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp702->hd){
struct Cyc_Absyn_Aggrfield*_tmp705;struct Cyc_Absyn_Exp*_tmp706;struct _tuple22*_tmp704=(struct _tuple22*)_tmp703->hd;_tmp705=_tmp704->f1;_tmp706=_tmp704->f2;{
void*_tmp707=Cyc_Toc_typ_to_c(_tmp705->type);
void*_tmp708=Cyc_Toc_typ_to_c((void*)_check_null(_tmp706->topt));
Cyc_Toc_exp_to_c(nv,_tmp706);
# 3543
if(!Cyc_Tcutil_unify(_tmp707,_tmp708)){
# 3545
if(!Cyc_Tcutil_is_arithmetic_type(_tmp707) || !
Cyc_Tcutil_is_arithmetic_type(_tmp708))
_tmp706=Cyc_Toc_cast_it(_tmp707,Cyc_Absyn_copy_exp(_tmp706));}
{struct _tuple16*_tmpCA1;struct Cyc_List_List*_tmpCA0;_tmp701=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->hd=((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->f1=0,((_tmpCA1->f2=_tmp706,_tmpCA1)))))),((_tmpCA0->tl=_tmp701,_tmpCA0))))));}
break;};}}}
# 3552
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp701));}}
# 3510
;_pop_region(rgn);};}
# 3556
goto _LL262;_LL29D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp539=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp539->tag != 28)goto _LL29F;else{_tmp53A=(void*)_tmp539->f1;_tmp53B=_tmp539->f2;}}_LL29E: {
# 3558
struct Cyc_List_List*fs;
{void*_tmp70B=Cyc_Tcutil_compress(_tmp53A);struct Cyc_List_List*_tmp70D;_LL368: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp70C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp70B;if(_tmp70C->tag != 12)goto _LL36A;else{_tmp70D=_tmp70C->f2;}}_LL369:
 fs=_tmp70D;goto _LL367;_LL36A:;_LL36B: {
const char*_tmpCA5;void*_tmpCA4[1];struct Cyc_String_pa_PrintArg_struct _tmpCA3;(_tmpCA3.tag=0,((_tmpCA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp53A)),((_tmpCA4[0]=& _tmpCA3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="anon struct has type %s",_tag_dyneither(_tmpCA5,sizeof(char),24))),_tag_dyneither(_tmpCA4,sizeof(void*),1)))))));}_LL367:;}{
# 3563
struct _RegionHandle _tmp711=_new_region("rgn");struct _RegionHandle*rgn=& _tmp711;_push_region(rgn);{
struct Cyc_List_List*_tmp712=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp53B,Cyc_Absyn_StructA,fs);
for(0;_tmp712 != 0;_tmp712=_tmp712->tl){
struct Cyc_Absyn_Aggrfield*_tmp714;struct Cyc_Absyn_Exp*_tmp715;struct _tuple22*_tmp713=(struct _tuple22*)_tmp712->hd;_tmp714=_tmp713->f1;_tmp715=_tmp713->f2;{
void*_tmp716=(void*)_check_null(_tmp715->topt);
void*_tmp717=_tmp714->type;
Cyc_Toc_exp_to_c(nv,_tmp715);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp717) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp716))
_tmp715->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp715->r,0));};}
# 3577
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp53B);}
# 3579
_npop_handler(0);goto _LL262;
# 3563
;_pop_region(rgn);};}_LL29F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp53C->tag != 29)goto _LL2A1;else{_tmp53D=_tmp53C->f1;_tmp53E=_tmp53C->f2;_tmp53F=_tmp53C->f3;}}_LL2A0: {
# 3582
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp718=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp719=Cyc_Absyn_var_exp(_tmp718,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp53F->name,_tmp53E->name));
tag_exp=_tmp53E->is_extensible?Cyc_Absyn_var_exp(_tmp53F->name,0):
 Cyc_Toc_datatype_tag(_tmp53E,_tmp53F->name);
mem_exp=_tmp719;{
struct Cyc_List_List*_tmp71A=_tmp53F->typs;
# 3593
if(Cyc_Toc_is_toplevel(nv)){
# 3595
struct Cyc_List_List*dles=0;
for(0;_tmp53D != 0;(_tmp53D=_tmp53D->tl,_tmp71A=_tmp71A->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp53D->hd;
void*_tmp71B=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp71A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp71B))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpCA8;struct Cyc_List_List*_tmpCA7;dles=((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->hd=((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->f1=0,((_tmpCA8->f2=cur_e,_tmpCA8)))))),((_tmpCA7->tl=dles,_tmpCA7))))));};}
# 3606
{struct _tuple16*_tmpCAB;struct Cyc_List_List*_tmpCAA;dles=((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->hd=((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->f1=0,((_tmpCAB->f2=tag_exp,_tmpCAB)))))),((_tmpCAA->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpCAA))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3612
struct Cyc_List_List*_tmpCAC;struct Cyc_List_List*_tmp720=
(_tmpCAC=_cycalloc(sizeof(*_tmpCAC)),((_tmpCAC->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpCAC->tl=0,_tmpCAC)))));
# 3615
{int i=1;for(0;_tmp53D != 0;(((_tmp53D=_tmp53D->tl,i ++)),_tmp71A=_tmp71A->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp53D->hd;
void*_tmp721=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp71A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp721))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp722=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3625
struct Cyc_List_List*_tmpCAD;_tmp720=((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD->hd=_tmp722,((_tmpCAD->tl=_tmp720,_tmpCAD))))));};}}{
# 3627
struct Cyc_Absyn_Stmt*_tmp724=Cyc_Absyn_exp_stmt(_tmp719,0);
struct Cyc_List_List*_tmpCAE;struct Cyc_Absyn_Stmt*_tmp725=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->hd=_tmp724,((_tmpCAE->tl=_tmp720,_tmpCAE))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp718,datatype_ctype,0,_tmp725,0));};}
# 3631
goto _LL262;};}_LL2A1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp540=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp540->tag != 30)goto _LL2A3;}_LL2A2:
# 3633
 goto _LL2A4;_LL2A3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp541->tag != 31)goto _LL2A5;}_LL2A4:
 goto _LL262;_LL2A5: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp542=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp542->tag != 32)goto _LL2A7;else{_tmp543=(_tmp542->f1).is_calloc;_tmp544=(_tmp542->f1).rgn;_tmp545=(_tmp542->f1).elt_type;_tmp546=(_tmp542->f1).num_elts;_tmp547=(_tmp542->f1).fat_result;}}_LL2A6: {
# 3637
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp545)));
Cyc_Toc_exp_to_c(nv,_tmp546);
# 3641
if(_tmp547){
struct _tuple0*_tmp728=Cyc_Toc_temp_var();
struct _tuple0*_tmp729=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp543){
xexp=_tmp546;
if(_tmp544 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp544;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp728,0));}else{
# 3652
pexp=Cyc_Toc_calloc_exp(*_tmp545,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp728,0));}{
# 3654
struct Cyc_Absyn_Exp*_tmpCAF[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCAF[2]=
Cyc_Absyn_var_exp(_tmp728,0),((_tmpCAF[1]=
# 3655
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCAF[0]=Cyc_Absyn_var_exp(_tmp729,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3658
if(_tmp544 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp544;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp728,0));}else{
# 3663
pexp=Cyc_Toc_malloc_exp(*_tmp545,Cyc_Absyn_var_exp(_tmp728,0));}{
# 3665
struct Cyc_Absyn_Exp*_tmpCB0[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpCB0[2]=
Cyc_Absyn_var_exp(_tmp728,0),((_tmpCB0[1]=
# 3665
Cyc_Absyn_uint_exp(1,0),((_tmpCB0[0]=Cyc_Absyn_var_exp(_tmp729,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3668
struct Cyc_Absyn_Stmt*_tmp72C=Cyc_Absyn_declare_stmt(_tmp728,Cyc_Absyn_uint_typ,_tmp546,
Cyc_Absyn_declare_stmt(_tmp729,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp72C);};}else{
# 3673
if(_tmp543){
if(_tmp544 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp544;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp546))->r;}else{
# 3679
e->r=(Cyc_Toc_calloc_exp(*_tmp545,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp546))->r;}}else{
# 3682
if(_tmp544 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp544;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp546))->r;}else{
# 3687
e->r=(Cyc_Toc_malloc_exp(*_tmp545,_tmp546))->r;}}}
# 3691
goto _LL262;}_LL2A7: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp548=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp548->tag != 33)goto _LL2A9;else{_tmp549=_tmp548->f1;_tmp54A=_tmp548->f2;}}_LL2A8: {
# 3700
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp549->topt);
void*e2_old_typ=(void*)_check_null(_tmp54A->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpCB3;void*_tmpCB2;(_tmpCB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB3="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpCB3,sizeof(char),57))),_tag_dyneither(_tmpCB2,sizeof(void*),0)));}{
# 3707
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3711
swap_fn=Cyc_Toc__swap_word_e;}{
# 3715
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp549,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp54A,& f2_tag,& tagged_mem_type2,1);
# 3722
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp549);
Cyc_Toc_exp_to_c(nv,_tmp54A);
Cyc_Toc_set_lhs(nv,0);
# 3728
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp549,f1_tag,tagged_mem_type1);else{
# 3732
_tmp549=Cyc_Toc_push_address_exp(_tmp549);}
# 3734
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp54A,f2_tag,tagged_mem_type2);else{
# 3738
_tmp54A=Cyc_Toc_push_address_exp(_tmp54A);}
# 3740
{struct Cyc_Absyn_Exp*_tmpCB4[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCB4[1]=_tmp54A,((_tmpCB4[0]=_tmp549,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB4,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3742
goto _LL262;};};}_LL2A9: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp54B=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp54B->tag != 36)goto _LL2AB;else{_tmp54C=_tmp54B->f1;_tmp54D=_tmp54B->f2;}}_LL2AA: {
# 3745
void*_tmp730=Cyc_Tcutil_compress((void*)_check_null(_tmp54C->topt));
Cyc_Toc_exp_to_c(nv,_tmp54C);
{void*_tmp731=_tmp730;struct Cyc_Absyn_Aggrdecl*_tmp733;_LL36D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp732=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp731;if(_tmp732->tag != 11)goto _LL36F;else{if((((_tmp732->f1).aggr_info).KnownAggr).tag != 2)goto _LL36F;_tmp733=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp732->f1).aggr_info).KnownAggr).val);}}_LL36E: {
# 3749
struct Cyc_Absyn_Exp*_tmp734=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp733,_tmp54D),0);
struct Cyc_Absyn_Exp*_tmp735=Cyc_Toc_member_exp(_tmp54C,_tmp54D,0);
struct Cyc_Absyn_Exp*_tmp736=Cyc_Toc_member_exp(_tmp735,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp736,_tmp734,0))->r;
goto _LL36C;}_LL36F:;_LL370: {
const char*_tmpCB7;void*_tmpCB6;(_tmpCB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB7="non-aggregate type in tagcheck",_tag_dyneither(_tmpCB7,sizeof(char),31))),_tag_dyneither(_tmpCB6,sizeof(void*),0)));}_LL36C:;}
# 3756
goto _LL262;}_LL2AB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp54E->tag != 35)goto _LL2AD;else{_tmp54F=_tmp54E->f1;}}_LL2AC:
 Cyc_Toc_stmt_to_c(nv,_tmp54F);goto _LL262;_LL2AD: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp550->tag != 34)goto _LL2AF;}_LL2AE: {
const char*_tmpCBA;void*_tmpCB9;(_tmpCB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBA="UnresolvedMem",_tag_dyneither(_tmpCBA,sizeof(char),14))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}_LL2AF: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp551=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp551->tag != 24)goto _LL2B1;}_LL2B0: {
const char*_tmpCBD;void*_tmpCBC;(_tmpCBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCBD="compoundlit",_tag_dyneither(_tmpCBD,sizeof(char),12))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}_LL2B1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp552=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp552->tag != 37)goto _LL2B3;}_LL2B2: {
const char*_tmpCC0;void*_tmpCBF;(_tmpCBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC0="valueof(-)",_tag_dyneither(_tmpCC0,sizeof(char),11))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}_LL2B3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp553=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4DE;if(_tmp553->tag != 38)goto _LL262;}_LL2B4: {
const char*_tmpCC3;void*_tmpCC2;(_tmpCC2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC3="__asm__",_tag_dyneither(_tmpCC3,sizeof(char),8))),_tag_dyneither(_tmpCC2,sizeof(void*),0)));}_LL262:;};}
# 3793 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3796
Cyc_Toc_skip_stmt_dl(),0);}
# 3799
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp741=e->r;struct Cyc_Absyn_Exp*_tmp743;struct Cyc_Absyn_Exp*_tmp745;struct Cyc_Absyn_Exp*_tmp747;_LL372: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp742=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp741;if(_tmp742->tag != 13)goto _LL374;else{_tmp743=_tmp742->f2;}}_LL373:
 return Cyc_Toc_path_length(_tmp743);_LL374: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp744=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp741;if(_tmp744->tag != 19)goto _LL376;else{_tmp745=_tmp744->f1;}}_LL375:
 return 1 + Cyc_Toc_path_length(_tmp745);_LL376: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp746=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp741;if(_tmp746->tag != 20)goto _LL378;else{_tmp747=_tmp746->f1;}}_LL377:
 return Cyc_Toc_path_length(_tmp747);_LL378:;_LL379:
 return 0;_LL371:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3808
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3813
struct Cyc_Absyn_Stmt*s;
{void*_tmp748=p->r;struct _tuple0*_tmp74A;struct Cyc_Absyn_Pat*_tmp74B;struct _tuple0*_tmp74D;struct Cyc_Absyn_Vardecl*_tmp750;struct Cyc_Absyn_Pat*_tmp751;enum Cyc_Absyn_Sign _tmp754;int _tmp755;char _tmp757;struct _dyneither_ptr _tmp759;int _tmp75A;struct Cyc_Absyn_Enumdecl*_tmp75C;struct Cyc_Absyn_Enumfield*_tmp75D;void*_tmp75F;struct Cyc_Absyn_Enumfield*_tmp760;struct Cyc_Absyn_Datatypedecl*_tmp763;struct Cyc_Absyn_Datatypefield*_tmp764;struct Cyc_List_List*_tmp765;struct Cyc_List_List*_tmp767;struct Cyc_List_List*_tmp769;union Cyc_Absyn_AggrInfoU _tmp76C;struct Cyc_List_List*_tmp76D;struct Cyc_Absyn_Pat*_tmp76F;_LL37B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp749=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp749->tag != 1)goto _LL37D;else{_tmp74A=(_tmp749->f1)->name;_tmp74B=_tmp749->f2;}}_LL37C: {
# 3817
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp773=(void*)_check_null(_tmp74B->topt);
{struct _tuple24*_tmpCC6;struct Cyc_List_List*_tmpCC5;decls=((_tmpCC5=_region_malloc(rgn,sizeof(*_tmpCC5)),((_tmpCC5->hd=((_tmpCC6=_region_malloc(rgn,sizeof(*_tmpCC6)),((_tmpCC6->f1=v,((_tmpCC6->f2=Cyc_Toc_typ_to_c_array(_tmp773),_tmpCC6)))))),((_tmpCC5->tl=decls,_tmpCC5))))));}{
struct _tuple23 _tmp776=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp74A,Cyc_Absyn_var_exp(v,0)),rgn,_tmp773,
Cyc_Absyn_var_exp(v,0),path,_tmp74B,fail_stmt,decls);
nv=_tmp776.f1;
decls=_tmp776.f2;{
struct Cyc_Absyn_Stmt*_tmp777=_tmp776.f3;
struct Cyc_Absyn_Stmt*_tmp778=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp778,_tmp777,0);
goto _LL37A;};};}_LL37D: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp74C=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp74C->tag != 3)goto _LL37F;else{_tmp74D=(_tmp74C->f2)->name;}}_LL37E: {
# 3830
struct _tuple0*_tmp779=Cyc_Toc_temp_var();
void*_tmp77A=(void*)_check_null(p->topt);
{struct _tuple24*_tmpCC9;struct Cyc_List_List*_tmpCC8;decls=((_tmpCC8=_region_malloc(rgn,sizeof(*_tmpCC8)),((_tmpCC8->hd=((_tmpCC9=_region_malloc(rgn,sizeof(*_tmpCC9)),((_tmpCC9->f1=_tmp779,((_tmpCC9->f2=Cyc_Toc_typ_to_c_array(_tmp77A),_tmpCC9)))))),((_tmpCC8->tl=decls,_tmpCC8))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp74D,Cyc_Absyn_var_exp(_tmp779,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp779,0),r,0);
goto _LL37A;}_LL37F: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp74E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp74E->tag != 0)goto _LL381;}_LL380:
 s=Cyc_Toc_skip_stmt_dl();goto _LL37A;_LL381: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp74F=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp74F->tag != 2)goto _LL383;else{_tmp750=_tmp74F->f1;_tmp751=_tmp74F->f2;}}_LL382: {
# 3839
struct _tuple0*_tmp77D=Cyc_Toc_temp_var();
{struct _tuple24*_tmpCCC;struct Cyc_List_List*_tmpCCB;decls=((_tmpCCB=_region_malloc(rgn,sizeof(*_tmpCCB)),((_tmpCCB->hd=((_tmpCCC=_region_malloc(rgn,sizeof(*_tmpCCC)),((_tmpCCC->f1=_tmp77D,((_tmpCCC->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCCC)))))),((_tmpCCB->tl=decls,_tmpCCB))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp750->name,Cyc_Absyn_var_exp(_tmp77D,0));
# 3843
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp77D,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp780=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp751,fail_stmt,decls);
_tmp780.f3=Cyc_Absyn_seq_stmt(s,_tmp780.f3,0);
return _tmp780;};}_LL383: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp752=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp752->tag != 8)goto _LL385;}_LL384:
# 3850
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL37A;_LL385: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp753=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp753->tag != 9)goto _LL387;else{_tmp754=_tmp753->f1;_tmp755=_tmp753->f2;}}_LL386:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp754,_tmp755,0),fail_stmt);goto _LL37A;_LL387: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp756=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp756->tag != 10)goto _LL389;else{_tmp757=_tmp756->f1;}}_LL388:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp757,0),fail_stmt);goto _LL37A;_LL389: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp758=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp758->tag != 11)goto _LL38B;else{_tmp759=_tmp758->f1;_tmp75A=_tmp758->f2;}}_LL38A:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp759,_tmp75A,0),fail_stmt);goto _LL37A;_LL38B: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp75B=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp75B->tag != 12)goto _LL38D;else{_tmp75C=_tmp75B->f1;_tmp75D=_tmp75B->f2;}}_LL38C:
# 3855
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCCF;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCCE;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((_tmpCCF.tag=30,((_tmpCCF.f1=_tmp75D->name,((_tmpCCF.f2=_tmp75C,((_tmpCCF.f3=_tmp75D,_tmpCCF)))))))),_tmpCCE)))),0),fail_stmt);}
goto _LL37A;_LL38D: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp75E=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp75E->tag != 13)goto _LL38F;else{_tmp75F=(void*)_tmp75E->f1;_tmp760=_tmp75E->f2;}}_LL38E:
# 3858
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCD2;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCD1;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD2.tag=31,((_tmpCD2.f1=_tmp760->name,((_tmpCD2.f2=_tmp75F,((_tmpCD2.f3=_tmp760,_tmpCD2)))))))),_tmpCD1)))),0),fail_stmt);}
goto _LL37A;_LL38F: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp761=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp761->tag != 5)goto _LL391;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp762=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp761->f1)->r;if(_tmp762->tag != 7)goto _LL391;else{_tmp763=_tmp762->f1;_tmp764=_tmp762->f2;_tmp765=_tmp762->f3;}}}_LL390:
# 3868
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp785=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp764->name,_tmp763->name);
void*_tmp786=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp786,r);
struct Cyc_List_List*_tmp787=_tmp764->typs;
for(0;_tmp765 != 0;(((_tmp765=_tmp765->tl,_tmp787=((struct Cyc_List_List*)_check_null(_tmp787))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp788=(struct Cyc_Absyn_Pat*)_tmp765->hd;
if(_tmp788->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp789=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp787))->hd)).f2;
void*_tmp78A=(void*)_check_null(_tmp788->topt);
void*_tmp78B=Cyc_Toc_typ_to_c_array(_tmp78A);
struct Cyc_Absyn_Exp*_tmp78C=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp789)))
_tmp78C=Cyc_Toc_cast_it(_tmp78B,_tmp78C);{
# 3886
struct _tuple23 _tmp78D=Cyc_Toc_xlate_pat(nv,rgn,_tmp78A,_tmp78C,_tmp78C,_tmp788,fail_stmt,decls);
# 3888
nv=_tmp78D.f1;
decls=_tmp78D.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp78D.f3,0);};};}{
# 3892
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp785,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp763->is_extensible?Cyc_Absyn_var_exp(_tmp764->name,0): Cyc_Toc_datatype_tag(_tmp763,_tmp764->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3900
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3903
s=Cyc_Absyn_declare_stmt(_tmp785,_tmp786,rcast,s,0);
goto _LL37A;};};_LL391: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp766=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp766->tag != 7)goto _LL393;else{_tmp767=_tmp766->f3;}}_LL392:
# 3906
 _tmp769=_tmp767;goto _LL394;_LL393: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp768=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp768->tag != 4)goto _LL395;else{_tmp769=_tmp768->f1;}}_LL394:
# 3914
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp769 != 0;(_tmp769=_tmp769->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp78E=(struct Cyc_Absyn_Pat*)_tmp769->hd;
if(_tmp78E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp78F=(void*)_check_null(_tmp78E->topt);
struct _dyneither_ptr*_tmp790=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp791=Cyc_Toc_xlate_pat(nv,rgn,_tmp78F,Cyc_Toc_member_exp(r,_tmp790,0),
Cyc_Toc_member_exp(path,_tmp790,0),_tmp78E,fail_stmt,decls);
nv=_tmp791.f1;
decls=_tmp791.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp791.f3,0);};}
# 3928
goto _LL37A;};_LL395: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp76A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp76A->tag != 6)goto _LL397;else{if(_tmp76A->f1 != 0)goto _LL397;}}_LL396: {
# 3930
const char*_tmpCD5;void*_tmpCD4;(_tmpCD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD5="unresolved aggregate pattern!",_tag_dyneither(_tmpCD5,sizeof(char),30))),_tag_dyneither(_tmpCD4,sizeof(void*),0)));}_LL397: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp76B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp76B->tag != 6)goto _LL399;else{if(_tmp76B->f1 == 0)goto _LL399;_tmp76C=(_tmp76B->f1)->aggr_info;_tmp76D=_tmp76B->f3;}}_LL398: {
# 3932
struct Cyc_Absyn_Aggrdecl*_tmp794=Cyc_Absyn_get_known_aggrdecl(_tmp76C);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp794->impl))->tagged){
# 3936
struct Cyc_List_List*_tmp796;struct Cyc_Absyn_Pat*_tmp797;struct _tuple25*_tmp795=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp76D))->hd;_tmp796=_tmp795->f1;_tmp797=_tmp795->f2;{
struct _dyneither_ptr*f;
{void*_tmp798=(void*)((struct Cyc_List_List*)_check_null(_tmp796))->hd;struct _dyneither_ptr*_tmp79A;_LL3A2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp799=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp798;if(_tmp799->tag != 1)goto _LL3A4;else{_tmp79A=_tmp799->f1;}}_LL3A3:
 f=_tmp79A;goto _LL3A1;_LL3A4:;_LL3A5: {
const char*_tmpCD8;void*_tmpCD7;(_tmpCD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD8="no field name in tagged union pattern",_tag_dyneither(_tmpCD8,sizeof(char),38))),_tag_dyneither(_tmpCD7,sizeof(void*),0)));}_LL3A1:;}{
# 3943
void*_tmp79D=(void*)_check_null(_tmp797->topt);
void*_tmp79E=Cyc_Toc_typ_to_c_array(_tmp79D);
# 3946
struct Cyc_Absyn_Exp*_tmp79F=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7A0=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7A0=Cyc_Toc_cast_it(_tmp79E,_tmp7A0);
_tmp79F=Cyc_Toc_cast_it(_tmp79E,_tmp79F);{
struct _tuple23 _tmp7A1=Cyc_Toc_xlate_pat(nv,rgn,_tmp79D,_tmp7A0,_tmp79F,_tmp797,fail_stmt,decls);
# 3954
nv=_tmp7A1.f1;
decls=_tmp7A1.f2;{
struct Cyc_Absyn_Stmt*_tmp7A2=_tmp7A1.f3;
struct Cyc_Absyn_Stmt*_tmp7A3=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp794,f),0),fail_stmt);
# 3962
s=Cyc_Absyn_seq_stmt(_tmp7A3,_tmp7A2,0);};};};};}else{
# 3965
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp76D != 0;_tmp76D=_tmp76D->tl){
struct _tuple25*_tmp7A4=(struct _tuple25*)_tmp76D->hd;
struct Cyc_Absyn_Pat*_tmp7A5=(*_tmp7A4).f2;
if(_tmp7A5->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7A6=(void*)((struct Cyc_List_List*)_check_null((*_tmp7A4).f1))->hd;struct _dyneither_ptr*_tmp7A8;_LL3A7: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7A7=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7A6;if(_tmp7A7->tag != 1)goto _LL3A9;else{_tmp7A8=_tmp7A7->f1;}}_LL3A8:
 f=_tmp7A8;goto _LL3A6;_LL3A9:;_LL3AA: {
struct Cyc_Toc_Match_error_exn_struct _tmpCDB;struct Cyc_Toc_Match_error_exn_struct*_tmpCDA;(int)_throw((void*)((_tmpCDA=_cycalloc_atomic(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCDB.tag=Cyc_Toc_Match_error,_tmpCDB)),_tmpCDA)))));}_LL3A6:;}{
# 3976
void*_tmp7AB=(void*)_check_null(_tmp7A5->topt);
void*_tmp7AC=Cyc_Toc_typ_to_c_array(_tmp7AB);
struct Cyc_Absyn_Exp*_tmp7AD=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp794->impl))->fields,f)))->type))
_tmp7AD=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp7AC),_tmp7AD);{
struct _tuple23 _tmp7AE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AB,_tmp7AD,
Cyc_Toc_member_exp(path,f,0),_tmp7A5,fail_stmt,decls);
nv=_tmp7AE.f1;
decls=_tmp7AE.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7AE.f3,0);};};};}}
# 3988
goto _LL37A;}_LL399: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp76E=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp76E->tag != 5)goto _LL39B;else{_tmp76F=_tmp76E->f1;}}_LL39A: {
# 3991
void*_tmp7AF=(void*)_check_null(_tmp76F->topt);
# 3994
struct _tuple23 _tmp7B0=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AF,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp76F,fail_stmt,decls);
# 3996
nv=_tmp7B0.f1;
decls=_tmp7B0.f2;{
struct Cyc_Absyn_Stmt*_tmp7B1=_tmp7B0.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4002
Cyc_Toc_skip_stmt_dl(),0),_tmp7B1,0);else{
# 4004
s=_tmp7B1;}
goto _LL37A;};}_LL39B: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp770=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp770->tag != 14)goto _LL39D;}_LL39C: {
const char*_tmpCDE;void*_tmpCDD;(_tmpCDD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDE="unknownid pat",_tag_dyneither(_tmpCDE,sizeof(char),14))),_tag_dyneither(_tmpCDD,sizeof(void*),0)));}_LL39D: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp771=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp771->tag != 15)goto _LL39F;}_LL39E: {
const char*_tmpCE1;void*_tmpCE0;(_tmpCE0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE1="unknowncall pat",_tag_dyneither(_tmpCE1,sizeof(char),16))),_tag_dyneither(_tmpCE0,sizeof(void*),0)));}_LL39F: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp772=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp748;if(_tmp772->tag != 16)goto _LL37A;}_LL3A0: {
const char*_tmpCE4;void*_tmpCE3;(_tmpCE3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE4="exp pat",_tag_dyneither(_tmpCE4,sizeof(char),8))),_tag_dyneither(_tmpCE3,sizeof(void*),0)));}_LL37A:;}{
# 4010
struct _tuple23 _tmpCE5;return(_tmpCE5.f1=nv,((_tmpCE5.f2=decls,((_tmpCE5.f3=s,_tmpCE5)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4047 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4049
struct _tuple26*_tmpCE6;return(_tmpCE6=_region_malloc(r,sizeof(*_tmpCE6)),((_tmpCE6->f1=Cyc_Toc_fresh_label(),((_tmpCE6->f2=Cyc_Toc_fresh_label(),((_tmpCE6->f3=sc,_tmpCE6)))))));}
# 4052
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4054
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7BA=(void*)_check_null(e->topt);
# 4059
int leave_as_switch;
{void*_tmp7BB=Cyc_Tcutil_compress(_tmp7BA);_LL3AC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BB;if(_tmp7BC->tag != 6)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7BD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7BB;if(_tmp7BD->tag != 13)goto _LL3B0;}_LL3AF:
 leave_as_switch=1;goto _LL3AB;_LL3B0:;_LL3B1:
 leave_as_switch=0;goto _LL3AB;_LL3AB:;}
# 4065
{struct Cyc_List_List*_tmp7BE=scs;for(0;_tmp7BE != 0;_tmp7BE=_tmp7BE->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7BE->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7BE->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4070
if(leave_as_switch){
# 4072
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7BF=scs;for(0;_tmp7BF != 0;_tmp7BF=_tmp7BF->tl){
struct Cyc_Absyn_Stmt*_tmp7C0=((struct Cyc_Absyn_Switch_clause*)_tmp7BF->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7BF->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7C0,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7C1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7C1;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7C0);;_pop_region(rgn);};}}
# 4081
return;}{
# 4084
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4089
struct _RegionHandle _tmp7C2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7C2;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7C3=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4100
{struct Cyc_List_List*_tmp7C4=lscs;for(0;_tmp7C4 != 0;_tmp7C4=_tmp7C4->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp7C4->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7C4->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp7C4->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp7C6;struct Cyc_List_List*_tmp7C7;struct Cyc_Absyn_Stmt*_tmp7C8;struct _tuple23 _tmp7C5=Cyc_Toc_xlate_pat(_tmp7C3,rgn,_tmp7BA,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp7C6=_tmp7C5.f1;_tmp7C7=_tmp7C5.f2;_tmp7C8=_tmp7C5.f3;
# 4106
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7C9=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7C6,_tmp7C9);
_tmp7C8=Cyc_Absyn_seq_stmt(_tmp7C8,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7C9,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4113
decls=_tmp7C7;
{struct Cyc_List_List*_tmpCE7;nvs=((_tmpCE7=_region_malloc(rgn,sizeof(*_tmpCE7)),((_tmpCE7->hd=_tmp7C6,((_tmpCE7->tl=nvs,_tmpCE7))))));}{
struct Cyc_List_List*_tmpCE8;test_stmts=((_tmpCE8=_region_malloc(rgn,sizeof(*_tmpCE8)),((_tmpCE8->hd=_tmp7C8,((_tmpCE8->tl=test_stmts,_tmpCE8))))));};}}
# 4117
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4122
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp7CD;struct _dyneither_ptr*_tmp7CE;struct Cyc_Absyn_Switch_clause*_tmp7CF;struct _tuple26*_tmp7CC=(struct _tuple26*)lscs->hd;_tmp7CD=_tmp7CC->f1;_tmp7CE=_tmp7CC->f2;_tmp7CF=_tmp7CC->f3;{
struct Cyc_Toc_Env*_tmp7D0=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7CF->body;
struct _RegionHandle _tmp7D1=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7D1;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp7D3;struct Cyc_Absyn_Switch_clause*_tmp7D4;struct _tuple26*_tmp7D2=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7D3=_tmp7D2->f2;_tmp7D4=_tmp7D2->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7D0,end_l,_tmp7D3,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7D4->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4135
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7D0,end_l),s);}
# 4137
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7CD,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7CE,s,0),0);{
struct Cyc_List_List*_tmpCE9;stmts=((_tmpCE9=_region_malloc(rgn,sizeof(*_tmpCE9)),((_tmpCE9->hd=s,((_tmpCE9->tl=stmts,_tmpCE9))))));};
# 4127
;_pop_region(rgn2);};}{
# 4141
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4144
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp7D7;void*_tmp7D8;struct _tuple24*_tmp7D6=(struct _tuple24*)decls->hd;_tmp7D7=_tmp7D6->f1;_tmp7D8=_tmp7D6->f2;
res=Cyc_Absyn_declare_stmt(_tmp7D7,_tmp7D8,0,res,0);}
# 4149
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4090
;_pop_region(rgn);};};}
# 4154
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4160
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4162
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpCEA;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpCEA->tl=0,_tmpCEA)))))),0),0);}
# 4166
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4171
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4173
while(1){
void*_tmp7DA=s->r;struct Cyc_Absyn_Exp*_tmp7DD;struct Cyc_Absyn_Stmt*_tmp7DF;struct Cyc_Absyn_Stmt*_tmp7E0;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Exp*_tmp7E4;struct Cyc_Absyn_Stmt*_tmp7E5;struct Cyc_Absyn_Stmt*_tmp7E6;struct Cyc_Absyn_Exp*_tmp7E8;struct Cyc_Absyn_Stmt*_tmp7E9;struct Cyc_Absyn_Stmt*_tmp7EB;struct Cyc_Absyn_Stmt*_tmp7ED;struct Cyc_Absyn_Stmt*_tmp7EF;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F2;struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Stmt*_tmp7F4;struct Cyc_Absyn_Exp*_tmp7F6;struct Cyc_List_List*_tmp7F7;struct Cyc_List_List*_tmp7F9;struct Cyc_Absyn_Switch_clause**_tmp7FA;struct Cyc_Absyn_Decl*_tmp7FC;struct Cyc_Absyn_Stmt*_tmp7FD;struct _dyneither_ptr*_tmp7FF;struct Cyc_Absyn_Stmt*_tmp800;struct Cyc_Absyn_Stmt*_tmp802;struct Cyc_Absyn_Exp*_tmp803;struct Cyc_Absyn_Stmt*_tmp805;struct Cyc_List_List*_tmp806;struct Cyc_Absyn_Exp*_tmp808;_LL3B3: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7DB=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7DB->tag != 0)goto _LL3B5;}_LL3B4:
# 4176
 return;_LL3B5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7DC=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7DC->tag != 1)goto _LL3B7;else{_tmp7DD=_tmp7DC->f1;}}_LL3B6:
# 4178
 Cyc_Toc_exp_to_c(nv,_tmp7DD);
return;_LL3B7: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7DE=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7DE->tag != 2)goto _LL3B9;else{_tmp7DF=_tmp7DE->f1;_tmp7E0=_tmp7DE->f2;}}_LL3B8:
# 4181
 Cyc_Toc_stmt_to_c(nv,_tmp7DF);
s=_tmp7E0;
continue;_LL3B9: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7E1=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7E1->tag != 3)goto _LL3BB;else{_tmp7E2=_tmp7E1->f1;}}_LL3BA: {
# 4185
void*topt=0;
if(_tmp7E2 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7E2->topt));
Cyc_Toc_exp_to_c(nv,_tmp7E2);}
# 4191
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp809=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp80A=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp809,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp809,topt,_tmp7E2,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp80A,0),0))->r;}else{
# 4199
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4201
return;}_LL3BB: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7E3=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7E3->tag != 4)goto _LL3BD;else{_tmp7E4=_tmp7E3->f1;_tmp7E5=_tmp7E3->f2;_tmp7E6=_tmp7E3->f3;}}_LL3BC:
# 4203
 Cyc_Toc_exp_to_c(nv,_tmp7E4);
Cyc_Toc_stmt_to_c(nv,_tmp7E5);
s=_tmp7E6;
continue;_LL3BD: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7E7=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7E7->tag != 5)goto _LL3BF;else{_tmp7E8=(_tmp7E7->f1).f1;_tmp7E9=_tmp7E7->f2;}}_LL3BE:
# 4208
 Cyc_Toc_exp_to_c(nv,_tmp7E8);{
struct _RegionHandle _tmp80B=_new_region("temp");struct _RegionHandle*temp=& _tmp80B;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E9);
# 4212
_npop_handler(0);return;
# 4209
;_pop_region(temp);};_LL3BF: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7EA=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7EA->tag != 6)goto _LL3C1;else{_tmp7EB=_tmp7EA->f1;}}_LL3C0: {
# 4214
struct _dyneither_ptr**_tmp80D;struct Cyc_Toc_Env*_tmp80C=nv;_tmp80D=_tmp80C->break_lab;
if(_tmp80D != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp80D,0);{
# 4218
int dest_depth=_tmp7EB == 0?0: _tmp7EB->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL3C1: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7EC=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7EC->tag != 7)goto _LL3C3;else{_tmp7ED=_tmp7EC->f1;}}_LL3C2: {
# 4222
struct _dyneither_ptr**_tmp80F;struct Cyc_Toc_Env*_tmp80E=nv;_tmp80F=_tmp80E->continue_lab;
if(_tmp80F != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp80F,0);
_tmp7EF=_tmp7ED;goto _LL3C4;}_LL3C3: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7EE=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7EE->tag != 8)goto _LL3C5;else{_tmp7EF=_tmp7EE->f2;}}_LL3C4:
# 4228
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7EF))->try_depth,s);
return;_LL3C5: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7F0=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7F0->tag != 9)goto _LL3C7;else{_tmp7F1=_tmp7F0->f1;_tmp7F2=(_tmp7F0->f2).f1;_tmp7F3=(_tmp7F0->f3).f1;_tmp7F4=_tmp7F0->f4;}}_LL3C6:
# 4232
 Cyc_Toc_exp_to_c(nv,_tmp7F1);Cyc_Toc_exp_to_c(nv,_tmp7F2);Cyc_Toc_exp_to_c(nv,_tmp7F3);{
struct _RegionHandle _tmp810=_new_region("temp");struct _RegionHandle*temp=& _tmp810;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7F4);
# 4236
_npop_handler(0);return;
# 4233
;_pop_region(temp);};_LL3C7: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7F5=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7F5->tag != 10)goto _LL3C9;else{_tmp7F6=_tmp7F5->f1;_tmp7F7=_tmp7F5->f2;}}_LL3C8:
# 4238
 Cyc_Toc_xlate_switch(nv,s,_tmp7F6,_tmp7F7);
return;_LL3C9: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7F8=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7F8->tag != 11)goto _LL3CB;else{_tmp7F9=_tmp7F8->f1;_tmp7FA=_tmp7F8->f2;}}_LL3CA: {
# 4241
struct Cyc_Toc_FallthruInfo*_tmp812;struct Cyc_Toc_Env*_tmp811=nv;_tmp812=_tmp811->fallthru_info;
if(_tmp812 == 0){
const char*_tmpCED;void*_tmpCEC;(_tmpCEC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCED="fallthru in unexpected place",_tag_dyneither(_tmpCED,sizeof(char),29))),_tag_dyneither(_tmpCEC,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp816;struct Cyc_List_List*_tmp817;struct Cyc_Dict_Dict _tmp818;struct Cyc_Toc_FallthruInfo _tmp815=*_tmp812;_tmp816=_tmp815.label;_tmp817=_tmp815.binders;_tmp818=_tmp815.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp816,0);
# 4247
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7FA)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp819=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp817);
struct Cyc_List_List*_tmp81A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7F9);
for(0;_tmp819 != 0;(_tmp819=_tmp819->tl,_tmp81A=_tmp81A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp81A))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp818,(struct _tuple0*)_tmp819->hd),(struct Cyc_Absyn_Exp*)_tmp81A->hd,0),s2,0);}
# 4255
s->r=s2->r;
return;};};};}_LL3CB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7FB=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7FB->tag != 12)goto _LL3CD;else{_tmp7FC=_tmp7FB->f1;_tmp7FD=_tmp7FB->f2;}}_LL3CC:
# 4261
{void*_tmp81B=_tmp7FC->r;struct Cyc_Absyn_Vardecl*_tmp81D;struct Cyc_Absyn_Pat*_tmp81F;struct Cyc_Absyn_Exp*_tmp820;struct Cyc_List_List*_tmp822;struct Cyc_Absyn_Fndecl*_tmp824;struct Cyc_Absyn_Tvar*_tmp826;struct Cyc_Absyn_Vardecl*_tmp827;int _tmp828;struct Cyc_Absyn_Exp*_tmp829;struct Cyc_Absyn_Tvar*_tmp82B;struct Cyc_Absyn_Vardecl*_tmp82C;_LL3D6: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp81C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp81B;if(_tmp81C->tag != 0)goto _LL3D8;else{_tmp81D=_tmp81C->f1;}}_LL3D7: {
# 4263
struct _RegionHandle _tmp82D=_new_region("temp");struct _RegionHandle*temp=& _tmp82D;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCF0;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCEF;struct Cyc_Toc_Env*_tmp82E=Cyc_Toc_add_varmap(temp,nv,_tmp81D->name,
Cyc_Absyn_varb_exp(_tmp81D->name,(void*)((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF[0]=((_tmpCF0.tag=4,((_tmpCF0.f1=_tmp81D,_tmpCF0)))),_tmpCEF)))),0));
Cyc_Toc_local_decl_to_c(_tmp82E,_tmp82E,_tmp81D,_tmp7FD);}
# 4268
_npop_handler(0);goto _LL3D5;
# 4263
;_pop_region(temp);}_LL3D8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp81E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp81B;if(_tmp81E->tag != 2)goto _LL3DA;else{_tmp81F=_tmp81E->f1;_tmp820=_tmp81E->f3;}}_LL3D9:
# 4272
{void*_tmp831=_tmp81F->r;struct Cyc_Absyn_Vardecl*_tmp833;_LL3E5: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp832=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp831;if(_tmp832->tag != 1)goto _LL3E7;else{_tmp833=_tmp832->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp834=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp832->f2)->r;if(_tmp834->tag != 0)goto _LL3E7;};}}_LL3E6: {
# 4274
struct _tuple0*old_name=_tmp833->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp833->name=new_name;
_tmp833->initializer=_tmp820;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCF3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCF2;_tmp7FC->r=(void*)((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2[0]=((_tmpCF3.tag=0,((_tmpCF3.f1=_tmp833,_tmpCF3)))),_tmpCF2))));}{
struct _RegionHandle _tmp837=_new_region("temp");struct _RegionHandle*temp=& _tmp837;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCF6;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCF5;struct Cyc_Toc_Env*_tmp838=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5[0]=((_tmpCF6.tag=4,((_tmpCF6.f1=_tmp833,_tmpCF6)))),_tmpCF5)))),0));
Cyc_Toc_local_decl_to_c(_tmp838,nv,_tmp833,_tmp7FD);}
# 4285
_npop_handler(0);goto _LL3E4;
# 4279
;_pop_region(temp);};}_LL3E7:;_LL3E8:
# 4290
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp81F,(void*)_check_null(_tmp820->topt),_tmp820,_tmp7FD))->r;
goto _LL3E4;_LL3E4:;}
# 4293
goto _LL3D5;_LL3DA: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp821=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp81B;if(_tmp821->tag != 3)goto _LL3DC;else{_tmp822=_tmp821->f1;}}_LL3DB: {
# 4306 "toc.cyc"
struct Cyc_List_List*_tmp83B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp822);
if(_tmp83B == 0){
const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF9="empty Letv_d",_tag_dyneither(_tmpCF9,sizeof(char),13))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCFC;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCFB;_tmp7FC->r=(void*)((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=0,((_tmpCFC.f1=(struct Cyc_Absyn_Vardecl*)_tmp83B->hd,_tmpCFC)))),_tmpCFB))));}
_tmp83B=_tmp83B->tl;
for(0;_tmp83B != 0;_tmp83B=_tmp83B->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCFF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCFE;struct Cyc_Absyn_Decl*_tmp840=Cyc_Absyn_new_decl((void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=0,((_tmpCFF.f1=(struct Cyc_Absyn_Vardecl*)_tmp83B->hd,_tmpCFF)))),_tmpCFE)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp840,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4315
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3D5;}_LL3DC: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp823=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp81B;if(_tmp823->tag != 1)goto _LL3DE;else{_tmp824=_tmp823->f1;}}_LL3DD: {
# 4318
struct _tuple0*_tmp843=_tmp824->name;
struct _RegionHandle _tmp844=_new_region("temp");struct _RegionHandle*temp=& _tmp844;_push_region(temp);{
struct Cyc_Toc_Env*_tmp845=Cyc_Toc_add_varmap(temp,nv,_tmp824->name,Cyc_Absyn_var_exp(_tmp843,0));
Cyc_Toc_fndecl_to_c(_tmp845,_tmp824,0);
Cyc_Toc_stmt_to_c(_tmp845,_tmp7FD);}
# 4324
_npop_handler(0);goto _LL3D5;
# 4319
;_pop_region(temp);}_LL3DE: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp825=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp81B;if(_tmp825->tag != 4)goto _LL3E0;else{_tmp826=_tmp825->f1;_tmp827=_tmp825->f2;_tmp828=_tmp825->f3;_tmp829=_tmp825->f4;}}_LL3DF: {
# 4326
struct Cyc_Absyn_Stmt*_tmp846=_tmp7FD;
# 4342 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp827->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4349
struct _RegionHandle _tmp847=_new_region("temp");struct _RegionHandle*temp=& _tmp847;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp846);
# 4352
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp846,0))->r;else{
if(_tmp829 == 0){
struct Cyc_Absyn_Exp*_tmpD02[1];struct Cyc_Absyn_Exp*_tmpD01[1];struct Cyc_List_List*_tmpD00;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD00->tl=0,_tmpD00)))))),0),
# 4361
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD01[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD01,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp846,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD02[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD02,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4367
Cyc_Toc_exp_to_c(nv,_tmp829);{
struct Cyc_Absyn_Exp*_tmpD04[1];struct Cyc_Absyn_Exp*_tmpD03[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpD03[1]=_tmp829,((_tmpD03[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD03,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4375
Cyc_Absyn_seq_stmt(_tmp846,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpD04[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD04,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4380
_npop_handler(0);return;
# 4349
;_pop_region(temp);}_LL3E0: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp82A=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp81B;if(_tmp82A->tag != 5)goto _LL3E2;else{_tmp82B=_tmp82A->f1;_tmp82C=_tmp82A->f2;}}_LL3E1: {
# 4383
struct _tuple0*old_name=_tmp82C->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp82C->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD13;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD12;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD11;struct Cyc_Absyn_Decl*_tmpD10;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD0F;s->r=(void*)((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F[0]=((_tmpD13.tag=12,((_tmpD13.f1=((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->r=(void*)((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12[0]=((_tmpD11.tag=0,((_tmpD11.f1=_tmp82C,_tmpD11)))),_tmpD12)))),((_tmpD10->loc=0,_tmpD10)))))),((_tmpD13.f2=_tmp7FD,_tmpD13)))))),_tmpD0F))));}{
struct _RegionHandle _tmp852=_new_region("temp");struct _RegionHandle*temp=& _tmp852;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD16;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD15;struct Cyc_Toc_Env*_tmp853=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15[0]=((_tmpD16.tag=4,((_tmpD16.f1=_tmp82C,_tmpD16)))),_tmpD15)))),0));
Cyc_Toc_local_decl_to_c(_tmp853,nv,_tmp82C,_tmp7FD);}
# 4393
_npop_handler(0);return;
# 4387
;_pop_region(temp);};}_LL3E2:;_LL3E3: {
# 4395
const char*_tmpD19;void*_tmpD18;(_tmpD18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD19="bad nested declaration within function",_tag_dyneither(_tmpD19,sizeof(char),39))),_tag_dyneither(_tmpD18,sizeof(void*),0)));}_LL3D5:;}
# 4397
return;_LL3CD: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7FE=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp7FE->tag != 13)goto _LL3CF;else{_tmp7FF=_tmp7FE->f1;_tmp800=_tmp7FE->f2;}}_LL3CE:
# 4399
 s=_tmp800;continue;_LL3CF: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp801=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp801->tag != 14)goto _LL3D1;else{_tmp802=_tmp801->f1;_tmp803=(_tmp801->f2).f1;}}_LL3D0: {
# 4401
struct _RegionHandle _tmp858=_new_region("temp");struct _RegionHandle*temp=& _tmp858;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp802);
Cyc_Toc_exp_to_c(nv,_tmp803);
# 4405
_npop_handler(0);return;
# 4401
;_pop_region(temp);}_LL3D1: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp804=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp804->tag != 15)goto _LL3D3;else{_tmp805=_tmp804->f1;_tmp806=_tmp804->f2;}}_LL3D2: {
# 4422 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4433
e_exp->topt=e_typ;{
struct _RegionHandle _tmp859=_new_region("temp");struct _RegionHandle*temp=& _tmp859;_push_region(temp);{
struct Cyc_Toc_Env*_tmp85A=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4437
Cyc_Toc_stmt_to_c(_tmp85A,_tmp805);{
struct Cyc_Absyn_Stmt*_tmp85B=Cyc_Absyn_seq_stmt(_tmp805,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4443
struct _tuple0*_tmp85C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp85D=Cyc_Absyn_var_exp(_tmp85C,0);
struct Cyc_Absyn_Vardecl*_tmp85E=Cyc_Absyn_new_vardecl(_tmp85C,Cyc_Absyn_exn_typ(),0);
_tmp85D->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD23;struct Cyc_Absyn_Pat*_tmpD22;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD21;struct Cyc_Absyn_Pat*_tmpD20;struct Cyc_Absyn_Pat*_tmp85F=(_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->r=(void*)((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23[0]=((_tmpD21.tag=1,((_tmpD21.f1=_tmp85E,((_tmpD21.f2=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD22->topt=Cyc_Absyn_exn_typ(),((_tmpD22->loc=0,_tmpD22)))))))),_tmpD21)))))),_tmpD23)))),((_tmpD20->topt=Cyc_Absyn_exn_typ(),((_tmpD20->loc=0,_tmpD20)))))));
struct Cyc_Absyn_Exp*_tmp860=Cyc_Absyn_throw_exp(_tmp85D,0);
_tmp860->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp861=Cyc_Absyn_exp_stmt(_tmp860,0);
struct Cyc_Core_Opt*_tmpD29;struct Cyc_List_List*_tmpD28;struct Cyc_Absyn_Switch_clause*_tmpD27;struct Cyc_Absyn_Switch_clause*_tmp862=
(_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->pattern=_tmp85F,((_tmpD27->pat_vars=((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->v=((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->hd=_tmp85E,((_tmpD28->tl=0,_tmpD28)))))),_tmpD29)))),((_tmpD27->where_clause=0,((_tmpD27->body=_tmp861,((_tmpD27->loc=0,_tmpD27)))))))))));
# 4454
struct Cyc_List_List*_tmpD2A;struct Cyc_Absyn_Stmt*_tmp863=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp806,(
(_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->hd=_tmp862,((_tmpD2A->tl=0,_tmpD2A))))))),0);
# 4458
Cyc_Toc_stmt_to_c(_tmp85A,_tmp863);{
# 4461
struct Cyc_List_List*_tmpD2B;struct Cyc_Absyn_Exp*_tmp864=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD2B->tl=0,_tmpD2B)))))),0);
# 4465
struct Cyc_List_List*_tmpD2C;struct Cyc_Absyn_Stmt*_tmp865=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD2C->tl=0,_tmpD2C)))))),0),0);
# 4469
struct Cyc_Absyn_Exp*_tmp866=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp865,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp866,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp864,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp867,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp85B,
# 4480
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp863,0),0),0),0),0),0))->r;};};};};}
# 4484
_npop_handler(0);return;
# 4434
;_pop_region(temp);};}_LL3D3: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp807=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7DA;if(_tmp807->tag != 16)goto _LL3B2;else{_tmp808=_tmp807->f1;}}_LL3D4:
# 4486
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4489
Cyc_Toc_exp_to_c(nv,_tmp808);{
struct Cyc_List_List*_tmpD2D;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=_tmp808,((_tmpD2D->tl=0,_tmpD2D)))))),0));};}
# 4492
return;_LL3B2:;}}
# 4501
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp873=_new_region("frgn");struct _RegionHandle*frgn=& _tmp873;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp874=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp875=f->args;for(0;_tmp875 != 0;_tmp875=_tmp875->tl){
struct _tuple0*_tmpD2E;struct _tuple0*_tmp876=(_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E->f1=Cyc_Absyn_Loc_n,((_tmpD2E->f2=(*((struct _tuple8*)_tmp875->hd)).f1,_tmpD2E)))));
(*((struct _tuple8*)_tmp875->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp875->hd)).f3);
_tmp874=Cyc_Toc_add_varmap(frgn,_tmp874,_tmp876,Cyc_Absyn_var_exp(_tmp876,0));}}
# 4516
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4520
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp879;struct Cyc_Absyn_Tqual _tmp87A;void*_tmp87B;int _tmp87C;struct Cyc_Absyn_VarargInfo _tmp878=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp879=_tmp878.name;_tmp87A=_tmp878.tq;_tmp87B=_tmp878.type;_tmp87C=_tmp878.inject;{
void*_tmp87D=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp87B,(void*)& Cyc_Absyn_HeapRgn_val,_tmp87A,Cyc_Absyn_false_conref));
struct _tuple0*_tmpD2F;struct _tuple0*_tmp87E=(_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->f1=Cyc_Absyn_Loc_n,((_tmpD2F->f2=(struct _dyneither_ptr*)_check_null(_tmp879),_tmpD2F)))));
{struct _tuple8*_tmpD32;struct Cyc_List_List*_tmpD31;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->hd=((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32->f1=_tmp879,((_tmpD32->f2=_tmp87A,((_tmpD32->f3=_tmp87D,_tmpD32)))))))),((_tmpD31->tl=0,_tmpD31)))))));}
_tmp874=Cyc_Toc_add_varmap(frgn,_tmp874,_tmp87E,Cyc_Absyn_var_exp(_tmp87E,0));
f->cyc_varargs=0;};}
# 4529
{struct Cyc_List_List*_tmp882=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp882 != 0;_tmp882=_tmp882->tl){
((struct Cyc_Absyn_Vardecl*)_tmp882->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp882->hd)->type);}}
# 4532
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp874),f->body);}
# 4508
;_pop_region(frgn);};}
# 4537
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3E9:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3EA:
 return Cyc_Absyn_Extern;default: _LL3EB:
 return s;}}
# 4553 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp883=ad->name;
struct _DynRegionHandle*_tmp885;struct Cyc_Dict_Dict*_tmp886;struct Cyc_Toc_TocState _tmp884=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp885=_tmp884.dyn;_tmp886=_tmp884.aggrs_so_far;{
struct _DynRegionFrame _tmp887;struct _RegionHandle*d=_open_dynregion(& _tmp887,_tmp885);
{int seen_defn_before;
struct _tuple15**_tmp888=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp886,_tmp883);
if(_tmp888 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpD33;v=((_tmpD33=_region_malloc(d,sizeof(*_tmpD33)),((_tmpD33->f1=ad,((_tmpD33->f2=Cyc_Absyn_strctq(_tmp883),_tmpD33))))));}else{
# 4565
struct _tuple15*_tmpD34;v=((_tmpD34=_region_malloc(d,sizeof(*_tmpD34)),((_tmpD34->f1=ad,((_tmpD34->f2=Cyc_Absyn_unionq_typ(_tmp883),_tmpD34))))));}
*_tmp886=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp886,_tmp883,v);};}else{
# 4568
struct Cyc_Absyn_Aggrdecl*_tmp88C;void*_tmp88D;struct _tuple15*_tmp88B=*_tmp888;_tmp88C=_tmp88B->f1;_tmp88D=_tmp88B->f2;
if(_tmp88C->impl == 0){
{struct _tuple15*_tmpD35;*_tmp886=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp886,_tmp883,((_tmpD35=_region_malloc(d,sizeof(*_tmpD35)),((_tmpD35->f1=ad,((_tmpD35->f2=_tmp88D,_tmpD35)))))));}
seen_defn_before=0;}else{
# 4573
seen_defn_before=1;}}{
# 4575
struct Cyc_Absyn_Aggrdecl*_tmpD36;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->kind=ad->kind,((_tmpD36->sc=Cyc_Absyn_Public,((_tmpD36->name=ad->name,((_tmpD36->tvs=0,((_tmpD36->impl=0,((_tmpD36->attributes=ad->attributes,_tmpD36)))))))))))));
# 4581
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD37;new_ad->impl=((_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->exist_vars=0,((_tmpD37->rgn_po=0,((_tmpD37->fields=0,((_tmpD37->tagged=0,_tmpD37))))))))));}{
# 4586
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp890=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp890 != 0;_tmp890=_tmp890->tl){
# 4590
struct Cyc_Absyn_Aggrfield*_tmp891=(struct Cyc_Absyn_Aggrfield*)_tmp890->hd;
void*_tmp892=_tmp891->type;
if((ad->kind == Cyc_Absyn_StructA  && _tmp890->tl == 0) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp892))){
# 4599
void*_tmp893=Cyc_Tcutil_compress(_tmp892);void*_tmp895;struct Cyc_Absyn_Tqual _tmp896;union Cyc_Absyn_Constraint*_tmp897;unsigned int _tmp898;_LL3EE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp894=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp893;if(_tmp894->tag != 8)goto _LL3F0;else{_tmp895=(_tmp894->f1).elt_type;_tmp896=(_tmp894->f1).tq;_tmp897=(_tmp894->f1).zero_term;_tmp898=(_tmp894->f1).zt_loc;}}_LL3EF:
# 4601
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD3D;struct Cyc_Absyn_ArrayInfo _tmpD3C;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD3B;_tmp892=(void*)((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B[0]=((_tmpD3D.tag=8,((_tmpD3D.f1=((_tmpD3C.elt_type=_tmp895,((_tmpD3C.tq=_tmp896,((_tmpD3C.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD3C.zero_term=_tmp897,((_tmpD3C.zt_loc=_tmp898,_tmpD3C)))))))))),_tmpD3D)))),_tmpD3B))));}
goto _LL3ED;_LL3F0:;_LL3F1:
# 4604
 if(Cyc_Toc_array_or_aggr_with_array(_tmp892))goto _LL3ED;else{
continue;}_LL3ED:;}{
# 4608
struct Cyc_Absyn_Aggrfield*_tmpD3E;struct Cyc_Absyn_Aggrfield*_tmp89C=(_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->name=_tmp891->name,((_tmpD3E->tq=Cyc_Toc_mt_tq,((_tmpD3E->type=
# 4610
Cyc_Toc_typ_to_c(_tmp892),((_tmpD3E->width=_tmp891->width,((_tmpD3E->attributes=_tmp891->attributes,_tmpD3E)))))))))));
# 4616
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp89D=_tmp89C->type;
struct _dyneither_ptr*_tmp89E=_tmp89C->name;
const char*_tmpD43;void*_tmpD42[2];struct Cyc_String_pa_PrintArg_struct _tmpD41;struct Cyc_String_pa_PrintArg_struct _tmpD40;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD40.tag=0,((_tmpD40.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp89E),((_tmpD41.tag=0,((_tmpD41.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD42[0]=& _tmpD41,((_tmpD42[1]=& _tmpD40,Cyc_aprintf(((_tmpD43="_union_%s_%s",_tag_dyneither(_tmpD43,sizeof(char),13))),_tag_dyneither(_tmpD42,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD44;struct _dyneither_ptr*str=(_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44[0]=s,_tmpD44)));
struct Cyc_Absyn_Aggrfield*_tmpD45;struct Cyc_Absyn_Aggrfield*_tmp89F=(_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->name=Cyc_Toc_val_sp,((_tmpD45->tq=Cyc_Toc_mt_tq,((_tmpD45->type=_tmp89D,((_tmpD45->width=0,((_tmpD45->attributes=0,_tmpD45)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD46;struct Cyc_Absyn_Aggrfield*_tmp8A0=(_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->name=Cyc_Toc_tag_sp,((_tmpD46->tq=Cyc_Toc_mt_tq,((_tmpD46->type=Cyc_Absyn_sint_typ,((_tmpD46->width=0,((_tmpD46->attributes=0,_tmpD46)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD47[2];struct Cyc_List_List*_tmp8A1=(_tmpD47[1]=_tmp89F,((_tmpD47[0]=_tmp8A0,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD47,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD4C;struct _tuple0*_tmpD4B;struct Cyc_Absyn_Aggrdecl*_tmpD4A;struct Cyc_Absyn_Aggrdecl*_tmp8A2=(_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->kind=Cyc_Absyn_StructA,((_tmpD4A->sc=Cyc_Absyn_Public,((_tmpD4A->name=(
(_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B->f1=Cyc_Absyn_Loc_n,((_tmpD4B->f2=str,_tmpD4B)))))),((_tmpD4A->tvs=0,((_tmpD4A->impl=(
(_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C->exist_vars=0,((_tmpD4C->rgn_po=0,((_tmpD4C->fields=_tmp8A1,((_tmpD4C->tagged=0,_tmpD4C)))))))))),((_tmpD4A->attributes=0,_tmpD4A)))))))))))));
# 4628
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD52;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD51;struct Cyc_List_List*_tmpD50;Cyc_Toc_result_decls=((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->hd=Cyc_Absyn_new_decl((void*)((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD51.tag=6,((_tmpD51.f1=_tmp8A2,_tmpD51)))),_tmpD52)))),0),((_tmpD50->tl=Cyc_Toc_result_decls,_tmpD50))))));}
_tmp89C->type=Cyc_Absyn_strct(str);}{
# 4631
struct Cyc_List_List*_tmpD53;new_fields=((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->hd=_tmp89C,((_tmpD53->tl=new_fields,_tmpD53))))));};};}}
# 4633
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4635
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD5D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD5C;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD5B;struct Cyc_List_List*_tmpD5A;Cyc_Toc_result_decls=((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->hd=((_tmpD5D=_cycalloc(sizeof(*_tmpD5D)),((_tmpD5D->r=(void*)((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=((_tmpD5C.tag=6,((_tmpD5C.f1=new_ad,_tmpD5C)))),_tmpD5B)))),((_tmpD5D->loc=0,_tmpD5D)))))),((_tmpD5A->tl=Cyc_Toc_result_decls,_tmpD5A))))));}};}
# 4557
;_pop_dynregion(d);};}
# 4664 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8B9;struct Cyc_Set_Set**_tmp8BA;struct Cyc_Toc_TocState _tmp8B8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B9=_tmp8B8.dyn;_tmp8BA=_tmp8B8.datatypes_so_far;{
struct _DynRegionFrame _tmp8BB;struct _RegionHandle*d=_open_dynregion(& _tmp8BB,_tmp8B9);{
struct _tuple0*_tmp8BC=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp8BA,_tmp8BC)){
_npop_handler(0);return;}
*_tmp8BA=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp8BA,_tmp8BC);}{
# 4672
struct Cyc_List_List*_tmp8BD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8BD != 0;_tmp8BD=_tmp8BD->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8BD->hd;
# 4675
struct Cyc_List_List*_tmp8BE=0;
int i=1;
{struct Cyc_List_List*_tmp8BF=f->typs;for(0;_tmp8BF != 0;(_tmp8BF=_tmp8BF->tl,i ++)){
struct _dyneither_ptr*_tmp8C0=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD5E;struct Cyc_Absyn_Aggrfield*_tmp8C1=(_tmpD5E=_cycalloc(sizeof(*_tmpD5E)),((_tmpD5E->name=_tmp8C0,((_tmpD5E->tq=(*((struct _tuple10*)_tmp8BF->hd)).f1,((_tmpD5E->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8BF->hd)).f2),((_tmpD5E->width=0,((_tmpD5E->attributes=0,_tmpD5E)))))))))));
struct Cyc_List_List*_tmpD5F;_tmp8BE=((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->hd=_tmp8C1,((_tmpD5F->tl=_tmp8BE,_tmpD5F))))));}}
# 4683
{struct Cyc_Absyn_Aggrfield*_tmpD62;struct Cyc_List_List*_tmpD61;_tmp8BE=((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61->hd=((_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->name=Cyc_Toc_tag_sp,((_tmpD62->tq=Cyc_Toc_mt_tq,((_tmpD62->type=Cyc_Absyn_sint_typ,((_tmpD62->width=0,((_tmpD62->attributes=0,_tmpD62)))))))))))),((_tmpD61->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8BE),_tmpD61))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD65;struct Cyc_Absyn_Aggrdecl*_tmpD64;struct Cyc_Absyn_Aggrdecl*_tmp8C6=
(_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->kind=Cyc_Absyn_StructA,((_tmpD64->sc=Cyc_Absyn_Public,((_tmpD64->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD64->tvs=0,((_tmpD64->impl=(
# 4689
(_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->exist_vars=0,((_tmpD65->rgn_po=0,((_tmpD65->fields=_tmp8BE,((_tmpD65->tagged=0,_tmpD65)))))))))),((_tmpD64->attributes=0,_tmpD64)))))))))))));
# 4691
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD6B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD6A;struct Cyc_List_List*_tmpD69;Cyc_Toc_result_decls=((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->hd=Cyc_Absyn_new_decl((void*)((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B[0]=((_tmpD6A.tag=6,((_tmpD6A.f1=_tmp8C6,_tmpD6A)))),_tmpD6B)))),0),((_tmpD69->tl=Cyc_Toc_result_decls,_tmpD69))))));};}};
# 4666
;_pop_dynregion(d);};}
# 4712 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp8CD;struct Cyc_Dict_Dict*_tmp8CE;struct Cyc_Toc_TocState _tmp8CC=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8CD=_tmp8CC.dyn;_tmp8CE=_tmp8CC.xdatatypes_so_far;{
struct _DynRegionFrame _tmp8CF;struct _RegionHandle*d=_open_dynregion(& _tmp8CF,_tmp8CD);
{struct _tuple0*_tmp8D0=xd->name;
struct Cyc_List_List*_tmp8D1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp8D1 != 0;_tmp8D1=_tmp8D1->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8D1->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8D2=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8D3=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8D2,Cyc_Absyn_false_conref,0);
# 4724
int*_tmp8D4=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8CE,f->name);_LL3F3: if(_tmp8D4 != 0)goto _LL3F5;_LL3F4: {
# 4726
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4730
struct Cyc_Absyn_Vardecl*_tmp8D5=Cyc_Absyn_new_vardecl(f->name,_tmp8D3,initopt);
_tmp8D5->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD71;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD70;struct Cyc_List_List*_tmpD6F;Cyc_Toc_result_decls=((_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->hd=Cyc_Absyn_new_decl((void*)((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD70.tag=0,((_tmpD70.f1=_tmp8D5,_tmpD70)))),_tmpD71)))),0),((_tmpD6F->tl=Cyc_Toc_result_decls,_tmpD6F))))));}
*_tmp8CE=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8CE,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8D9=f->typs;for(0;_tmp8D9 != 0;(_tmp8D9=_tmp8D9->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD79;void*_tmpD78[1];const char*_tmpD77;struct _dyneither_ptr*_tmpD76;struct _dyneither_ptr*_tmp8DA=(_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76[0]=(struct _dyneither_ptr)((_tmpD79.tag=1,((_tmpD79.f1=(unsigned long)i,((_tmpD78[0]=& _tmpD79,Cyc_aprintf(((_tmpD77="f%d",_tag_dyneither(_tmpD77,sizeof(char),4))),_tag_dyneither(_tmpD78,sizeof(void*),1)))))))),_tmpD76)));
struct Cyc_Absyn_Aggrfield*_tmpD7A;struct Cyc_Absyn_Aggrfield*_tmp8DB=(_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->name=_tmp8DA,((_tmpD7A->tq=(*((struct _tuple10*)_tmp8D9->hd)).f1,((_tmpD7A->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8D9->hd)).f2),((_tmpD7A->width=0,((_tmpD7A->attributes=0,_tmpD7A)))))))))));
struct Cyc_List_List*_tmpD7B;fields=((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->hd=_tmp8DB,((_tmpD7B->tl=fields,_tmpD7B))))));}}
# 4743
{struct Cyc_Absyn_Aggrfield*_tmpD7E;struct Cyc_List_List*_tmpD7D;fields=((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D->hd=((_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->name=Cyc_Toc_tag_sp,((_tmpD7E->tq=Cyc_Toc_mt_tq,((_tmpD7E->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD7E->width=0,((_tmpD7E->attributes=0,_tmpD7E)))))))))))),((_tmpD7D->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD7D))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD81;struct Cyc_Absyn_Aggrdecl*_tmpD80;struct Cyc_Absyn_Aggrdecl*_tmp8E4=
(_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->kind=Cyc_Absyn_StructA,((_tmpD80->sc=Cyc_Absyn_Public,((_tmpD80->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD80->tvs=0,((_tmpD80->impl=(
# 4750
(_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->exist_vars=0,((_tmpD81->rgn_po=0,((_tmpD81->fields=fields,((_tmpD81->tagged=0,_tmpD81)))))))))),((_tmpD80->attributes=0,_tmpD80)))))))))))));
# 4752
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD87;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD86;struct Cyc_List_List*_tmpD85;Cyc_Toc_result_decls=((_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->hd=Cyc_Absyn_new_decl((void*)((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87[0]=((_tmpD86.tag=6,((_tmpD86.f1=_tmp8E4,_tmpD86)))),_tmpD87)))),0),((_tmpD85->tl=Cyc_Toc_result_decls,_tmpD85))))));}
# 4754
goto _LL3F2;};};};}_LL3F5: if(_tmp8D4 == 0)goto _LL3F7;if(*_tmp8D4 != 0)goto _LL3F7;_LL3F6:
# 4756
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8EA=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8EB=Cyc_Absyn_new_vardecl(f->name,_tmp8D3,_tmp8EA);
_tmp8EB->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD8D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD8C;struct Cyc_List_List*_tmpD8B;Cyc_Toc_result_decls=((_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->hd=Cyc_Absyn_new_decl((void*)((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8C.tag=0,((_tmpD8C.f1=_tmp8EB,_tmpD8C)))),_tmpD8D)))),0),((_tmpD8B->tl=Cyc_Toc_result_decls,_tmpD8B))))));}
*_tmp8CE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8CE,f->name,1);}
# 4763
goto _LL3F2;_LL3F7:;_LL3F8:
 goto _LL3F2;_LL3F2:;}}
# 4717
;_pop_dynregion(d);};};}
# 4770
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4778
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4782
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8EF=init->r;struct Cyc_Absyn_Vardecl*_tmp8F1;struct Cyc_Absyn_Exp*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F3;int _tmp8F4;_LL3FA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8F0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8EF;if(_tmp8F0->tag != 26)goto _LL3FC;else{_tmp8F1=_tmp8F0->f1;_tmp8F2=_tmp8F0->f2;_tmp8F3=_tmp8F0->f3;_tmp8F4=_tmp8F0->f4;}}_LL3FB:
# 4789
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp8F1,_tmp8F2,_tmp8F3,_tmp8F4,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3F9;_LL3FC:;_LL3FD:
# 4794
 if(vd->sc == Cyc_Absyn_Static){
# 4798
struct _RegionHandle _tmp8F5=_new_region("temp");struct _RegionHandle*temp=& _tmp8F5;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8F6=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8F6,init);}
# 4799
;_pop_region(temp);}else{
# 4803
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3F9;_LL3F9:;}else{
# 4808
void*_tmp8F7=Cyc_Tcutil_compress(old_typ);void*_tmp8F9;struct Cyc_Absyn_Exp*_tmp8FA;union Cyc_Absyn_Constraint*_tmp8FB;_LL3FF:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8F7;if(_tmp8F8->tag != 8)goto _LL401;else{_tmp8F9=(_tmp8F8->f1).elt_type;_tmp8FA=(_tmp8F8->f1).num_elts;_tmp8FB=(_tmp8F8->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8FB))goto _LL401;_LL400:
 if(_tmp8FA == 0){
const char*_tmpD90;void*_tmpD8F;(_tmpD8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD90="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD90,sizeof(char),55))),_tag_dyneither(_tmpD8F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8FA;
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4816
struct Cyc_Absyn_Exp*_tmp8FF=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8FE,_tmp8FF,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3FE;};_LL401:;_LL402:
 goto _LL3FE;_LL3FE:;}}
# 4830
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp900=_new_region("prgn");struct _RegionHandle*prgn=& _tmp900;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp901=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp902=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp904;struct Cyc_List_List*_tmp905;struct Cyc_Absyn_Stmt*_tmp906;struct _tuple23 _tmp903=
Cyc_Toc_xlate_pat(_tmp902,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp904=_tmp903.f1;_tmp905=_tmp903.f2;_tmp906=_tmp903.f3;
Cyc_Toc_stmt_to_c(_tmp904,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp906,s,0),0);
for(0;_tmp905 != 0;_tmp905=_tmp905->tl){
struct _tuple0*_tmp908;void*_tmp909;struct _tuple24*_tmp907=(struct _tuple24*)_tmp905->hd;_tmp908=_tmp907->f1;_tmp909=_tmp907->f2;
s=Cyc_Absyn_declare_stmt(_tmp908,_tmp909,0,s,0);}}{
# 4846
struct Cyc_Absyn_Stmt*_tmp90A=s;_npop_handler(0);return _tmp90A;};
# 4833
;_pop_region(prgn);};}
# 4852
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp90B=e->r;struct Cyc_Absyn_Exp*_tmp90D;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_List_List*_tmp91D;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;struct Cyc_Absyn_Exp*_tmp922;struct Cyc_Absyn_Exp*_tmp923;struct Cyc_Absyn_Exp*_tmp925;struct Cyc_Absyn_Exp*_tmp926;struct Cyc_Absyn_Exp*_tmp928;struct Cyc_Absyn_Exp*_tmp929;struct Cyc_Absyn_Exp*_tmp92B;struct Cyc_Absyn_Exp*_tmp92C;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Exp*_tmp92F;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_Absyn_Exp*_tmp932;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_Absyn_Exp*_tmp935;struct Cyc_List_List*_tmp936;void**_tmp938;struct Cyc_Absyn_Exp*_tmp939;void**_tmp93B;struct Cyc_List_List*_tmp93C;struct Cyc_List_List*_tmp93E;struct Cyc_List_List*_tmp940;void**_tmp942;void**_tmp944;struct Cyc_Absyn_Stmt*_tmp946;struct Cyc_Absyn_MallocInfo*_tmp948;_LL404: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp90C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp90C->tag != 19)goto _LL406;else{_tmp90D=_tmp90C->f1;}}_LL405:
 _tmp90F=_tmp90D;goto _LL407;_LL406: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp90E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp90E->tag != 20)goto _LL408;else{_tmp90F=_tmp90E->f1;}}_LL407:
 _tmp911=_tmp90F;goto _LL409;_LL408: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp910=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp910->tag != 21)goto _LL40A;else{_tmp911=_tmp910->f1;}}_LL409:
 _tmp913=_tmp911;goto _LL40B;_LL40A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp912=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp912->tag != 14)goto _LL40C;else{_tmp913=_tmp912->f1;}}_LL40B:
 _tmp915=_tmp913;goto _LL40D;_LL40C: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp914=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp914->tag != 10)goto _LL40E;else{_tmp915=_tmp914->f1;}}_LL40D:
 _tmp917=_tmp915;goto _LL40F;_LL40E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp916=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp916->tag != 11)goto _LL410;else{_tmp917=_tmp916->f1;}}_LL40F:
 _tmp919=_tmp917;goto _LL411;_LL410: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp918=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp918->tag != 17)goto _LL412;else{_tmp919=_tmp918->f1;}}_LL411:
 _tmp91B=_tmp919;goto _LL413;_LL412: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp91A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp91A->tag != 4)goto _LL414;else{_tmp91B=_tmp91A->f1;}}_LL413:
 Cyc_Toc_exptypes_to_c(_tmp91B);goto _LL403;_LL414: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp91C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp91C->tag != 2)goto _LL416;else{_tmp91D=_tmp91C->f2;}}_LL415:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp91D);goto _LL403;_LL416: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp91E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp91E->tag != 6)goto _LL418;else{_tmp91F=_tmp91E->f1;_tmp920=_tmp91E->f2;}}_LL417:
 _tmp922=_tmp91F;_tmp923=_tmp920;goto _LL419;_LL418: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp921=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp921->tag != 7)goto _LL41A;else{_tmp922=_tmp921->f1;_tmp923=_tmp921->f2;}}_LL419:
 _tmp925=_tmp922;_tmp926=_tmp923;goto _LL41B;_LL41A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp924=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp924->tag != 8)goto _LL41C;else{_tmp925=_tmp924->f1;_tmp926=_tmp924->f2;}}_LL41B:
 _tmp928=_tmp925;_tmp929=_tmp926;goto _LL41D;_LL41C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp927=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp927->tag != 22)goto _LL41E;else{_tmp928=_tmp927->f1;_tmp929=_tmp927->f2;}}_LL41D:
 _tmp92B=_tmp928;_tmp92C=_tmp929;goto _LL41F;_LL41E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp92A=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp92A->tag != 33)goto _LL420;else{_tmp92B=_tmp92A->f1;_tmp92C=_tmp92A->f2;}}_LL41F:
 _tmp92E=_tmp92B;_tmp92F=_tmp92C;goto _LL421;_LL420: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp92D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp92D->tag != 3)goto _LL422;else{_tmp92E=_tmp92D->f1;_tmp92F=_tmp92D->f3;}}_LL421:
 Cyc_Toc_exptypes_to_c(_tmp92E);Cyc_Toc_exptypes_to_c(_tmp92F);goto _LL403;_LL422: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp930=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp930->tag != 5)goto _LL424;else{_tmp931=_tmp930->f1;_tmp932=_tmp930->f2;_tmp933=_tmp930->f3;}}_LL423:
# 4870
 Cyc_Toc_exptypes_to_c(_tmp931);Cyc_Toc_exptypes_to_c(_tmp932);Cyc_Toc_exptypes_to_c(_tmp933);goto _LL403;_LL424: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp934=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp934->tag != 9)goto _LL426;else{_tmp935=_tmp934->f1;_tmp936=_tmp934->f2;}}_LL425:
# 4872
 Cyc_Toc_exptypes_to_c(_tmp935);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp936);goto _LL403;_LL426: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp937=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp937->tag != 13)goto _LL428;else{_tmp938=(void**)((void**)& _tmp937->f1);_tmp939=_tmp937->f2;}}_LL427:
*_tmp938=Cyc_Toc_typ_to_c(*_tmp938);Cyc_Toc_exptypes_to_c(_tmp939);goto _LL403;_LL428: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp93A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp93A->tag != 24)goto _LL42A;else{_tmp93B=(void**)&(_tmp93A->f1)->f3;_tmp93C=_tmp93A->f2;}}_LL429:
# 4875
*_tmp93B=Cyc_Toc_typ_to_c(*_tmp93B);
_tmp93E=_tmp93C;goto _LL42B;_LL42A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp93D=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp93D->tag != 34)goto _LL42C;else{_tmp93E=_tmp93D->f2;}}_LL42B:
 _tmp940=_tmp93E;goto _LL42D;_LL42C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp93F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp93F->tag != 25)goto _LL42E;else{_tmp940=_tmp93F->f1;}}_LL42D:
# 4879
 for(0;_tmp940 != 0;_tmp940=_tmp940->tl){
struct Cyc_Absyn_Exp*_tmp958;struct _tuple16 _tmp957=*((struct _tuple16*)_tmp940->hd);_tmp958=_tmp957.f2;
Cyc_Toc_exptypes_to_c(_tmp958);}
# 4883
goto _LL403;_LL42E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp941=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp941->tag != 18)goto _LL430;else{_tmp942=(void**)((void**)& _tmp941->f1);}}_LL42F:
 _tmp944=_tmp942;goto _LL431;_LL430: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp943=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp943->tag != 16)goto _LL432;else{_tmp944=(void**)((void**)& _tmp943->f1);}}_LL431:
*_tmp944=Cyc_Toc_typ_to_c(*_tmp944);goto _LL403;_LL432: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp945=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp945->tag != 35)goto _LL434;else{_tmp946=_tmp945->f1;}}_LL433:
 Cyc_Toc_stmttypes_to_c(_tmp946);goto _LL403;_LL434: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp947=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp947->tag != 32)goto _LL436;else{_tmp948=(struct Cyc_Absyn_MallocInfo*)& _tmp947->f1;}}_LL435:
# 4888
 if(_tmp948->elt_type != 0){
void**_tmpD91;_tmp948->elt_type=((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp948->elt_type))),_tmpD91))));}
Cyc_Toc_exptypes_to_c(_tmp948->num_elts);
goto _LL403;_LL436: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp949=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp949->tag != 0)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp94A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp94A->tag != 1)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp94B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp94B->tag != 30)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp94C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp94C->tag != 38)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp94D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp94D->tag != 31)goto _LL440;}_LL43F:
 goto _LL403;_LL440: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp94E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp94E->tag != 28)goto _LL442;}_LL441:
# 4898
 goto _LL443;_LL442: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp94F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp94F->tag != 29)goto _LL444;}_LL443:
 goto _LL445;_LL444: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp950=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp950->tag != 27)goto _LL446;}_LL445:
 goto _LL447;_LL446: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp951=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp951->tag != 26)goto _LL448;}_LL447:
 goto _LL449;_LL448: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp952=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp952->tag != 23)goto _LL44A;}_LL449:
 goto _LL44B;_LL44A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp953=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp953->tag != 12)goto _LL44C;}_LL44B:
 goto _LL44D;_LL44C: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp954=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp954->tag != 15)goto _LL44E;}_LL44D:
 goto _LL44F;_LL44E: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp955=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp955->tag != 37)goto _LL450;}_LL44F:
 goto _LL451;_LL450: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp956=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp90B;if(_tmp956->tag != 36)goto _LL403;}_LL451:
# 4907
{const char*_tmpD94;void*_tmpD93;(_tmpD93=0,Cyc_Tcutil_terr(e->loc,((_tmpD94="Cyclone expression within C code",_tag_dyneither(_tmpD94,sizeof(char),33))),_tag_dyneither(_tmpD93,sizeof(void*),0)));}
goto _LL403;_LL403:;}
# 4912
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp95C=d->r;struct Cyc_Absyn_Vardecl*_tmp95E;struct Cyc_Absyn_Fndecl*_tmp960;struct Cyc_Absyn_Aggrdecl*_tmp962;struct Cyc_Absyn_Enumdecl*_tmp964;struct Cyc_Absyn_Typedefdecl*_tmp966;_LL453: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp95D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp95D->tag != 0)goto _LL455;else{_tmp95E=_tmp95D->f1;}}_LL454:
# 4915
 _tmp95E->type=Cyc_Toc_typ_to_c(_tmp95E->type);
if(_tmp95E->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp95E->initializer));
goto _LL452;_LL455: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp95F=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp95F->tag != 1)goto _LL457;else{_tmp960=_tmp95F->f1;}}_LL456:
# 4919
 _tmp960->ret_type=Cyc_Toc_typ_to_c(_tmp960->ret_type);
{struct Cyc_List_List*_tmp972=_tmp960->args;for(0;_tmp972 != 0;_tmp972=_tmp972->tl){
(*((struct _tuple8*)_tmp972->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp972->hd)).f3);}}
# 4923
goto _LL452;_LL457: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp961=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp961->tag != 6)goto _LL459;else{_tmp962=_tmp961->f1;}}_LL458:
 Cyc_Toc_aggrdecl_to_c(_tmp962,1);goto _LL452;_LL459: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp963=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp963->tag != 8)goto _LL45B;else{_tmp964=_tmp963->f1;}}_LL45A:
# 4926
 if(_tmp964->fields != 0){
struct Cyc_List_List*_tmp973=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp964->fields))->v;for(0;_tmp973 != 0;_tmp973=_tmp973->tl){
struct Cyc_Absyn_Enumfield*_tmp974=(struct Cyc_Absyn_Enumfield*)_tmp973->hd;
if(_tmp974->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp974->tag));}}
# 4931
goto _LL452;_LL45B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp965=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp965->tag != 9)goto _LL45D;else{_tmp966=_tmp965->f1;}}_LL45C:
 _tmp966->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp966->defn));goto _LL452;_LL45D: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp967=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp967->tag != 2)goto _LL45F;}_LL45E:
 goto _LL460;_LL45F: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp968=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp968->tag != 3)goto _LL461;}_LL460:
 goto _LL462;_LL461: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp969=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp969->tag != 7)goto _LL463;}_LL462:
 goto _LL464;_LL463: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp96A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp96A->tag != 10)goto _LL465;}_LL464:
 goto _LL466;_LL465: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp96B=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp96B->tag != 11)goto _LL467;}_LL466:
 goto _LL468;_LL467: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp96C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp96C->tag != 12)goto _LL469;}_LL468:
 goto _LL46A;_LL469: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp96D=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp96D->tag != 13)goto _LL46B;}_LL46A:
 goto _LL46C;_LL46B: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp96E=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp96E->tag != 4)goto _LL46D;}_LL46C:
 goto _LL46E;_LL46D: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp96F=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp96F->tag != 5)goto _LL46F;}_LL46E:
# 4942
{const char*_tmpD97;void*_tmpD96;(_tmpD96=0,Cyc_Tcutil_terr(d->loc,((_tmpD97="Cyclone declaration within C code",_tag_dyneither(_tmpD97,sizeof(char),34))),_tag_dyneither(_tmpD96,sizeof(void*),0)));}
goto _LL452;_LL46F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp970=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp970->tag != 14)goto _LL471;}_LL470:
 goto _LL472;_LL471: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp971=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp95C;if(_tmp971->tag != 15)goto _LL452;}_LL472:
# 4946
 goto _LL452;_LL452:;}
# 4950
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp977=s->r;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Stmt*_tmp97B;struct Cyc_Absyn_Stmt*_tmp97C;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*_tmp980;struct Cyc_Absyn_Stmt*_tmp981;struct Cyc_Absyn_Stmt*_tmp982;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_Absyn_Stmt*_tmp985;struct Cyc_Absyn_Exp*_tmp987;struct Cyc_Absyn_Exp*_tmp988;struct Cyc_Absyn_Exp*_tmp989;struct Cyc_Absyn_Stmt*_tmp98A;struct Cyc_Absyn_Exp*_tmp98C;struct Cyc_List_List*_tmp98D;struct Cyc_Absyn_Decl*_tmp98F;struct Cyc_Absyn_Stmt*_tmp990;struct Cyc_Absyn_Stmt*_tmp992;struct Cyc_Absyn_Exp*_tmp993;_LL474: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp978=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp978->tag != 1)goto _LL476;else{_tmp979=_tmp978->f1;}}_LL475:
 Cyc_Toc_exptypes_to_c(_tmp979);goto _LL473;_LL476: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp97A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp97A->tag != 2)goto _LL478;else{_tmp97B=_tmp97A->f1;_tmp97C=_tmp97A->f2;}}_LL477:
 Cyc_Toc_stmttypes_to_c(_tmp97B);Cyc_Toc_stmttypes_to_c(_tmp97C);goto _LL473;_LL478: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp97D=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp97D->tag != 3)goto _LL47A;else{_tmp97E=_tmp97D->f1;}}_LL479:
 if(_tmp97E != 0)Cyc_Toc_exptypes_to_c(_tmp97E);goto _LL473;_LL47A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp97F=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp97F->tag != 4)goto _LL47C;else{_tmp980=_tmp97F->f1;_tmp981=_tmp97F->f2;_tmp982=_tmp97F->f3;}}_LL47B:
# 4956
 Cyc_Toc_exptypes_to_c(_tmp980);Cyc_Toc_stmttypes_to_c(_tmp981);Cyc_Toc_stmttypes_to_c(_tmp982);goto _LL473;_LL47C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp983=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp983->tag != 5)goto _LL47E;else{_tmp984=(_tmp983->f1).f1;_tmp985=_tmp983->f2;}}_LL47D:
# 4958
 Cyc_Toc_exptypes_to_c(_tmp984);Cyc_Toc_stmttypes_to_c(_tmp985);goto _LL473;_LL47E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp986=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp986->tag != 9)goto _LL480;else{_tmp987=_tmp986->f1;_tmp988=(_tmp986->f2).f1;_tmp989=(_tmp986->f3).f1;_tmp98A=_tmp986->f4;}}_LL47F:
# 4960
 Cyc_Toc_exptypes_to_c(_tmp987);Cyc_Toc_exptypes_to_c(_tmp988);Cyc_Toc_exptypes_to_c(_tmp989);
Cyc_Toc_stmttypes_to_c(_tmp98A);goto _LL473;_LL480: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp98B=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp98B->tag != 10)goto _LL482;else{_tmp98C=_tmp98B->f1;_tmp98D=_tmp98B->f2;}}_LL481:
# 4963
 Cyc_Toc_exptypes_to_c(_tmp98C);
for(0;_tmp98D != 0;_tmp98D=_tmp98D->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp98D->hd)->body);}
goto _LL473;_LL482: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp98E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp98E->tag != 12)goto _LL484;else{_tmp98F=_tmp98E->f1;_tmp990=_tmp98E->f2;}}_LL483:
 Cyc_Toc_decltypes_to_c(_tmp98F);Cyc_Toc_stmttypes_to_c(_tmp990);goto _LL473;_LL484: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp991=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp991->tag != 14)goto _LL486;else{_tmp992=_tmp991->f1;_tmp993=(_tmp991->f2).f1;}}_LL485:
 Cyc_Toc_stmttypes_to_c(_tmp992);Cyc_Toc_exptypes_to_c(_tmp993);goto _LL473;_LL486: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp994=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp994->tag != 0)goto _LL488;}_LL487:
 goto _LL489;_LL488: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp995=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp995->tag != 6)goto _LL48A;}_LL489:
 goto _LL48B;_LL48A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp996=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp996->tag != 7)goto _LL48C;}_LL48B:
 goto _LL48D;_LL48C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp997=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp997->tag != 8)goto _LL48E;}_LL48D:
 goto _LL473;_LL48E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp998=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp998->tag != 11)goto _LL490;}_LL48F:
 goto _LL491;_LL490: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp999=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp999->tag != 13)goto _LL492;}_LL491:
 goto _LL493;_LL492: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp99A=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp99A->tag != 15)goto _LL494;}_LL493:
 goto _LL495;_LL494: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp99B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp977;if(_tmp99B->tag != 16)goto _LL473;}_LL495:
{const char*_tmpD9A;void*_tmpD99;(_tmpD99=0,Cyc_Tcutil_terr(s->loc,((_tmpD9A="Cyclone statement in C code",_tag_dyneither(_tmpD9A,sizeof(char),28))),_tag_dyneither(_tmpD99,sizeof(void*),0)));}
goto _LL473;_LL473:;}
# 4984
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD9D;void*_tmpD9C;(_tmpD9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD9D="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD9D,sizeof(char),29))),_tag_dyneither(_tmpD9C,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp9A0=d->r;struct Cyc_Absyn_Vardecl*_tmp9A2;struct Cyc_Absyn_Fndecl*_tmp9A4;struct Cyc_Absyn_Aggrdecl*_tmp9AA;struct Cyc_Absyn_Datatypedecl*_tmp9AC;struct Cyc_Absyn_Enumdecl*_tmp9AE;struct Cyc_Absyn_Typedefdecl*_tmp9B0;struct Cyc_List_List*_tmp9B4;struct Cyc_List_List*_tmp9B6;struct Cyc_List_List*_tmp9B8;struct Cyc_List_List*_tmp9BA;_LL497: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9A1=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A1->tag != 0)goto _LL499;else{_tmp9A2=_tmp9A1->f1;}}_LL498: {
# 4991
struct _tuple0*_tmp9BB=_tmp9A2->name;
# 4993
if(_tmp9A2->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpD9E;_tmp9BB=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->f1=Cyc_Absyn_Rel_n(0),((_tmpD9E->f2=(*_tmp9BB).f2,_tmpD9E))))));}
if(_tmp9A2->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9A2->initializer));else{
# 4999
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9A2->initializer));}}
# 5001
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpDA1;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDA0;nv=Cyc_Toc_add_varmap(r,nv,_tmp9A2->name,Cyc_Absyn_varb_exp(_tmp9BB,(void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpDA1.tag=1,((_tmpDA1.f1=_tmp9A2,_tmpDA1)))),_tmpDA0)))),0));}
_tmp9A2->name=_tmp9BB;
_tmp9A2->sc=Cyc_Toc_scope_to_c(_tmp9A2->sc);
_tmp9A2->type=Cyc_Toc_typ_to_c(_tmp9A2->type);
{struct Cyc_List_List*_tmpDA2;Cyc_Toc_result_decls=((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2->hd=d,((_tmpDA2->tl=Cyc_Toc_result_decls,_tmpDA2))))));}
goto _LL496;}_LL499: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9A3=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A3->tag != 1)goto _LL49B;else{_tmp9A4=_tmp9A3->f1;}}_LL49A: {
# 5008
struct _tuple0*_tmp9C0=_tmp9A4->name;
# 5010
if(_tmp9A4->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpDA3;_tmp9C0=((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->f1=Cyc_Absyn_Abs_n(0,1),((_tmpDA3->f2=(*_tmp9C0).f2,_tmpDA3))))));}
_tmp9A4->sc=Cyc_Absyn_Public;}
# 5014
nv=Cyc_Toc_add_varmap(r,nv,_tmp9A4->name,Cyc_Absyn_var_exp(_tmp9C0,0));
_tmp9A4->name=_tmp9C0;
Cyc_Toc_fndecl_to_c(nv,_tmp9A4,cinclude);
{struct Cyc_List_List*_tmpDA4;Cyc_Toc_result_decls=((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->hd=d,((_tmpDA4->tl=Cyc_Toc_result_decls,_tmpDA4))))));}
goto _LL496;}_LL49B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9A5=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A5->tag != 2)goto _LL49D;}_LL49C:
 goto _LL49E;_LL49D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9A6=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A6->tag != 3)goto _LL49F;}_LL49E: {
# 5021
const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA7="letdecl at toplevel",_tag_dyneither(_tmpDA7,sizeof(char),20))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}_LL49F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9A7=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A7->tag != 4)goto _LL4A1;}_LL4A0: {
# 5023
const char*_tmpDAA;void*_tmpDA9;(_tmpDA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDAA="region decl at toplevel",_tag_dyneither(_tmpDAA,sizeof(char),24))),_tag_dyneither(_tmpDA9,sizeof(void*),0)));}_LL4A1: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9A8=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A8->tag != 5)goto _LL4A3;}_LL4A2: {
# 5025
const char*_tmpDAD;void*_tmpDAC;(_tmpDAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDAD="alias decl at toplevel",_tag_dyneither(_tmpDAD,sizeof(char),23))),_tag_dyneither(_tmpDAC,sizeof(void*),0)));}_LL4A3: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9A9=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9A9->tag != 6)goto _LL4A5;else{_tmp9AA=_tmp9A9->f1;}}_LL4A4:
# 5027
 Cyc_Toc_aggrdecl_to_c(_tmp9AA,1);
goto _LL496;_LL4A5: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9AB=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9AB->tag != 7)goto _LL4A7;else{_tmp9AC=_tmp9AB->f1;}}_LL4A6:
# 5030
 if(_tmp9AC->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp9AC);else{
# 5033
Cyc_Toc_datatypedecl_to_c(_tmp9AC);}
goto _LL496;_LL4A7: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9AD=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9AD->tag != 8)goto _LL4A9;else{_tmp9AE=_tmp9AD->f1;}}_LL4A8:
# 5036
 Cyc_Toc_enumdecl_to_c(_tmp9AE);
{struct Cyc_List_List*_tmpDAE;Cyc_Toc_result_decls=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->hd=d,((_tmpDAE->tl=Cyc_Toc_result_decls,_tmpDAE))))));}
goto _LL496;_LL4A9: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9AF=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9AF->tag != 9)goto _LL4AB;else{_tmp9B0=_tmp9AF->f1;}}_LL4AA:
# 5040
 _tmp9B0->name=_tmp9B0->name;
_tmp9B0->tvs=0;
if(_tmp9B0->defn != 0)
_tmp9B0->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9B0->defn));else{
# 5045
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp9B0->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL4B7:
 _tmp9B0->defn=Cyc_Absyn_void_star_typ();break;default: _LL4B8:
 _tmp9B0->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5050
{struct Cyc_List_List*_tmpDAF;Cyc_Toc_result_decls=((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->hd=d,((_tmpDAF->tl=Cyc_Toc_result_decls,_tmpDAF))))));}
goto _LL496;_LL4AB: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9B1=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9B1->tag != 14)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9B2=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9B2->tag != 15)goto _LL4AF;}_LL4AE:
# 5054
 goto _LL496;_LL4AF: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9B3=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9B3->tag != 10)goto _LL4B1;else{_tmp9B4=_tmp9B3->f2;}}_LL4B0:
 _tmp9B6=_tmp9B4;goto _LL4B2;_LL4B1: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9B5=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9B5->tag != 11)goto _LL4B3;else{_tmp9B6=_tmp9B5->f2;}}_LL4B2:
 _tmp9B8=_tmp9B6;goto _LL4B4;_LL4B3: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9B7=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9B7->tag != 12)goto _LL4B5;else{_tmp9B8=_tmp9B7->f1;}}_LL4B4:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9B8,top,cinclude);goto _LL496;_LL4B5: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9B9=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9A0;if(_tmp9B9->tag != 13)goto _LL496;else{_tmp9BA=_tmp9B9->f1;}}_LL4B6:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9BA,top,1);goto _LL496;_LL496:;};}
# 5061
return nv;}
# 5065
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp9CC;struct Cyc_Core_NewRegion _tmp9CB=Cyc_Core__new_dynregion("internal-error",0);_tmp9CC=_tmp9CB.dynregion;{
struct _DynRegionFrame _tmp9CD;struct _RegionHandle*d=_open_dynregion(& _tmp9CD,_tmp9CC);{
struct Cyc_Dict_Dict*_tmpDBC;struct Cyc_Dict_Dict*_tmpDBB;struct Cyc_Set_Set**_tmpDBA;struct Cyc_List_List**_tmpDB9;struct Cyc_Dict_Dict*_tmpDB8;struct Cyc_List_List**_tmpDB7;struct Cyc_Toc_TocState*_tmpDB6;Cyc_Toc_toc_state=(
(_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->dyn=(struct _DynRegionHandle*)_tmp9CC,((_tmpDB6->tuple_types=(struct Cyc_List_List**)(
(_tmpDB7=_region_malloc(d,sizeof(*_tmpDB7)),((_tmpDB7[0]=0,_tmpDB7)))),((_tmpDB6->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDB8=_region_malloc(d,sizeof(*_tmpDB8)),((_tmpDB8[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDB8)))),((_tmpDB6->abs_struct_types=(struct Cyc_List_List**)(
(_tmpDB9=_region_malloc(d,sizeof(*_tmpDB9)),((_tmpDB9[0]=0,_tmpDB9)))),((_tmpDB6->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpDBA=_region_malloc(d,sizeof(*_tmpDBA)),((_tmpDBA[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDBA)))),((_tmpDB6->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDBB=_region_malloc(d,sizeof(*_tmpDBB)),((_tmpDBB[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDBB)))),((_tmpDB6->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpDBC=_region_malloc(d,sizeof(*_tmpDBC)),((_tmpDBC[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpDBC)))),((_tmpDB6->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpDB6))))))))))))))))));}
# 5079
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDBD;Cyc_Toc_globals=_tag_dyneither(((_tmpDBD=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDBD[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpDBD[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpDBD[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpDBD[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpDBD[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpDBD[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpDBD[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpDBD[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpDBD[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDBD[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpDBD[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpDBD[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpDBD[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpDBD[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDBD[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpDBD[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpDBD[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDBD[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpDBD[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDBD[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDBD[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDBD[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDBD[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpDBD[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpDBD[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpDBD[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpDBD[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpDBD[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpDBD[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpDBD[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpDBD[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpDBD[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpDBD[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpDBD[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpDBD[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpDBD[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpDBD[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpDBD[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDBD[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpDBD[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpDBD[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpDBD)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5067
;_pop_dynregion(d);};}
# 5132
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9D6=_new_region("start");struct _RegionHandle*start=& _tmp9D6;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5141
struct _DynRegionHandle*_tmp9D8;struct Cyc_Xarray_Xarray*_tmp9D9;struct Cyc_Toc_TocState _tmp9D7=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp9D8=_tmp9D7.dyn;_tmp9D9=_tmp9D7.temp_labels;
{struct _DynRegionFrame _tmp9DA;struct _RegionHandle*d=_open_dynregion(& _tmp9DA,_tmp9D8);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9D9);;_pop_dynregion(d);}
# 5145
Cyc_Core_free_dynregion(_tmp9D8);};{
# 5147
struct Cyc_List_List*_tmp9DB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9DB;};
# 5134
;_pop_region(start);};}
