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
{const char*_tmp9D7;void*_tmp9D6;(_tmp9D6=0,Cyc_fprintf(Cyc_stderr,((_tmp9D7="\n",_tag_dyneither(_tmp9D7,sizeof(char),2))),_tag_dyneither(_tmp9D6,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmp9DA;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmp9D9;(int)_throw((void*)((_tmp9D9=_cycalloc_atomic(sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=Cyc_Toc_Toc_Unimplemented,_tmp9DA)),_tmp9D9)))));};}
# 144
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 147
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmp9DD;void*_tmp9DC;(_tmp9DC=0,Cyc_fprintf(Cyc_stderr,((_tmp9DD="\n",_tag_dyneither(_tmp9DD,sizeof(char),2))),_tag_dyneither(_tmp9DC,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmp9E0;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmp9DF;(int)_throw((void*)((_tmp9DF=_cycalloc_atomic(sizeof(*_tmp9DF)),((_tmp9DF[0]=((_tmp9E0.tag=Cyc_Toc_Toc_Impossible,_tmp9E0)),_tmp9DF)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmp9E1;skip_stmt_opt=((_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1[0]=Cyc_Absyn_skip_stmt(0),_tmp9E1))));}
return*skip_stmt_opt;}
# 273
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 276
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp9E4;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9E3;return(void*)((_tmp9E3=_cycalloc(sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=13,((_tmp9E4.f1=t,((_tmp9E4.f2=e,((_tmp9E4.f3=0,((_tmp9E4.f4=Cyc_Absyn_No_coercion,_tmp9E4)))))))))),_tmp9E3))));}
# 279
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp9E7;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9E6;return(void*)((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=19,((_tmp9E7.f1=e,_tmp9E7)))),_tmp9E6))));}
# 282
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp9EA;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9E9;return(void*)((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((_tmp9E9[0]=((_tmp9EA.tag=22,((_tmp9EA.f1=e1,((_tmp9EA.f2=e2,_tmp9EA)))))),_tmp9E9))));}
# 285
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp9ED;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9EC;return(void*)((_tmp9EC=_cycalloc(sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=35,((_tmp9ED.f1=s,_tmp9ED)))),_tmp9EC))));}
# 288
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp9F0;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9EF;return(void*)((_tmp9EF=_cycalloc(sizeof(*_tmp9EF)),((_tmp9EF[0]=((_tmp9F0.tag=16,((_tmp9F0.f1=t,_tmp9F0)))),_tmp9EF))));}
# 291
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp9F3;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9F2;return(void*)((_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=9,((_tmp9F3.f1=e,((_tmp9F3.f2=es,((_tmp9F3.f3=0,((_tmp9F3.f4=1,_tmp9F3)))))))))),_tmp9F2))));}
# 294
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp9F6;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9F5;return(void*)((_tmp9F5=_cycalloc(sizeof(*_tmp9F5)),((_tmp9F5[0]=((_tmp9F6.tag=1,((_tmp9F6.f1=e,_tmp9F6)))),_tmp9F5))));}
# 297
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp9F9;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9F8;return(void*)((_tmp9F8=_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=2,((_tmp9F9.f1=s1,((_tmp9F9.f2=s2,_tmp9F9)))))),_tmp9F8))));}
# 300
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp9FC;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9FB;return(void*)((_tmp9FB=_cycalloc(sizeof(*_tmp9FB)),((_tmp9FB[0]=((_tmp9FC.tag=5,((_tmp9FC.f1=e1,((_tmp9FC.f2=e2,((_tmp9FC.f3=e3,_tmp9FC)))))))),_tmp9FB))));}
# 303
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp9FF;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp9FE;return(void*)((_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=20,((_tmp9FF.f1=e,((_tmp9FF.f2=n,((_tmp9FF.f3=0,((_tmp9FF.f4=0,_tmp9FF)))))))))),_tmp9FE))));}
# 306
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA02;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA01;return(void*)((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA02.tag=21,((_tmpA02.f1=e,((_tmpA02.f2=n,((_tmpA02.f3=0,((_tmpA02.f4=0,_tmpA02)))))))))),_tmpA01))));}
# 309
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 312
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA05;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA04;return(void*)((_tmpA04=_cycalloc(sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=34,((_tmpA05.f1=tdopt,((_tmpA05.f2=ds,_tmpA05)))))),_tmpA04))));}
# 314
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA08;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA07;return(void*)((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07[0]=((_tmpA08.tag=8,((_tmpA08.f1=v,((_tmpA08.f2=s,_tmpA08)))))),_tmpA07))));}
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
struct Cyc_Core_Impossible_exn_struct _tmpA0E;const char*_tmpA0D;struct Cyc_Core_Impossible_exn_struct*_tmpA0C;(int)_throw((void*)((_tmpA0C=_cycalloc(sizeof(*_tmpA0C)),((_tmpA0C[0]=((_tmpA0E.tag=Cyc_Core_Impossible,((_tmpA0E.f1=((_tmpA0D="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA0D,sizeof(char),44))),_tmpA0E)))),_tmpA0C)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA1B;const char*_tmpA1A;void*_tmpA19[1];struct Cyc_String_pa_PrintArg_struct _tmpA18;struct Cyc_Core_Impossible_exn_struct*_tmpA17;(int)_throw((void*)((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17[0]=((_tmpA1B.tag=Cyc_Core_Impossible,((_tmpA1B.f1=(struct _dyneither_ptr)((_tmpA18.tag=0,((_tmpA18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA19[0]=& _tmpA18,Cyc_aprintf(((_tmpA1A="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA1A,sizeof(char),67))),_tag_dyneither(_tmpA19,sizeof(void*),1)))))))),_tmpA1B)))),_tmpA17)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA21;const char*_tmpA20;struct Cyc_Core_Impossible_exn_struct*_tmpA1F;(int)_throw((void*)((_tmpA1F=_cycalloc(sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA21.tag=Cyc_Core_Impossible,((_tmpA21.f1=((_tmpA20="impossible type (not pointer)",_tag_dyneither(_tmpA20,sizeof(char),30))),_tmpA21)))),_tmpA1F)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmpA24;void*_tmpA23;(_tmpA23=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA24="is_nullable",_tag_dyneither(_tmpA24,sizeof(char),12))),_tag_dyneither(_tmpA23,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA26;unsigned int _tmpA25;struct _dyneither_ptr buf=(_tmpA25=len,((_tmpA26=_cyccalloc_atomic(sizeof(char),_tmpA25),_tag_dyneither(_tmpA26,sizeof(char),_tmpA25))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmpA29;char _tmpA28;struct _dyneither_ptr _tmpA27;(_tmpA27=p,((_tmpA28=*((char*)_check_dyneither_subscript(_tmpA27,sizeof(char),0)),((_tmpA29='_',((_get_dyneither_size(_tmpA27,sizeof(char))== 1  && (_tmpA28 == '\000'  && _tmpA29 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA27.curr)=_tmpA29)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 484
{char _tmpA2C;char _tmpA2B;struct _dyneither_ptr _tmpA2A;(_tmpA2A=p,((_tmpA2B=*((char*)_check_dyneither_subscript(_tmpA2A,sizeof(char),0)),((_tmpA2C='_',((_get_dyneither_size(_tmpA2A,sizeof(char))== 1  && (_tmpA2B == '\000'  && _tmpA2C != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA2A.curr)=_tmpA2C)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA2D;Cyc_strcpy(p,((_tmpA2D="_struct",_tag_dyneither(_tmpA2D,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 496
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmpA2E;pair=((_tmpA2E.f1=fieldname,((_tmpA2E.f2=dtname,_tmpA2E))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmpA2F;struct _tuple14*_tmp115=(_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F->f1=fieldname,((_tmpA2F->f2=dtname,_tmpA2F)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA32;struct _tuple0*_tmpA31;struct _tuple0*res=(_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->f1=_tmp117,((_tmpA31->f2=((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=newvar,_tmpA32)))),_tmpA31)))));
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
struct Cyc_Int_pa_PrintArg_struct _tmpA3A;void*_tmpA39[1];const char*_tmpA38;struct _dyneither_ptr*_tmpA37;struct _dyneither_ptr*xname=(_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=(struct _dyneither_ptr)((_tmpA3A.tag=1,((_tmpA3A.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA39[0]=& _tmpA3A,Cyc_aprintf(((_tmpA38="_tuple%d",_tag_dyneither(_tmpA38,sizeof(char),9))),_tag_dyneither(_tmpA39,sizeof(void*),1)))))))),_tmpA37)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA3D;struct Cyc_List_List*_tmpA3C;_tmp128=((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C->hd=((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->name=Cyc_Absyn_fieldname(i),((_tmpA3D->tq=Cyc_Toc_mt_tq,((_tmpA3D->type=(void*)ts2->hd,((_tmpA3D->width=0,((_tmpA3D->attributes=0,_tmpA3D)))))))))))),((_tmpA3C->tl=_tmp128,_tmpA3C))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA42;struct _tuple0*_tmpA41;struct Cyc_Absyn_Aggrdecl*_tmpA40;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->kind=Cyc_Absyn_StructA,((_tmpA40->sc=Cyc_Absyn_Public,((_tmpA40->name=(
(_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41->f1=Cyc_Absyn_Rel_n(0),((_tmpA41->f2=xname,_tmpA41)))))),((_tmpA40->tvs=0,((_tmpA40->impl=(
(_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->exist_vars=0,((_tmpA42->rgn_po=0,((_tmpA42->fields=_tmp128,((_tmpA42->tagged=0,_tmpA42)))))))))),((_tmpA40->attributes=0,_tmpA40)))))))))))));
# 547
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA48;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA47;struct Cyc_List_List*_tmpA46;Cyc_Toc_result_decls=((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->hd=Cyc_Absyn_new_decl((void*)((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA47.tag=6,((_tmpA47.f1=_tmp12B,_tmpA47)))),_tmpA48)))),0),((_tmpA46->tl=Cyc_Toc_result_decls,_tmpA46))))));}
{struct _tuple17*_tmpA4B;struct Cyc_List_List*_tmpA4A;*_tmp120=((_tmpA4A=_region_malloc(d,sizeof(*_tmpA4A)),((_tmpA4A->hd=((_tmpA4B=_region_malloc(d,sizeof(*_tmpA4B)),((_tmpA4B->f1=x,((_tmpA4B->f2=ts,_tmpA4B)))))),((_tmpA4A->tl=*_tmp120,_tmpA4A))))));}{
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
struct Cyc_Int_pa_PrintArg_struct _tmpA53;void*_tmpA52[1];const char*_tmpA51;struct _dyneither_ptr*_tmpA50;struct _dyneither_ptr*xname=(_tmpA50=_cycalloc(sizeof(*_tmpA50)),((_tmpA50[0]=(struct _dyneither_ptr)((_tmpA53.tag=1,((_tmpA53.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA52[0]=& _tmpA53,Cyc_aprintf(((_tmpA51="_tuple%d",_tag_dyneither(_tmpA51,sizeof(char),9))),_tag_dyneither(_tmpA52,sizeof(void*),1)))))))),_tmpA50)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 611
{struct _tuple18*_tmpA56;struct Cyc_List_List*_tmpA55;*_tmp13B=((_tmpA55=_region_malloc(d,sizeof(*_tmpA55)),((_tmpA55->hd=((_tmpA56=_region_malloc(d,sizeof(*_tmpA56)),((_tmpA56->f1=struct_name,((_tmpA56->f2=type_args,((_tmpA56->f3=x,_tmpA56)))))))),((_tmpA55->tl=*_tmp13B,_tmpA55))))));}{
# 614
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 621
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpA59;struct Cyc_List_List*_tmpA58;_tmp147=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->hd=((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59->name=_tmp14C->name,((_tmpA59->tq=Cyc_Toc_mt_tq,((_tmpA59->type=t,((_tmpA59->width=_tmp14C->width,((_tmpA59->attributes=_tmp14C->attributes,_tmpA59)))))))))))),((_tmpA58->tl=_tmp147,_tmpA58))))));};}
# 625
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA5E;struct _tuple0*_tmpA5D;struct Cyc_Absyn_Aggrdecl*_tmpA5C;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C->kind=Cyc_Absyn_StructA,((_tmpA5C->sc=Cyc_Absyn_Public,((_tmpA5C->name=(
(_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->f1=Cyc_Absyn_Rel_n(0),((_tmpA5D->f2=xname,_tmpA5D)))))),((_tmpA5C->tvs=0,((_tmpA5C->impl=(
(_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E->exist_vars=0,((_tmpA5E->rgn_po=0,((_tmpA5E->fields=_tmp147,((_tmpA5E->tagged=0,_tmpA5E)))))))))),((_tmpA5C->attributes=0,_tmpA5C)))))))))))));
# 633
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA64;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA63;struct Cyc_List_List*_tmpA62;Cyc_Toc_result_decls=((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62->hd=Cyc_Absyn_new_decl((void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA63.tag=6,((_tmpA63.f1=_tmp14F,_tmpA63)))),_tmpA64)))),0),((_tmpA62->tl=Cyc_Toc_result_decls,_tmpA62))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 615
;_pop_region(r);};};
# 569
;_pop_dynregion(d);};}
# 642
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpA71;const char*_tmpA70;void*_tmpA6F[1];struct Cyc_Int_pa_PrintArg_struct _tmpA6E;struct _tuple0*_tmpA6D;struct _tuple0*res=(_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->f1=Cyc_Absyn_Loc_n,((_tmpA6D->f2=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=(struct _dyneither_ptr)((_tmpA6E.tag=1,((_tmpA6E.f1=(unsigned int)_tmp15B,((_tmpA6F[0]=& _tmpA6E,Cyc_aprintf(((_tmpA70="_tmp%X",_tag_dyneither(_tmpA70,sizeof(char),7))),_tag_dyneither(_tmpA6F,sizeof(void*),1)))))))),_tmpA71)))),_tmpA6D)))));
return res;}
# 650
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpA79;void*_tmpA78[1];const char*_tmpA77;struct _dyneither_ptr*_tmpA76;struct _dyneither_ptr*res=(_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=(struct _dyneither_ptr)((_tmpA79.tag=1,((_tmpA79.f1=(unsigned int)_tmp165,((_tmpA78[0]=& _tmpA79,Cyc_aprintf(((_tmpA77="_LL%X",_tag_dyneither(_tmpA77,sizeof(char),6))),_tag_dyneither(_tmpA78,sizeof(void*),1)))))))),_tmpA76)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpA7C;void*_tmpA7B;(_tmpA7B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA7C="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpA7C,sizeof(char),43))),_tag_dyneither(_tmpA7B,sizeof(void*),0)));}{
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
struct _tuple8*_tmpA7D;return(_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->f1=_tmp170,((_tmpA7D->f2=_tmp171,((_tmpA7D->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpA7D)))))));};}
# 687
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpA7E;return(_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->f1=_tmp175,((_tmpA7E->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpA7E)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpA7F;return(_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->name=f->name,((_tmpA7F->tq=Cyc_Toc_mt_tq,((_tmpA7F->type=
# 719
Cyc_Toc_typ_to_c(f->type),((_tmpA7F->width=f->width,((_tmpA7F->attributes=f->attributes,_tmpA7F)))))))))));}
# 723
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 725
return;}
# 728
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpA80;cs=((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpA80))));}
# 733
return*cs;}
# 735
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA81;r=((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpA81))));}
# 740
return*r;}
# 742
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpA82;r=((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpA82))));}
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
const char*_tmpA85;void*_tmpA84;(_tmpA84=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA85="unresolved DatatypeFieldType",_tag_dyneither(_tmpA85,sizeof(char),29))),_tag_dyneither(_tmpA84,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
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
{struct Cyc_List_List*_tmpA86;_tmp1D1=((_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->hd=(void*)_tmp1A5->hd,((_tmpA86->tl=_tmp1D1,_tmpA86))))));}goto _LL9F;_LL9F:;}{
# 812
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 815
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpA87;struct _tuple8*_tmp1DC=(_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->f1=_tmp1A4->name,((_tmpA87->f2=_tmp1A4->tq,((_tmpA87->f3=_tmp1DB,_tmpA87)))))));
struct Cyc_List_List*_tmpA88;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->hd=_tmp1DC,((_tmpA88->tl=0,_tmpA88)))))));}{
# 819
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA8E;struct Cyc_Absyn_FnInfo _tmpA8D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA8C;return(void*)((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8E.tag=9,((_tmpA8E.f1=((_tmpA8D.tvars=0,((_tmpA8D.effect=0,((_tmpA8D.ret_tqual=_tmp1A0,((_tmpA8D.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpA8D.args=_tmp1DA,((_tmpA8D.c_varargs=_tmp1A3,((_tmpA8D.cyc_varargs=0,((_tmpA8D.rgn_po=0,((_tmpA8D.attributes=_tmp1D1,_tmpA8D)))))))))))))))))),_tmpA8E)))),_tmpA8C))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 824
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 829
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpA91;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA90;return(void*)((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=12,((_tmpA91.f1=_tmp1A9,((_tmpA91.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpA91)))))),_tmpA90))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA94;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA93;return(void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA94.tag=17,((_tmpA94.f1=_tmp1B3,((_tmpA94.f2=0,((_tmpA94.f3=_tmp1B5,((_tmpA94.f4=0,_tmpA94)))))))))),_tmpA93))));}else{
return t;}}else{
# 866
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpA97;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA96;return(void*)((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=17,((_tmpA97.f1=_tmp1B3,((_tmpA97.f2=0,((_tmpA97.f3=_tmp1B5,((_tmpA97.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpA97)))))))))),_tmpA96))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
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
const char*_tmpA9B;void*_tmpA9A[1];struct Cyc_String_pa_PrintArg_struct _tmpA99;(_tmpA99.tag=0,((_tmpA99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA9A[0]=& _tmpA99,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9B="atomic_typ:  bad type %s",_tag_dyneither(_tmpA9B,sizeof(char),25))),_tag_dyneither(_tmpA9A,sizeof(void*),1)))))));}_LLB8:;}
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
const char*_tmpA9E;void*_tmpA9D;(_tmpA9D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9E="is_poly_field: type missing fields",_tag_dyneither(_tmpA9E,sizeof(char),35))),_tag_dyneither(_tmpA9D,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 982
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpAA2;void*_tmpAA1[1];struct Cyc_String_pa_PrintArg_struct _tmpAA0;(_tmpAA0.tag=0,((_tmpAA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAA1[0]=& _tmpAA0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA2="is_poly_field: bad field %s",_tag_dyneither(_tmpAA2,sizeof(char),28))),_tag_dyneither(_tmpAA1,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpAA6;void*_tmpAA5[1];struct Cyc_String_pa_PrintArg_struct _tmpAA4;(_tmpAA4.tag=0,((_tmpAA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAA5[0]=& _tmpAA4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA6="is_poly_field: bad type %s",_tag_dyneither(_tmpAA6,sizeof(char),27))),_tag_dyneither(_tmpAA5,sizeof(void*),1)))))));}_LLEC:;}
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
const char*_tmpAAA;void*_tmpAA9[1];struct Cyc_String_pa_PrintArg_struct _tmpAA8;(_tmpAA8.tag=0,((_tmpAA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpAA9[0]=& _tmpAA8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAAA="is_poly_project: bad type %s",_tag_dyneither(_tmpAAA,sizeof(char),29))),_tag_dyneither(_tmpAA9,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1004
 return 0;_LLF3:;}
# 1009
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAAB;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->hd=s,((_tmpAAB->tl=0,_tmpAAB)))))),0);}
# 1013
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpAAC;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->hd=s,((_tmpAAC->tl=0,_tmpAAC)))))),0);}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpAAD[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAAD[1]=s,((_tmpAAD[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAAD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpAAE[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAAE[1]=n,((_tmpAAE[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAAE,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1031
struct Cyc_Absyn_Exp*_tmpAAF[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAAF[1]=n,((_tmpAAF[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAAF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpAB0[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpAB0[2]=n,((_tmpAB0[1]=s,((_tmpAB0[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpAB1;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1->hd=e,((_tmpAB1->tl=0,_tmpAB1)))))),0);}
# 1043
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpAB2[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAB2,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpAB8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpAB7;struct Cyc_List_List*_tmpAB6;Cyc_Toc_result_decls=((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=Cyc_Absyn_new_decl((void*)((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8[0]=((_tmpAB7.tag=0,((_tmpAB7.f1=vd,_tmpAB7)))),_tmpAB8)))),0),((_tmpAB6->tl=Cyc_Toc_result_decls,_tmpAB6))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1073
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1075
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1077
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpABF;struct _tuple16*_tmpABE;struct _tuple16*_tmpABD;struct _tuple16*_tmpABC[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpABC[2]=(
# 1080
(_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->f1=0,((_tmpABD->f2=xplussz,_tmpABD)))))),((_tmpABC[1]=(
# 1079
(_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE->f1=0,((_tmpABE->f2=xexp,_tmpABE)))))),((_tmpABC[0]=(
# 1078
(_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->f1=0,((_tmpABF->f2=xexp,_tmpABF)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpABC,sizeof(struct _tuple16*),3)))))))),0);}
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
int*_tmpAC7;struct Cyc_Absyn_Exp**_tmpAC6;int*_tmpAC5;struct Cyc_Toc_Env*_tmpAC4;return(_tmpAC4=_region_malloc(r,sizeof(*_tmpAC4)),((_tmpAC4->break_lab=(struct _dyneither_ptr**)0,((_tmpAC4->continue_lab=(struct _dyneither_ptr**)0,((_tmpAC4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAC4->varmap=(struct Cyc_Dict_Dict)
# 1145
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpAC4->toplevel=(int)1,((_tmpAC4->in_lhs=(int*)(
# 1147
(_tmpAC5=_region_malloc(r,sizeof(*_tmpAC5)),((_tmpAC5[0]=0,_tmpAC5)))),((_tmpAC4->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpAC4->struct_info).varsizeexp=((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=0,_tmpAC6)))),(((_tmpAC4->struct_info).lhs_exp=0,_tmpAC4->struct_info)))),((_tmpAC4->in_sizeof=(int*)(
(_tmpAC7=_region_malloc(r,sizeof(*_tmpAC7)),((_tmpAC7[0]=0,_tmpAC7)))),((_tmpAC4->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAC4)))))))))))))))))));}
# 1153
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpAC8;return(_tmpAC8=_region_malloc(r,sizeof(*_tmpAC8)),((_tmpAC8->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpAC8->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpAC8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpAC8->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpAC8->toplevel=(int)_tmp255,((_tmpAC8->in_lhs=(int*)_tmp256,((_tmpAC8->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpAC8->in_sizeof=(int*)_tmp258,((_tmpAC8->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAC8)))))))))))))))))));};}
# 1158
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpAC9;return(_tmpAC9=_region_malloc(r,sizeof(*_tmpAC9)),((_tmpAC9->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpAC9->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpAC9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpAC9->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpAC9->toplevel=(int)0,((_tmpAC9->in_lhs=(int*)_tmp260,((_tmpAC9->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpAC9->in_sizeof=(int*)_tmp262,((_tmpAC9->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAC9)))))))))))))))))));};}
# 1162
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpACA;return(_tmpACA=_region_malloc(r,sizeof(*_tmpACA)),((_tmpACA->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpACA->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpACA->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpACA->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpACA->toplevel=(int)1,((_tmpACA->in_lhs=(int*)_tmp26A,((_tmpACA->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpACA->in_sizeof=(int*)_tmp26C,((_tmpACA->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpACA)))))))))))))))))));};}
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
struct Cyc_Toc_Env*_tmpACB;return(_tmpACB=_region_malloc(r,sizeof(*_tmpACB)),((_tmpACB->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpACB->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpACB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpACB->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpACB->toplevel=(int)_tmp27A,((_tmpACB->in_lhs=(int*)_tmp27B,((_tmpACB->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpACB->struct_info).varsizeexp=_tmp27C,(((_tmpACB->struct_info).lhs_exp=exp,_tmpACB->struct_info)))),((_tmpACB->in_sizeof=(int*)_tmp27D,((_tmpACB->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpACB)))))))))))))))))));};}
# 1189
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp27F=(*x).f1;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1192
const char*_tmpACF;void*_tmpACE[1];struct Cyc_String_pa_PrintArg_struct _tmpACD;(_tmpACD.tag=0,((_tmpACD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpACE[0]=& _tmpACD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACF="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpACF,sizeof(char),30))),_tag_dyneither(_tmpACE,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1195
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpAD0;return(_tmpAD0=_region_malloc(r,sizeof(*_tmpAD0)),((_tmpAD0->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpAD0->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpAD0->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpAD0->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpAD0->toplevel=(int)_tmp288,((_tmpAD0->in_lhs=(int*)_tmp289,((_tmpAD0->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpAD0->in_sizeof=(int*)_tmp28B,((_tmpAD0->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD0)))))))))))))))))));};};}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpAD1;return(_tmpAD1=_region_malloc(r,sizeof(*_tmpAD1)),((_tmpAD1->break_lab=(struct _dyneither_ptr**)0,((_tmpAD1->continue_lab=(struct _dyneither_ptr**)0,((_tmpAD1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpAD1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpAD1->toplevel=(int)_tmp293,((_tmpAD1->in_lhs=(int*)_tmp294,((_tmpAD1->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpAD1->in_sizeof=(int*)_tmp296,((_tmpAD1->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD1)))))))))))))))))));};}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1213
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpAD2;fallthru_vars=((_tmpAD2=_region_malloc(r,sizeof(*_tmpAD2)),((_tmpAD2->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpAD2->tl=fallthru_vars,_tmpAD2))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpAD3;struct Cyc_Toc_FallthruInfo*fi=
(_tmpAD3=_region_malloc(r,sizeof(*_tmpAD3)),((_tmpAD3->label=fallthru_l,((_tmpAD3->binders=fallthru_vars,((_tmpAD3->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpAD3)))))));
struct _dyneither_ptr**_tmpAD6;struct Cyc_Toc_Env*_tmpAD5;return(_tmpAD5=_region_malloc(r,sizeof(*_tmpAD5)),((_tmpAD5->break_lab=(struct _dyneither_ptr**)((_tmpAD6=_region_malloc(r,sizeof(*_tmpAD6)),((_tmpAD6[0]=break_l,_tmpAD6)))),((_tmpAD5->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpAD5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAD5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpAD5->toplevel=(int)_tmp29E,((_tmpAD5->in_lhs=(int*)_tmp29F,((_tmpAD5->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpAD5->in_sizeof=(int*)_tmp2A1,((_tmpAD5->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD5)))))))))))))))))));};};};}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1228
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpAD9;struct Cyc_Toc_Env*_tmpAD8;return(_tmpAD8=_region_malloc(r,sizeof(*_tmpAD8)),((_tmpAD8->break_lab=(struct _dyneither_ptr**)((_tmpAD9=_region_malloc(r,sizeof(*_tmpAD9)),((_tmpAD9[0]=break_l,_tmpAD9)))),((_tmpAD8->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpAD8->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpAD8->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpAD8->toplevel=(int)_tmp2AC,((_tmpAD8->in_lhs=(int*)_tmp2AD,((_tmpAD8->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpAD8->in_sizeof=(int*)_tmp2AF,((_tmpAD8->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpAD8)))))))))))))))))));};}
# 1235
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1238
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpADC;struct Cyc_Toc_Env*_tmpADB;return(_tmpADB=_region_malloc(r,sizeof(*_tmpADB)),((_tmpADB->break_lab=(struct _dyneither_ptr**)0,((_tmpADB->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpADB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpADC=_region_malloc(r,sizeof(*_tmpADC)),((_tmpADC->label=next_l,((_tmpADC->binders=0,((_tmpADC->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpADC)))))))),((_tmpADB->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpADB->toplevel=(int)_tmp2B7,((_tmpADB->in_lhs=(int*)_tmp2B8,((_tmpADB->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpADB->in_sizeof=(int*)_tmp2BA,((_tmpADB->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpADB)))))))))))))))))));};}
# 1253 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1256
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2BD=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1261
{const char*_tmpADF;void*_tmpADE;(_tmpADE=0,Cyc_Tcutil_terr(e->loc,((_tmpADF="dereference of NULL pointer",_tag_dyneither(_tmpADF,sizeof(char),28))),_tag_dyneither(_tmpADE,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1266
 return 0;_LL114:;_LL115: {
const char*_tmpAE2;void*_tmpAE1;(_tmpAE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE2="need_null_check",_tag_dyneither(_tmpAE2,sizeof(char),16))),_tag_dyneither(_tmpAE1,sizeof(void*),0)));}_LL10B:;}
# 1271
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2C6=e->annot;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1276
{const char*_tmpAE5;void*_tmpAE4;(_tmpAE4=0,Cyc_Tcutil_terr(e->loc,((_tmpAE5="dereference of NULL pointer",_tag_dyneither(_tmpAE5,sizeof(char),28))),_tag_dyneither(_tmpAE4,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpAE8;void*_tmpAE7;(_tmpAE7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE8="get_relns",_tag_dyneither(_tmpAE8,sizeof(char),10))),_tag_dyneither(_tmpAE7,sizeof(void*),0)));}_LL116:;}
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
{struct _tuple19 _tmpAE9;struct _tuple19 _tmp338=(_tmpAE9.f1=Cyc_Tcutil_compress(_tmp334),((_tmpAE9.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpAE9)));void*_tmp33A;union Cyc_Absyn_Constraint*_tmp33C;_LL17E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp338.f1;if(_tmp339->tag != 19)goto _LL180;else{_tmp33A=(void*)_tmp339->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp338.f2;if(_tmp33B->tag != 5)goto _LL180;else{_tmp33C=((_tmp33B->f1).ptr_atts).bounds;}};_LL17F:
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
if(e->topt == 0){const char*_tmpAEC;void*_tmpAEB;(_tmpAEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEC="Missing type in primop ",_tag_dyneither(_tmpAEC,sizeof(char),24))),_tag_dyneither(_tmpAEB,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1484
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpAEF;void*_tmpAEE;(_tmpAEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAEF="Missing type in primop ",_tag_dyneither(_tmpAEF,sizeof(char),24))),_tag_dyneither(_tmpAEE,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1488
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpAF0;return(_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->f1=Cyc_Toc_mt_tq,((_tmpAF0->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpAF0)))));}
# 1491
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpAF1;return(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->f1=0,((_tmpAF1->f2=e,_tmpAF1)))));};}
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
const char*_tmpAF4;void*_tmpAF3;(_tmpAF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF4="bogus x to make_struct",_tag_dyneither(_tmpAF4,sizeof(char),23))),_tag_dyneither(_tmpAF3,sizeof(void*),0)));}_LL194:;}else{
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
const char*_tmpAF8;void*_tmpAF7[1];struct Cyc_String_pa_PrintArg_struct _tmpAF6;(_tmpAF6.tag=0,((_tmpAF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAF7[0]=& _tmpAF6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF8="array_elt_type called on %s",_tag_dyneither(_tmpAF8,sizeof(char),28))),_tag_dyneither(_tmpAF7,sizeof(void*),1)))))));}_LL1A2:;}
# 1560
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1564
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1569
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1572
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp363=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp363 != 0;_tmp363=_tmp363->tl){
# 1575
struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct _tuple16*_tmp364=(struct _tuple16*)_tmp363->hd;_tmp365=_tmp364->f1;_tmp366=_tmp364->f2;{
# 1579
struct Cyc_Absyn_Exp*e_index;
if(_tmp365 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1583
if(_tmp365->tl != 0){const char*_tmpAFB;void*_tmpAFA;(_tmpAFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAFB="multiple designators in array",_tag_dyneither(_tmpAFB,sizeof(char),30))),_tag_dyneither(_tmpAFA,sizeof(void*),0)));}{
void*_tmp369=(void*)_tmp365->hd;
void*_tmp36A=_tmp369;struct Cyc_Absyn_Exp*_tmp36C;_LL1A8: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp36B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp36A;if(_tmp36B->tag != 0)goto _LL1AA;else{_tmp36C=_tmp36B->f1;}}_LL1A9:
# 1587
 Cyc_Toc_exp_to_c(nv,_tmp36C);
e_index=_tmp36C;
goto _LL1A7;_LL1AA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp36D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp36A;if(_tmp36D->tag != 1)goto _LL1A7;}_LL1AB: {
const char*_tmpAFE;void*_tmpAFD;(_tmpAFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpAFE="field name designators in array",_tag_dyneither(_tmpAFE,sizeof(char),32))),_tag_dyneither(_tmpAFD,sizeof(void*),0)));}_LL1A7:;};}{
# 1593
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp370=_tmp366->r;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp376;int _tmp377;void*_tmp379;struct Cyc_List_List*_tmp37A;_LL1AD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp371=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp371->tag != 25)goto _LL1AF;else{_tmp372=_tmp371->f1;}}_LL1AE:
# 1596
 s=Cyc_Toc_init_array(nv,lval,_tmp372,s);
goto _LL1AC;_LL1AF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp373->tag != 26)goto _LL1B1;else{_tmp374=_tmp373->f1;_tmp375=_tmp373->f2;_tmp376=_tmp373->f3;_tmp377=_tmp373->f4;}}_LL1B0:
# 1599
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp374,_tmp375,_tmp376,_tmp377,s,0);
goto _LL1AC;_LL1B1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp378->tag != 28)goto _LL1B3;else{_tmp379=(void*)_tmp378->f1;_tmp37A=_tmp378->f2;}}_LL1B2:
# 1602
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp379,_tmp37A,s);
goto _LL1AC;_LL1B3:;_LL1B4:
# 1605
 Cyc_Toc_exp_to_c(nv,_tmp366);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp366,0),s,0);
goto _LL1AC;_LL1AC:;};};}}
# 1610
return s;}
# 1615
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1618
struct _tuple0*_tmp37B=vd->name;
void*_tmp37C=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{
struct _RegionHandle _tmp37D=_new_region("r2");struct _RegionHandle*r2=& _tmp37D;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB01;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB00;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp37B,Cyc_Absyn_varb_exp(_tmp37B,(void*)((_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00[0]=((_tmpB01.tag=4,((_tmpB01.f1=vd,_tmpB01)))),_tmpB00)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp37B,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp37B,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp37B,0),0);
# 1630
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp37B,0),0);
struct Cyc_Absyn_Stmt*body;
# 1634
{void*_tmp37E=e2->r;struct Cyc_List_List*_tmp380;struct Cyc_Absyn_Vardecl*_tmp382;struct Cyc_Absyn_Exp*_tmp383;struct Cyc_Absyn_Exp*_tmp384;int _tmp385;void*_tmp387;struct Cyc_List_List*_tmp388;_LL1B6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp37F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp37F->tag != 25)goto _LL1B8;else{_tmp380=_tmp37F->f1;}}_LL1B7:
# 1636
 body=Cyc_Toc_init_array(nv2,lval,_tmp380,Cyc_Toc_skip_stmt_dl());
goto _LL1B5;_LL1B8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp381=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp381->tag != 26)goto _LL1BA;else{_tmp382=_tmp381->f1;_tmp383=_tmp381->f2;_tmp384=_tmp381->f3;_tmp385=_tmp381->f4;}}_LL1B9:
# 1639
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp382,_tmp383,_tmp384,_tmp385,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1B5;_LL1BA: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp386=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp37E;if(_tmp386->tag != 28)goto _LL1BC;else{_tmp387=(void*)_tmp386->f1;_tmp388=_tmp386->f2;}}_LL1BB:
# 1642
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp387,_tmp388,Cyc_Toc_skip_stmt_dl());
goto _LL1B5;_LL1BC:;_LL1BD:
# 1645
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,e2,0);
goto _LL1B5;_LL1B5:;}{
# 1649
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1651
if(zero_term){
# 1656
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp37C,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1661
struct Cyc_Absyn_Stmt*_tmp389=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp37B,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp389;};};}
# 1623
;_pop_region(r2);};}
# 1667
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1670
{struct Cyc_List_List*_tmp38C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp38C != 0;_tmp38C=_tmp38C->tl){
struct Cyc_List_List*_tmp38E;struct Cyc_Absyn_Exp*_tmp38F;struct _tuple16*_tmp38D=(struct _tuple16*)_tmp38C->hd;_tmp38E=_tmp38D->f1;_tmp38F=_tmp38D->f2;
if(_tmp38E == 0){
const char*_tmpB04;void*_tmpB03;(_tmpB03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB04="empty designator list",_tag_dyneither(_tmpB04,sizeof(char),22))),_tag_dyneither(_tmpB03,sizeof(void*),0)));}
if(_tmp38E->tl != 0){
const char*_tmpB07;void*_tmpB06;(_tmpB06=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB07="too many designators in anonymous struct",_tag_dyneither(_tmpB07,sizeof(char),41))),_tag_dyneither(_tmpB06,sizeof(void*),0)));}{
void*_tmp394=(void*)_tmp38E->hd;struct _dyneither_ptr*_tmp396;_LL1BF: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp395=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp394;if(_tmp395->tag != 1)goto _LL1C1;else{_tmp396=_tmp395->f1;}}_LL1C0: {
# 1678
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp396,0);
{void*_tmp397=_tmp38F->r;struct Cyc_List_List*_tmp399;struct Cyc_Absyn_Vardecl*_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;int _tmp39E;void*_tmp3A0;struct Cyc_List_List*_tmp3A1;_LL1C4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp398=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp398->tag != 25)goto _LL1C6;else{_tmp399=_tmp398->f1;}}_LL1C5:
# 1681
 s=Cyc_Toc_init_array(nv,lval,_tmp399,s);goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp39A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp39A->tag != 26)goto _LL1C8;else{_tmp39B=_tmp39A->f1;_tmp39C=_tmp39A->f2;_tmp39D=_tmp39A->f3;_tmp39E=_tmp39A->f4;}}_LL1C7:
# 1683
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp39B,_tmp39C,_tmp39D,_tmp39E,s,0);goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp39F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp397;if(_tmp39F->tag != 28)goto _LL1CA;else{_tmp3A0=(void*)_tmp39F->f1;_tmp3A1=_tmp39F->f2;}}_LL1C9:
# 1685
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3A0,_tmp3A1,s);goto _LL1C3;_LL1CA:;_LL1CB:
# 1687
 Cyc_Toc_exp_to_c(nv,_tmp38F);
# 1689
if(Cyc_Toc_is_poly_field(struct_type,_tmp396))
_tmp38F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp38F);
# 1692
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp38F,0),0),s,0);
goto _LL1C3;_LL1C3:;}
# 1695
goto _LL1BE;}_LL1C1:;_LL1C2: {
const char*_tmpB0A;void*_tmpB09;(_tmpB09=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0A="array designator in struct",_tag_dyneither(_tmpB0A,sizeof(char),27))),_tag_dyneither(_tmpB09,sizeof(void*),0)));}_LL1BE:;};}}
# 1699
return s;}
# 1704
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1708
struct _RegionHandle _tmp3A4=_new_region("r");struct _RegionHandle*r=& _tmp3A4;_push_region(r);
{struct Cyc_List_List*_tmp3A5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp3A6=Cyc_Toc_add_tuple_type(_tmp3A5);
# 1712
struct _tuple0*_tmp3A7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3A8=Cyc_Absyn_var_exp(_tmp3A7,0);
struct Cyc_Absyn_Stmt*_tmp3A9=Cyc_Absyn_exp_stmt(_tmp3A8,0);
# 1716
struct Cyc_Absyn_Exp*(*_tmp3AA)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1718
int is_atomic=1;
struct Cyc_List_List*_tmp3AB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3AB);for(0;_tmp3AB != 0;(_tmp3AB=_tmp3AB->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3AB->hd;
struct Cyc_Absyn_Exp*lval=_tmp3AA(_tmp3A8,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp3AC=e->r;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Vardecl*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;int _tmp3B3;_LL1CD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3AD=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3AD->tag != 25)goto _LL1CF;else{_tmp3AE=_tmp3AD->f1;}}_LL1CE:
# 1726
 _tmp3A9=Cyc_Toc_init_array(nv,lval,_tmp3AE,_tmp3A9);
goto _LL1CC;_LL1CF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3AF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3AC;if(_tmp3AF->tag != 26)goto _LL1D1;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=_tmp3AF->f2;_tmp3B2=_tmp3AF->f3;_tmp3B3=_tmp3AF->f4;}}_LL1D0:
# 1729
 _tmp3A9=Cyc_Toc_init_comprehension(nv,lval,_tmp3B0,_tmp3B1,_tmp3B2,_tmp3B3,_tmp3A9,0);
goto _LL1CC;_LL1D1:;_LL1D2:
# 1733
 Cyc_Toc_exp_to_c(nv,e);
_tmp3A9=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3AA(_tmp3A8,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3A9,0);
# 1736
goto _LL1CC;_LL1CC:;};}}{
# 1739
struct Cyc_Absyn_Exp*_tmp3B4=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp3A7,0),_tmp3A6,_tmp3A9,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp3B4;};}
# 1709
;_pop_region(r);}
# 1743
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3B5 != 0;_tmp3B5=_tmp3B5->tl){
struct Cyc_Absyn_Aggrfield*_tmp3B6=(struct Cyc_Absyn_Aggrfield*)_tmp3B5->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3B6->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1750
struct Cyc_String_pa_PrintArg_struct _tmpB12;void*_tmpB11[1];const char*_tmpB10;void*_tmpB0F;(_tmpB0F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB12.tag=0,((_tmpB12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB11[0]=& _tmpB12,Cyc_aprintf(((_tmpB10="get_member_offset %s failed",_tag_dyneither(_tmpB10,sizeof(char),28))),_tag_dyneither(_tmpB11,sizeof(void*),1)))))))),_tag_dyneither(_tmpB0F,sizeof(void*),0)));};}
# 1753
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3BB=Cyc_Tcutil_compress(*typ);void*_tmp3BD;struct Cyc_Absyn_Tqual _tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;union Cyc_Absyn_Constraint*_tmp3C0;unsigned int _tmp3C1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3BC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BB;if(_tmp3BC->tag != 8)goto _LL1D6;else{_tmp3BD=(_tmp3BC->f1).elt_type;_tmp3BE=(_tmp3BC->f1).tq;_tmp3BF=(_tmp3BC->f1).num_elts;_tmp3C0=(_tmp3BC->f1).zero_term;_tmp3C1=(_tmp3BC->f1).zt_loc;}}_LL1D5:
# 1758
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3BF))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB18;struct Cyc_Absyn_ArrayInfo _tmpB17;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB16;*typ=(void*)((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB18.tag=8,((_tmpB18.f1=((_tmpB17.elt_type=_tmp3BD,((_tmpB17.tq=_tmp3BE,((_tmpB17.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB17.zero_term=_tmp3C0,((_tmpB17.zt_loc=_tmp3C1,_tmpB17)))))))))),_tmpB18)))),_tmpB16))));}
return 1;}
# 1762
goto _LL1D3;_LL1D6:;_LL1D7:
 goto _LL1D3;_LL1D3:;}
# 1765
return 0;}
# 1768
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1774
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple0*_tmp3C5=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3C5,0);
do_declare=1;}else{
# 1782
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1785
struct Cyc_Absyn_Stmt*_tmp3C6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1787
struct Cyc_Absyn_Exp*(*_tmp3C7)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*_tmp3C8=Cyc_Toc_typ_to_c(struct_type);
# 1790
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3C9=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp3CB;struct Cyc_List_List*_tmp3CC;_LL1D9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C9;if(_tmp3CA->tag != 11)goto _LL1DB;else{_tmp3CB=(_tmp3CA->f1).aggr_info;_tmp3CC=(_tmp3CA->f1).targs;}}_LL1DA:
# 1797
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3CB);
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
forall_types=_tmp3CC;
goto _LL1D8;_LL1DB:;_LL1DC: {
const char*_tmpB1B;void*_tmpB1A;(_tmpB1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1B="init_struct: bad struct type",_tag_dyneither(_tmpB1B,sizeof(char),29))),_tag_dyneither(_tmpB1A,sizeof(void*),0)));}_LL1D8:;}{
# 1803
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3CF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3CF->tl != 0;_tmp3CF=_tmp3CF->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3D0=(struct Cyc_Absyn_Aggrfield*)_tmp3CF->hd;
struct _RegionHandle _tmp3D1=_new_region("temp");struct _RegionHandle*temp=& _tmp3D1;_push_region(temp);
{void*cast_type;
if(pointer)
cast_type=Cyc_Absyn_cstar_typ(aggr_typ,Cyc_Toc_mt_tq);else{
# 1813
cast_type=aggr_typ;}
_tmp3C6=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(xexp)),0);{
struct Cyc_List_List*_tmp3D2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1819
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3D2,_tmp3D0->type))){
struct Cyc_List_List*_tmp3D3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3D4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3D2,_tmp3D3);
# 1823
struct Cyc_List_List*new_fields=0;
for(_tmp3CF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3CF != 0;_tmp3CF=_tmp3CF->tl){
struct Cyc_Absyn_Aggrfield*_tmp3D5=(struct Cyc_Absyn_Aggrfield*)_tmp3CF->hd;
struct Cyc_Absyn_Aggrfield*_tmpB1C;struct Cyc_Absyn_Aggrfield*_tmp3D6=(_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C->name=_tmp3D5->name,((_tmpB1C->tq=Cyc_Toc_mt_tq,((_tmpB1C->type=
# 1828
Cyc_Tcutil_rsubstitute(temp,_tmp3D4,_tmp3D5->type),((_tmpB1C->width=_tmp3D5->width,((_tmpB1C->attributes=_tmp3D5->attributes,_tmpB1C)))))))))));
# 1835
if(_tmp3CF->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3D6->type)){
struct _dyneither_ptr**_tmpB1D;index=((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=_tmp3D6->name,_tmpB1D))));}}{
# 1842
struct Cyc_List_List*_tmpB1E;new_fields=((_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->hd=_tmp3D6,((_tmpB1E->tl=new_fields,_tmpB1E))))));};}
# 1844
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB26;void*_tmpB25[1];const char*_tmpB24;struct _dyneither_ptr*_tmpB23;struct _dyneither_ptr*name=
(_tmpB23=_cycalloc(sizeof(*_tmpB23)),((_tmpB23[0]=(struct _dyneither_ptr)((_tmpB26.tag=1,((_tmpB26.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB25[0]=& _tmpB26,Cyc_aprintf(((_tmpB24="_genStruct%d",_tag_dyneither(_tmpB24,sizeof(char),13))),_tag_dyneither(_tmpB25,sizeof(void*),1)))))))),_tmpB23)));
struct _tuple0*_tmpB27;struct _tuple0*qv=(_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB27->f2=name,_tmpB27)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB28;struct Cyc_Absyn_AggrdeclImpl*_tmp3DA=(_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28->exist_vars=0,((_tmpB28->rgn_po=0,((_tmpB28->fields=aggrfields,((_tmpB28->tagged=0,_tmpB28)))))))));
# 1853
struct Cyc_Absyn_Aggrdecl*_tmpB29;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB29=_cycalloc(sizeof(*_tmpB29)),((_tmpB29->kind=Cyc_Absyn_StructA,((_tmpB29->sc=Cyc_Absyn_Public,((_tmpB29->name=qv,((_tmpB29->tvs=0,((_tmpB29->impl=_tmp3DA,((_tmpB29->attributes=0,_tmpB29)))))))))))));
# 1859
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB33;struct Cyc_Absyn_Aggrdecl**_tmpB32;struct Cyc_Absyn_AggrInfo _tmpB31;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB30;struct_type=(void*)((_tmpB30=_cycalloc(sizeof(*_tmpB30)),((_tmpB30[0]=((_tmpB33.tag=11,((_tmpB33.f1=((_tmpB31.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32[0]=new_ad,_tmpB32))))),((_tmpB31.targs=0,_tmpB31)))),_tmpB33)))),_tmpB30))));}
# 1863
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}};}
# 1809
;_pop_region(temp);};}{
# 1868
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3E6=_new_region("r");struct _RegionHandle*r=& _tmp3E6;_push_region(r);
{struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3E7 != 0;_tmp3E7=_tmp3E7->tl){
struct Cyc_List_List*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3EA;struct _tuple16*_tmp3E8=(struct _tuple16*)_tmp3E7->hd;_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3EA->topt));
if(_tmp3E9 == 0){
const char*_tmpB36;void*_tmpB35;(_tmpB35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB36="empty designator list",_tag_dyneither(_tmpB36,sizeof(char),22))),_tag_dyneither(_tmpB35,sizeof(void*),0)));}
if(_tmp3E9->tl != 0){
# 1878
struct _tuple0*_tmp3ED=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3EE=Cyc_Absyn_var_exp(_tmp3ED,0);
for(0;_tmp3E9 != 0;_tmp3E9=_tmp3E9->tl){
void*_tmp3EF=(void*)_tmp3E9->hd;struct _dyneither_ptr*_tmp3F1;_LL1DE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3EF;if(_tmp3F0->tag != 1)goto _LL1E0;else{_tmp3F1=_tmp3F0->f1;}}_LL1DF:
# 1884
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3F1))
_tmp3EE=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3EE);
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C7(xexp,_tmp3F1,0),_tmp3EE,0),0),_tmp3C6,0);
# 1888
goto _LL1DD;_LL1E0:;_LL1E1: {
const char*_tmpB39;void*_tmpB38;(_tmpB38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB39="array designator in struct",_tag_dyneither(_tmpB39,sizeof(char),27))),_tag_dyneither(_tmpB38,sizeof(void*),0)));}_LL1DD:;}
# 1892
Cyc_Toc_exp_to_c(nv,_tmp3EA);
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3EE,_tmp3EA,0),0),_tmp3C6,0);}else{
# 1895
void*_tmp3F4=(void*)_tmp3E9->hd;struct _dyneither_ptr*_tmp3F6;_LL1E3: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F5=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3F4;if(_tmp3F5->tag != 1)goto _LL1E5;else{_tmp3F6=_tmp3F5->f1;}}_LL1E4: {
# 1897
struct Cyc_Absyn_Exp*lval=_tmp3C7(xexp,_tmp3F6,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3F6);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3C6,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1906
{void*_tmp3F7=_tmp3EA->r;struct Cyc_List_List*_tmp3F9;struct Cyc_Absyn_Vardecl*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FD;int _tmp3FE;void*_tmp400;struct Cyc_List_List*_tmp401;_LL1E8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3F8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F7;if(_tmp3F8->tag != 25)goto _LL1EA;else{_tmp3F9=_tmp3F8->f1;}}_LL1E9:
# 1908
 _tmp3C6=Cyc_Toc_init_array(nv,lval,_tmp3F9,_tmp3C6);goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3FA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3F7;if(_tmp3FA->tag != 26)goto _LL1EC;else{_tmp3FB=_tmp3FA->f1;_tmp3FC=_tmp3FA->f2;_tmp3FD=_tmp3FA->f3;_tmp3FE=_tmp3FA->f4;}}_LL1EB:
# 1912
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3F6)){
struct _tuple5 _tmpB3C;union Cyc_Absyn_Cnst _tmpB3B;*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_times_exp(_tmp3FC,Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)_check_null(_tmp3FD->topt)),0),0),
# 1916
Cyc_Absyn_const_exp((((_tmpB3B.Int_c).val=((_tmpB3C.f1=Cyc_Absyn_Unsigned,((_tmpB3C.f2=(int)sizeof(double),_tmpB3C)))),(((_tmpB3B.Int_c).tag=5,_tmpB3B)))),0),0);}
# 1918
_tmp3C6=Cyc_Toc_init_comprehension(nv,lval,_tmp3FB,_tmp3FC,_tmp3FD,_tmp3FE,_tmp3C6,0);goto _LL1E7;_LL1EC: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3FF=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3F7;if(_tmp3FF->tag != 28)goto _LL1EE;else{_tmp400=(void*)_tmp3FF->f1;_tmp401=_tmp3FF->f2;}}_LL1ED:
# 1920
 _tmp3C6=Cyc_Toc_init_anon_struct(nv,lval,_tmp400,_tmp401,_tmp3C6);goto _LL1E7;_LL1EE:;_LL1EF: {
# 1922
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp3EA->topt));
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3EA);{
struct Cyc_Absyn_Aggrfield*_tmp404=Cyc_Absyn_lookup_field(aggrfields,_tmp3F6);
# 1926
if(Cyc_Toc_is_poly_field(struct_type,_tmp3F6) && !was_ptr_type)
_tmp3EA=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3EA);
# 1929
if(exist_types != 0)
_tmp3EA=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp404))->type),_tmp3EA);
# 1932
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3EA,0),0),_tmp3C6,0);
goto _LL1E7;};}_LL1E7:;}
# 1935
goto _LL1E2;}_LL1E5:;_LL1E6: {
const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="array designator in struct",_tag_dyneither(_tmpB3F,sizeof(char),27))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}_LL1E2:;}}}{
# 1941
struct Cyc_Absyn_Exp*_tmp407=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3C6,pointer,rgnopt,is_atomic,do_declare);_npop_handler(0);return _tmp407;};
# 1870
;_pop_region(r);};};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1946
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1949
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 1952
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpB42;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB41;return Cyc_Absyn_new_exp((void*)((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB42.tag=4,((_tmpB42.f1=e1,((_tmpB42.f2=incr,_tmpB42)))))),_tmpB41)))),0);}
# 1956
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1965
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1967
void*_tmp40A=e1->r;struct Cyc_Absyn_Stmt*_tmp40C;void*_tmp40E;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*_tmp411;struct _dyneither_ptr*_tmp412;int _tmp413;int _tmp414;_LL1F1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp40B=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp40A;if(_tmp40B->tag != 35)goto _LL1F3;else{_tmp40C=_tmp40B->f1;}}_LL1F2:
 Cyc_Toc_lvalue_assign_stmt(_tmp40C,fs,f,f_env);goto _LL1F0;_LL1F3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp40D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40A;if(_tmp40D->tag != 13)goto _LL1F5;else{_tmp40E=(void*)_tmp40D->f1;_tmp40F=_tmp40D->f2;}}_LL1F4:
 Cyc_Toc_lvalue_assign(_tmp40F,fs,f,f_env);goto _LL1F0;_LL1F5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp410=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp40A;if(_tmp410->tag != 20)goto _LL1F7;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;_tmp413=_tmp410->f3;_tmp414=_tmp410->f4;}}_LL1F6:
# 1972
 e1->r=_tmp411->r;
{struct Cyc_List_List*_tmpB43;Cyc_Toc_lvalue_assign(e1,((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43->hd=_tmp412,((_tmpB43->tl=fs,_tmpB43)))))),f,f_env);}
goto _LL1F0;_LL1F7:;_LL1F8: {
# 1980
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1982
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1F0;}_LL1F0:;}
# 1988
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1990
void*_tmp416=s->r;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Decl*_tmp41A;struct Cyc_Absyn_Stmt*_tmp41B;struct Cyc_Absyn_Stmt*_tmp41D;_LL1FA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp417=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp416;if(_tmp417->tag != 1)goto _LL1FC;else{_tmp418=_tmp417->f1;}}_LL1FB:
 Cyc_Toc_lvalue_assign(_tmp418,fs,f,f_env);goto _LL1F9;_LL1FC: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp419=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp416;if(_tmp419->tag != 12)goto _LL1FE;else{_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;}}_LL1FD:
# 1993
 Cyc_Toc_lvalue_assign_stmt(_tmp41B,fs,f,f_env);goto _LL1F9;_LL1FE: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp41C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp416;if(_tmp41C->tag != 2)goto _LL200;else{_tmp41D=_tmp41C->f2;}}_LL1FF:
 Cyc_Toc_lvalue_assign_stmt(_tmp41D,fs,f,f_env);goto _LL1F9;_LL200:;_LL201: {
const char*_tmpB47;void*_tmpB46[1];struct Cyc_String_pa_PrintArg_struct _tmpB45;(_tmpB45.tag=0,((_tmpB45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB46[0]=& _tmpB45,Cyc_Toc_toc_impos(((_tmpB47="lvalue_assign_stmt: %s",_tag_dyneither(_tmpB47,sizeof(char),23))),_tag_dyneither(_tmpB46,sizeof(void*),1)))))));}_LL1F9:;}
# 1999
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2003
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp421=e->r;void**_tmp423;struct Cyc_Absyn_Exp**_tmp424;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Stmt*_tmp428;_LL203: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp422=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp421;if(_tmp422->tag != 13)goto _LL205;else{_tmp423=(void**)((void**)& _tmp422->f1);_tmp424=(struct Cyc_Absyn_Exp**)& _tmp422->f2;}}_LL204:
# 2006
*_tmp424=Cyc_Toc_push_address_exp(*_tmp424);
*_tmp423=Cyc_Absyn_cstar_typ(*_tmp423,Cyc_Toc_mt_tq);
return e;_LL205: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp425=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp421;if(_tmp425->tag != 19)goto _LL207;else{_tmp426=_tmp425->f1;}}_LL206:
# 2010
 return _tmp426;_LL207: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp427=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp421;if(_tmp427->tag != 35)goto _LL209;else{_tmp428=_tmp427->f1;}}_LL208:
# 2014
 Cyc_Toc_push_address_stmt(_tmp428);
return e;_LL209:;_LL20A:
# 2017
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpB4B;void*_tmpB4A[1];struct Cyc_String_pa_PrintArg_struct _tmpB49;(_tmpB49.tag=0,((_tmpB49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB4A[0]=& _tmpB49,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="can't take & of exp %s",_tag_dyneither(_tmpB4B,sizeof(char),23))),_tag_dyneither(_tmpB4A,sizeof(void*),1)))))));};_LL202:;}
# 2022
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp42C=s->r;struct Cyc_Absyn_Stmt*_tmp42E;struct Cyc_Absyn_Stmt*_tmp430;struct Cyc_Absyn_Exp**_tmp432;_LL20C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp42D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp42C;if(_tmp42D->tag != 2)goto _LL20E;else{_tmp42E=_tmp42D->f2;}}_LL20D:
 _tmp430=_tmp42E;goto _LL20F;_LL20E: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp42F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp42C;if(_tmp42F->tag != 12)goto _LL210;else{_tmp430=_tmp42F->f2;}}_LL20F:
 Cyc_Toc_push_address_stmt(_tmp430);goto _LL20B;_LL210: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp431=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp42C;if(_tmp431->tag != 1)goto _LL212;else{_tmp432=(struct Cyc_Absyn_Exp**)& _tmp431->f1;}}_LL211:
*_tmp432=Cyc_Toc_push_address_exp(*_tmp432);goto _LL20B;_LL212:;_LL213: {
# 2028
const char*_tmpB4F;void*_tmpB4E[1];struct Cyc_String_pa_PrintArg_struct _tmpB4D;(_tmpB4D.tag=0,((_tmpB4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB4E[0]=& _tmpB4D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4F="can't take & of stmt %s",_tag_dyneither(_tmpB4F,sizeof(char),24))),_tag_dyneither(_tmpB4E,sizeof(void*),1)))))));}_LL20B:;}
# 2033
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2035
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2037
if(x == 0)return 0;
{struct Cyc_List_List*_tmpB50;result=((_tmpB50=_region_malloc(r2,sizeof(*_tmpB50)),((_tmpB50->hd=f(x->hd,env),((_tmpB50->tl=0,_tmpB50))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpB51;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpB51=_region_malloc(r2,sizeof(*_tmpB51)),((_tmpB51->hd=f(x->hd,env),((_tmpB51->tl=0,_tmpB51))))));}
prev=prev->tl;}
# 2044
return result;}
# 2046
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2050
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpB52;return(_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->f1=0,((_tmpB52->f2=e,_tmpB52)))));}
# 2054
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp439=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp43B;_LL215: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp439;if(_tmp43A->tag != 5)goto _LL217;else{_tmp43B=_tmp43A->f1;}}_LL216:
 return _tmp43B;_LL217:;_LL218: {
const char*_tmpB55;void*_tmpB54;(_tmpB54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpB55,sizeof(char),28))),_tag_dyneither(_tmpB54,sizeof(void*),0)));}_LL214:;}
# 2064
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp43E=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp441;enum Cyc_Absyn_Sign _tmp443;enum Cyc_Absyn_Sign _tmp447;enum Cyc_Absyn_Sign _tmp449;enum Cyc_Absyn_Sign _tmp44B;int _tmp44F;_LL21A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp43F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43E;if(_tmp43F->tag != 5)goto _LL21C;}_LL21B:
 res=Cyc_Absyn_null_exp(0);goto _LL219;_LL21C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp440=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43E;if(_tmp440->tag != 6)goto _LL21E;else{_tmp441=_tmp440->f1;if(_tmp440->f2 != Cyc_Absyn_Char_sz)goto _LL21E;}}_LL21D:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp441,'\000'),0);goto _LL219;_LL21E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43E;if(_tmp442->tag != 6)goto _LL220;else{_tmp443=_tmp442->f1;if(_tmp442->f2 != Cyc_Absyn_Short_sz)goto _LL220;}}_LL21F:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp443,0),0);goto _LL219;_LL220: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp43E;if(_tmp444->tag != 13)goto _LL222;}_LL221:
 goto _LL223;_LL222: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp445=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp43E;if(_tmp445->tag != 14)goto _LL224;}_LL223:
 _tmp447=Cyc_Absyn_Unsigned;goto _LL225;_LL224: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43E;if(_tmp446->tag != 6)goto _LL226;else{_tmp447=_tmp446->f1;if(_tmp446->f2 != Cyc_Absyn_Int_sz)goto _LL226;}}_LL225:
 _tmp449=_tmp447;goto _LL227;_LL226: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43E;if(_tmp448->tag != 6)goto _LL228;else{_tmp449=_tmp448->f1;if(_tmp448->f2 != Cyc_Absyn_Long_sz)goto _LL228;}}_LL227:
# 2074
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp449,0),0);goto _LL219;_LL228: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp43E;if(_tmp44A->tag != 6)goto _LL22A;else{_tmp44B=_tmp44A->f1;if(_tmp44A->f2 != Cyc_Absyn_LongLong_sz)goto _LL22A;}}_LL229:
# 2076
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp44B,(long long)0),0);goto _LL219;_LL22A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp43E;if(_tmp44C->tag != 7)goto _LL22C;else{if(_tmp44C->f1 != 0)goto _LL22C;}}_LL22B:
{const char*_tmpB56;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB56="0.0F",_tag_dyneither(_tmpB56,sizeof(char),5))),0),0);}goto _LL219;_LL22C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp43E;if(_tmp44D->tag != 7)goto _LL22E;else{if(_tmp44D->f1 != 1)goto _LL22E;}}_LL22D:
{const char*_tmpB57;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB57="0.0",_tag_dyneither(_tmpB57,sizeof(char),4))),1),0);}goto _LL219;_LL22E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp43E;if(_tmp44E->tag != 7)goto _LL230;else{_tmp44F=_tmp44E->f1;}}_LL22F:
{const char*_tmpB58;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpB58="0.0L",_tag_dyneither(_tmpB58,sizeof(char),5))),_tmp44F),0);}goto _LL219;_LL230:;_LL231: {
# 2081
const char*_tmpB5C;void*_tmpB5B[1];struct Cyc_String_pa_PrintArg_struct _tmpB5A;(_tmpB5A.tag=0,((_tmpB5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB5B[0]=& _tmpB5A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5C="found non-zero type %s in generate_zero",_tag_dyneither(_tmpB5C,sizeof(char),40))),_tag_dyneither(_tmpB5B,sizeof(void*),1)))))));}_LL219:;}
# 2083
res->topt=t;
return res;}
# 2090
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2103 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp456=Cyc_Toc_typ_to_c(elt_type);
void*_tmp457=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp458=Cyc_Absyn_cstar_typ(_tmp456,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpB5D;struct Cyc_Core_Opt*_tmp459=(_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->v=_tmp458,_tmpB5D)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp45A=e1->r;struct Cyc_Absyn_Exp*_tmp45C;struct Cyc_Absyn_Exp*_tmp45E;struct Cyc_Absyn_Exp*_tmp45F;_LL233: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp45B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp45A;if(_tmp45B->tag != 19)goto _LL235;else{_tmp45C=_tmp45B->f1;}}_LL234:
# 2111
 if(!is_dyneither){
_tmp45C=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp45C,0,Cyc_Absyn_Other_coercion,0);
_tmp45C->topt=fat_ptr_type;}
# 2115
Cyc_Toc_exp_to_c(nv,_tmp45C);xinit=_tmp45C;goto _LL232;_LL235: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp45D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp45A;if(_tmp45D->tag != 22)goto _LL237;else{_tmp45E=_tmp45D->f1;_tmp45F=_tmp45D->f2;}}_LL236:
# 2117
 if(!is_dyneither){
_tmp45E=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp45E,0,Cyc_Absyn_Other_coercion,0);
_tmp45E->topt=fat_ptr_type;}
# 2121
Cyc_Toc_exp_to_c(nv,_tmp45E);Cyc_Toc_exp_to_c(nv,_tmp45F);
{struct Cyc_Absyn_Exp*_tmpB5E[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpB5E[2]=_tmp45F,((_tmpB5E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB5E[0]=_tmp45E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB5E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL232;_LL237:;_LL238: {
const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpB61,sizeof(char),53))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}_LL232:;}{
# 2127
struct _tuple0*_tmp463=Cyc_Toc_temp_var();
struct _RegionHandle _tmp464=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp464;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp465=Cyc_Toc_add_varmap(rgn2,nv,_tmp463,Cyc_Absyn_var_exp(_tmp463,0));
struct Cyc_Absyn_Vardecl*_tmpB62;struct Cyc_Absyn_Vardecl*_tmp466=(_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62->sc=Cyc_Absyn_Public,((_tmpB62->name=_tmp463,((_tmpB62->tq=Cyc_Toc_mt_tq,((_tmpB62->type=_tmp457,((_tmpB62->initializer=xinit,((_tmpB62->rgn=0,((_tmpB62->attributes=0,((_tmpB62->escapes=0,_tmpB62)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB65;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB64;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp467=(_tmpB64=_cycalloc(sizeof(*_tmpB64)),((_tmpB64[0]=((_tmpB65.tag=4,((_tmpB65.f1=_tmp466,_tmpB65)))),_tmpB64)));
struct Cyc_Absyn_Exp*_tmp468=Cyc_Absyn_varb_exp(_tmp463,(void*)_tmp467,0);
_tmp468->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp469=Cyc_Absyn_deref_exp(_tmp468,0);
_tmp469->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp465,_tmp469);{
struct _tuple0*_tmp46A=Cyc_Toc_temp_var();
_tmp465=Cyc_Toc_add_varmap(rgn2,_tmp465,_tmp46A,Cyc_Absyn_var_exp(_tmp46A,0));{
struct Cyc_Absyn_Vardecl*_tmpB66;struct Cyc_Absyn_Vardecl*_tmp46B=(_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66->sc=Cyc_Absyn_Public,((_tmpB66->name=_tmp46A,((_tmpB66->tq=Cyc_Toc_mt_tq,((_tmpB66->type=_tmp456,((_tmpB66->initializer=_tmp469,((_tmpB66->rgn=0,((_tmpB66->attributes=0,((_tmpB66->escapes=0,_tmpB66)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB69;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB68;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp46C=(_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB69.tag=4,((_tmpB69.f1=_tmp46B,_tmpB69)))),_tmpB68)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp46D=Cyc_Absyn_varb_exp(_tmp46A,(void*)_tmp46C,0);
_tmp46D->topt=_tmp469->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp46D,e2,0);
z_init->topt=_tmp46D->topt;}
# 2148
Cyc_Toc_exp_to_c(_tmp465,z_init);{
struct _tuple0*_tmp46E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpB6A;struct Cyc_Absyn_Vardecl*_tmp46F=(_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->sc=Cyc_Absyn_Public,((_tmpB6A->name=_tmp46E,((_tmpB6A->tq=Cyc_Toc_mt_tq,((_tmpB6A->type=_tmp456,((_tmpB6A->initializer=z_init,((_tmpB6A->rgn=0,((_tmpB6A->attributes=0,((_tmpB6A->escapes=0,_tmpB6A)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB6D;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB6C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp470=(_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6D.tag=4,((_tmpB6D.f1=_tmp46F,_tmpB6D)))),_tmpB6C)));
_tmp465=Cyc_Toc_add_varmap(rgn2,_tmp465,_tmp46E,Cyc_Absyn_var_exp(_tmp46E,0));{
# 2155
struct Cyc_Absyn_Exp*_tmp471=Cyc_Absyn_varb_exp(_tmp46A,(void*)_tmp46C,0);_tmp471->topt=_tmp469->topt;{
struct Cyc_Absyn_Exp*_tmp472=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp473=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp471,_tmp472,0);
_tmp473->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp465,_tmp473);{
# 2161
struct Cyc_Absyn_Exp*_tmp474=Cyc_Absyn_varb_exp(_tmp46E,(void*)_tmp470,0);_tmp474->topt=_tmp469->topt;{
struct Cyc_Absyn_Exp*_tmp475=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp476=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp474,_tmp475,0);
_tmp476->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp465,_tmp476);{
# 2167
struct Cyc_Absyn_Exp*_tmpB6E[2];struct Cyc_List_List*_tmp477=(_tmpB6E[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpB6E[0]=
# 2167
Cyc_Absyn_varb_exp(_tmp463,(void*)_tmp467,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB6E,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2169
struct Cyc_Absyn_Exp*_tmp478=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp477,0),_tmp478,0);{
# 2174
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp473,_tmp476,0),0);
struct Cyc_Absyn_Stmt*_tmp47A=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp47B=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp463,(void*)_tmp467,0),Cyc_Toc_curr_sp,0);
_tmp47B=Cyc_Toc_cast_it(_tmp458,_tmp47B);{
struct Cyc_Absyn_Exp*_tmp47C=Cyc_Absyn_deref_exp(_tmp47B,0);
struct Cyc_Absyn_Exp*_tmp47D=Cyc_Absyn_assign_exp(_tmp47C,Cyc_Absyn_var_exp(_tmp46E,0),0);
struct Cyc_Absyn_Stmt*_tmp47E=Cyc_Absyn_exp_stmt(_tmp47D,0);
_tmp47E=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp479,_tmp47A,Cyc_Absyn_skip_stmt(0),0),_tmp47E,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB74;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB73;struct Cyc_Absyn_Decl*_tmpB72;_tmp47E=Cyc_Absyn_decl_stmt(((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->r=(void*)((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74[0]=((_tmpB73.tag=0,((_tmpB73.f1=_tmp46F,_tmpB73)))),_tmpB74)))),((_tmpB72->loc=0,_tmpB72)))))),_tmp47E,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB7A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB79;struct Cyc_Absyn_Decl*_tmpB78;_tmp47E=Cyc_Absyn_decl_stmt(((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->r=(void*)((_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=((_tmpB79.tag=0,((_tmpB79.f1=_tmp46B,_tmpB79)))),_tmpB7A)))),((_tmpB78->loc=0,_tmpB78)))))),_tmp47E,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB80;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB7F;struct Cyc_Absyn_Decl*_tmpB7E;_tmp47E=Cyc_Absyn_decl_stmt(((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->r=(void*)((_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80[0]=((_tmpB7F.tag=0,((_tmpB7F.f1=_tmp466,_tmpB7F)))),_tmpB80)))),((_tmpB7E->loc=0,_tmpB7E)))))),_tmp47E,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp47E);};};};};};};};};};};};}
# 2129
;_pop_region(rgn2);};}
# 2200 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2204
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp493=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp495;_LL23A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp494=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp493;if(_tmp494->tag != 11)goto _LL23C;else{_tmp495=(_tmp494->f1).aggr_info;}}_LL23B:
# 2207
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp495);goto _LL239;_LL23C:;_LL23D: {
struct Cyc_String_pa_PrintArg_struct _tmpB88;void*_tmpB87[1];const char*_tmpB86;void*_tmpB85;(_tmpB85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpB88.tag=0,((_tmpB88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB87[0]=& _tmpB88,Cyc_aprintf(((_tmpB86="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpB86,sizeof(char),51))),_tag_dyneither(_tmpB87,sizeof(void*),1)))))))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}_LL239:;}{
# 2211
struct _tuple0*_tmp49A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp49B=Cyc_Absyn_var_exp(_tmp49A,0);
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_aggrarrow_exp(_tmp49B,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_neq_exp(_tmp49D,_tmp49C,0);
struct Cyc_Absyn_Exp*_tmp49F=Cyc_Absyn_aggrarrow_exp(_tmp49B,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4A0=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp49F,0),0);
struct Cyc_Absyn_Stmt*_tmp4A1=Cyc_Absyn_ifthenelse_stmt(_tmp49E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4A2=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4A4=Cyc_Absyn_declare_stmt(_tmp49A,_tmp4A2,_tmp4A3,Cyc_Absyn_seq_stmt(_tmp4A1,_tmp4A0,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4A4);}else{
# 2225
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Toc_member_exp(aggrproj(_tmp49B,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Absyn_neq_exp(_tmp4A5,_tmp49C,0);
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Toc_member_exp(aggrproj(_tmp49B,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4A8=Cyc_Absyn_exp_stmt(_tmp4A7,0);
struct Cyc_Absyn_Stmt*_tmp4A9=Cyc_Absyn_ifthenelse_stmt(_tmp4A6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4AA=Cyc_Absyn_declare_stmt(_tmp49A,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4A9,_tmp4A8,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4AA);}};}
# 2238
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2241
void*_tmp4AB=e->r;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AF;struct _dyneither_ptr*_tmp4B0;int*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct _dyneither_ptr*_tmp4B4;int*_tmp4B5;_LL23F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4AC=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AB;if(_tmp4AC->tag != 13)goto _LL241;else{_tmp4AD=_tmp4AC->f2;}}_LL240: {
const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8B="cast on lhs!",_tag_dyneither(_tmpB8B,sizeof(char),13))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}_LL241: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4AE=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4AB;if(_tmp4AE->tag != 20)goto _LL243;else{_tmp4AF=_tmp4AE->f1;_tmp4B0=_tmp4AE->f2;_tmp4B1=(int*)& _tmp4AE->f4;}}_LL242: {
# 2244
void*_tmp4B8=Cyc_Tcutil_compress((void*)_check_null(_tmp4AF->topt));union Cyc_Absyn_AggrInfoU _tmp4BA;_LL248: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B8;if(_tmp4B9->tag != 11)goto _LL24A;else{_tmp4BA=(_tmp4B9->f1).aggr_info;}}_LL249: {
# 2246
struct Cyc_Absyn_Aggrdecl*_tmp4BB=Cyc_Absyn_get_known_aggrdecl(_tmp4BA);
*f_tag=Cyc_Toc_get_member_offset(_tmp4BB,_tmp4B0);{
# 2249
const char*_tmpB90;void*_tmpB8F[2];struct Cyc_String_pa_PrintArg_struct _tmpB8E;struct Cyc_String_pa_PrintArg_struct _tmpB8D;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB8D.tag=0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B0),((_tmpB8E.tag=0,((_tmpB8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4BB->name).f2),((_tmpB8F[0]=& _tmpB8E,((_tmpB8F[1]=& _tmpB8D,Cyc_aprintf(((_tmpB90="_union_%s_%s",_tag_dyneither(_tmpB90,sizeof(char),13))),_tag_dyneither(_tmpB8F,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB91;*tagged_member_type=Cyc_Absyn_strct(((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91[0]=str,_tmpB91)))));}
if(clear_read)*_tmp4B1=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BB->impl))->tagged;};}_LL24A:;_LL24B:
 return 0;_LL247:;}_LL243: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4B2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4AB;if(_tmp4B2->tag != 21)goto _LL245;else{_tmp4B3=_tmp4B2->f1;_tmp4B4=_tmp4B2->f2;_tmp4B5=(int*)& _tmp4B2->f4;}}_LL244: {
# 2256
void*_tmp4C1=Cyc_Tcutil_compress((void*)_check_null(_tmp4B3->topt));void*_tmp4C3;_LL24D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C1;if(_tmp4C2->tag != 5)goto _LL24F;else{_tmp4C3=(_tmp4C2->f1).elt_typ;}}_LL24E: {
# 2258
void*_tmp4C4=Cyc_Tcutil_compress(_tmp4C3);union Cyc_Absyn_AggrInfoU _tmp4C6;_LL252: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4C5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C4;if(_tmp4C5->tag != 11)goto _LL254;else{_tmp4C6=(_tmp4C5->f1).aggr_info;}}_LL253: {
# 2260
struct Cyc_Absyn_Aggrdecl*_tmp4C7=Cyc_Absyn_get_known_aggrdecl(_tmp4C6);
*f_tag=Cyc_Toc_get_member_offset(_tmp4C7,_tmp4B4);{
# 2263
const char*_tmpB96;void*_tmpB95[2];struct Cyc_String_pa_PrintArg_struct _tmpB94;struct Cyc_String_pa_PrintArg_struct _tmpB93;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpB93.tag=0,((_tmpB93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B4),((_tmpB94.tag=0,((_tmpB94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4C7->name).f2),((_tmpB95[0]=& _tmpB94,((_tmpB95[1]=& _tmpB93,Cyc_aprintf(((_tmpB96="_union_%s_%s",_tag_dyneither(_tmpB96,sizeof(char),13))),_tag_dyneither(_tmpB95,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpB97;*tagged_member_type=Cyc_Absyn_strct(((_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97[0]=str,_tmpB97)))));}
if(clear_read)*_tmp4B5=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C7->impl))->tagged;};}_LL254:;_LL255:
 return 0;_LL251:;}_LL24F:;_LL250:
# 2269
 return 0;_LL24C:;}_LL245:;_LL246:
# 2271
 return 0;_LL23E:;}
# 2283 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp4CD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4CD,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4CE,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4CD,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2307 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2311
struct _tuple0*_tmp4CF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4CF,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2321
struct Cyc_Absyn_Exp*_tmp4D0=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4D0,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2324
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4CF,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpBE6(unsigned int*_tmpBE5,unsigned int*_tmpBE4,struct _tuple0***_tmpBE2){for(*_tmpBE5=0;*_tmpBE5 < *_tmpBE4;(*_tmpBE5)++){(*_tmpBE2)[*_tmpBE5]=
# 2701 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2330 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4D1=e->r;
if(e->topt == 0){
const char*_tmpB9B;void*_tmpB9A[1];struct Cyc_String_pa_PrintArg_struct _tmpB99;(_tmpB99.tag=0,((_tmpB99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpB9A[0]=& _tmpB99,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9B="exp_to_c: no type for %s",_tag_dyneither(_tmpB9B,sizeof(char),25))),_tag_dyneither(_tmpB9A,sizeof(void*),1)))))));}
# 2336
if((nv->struct_info).lhs_exp != 0){
void*_tmp4D5=_tmp4D1;_LL257: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4D6=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D5;if(_tmp4D6->tag != 27)goto _LL259;}_LL258:
 goto _LL256;_LL259:;_LL25A:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL256:;}{
# 2344
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4D7=_tmp4D1;struct _tuple0*_tmp4DB;void*_tmp4DC;enum Cyc_Absyn_Primop _tmp4DE;struct Cyc_List_List*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E1;enum Cyc_Absyn_Incrementor _tmp4E2;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Core_Opt*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_List_List*_tmp4F9;int _tmp4FA;struct Cyc_List_List*_tmp4FB;struct Cyc_Absyn_VarargInfo*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp502;struct Cyc_List_List*_tmp503;void**_tmp505;struct Cyc_Absyn_Exp*_tmp506;int _tmp507;enum Cyc_Absyn_Coercion _tmp508;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50F;void*_tmp511;void*_tmp513;struct Cyc_List_List*_tmp514;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp518;struct _dyneither_ptr*_tmp519;int _tmp51A;int _tmp51B;struct Cyc_Absyn_Exp*_tmp51D;struct _dyneither_ptr*_tmp51E;int _tmp51F;int _tmp520;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_List_List*_tmp525;struct Cyc_List_List*_tmp527;struct Cyc_Absyn_Vardecl*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp52B;int _tmp52C;struct _tuple0*_tmp52E;struct Cyc_List_List*_tmp52F;struct Cyc_List_List*_tmp530;struct Cyc_Absyn_Aggrdecl*_tmp531;void*_tmp533;struct Cyc_List_List*_tmp534;struct Cyc_List_List*_tmp536;struct Cyc_Absyn_Datatypedecl*_tmp537;struct Cyc_Absyn_Datatypefield*_tmp538;int _tmp53C;struct Cyc_Absyn_Exp*_tmp53D;void**_tmp53E;struct Cyc_Absyn_Exp*_tmp53F;int _tmp540;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp545;struct _dyneither_ptr*_tmp546;struct Cyc_Absyn_Stmt*_tmp548;_LL25C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4D8->tag != 0)goto _LL25E;else{if(((_tmp4D8->f1).Null_c).tag != 1)goto _LL25E;}}_LL25D: {
# 2351
struct Cyc_Absyn_Exp*_tmp54D=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp54D,_tmp54D))->r;else{
# 2356
struct Cyc_Absyn_Exp*_tmpB9C[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpB9C[2]=_tmp54D,((_tmpB9C[1]=_tmp54D,((_tmpB9C[0]=_tmp54D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB9C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2358
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2360
goto _LL25B;}_LL25E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4D9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4D9->tag != 0)goto _LL260;}_LL25F:
 goto _LL25B;_LL260: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4DA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4DA->tag != 1)goto _LL262;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=(void*)_tmp4DA->f2;}}_LL261:
# 2363
{struct _handler_cons _tmp54F;_push_handler(& _tmp54F);{int _tmp551=0;if(setjmp(_tmp54F.handler))_tmp551=1;if(!_tmp551){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4DB))->r;;_pop_handler();}else{void*_tmp550=(void*)_exn_thrown;void*_tmp553=_tmp550;void*_tmp555;_LL2AF: {struct Cyc_Dict_Absent_exn_struct*_tmp554=(struct Cyc_Dict_Absent_exn_struct*)_tmp553;if(_tmp554->tag != Cyc_Dict_Absent)goto _LL2B1;}_LL2B0: {
# 2365
const char*_tmpBA0;void*_tmpB9F[1];struct Cyc_String_pa_PrintArg_struct _tmpB9E;(_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4DB)),((_tmpB9F[0]=& _tmpB9E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA0="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBA0,sizeof(char),32))),_tag_dyneither(_tmpB9F,sizeof(void*),1)))))));}_LL2B1: _tmp555=_tmp553;_LL2B2:(void)_throw(_tmp555);_LL2AE:;}};}
# 2367
goto _LL25B;_LL262: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4DD=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4DD->tag != 2)goto _LL264;else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;}}_LL263: {
# 2370
struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4DF);
# 2372
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4DF);
switch(_tmp4DE){case Cyc_Absyn_Numelts: _LL2B3: {
# 2375
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
{void*_tmp55A=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp55C;void*_tmp55E;union Cyc_Absyn_Constraint*_tmp55F;union Cyc_Absyn_Constraint*_tmp560;union Cyc_Absyn_Constraint*_tmp561;_LL2B6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp55A;if(_tmp55B->tag != 8)goto _LL2B8;else{_tmp55C=(_tmp55B->f1).num_elts;}}_LL2B7:
# 2379
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp55C))){
const char*_tmpBA3;void*_tmpBA2;(_tmpBA2=0,Cyc_Tcutil_terr(e->loc,((_tmpBA3="can't calculate numelts",_tag_dyneither(_tmpBA3,sizeof(char),24))),_tag_dyneither(_tmpBA2,sizeof(void*),0)));}
e->r=_tmp55C->r;goto _LL2B5;_LL2B8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp55A;if(_tmp55D->tag != 5)goto _LL2BA;else{_tmp55E=(_tmp55D->f1).elt_typ;_tmp55F=((_tmp55D->f1).ptr_atts).nullable;_tmp560=((_tmp55D->f1).ptr_atts).bounds;_tmp561=((_tmp55D->f1).ptr_atts).zero_term;}}_LL2B9:
# 2383
{void*_tmp564=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp560);struct Cyc_Absyn_Exp*_tmp567;_LL2BD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp565=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp564;if(_tmp565->tag != 0)goto _LL2BF;}_LL2BE:
# 2385
{struct Cyc_Absyn_Exp*_tmpBA4[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpBA4[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp55E),0),((_tmpBA4[0]=(struct Cyc_Absyn_Exp*)_tmp4DF->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA4,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2BC;_LL2BF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp566=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp564;if(_tmp566->tag != 1)goto _LL2BC;else{_tmp567=_tmp566->f1;}}_LL2C0:
# 2391
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp561)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4DF->hd);
# 2395
struct Cyc_Absyn_Exp*_tmpBA5[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpBA5[1]=_tmp567,((_tmpBA5[0]=(struct Cyc_Absyn_Exp*)_tmp4DF->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA5,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55F)){
if(!Cyc_Evexp_c_can_eval(_tmp567)){
const char*_tmpBA8;void*_tmpBA7;(_tmpBA7=0,Cyc_Tcutil_terr(e->loc,((_tmpBA8="can't calculate numelts",_tag_dyneither(_tmpBA8,sizeof(char),24))),_tag_dyneither(_tmpBA7,sizeof(void*),0)));}
# 2400
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp567,Cyc_Absyn_uint_exp(0,0));}else{
# 2402
e->r=_tmp567->r;goto _LL2BC;}}
# 2404
goto _LL2BC;_LL2BC:;}
# 2406
goto _LL2B5;_LL2BA:;_LL2BB: {
# 2408
const char*_tmpBAD;void*_tmpBAC[2];struct Cyc_String_pa_PrintArg_struct _tmpBAB;struct Cyc_String_pa_PrintArg_struct _tmpBAA;(_tmpBAA.tag=0,((_tmpBAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBAB.tag=0,((_tmpBAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpBAC[0]=& _tmpBAB,((_tmpBAC[1]=& _tmpBAA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAD="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBAD,sizeof(char),41))),_tag_dyneither(_tmpBAC,sizeof(void*),2)))))))))))));}_LL2B5:;}
# 2411
break;}case Cyc_Absyn_Plus: _LL2B4:
# 2416
{void*_tmp570=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp559))->hd);void*_tmp572;union Cyc_Absyn_Constraint*_tmp573;union Cyc_Absyn_Constraint*_tmp574;_LL2C3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp570;if(_tmp571->tag != 5)goto _LL2C5;else{_tmp572=(_tmp571->f1).elt_typ;_tmp573=((_tmp571->f1).ptr_atts).bounds;_tmp574=((_tmp571->f1).ptr_atts).zero_term;}}_LL2C4:
# 2418
{void*_tmp575=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp573);struct Cyc_Absyn_Exp*_tmp578;_LL2C8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp576=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp575;if(_tmp576->tag != 0)goto _LL2CA;}_LL2C9: {
# 2420
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpBAE[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBAE[2]=e2,((_tmpBAE[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp572),0),((_tmpBAE[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2C7;}_LL2CA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp577=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp575;if(_tmp577->tag != 1)goto _LL2C7;else{_tmp578=_tmp577->f1;}}_LL2CB:
# 2426
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp574)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
struct Cyc_Absyn_Exp*_tmpBAF[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpBAF[2]=e2,((_tmpBAF[1]=_tmp578,((_tmpBAF[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAF,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2431
goto _LL2C7;_LL2C7:;}
# 2433
goto _LL2C2;_LL2C5:;_LL2C6:
# 2435
 goto _LL2C2;_LL2C2:;}
# 2437
break;case Cyc_Absyn_Minus: _LL2C1: {
# 2442
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp559))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp559->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2452
struct Cyc_Absyn_Exp*_tmpBB0[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBB0[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpBB0[1]=
# 2453
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBB0[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB0,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2457
break;}case Cyc_Absyn_Eq: _LL2CC:
 goto _LL2CD;case Cyc_Absyn_Neq: _LL2CD:
 goto _LL2CE;case Cyc_Absyn_Gt: _LL2CE:
 goto _LL2CF;case Cyc_Absyn_Gte: _LL2CF:
 goto _LL2D0;case Cyc_Absyn_Lt: _LL2D0:
 goto _LL2D1;case Cyc_Absyn_Lte: _LL2D1: {
# 2465
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4DF->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp559))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp559->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2D2:
 break;}
# 2477
goto _LL25B;}_LL264: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4E0=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4E0->tag != 4)goto _LL266;else{_tmp4E1=_tmp4E0->f1;_tmp4E2=_tmp4E0->f2;}}_LL265: {
# 2479
void*e2_cyc_typ=(void*)_check_null(_tmp4E1->topt);
# 2488 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpBB1;struct _dyneither_ptr incr_str=(_tmpBB1="increment",_tag_dyneither(_tmpBB1,sizeof(char),10));
if(_tmp4E2 == Cyc_Absyn_PreDec  || _tmp4E2 == Cyc_Absyn_PostDec){const char*_tmpBB2;incr_str=((_tmpBB2="decrement",_tag_dyneither(_tmpBB2,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E1,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpBB6;void*_tmpBB5[1];struct Cyc_String_pa_PrintArg_struct _tmpBB4;(_tmpBB4.tag=0,((_tmpBB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBB5[0]=& _tmpBB4,Cyc_Tcutil_terr(e->loc,((_tmpBB6="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpBB6,sizeof(char),74))),_tag_dyneither(_tmpBB5,sizeof(void*),1)))))));}{
const char*_tmpBB9;void*_tmpBB8;(_tmpBB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB9="in-place inc/dec on zero-term",_tag_dyneither(_tmpBB9,sizeof(char),30))),_tag_dyneither(_tmpBB8,sizeof(void*),0)));};}{
# 2497
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4E1,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp582=Cyc_Absyn_signed_int_exp(1,0);
_tmp582->topt=Cyc_Absyn_sint_typ;
switch(_tmp4E2){case Cyc_Absyn_PreInc: _LL2D4:
# 2504
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBBF;struct Cyc_Core_Opt*_tmpBBE;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBBD;e->r=(void*)((_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD[0]=((_tmpBBF.tag=3,((_tmpBBF.f1=_tmp4E1,((_tmpBBF.f2=((_tmpBBE=_cycalloc_atomic(sizeof(*_tmpBBE)),((_tmpBBE->v=(void*)Cyc_Absyn_Plus,_tmpBBE)))),((_tmpBBF.f3=_tmp582,_tmpBBF)))))))),_tmpBBD))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2D5:
# 2508
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpBC5;struct Cyc_Core_Opt*_tmpBC4;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBC3;e->r=(void*)((_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3[0]=((_tmpBC5.tag=3,((_tmpBC5.f1=_tmp4E1,((_tmpBC5.f2=((_tmpBC4=_cycalloc_atomic(sizeof(*_tmpBC4)),((_tmpBC4->v=(void*)Cyc_Absyn_Minus,_tmpBC4)))),((_tmpBC5.f3=_tmp582,_tmpBC5)))))))),_tmpBC3))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2D6:
# 2512
{const char*_tmpBC9;void*_tmpBC8[1];struct Cyc_String_pa_PrintArg_struct _tmpBC7;(_tmpBC7.tag=0,((_tmpBC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBC8[0]=& _tmpBC7,Cyc_Tcutil_terr(e->loc,((_tmpBC9="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpBC9,sizeof(char),59))),_tag_dyneither(_tmpBC8,sizeof(void*),1)))))));}{
# 2514
const char*_tmpBCC;void*_tmpBCB;(_tmpBCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCC="in-place inc/dec on @tagged union",_tag_dyneither(_tmpBCC,sizeof(char),34))),_tag_dyneither(_tmpBCB,sizeof(void*),0)));};}}
# 2517
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E1);
Cyc_Toc_set_lhs(nv,0);{
# 2522
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4E2 == Cyc_Absyn_PostInc  || _tmp4E2 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2528
if(_tmp4E2 == Cyc_Absyn_PreDec  || _tmp4E2 == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpBCD[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBCD[2]=
# 2532
Cyc_Absyn_signed_int_exp(change,0),((_tmpBCD[1]=
# 2531
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBCD[0]=
# 2530
Cyc_Toc_push_address_exp(_tmp4E1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCD,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2533
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2537
struct Cyc_Toc_functionSet*_tmp58F=_tmp4E2 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2539
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp58F,_tmp4E1);
struct Cyc_Absyn_Exp*_tmpBCE[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBCE[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpBCE[0]=
# 2540
Cyc_Toc_push_address_exp(_tmp4E1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2542
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4E1)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4E1,0,Cyc_Toc_incr_lvalue,_tmp4E2);
e->r=_tmp4E1->r;}}}
# 2546
goto _LL25B;};};}_LL266: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp4E3=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4E3->tag != 3)goto _LL268;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;_tmp4E6=_tmp4E3->f3;}}_LL267: {
# 2565 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4E4->topt);
void*e2_old_typ=(void*)_check_null(_tmp4E6->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4E4,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E6);
e->r=Cyc_Toc_tagged_union_assignop(_tmp4E4,e1_old_typ,_tmp4E5,_tmp4E6,e2_old_typ,f_tag,tagged_member_struct_type);
# 2576
return;}{
# 2578
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4E4,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4E4,_tmp4E5,_tmp4E6,ptr_type,is_dyneither,elt_type);
# 2584
return;}{
# 2588
int e1_poly=Cyc_Toc_is_poly_project(_tmp4E4);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4E4);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4E6);{
# 2594
int done=0;
if(_tmp4E5 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp4E5->v){case Cyc_Absyn_Plus: _LL2D8:
 change=_tmp4E6;break;case Cyc_Absyn_Minus: _LL2D9:
# 2602
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4E6,0);break;default: _LL2DA: {
const char*_tmpBD1;void*_tmpBD0;(_tmpBD0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD1="bad t ? pointer arithmetic",_tag_dyneither(_tmpBD1,sizeof(char),27))),_tag_dyneither(_tmpBD0,sizeof(void*),0)));}}
# 2605
done=1;{
# 2607
struct Cyc_Absyn_Exp*_tmp594=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpBD2[3];e->r=Cyc_Toc_fncall_exp_r(_tmp594,((_tmpBD2[2]=change,((_tmpBD2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBD2[0]=
# 2608
Cyc_Toc_push_address_exp(_tmp4E4),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2611
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2614
switch((enum Cyc_Absyn_Primop)_tmp4E5->v){case Cyc_Absyn_Plus: _LL2DC:
# 2616
 done=1;
{struct Cyc_Absyn_Exp*_tmpBD3[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4E4),((_tmpBD3[1]=_tmp4E6,((_tmpBD3[0]=_tmp4E4,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD3,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2DD: {
const char*_tmpBD6;void*_tmpBD5;(_tmpBD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD6="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpBD6,sizeof(char),39))),_tag_dyneither(_tmpBD5,sizeof(void*),0)));}}}}
# 2623
if(!done){
# 2625
if(e1_poly)
_tmp4E6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp4E6->r,0));
# 2631
if(!Cyc_Absyn_is_lvalue(_tmp4E4)){
{struct _tuple20 _tmpBD9;struct _tuple20*_tmpBD8;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp4E4,0,Cyc_Toc_assignop_lvalue,((_tmpBD8=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpBD8[0]=((_tmpBD9.f1=_tmp4E5,((_tmpBD9.f2=_tmp4E6,_tmpBD9)))),_tmpBD8)))));}
e->r=_tmp4E4->r;}}
# 2636
goto _LL25B;};};};}_LL268: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp4E7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4E7->tag != 5)goto _LL26A;else{_tmp4E8=_tmp4E7->f1;_tmp4E9=_tmp4E7->f2;_tmp4EA=_tmp4E7->f3;}}_LL269:
# 2638
 Cyc_Toc_exp_to_c(nv,_tmp4E8);
Cyc_Toc_exp_to_c(nv,_tmp4E9);
Cyc_Toc_exp_to_c(nv,_tmp4EA);
goto _LL25B;_LL26A: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp4EB=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4EB->tag != 6)goto _LL26C;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;}}_LL26B:
# 2643
 Cyc_Toc_exp_to_c(nv,_tmp4EC);
Cyc_Toc_exp_to_c(nv,_tmp4ED);
goto _LL25B;_LL26C: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp4EE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4EE->tag != 7)goto _LL26E;else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}}_LL26D:
# 2647
 Cyc_Toc_exp_to_c(nv,_tmp4EF);
Cyc_Toc_exp_to_c(nv,_tmp4F0);
goto _LL25B;_LL26E: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp4F1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4F1->tag != 8)goto _LL270;else{_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;}}_LL26F:
# 2651
 Cyc_Toc_exp_to_c(nv,_tmp4F2);
Cyc_Toc_exp_to_c(nv,_tmp4F3);
goto _LL25B;_LL270: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4F4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4F4->tag != 9)goto _LL272;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;if(_tmp4F4->f3 != 0)goto _LL272;}}_LL271:
# 2655
 Cyc_Toc_exp_to_c(nv,_tmp4F5);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4F6);
goto _LL25B;_LL272: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4F7->tag != 9)goto _LL274;else{_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F7->f2;if(_tmp4F7->f3 == 0)goto _LL274;_tmp4FA=(_tmp4F7->f3)->num_varargs;_tmp4FB=(_tmp4F7->f3)->injectors;_tmp4FC=(_tmp4F7->f3)->vai;}}_LL273: {
# 2667 "toc.cyc"
struct _RegionHandle _tmp59B=_new_region("r");struct _RegionHandle*r=& _tmp59B;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4FA,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4FC->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2675
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4F9);
int num_normargs=num_args - _tmp4FA;
# 2679
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4F9=_tmp4F9->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4F9))->hd);{
struct Cyc_List_List*_tmpBDA;new_args=((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA->hd=(struct Cyc_Absyn_Exp*)_tmp4F9->hd,((_tmpBDA->tl=new_args,_tmpBDA))))));};}}
# 2684
{struct Cyc_Absyn_Exp*_tmpBDD[3];struct Cyc_List_List*_tmpBDC;new_args=((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpBDD[2]=num_varargs_exp,((_tmpBDD[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpBDD[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBDC->tl=new_args,_tmpBDC))))));}
# 2689
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2691
Cyc_Toc_exp_to_c(nv,_tmp4F8);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4F8,new_args,0),0);
# 2695
if(_tmp4FC->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp59F=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4FC->type));struct Cyc_Absyn_Datatypedecl*_tmp5A1;_LL2E0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp59F;if(_tmp5A0->tag != 3)goto _LL2E2;else{if((((_tmp5A0->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2E2;_tmp5A1=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5A0->f1).datatype_info).KnownDatatype).val);}}_LL2E1:
 tud=_tmp5A1;goto _LL2DF;_LL2E2:;_LL2E3: {
const char*_tmpBE0;void*_tmpBDF;(_tmpBDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE0="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpBE0,sizeof(char),44))),_tag_dyneither(_tmpBDF,sizeof(void*),0)));}_LL2DF:;}{
# 2701
unsigned int _tmpBE5;unsigned int _tmpBE4;struct _dyneither_ptr _tmpBE3;struct _tuple0**_tmpBE2;unsigned int _tmpBE1;struct _dyneither_ptr vs=(_tmpBE1=(unsigned int)_tmp4FA,((_tmpBE2=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpBE1)),((_tmpBE3=_tag_dyneither(_tmpBE2,sizeof(struct _tuple0*),_tmpBE1),((((_tmpBE4=_tmpBE1,_tmpBE6(& _tmpBE5,& _tmpBE4,& _tmpBE2))),_tmpBE3)))))));
# 2703
if(_tmp4FA != 0){
# 2705
struct Cyc_List_List*_tmp5A4=0;
{int i=_tmp4FA - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpBE7;_tmp5A4=((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpBE7->tl=_tmp5A4,_tmpBE7))))));}}
# 2709
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5A4,0),s,0);{
# 2712
int i=0;for(0;_tmp4F9 != 0;(((_tmp4F9=_tmp4F9->tl,_tmp4FB=_tmp4FB->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4F9->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp5A7;struct Cyc_List_List*_tmp5A8;struct Cyc_Absyn_Datatypefield*_tmp5A6=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4FB))->hd;_tmp5A7=_tmp5A6->name;_tmp5A8=_tmp5A6->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp5A8))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2727
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2730
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5A7),0),s,0);
# 2733
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5A7,tud->name)),0,s,0);};}};}else{
# 2740
struct _tuple16*_tmpBE8[3];struct Cyc_List_List*_tmp5A9=(_tmpBE8[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBE8[1]=
# 2740
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBE8[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE8,sizeof(struct _tuple16*),3)))))));
# 2742
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2752
{int i=0;for(0;_tmp4F9 != 0;(_tmp4F9=_tmp4F9->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F9->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4F9->hd,0),s,0);}}
# 2758
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2771 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2773
_npop_handler(0);goto _LL25B;
# 2667 "toc.cyc"
;_pop_region(r);}_LL274: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4FD->tag != 10)goto _LL276;else{_tmp4FE=_tmp4FD->f1;}}_LL275:
# 2776 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp4FE);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp4FE),0))->r;
goto _LL25B;_LL276: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp4FF->tag != 11)goto _LL278;else{_tmp500=_tmp4FF->f1;}}_LL277:
 Cyc_Toc_exp_to_c(nv,_tmp500);goto _LL25B;_LL278: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp501=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp501->tag != 12)goto _LL27A;else{_tmp502=_tmp501->f1;_tmp503=_tmp501->f2;}}_LL279:
# 2781
 Cyc_Toc_exp_to_c(nv,_tmp502);
# 2790 "toc.cyc"
for(0;_tmp503 != 0;_tmp503=_tmp503->tl){
enum Cyc_Absyn_KindQual _tmp5AF=(Cyc_Tcutil_typ_kind((void*)_tmp503->hd))->kind;
if(_tmp5AF != Cyc_Absyn_EffKind  && _tmp5AF != Cyc_Absyn_RgnKind){
{void*_tmp5B0=Cyc_Tcutil_compress((void*)_tmp503->hd);_LL2E5: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5B0;if(_tmp5B1->tag != 2)goto _LL2E7;}_LL2E6:
 goto _LL2E8;_LL2E7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5B0;if(_tmp5B2->tag != 3)goto _LL2E9;}_LL2E8:
 continue;_LL2E9:;_LL2EA:
# 2797
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp502,0))->r;
goto _LL2E4;_LL2E4:;}
# 2800
break;}}
# 2803
goto _LL25B;_LL27A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp504=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp504->tag != 13)goto _LL27C;else{_tmp505=(void**)((void**)& _tmp504->f1);_tmp506=_tmp504->f2;_tmp507=_tmp504->f3;_tmp508=_tmp504->f4;}}_LL27B: {
# 2805
void*old_t2=(void*)_check_null(_tmp506->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp505;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp505=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp506);
# 2812
{struct _tuple19 _tmpBE9;struct _tuple19 _tmp5B4=(_tmpBE9.f1=Cyc_Tcutil_compress(old_t2),((_tmpBE9.f2=Cyc_Tcutil_compress(new_typ),_tmpBE9)));struct Cyc_Absyn_PtrInfo _tmp5B6;struct Cyc_Absyn_PtrInfo _tmp5B8;struct Cyc_Absyn_PtrInfo _tmp5BA;_LL2EC:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B4.f1;if(_tmp5B5->tag != 5)goto _LL2EE;else{_tmp5B6=_tmp5B5->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B4.f2;if(_tmp5B7->tag != 5)goto _LL2EE;else{_tmp5B8=_tmp5B7->f1;}};_LL2ED: {
# 2814
int _tmp5BC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5B6.ptr_atts).nullable);
int _tmp5BD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5B8.ptr_atts).nullable);
void*_tmp5BE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5B6.ptr_atts).bounds);
void*_tmp5BF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5B8.ptr_atts).bounds);
int _tmp5C0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5B6.ptr_atts).zero_term);
int _tmp5C1=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5B8.ptr_atts).zero_term);
{struct _tuple19 _tmpBEA;struct _tuple19 _tmp5C3=(_tmpBEA.f1=_tmp5BE,((_tmpBEA.f2=_tmp5BF,_tmpBEA)));struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CD;_LL2F3:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5C4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C3.f1;if(_tmp5C4->tag != 1)goto _LL2F5;else{_tmp5C5=_tmp5C4->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5C6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C3.f2;if(_tmp5C6->tag != 1)goto _LL2F5;else{_tmp5C7=_tmp5C6->f1;}};_LL2F4:
# 2822
 if((!Cyc_Evexp_c_can_eval(_tmp5C5) || !Cyc_Evexp_c_can_eval(_tmp5C7)) && !
Cyc_Evexp_same_const_exp(_tmp5C5,_tmp5C7)){
const char*_tmpBED;void*_tmpBEC;(_tmpBEC=0,Cyc_Tcutil_terr(e->loc,((_tmpBED="can't validate cast due to potential size differences",_tag_dyneither(_tmpBED,sizeof(char),54))),_tag_dyneither(_tmpBEC,sizeof(void*),0)));}
if(_tmp5BC  && !_tmp5BD){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpBF0;void*_tmpBEF;(_tmpBEF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpBF0="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpBF0,sizeof(char),44))),_tag_dyneither(_tmpBEF,sizeof(void*),0)));}
# 2831
if(_tmp508 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpBF4;void*_tmpBF3[1];struct Cyc_String_pa_PrintArg_struct _tmpBF2;(_tmpBF2.tag=0,((_tmpBF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBF3[0]=& _tmpBF2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBF4="null-check conversion mis-classified: %s",_tag_dyneither(_tmpBF4,sizeof(char),41))),_tag_dyneither(_tmpBF3,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp506);
if(do_null_check){
if(!_tmp507){
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,Cyc_Tcutil_warn(e->loc,((_tmpBF7="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpBF7,sizeof(char),58))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}{
# 2838
struct Cyc_List_List*_tmpBF8;e->r=Cyc_Toc_cast_it_r(*_tmp505,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpBF8=_cycalloc(sizeof(*_tmpBF8)),((_tmpBF8->hd=_tmp506,((_tmpBF8->tl=0,_tmpBF8)))))),0));};}else{
# 2841
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp506->r;}};}else{
# 2846
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp506->r;}
# 2853
goto _LL2F2;_LL2F5:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5C8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C3.f1;if(_tmp5C8->tag != 1)goto _LL2F7;else{_tmp5C9=_tmp5C8->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5CA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C3.f2;if(_tmp5CA->tag != 0)goto _LL2F7;};_LL2F6:
# 2855
 if(!Cyc_Evexp_c_can_eval(_tmp5C9)){
const char*_tmpBFB;void*_tmpBFA;(_tmpBFA=0,Cyc_Tcutil_terr(e->loc,((_tmpBFB="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpBFB,sizeof(char),71))),_tag_dyneither(_tmpBFA,sizeof(void*),0)));}
# 2858
if(_tmp508 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpBFF;void*_tmpBFE[1];struct Cyc_String_pa_PrintArg_struct _tmpBFD;(_tmpBFD.tag=0,((_tmpBFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBFE[0]=& _tmpBFD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpBFF="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpBFF,sizeof(char),44))),_tag_dyneither(_tmpBFE,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2862
if((_tmp5C0  && !(_tmp5B8.elt_tq).real_const) && !_tmp5C1)
# 2865
_tmp5C9=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5C9,Cyc_Absyn_uint_exp(1,0),0);
# 2867
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5C9,_tmp506))->r;}else{
# 2869
struct Cyc_Absyn_Exp*_tmp5DF=Cyc_Toc__tag_dyneither_e;
# 2871
if(_tmp5C0){
# 2876
struct _tuple0*_tmp5E0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5E1=Cyc_Absyn_var_exp(_tmp5E0,0);
struct Cyc_Absyn_Exp*arg3;
# 2881
{void*_tmp5E2=_tmp506->r;_LL2FC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5E3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E2;if(_tmp5E3->tag != 0)goto _LL2FE;else{if(((_tmp5E3->f1).String_c).tag != 8)goto _LL2FE;}}_LL2FD:
# 2883
 arg3=_tmp5C9;goto _LL2FB;_LL2FE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5E4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E2;if(_tmp5E4->tag != 0)goto _LL300;else{if(((_tmp5E4->f1).Wstring_c).tag != 9)goto _LL300;}}_LL2FF:
# 2885
 arg3=_tmp5C9;goto _LL2FB;_LL300:;_LL301:
# 2887
{struct Cyc_Absyn_Exp*_tmpC00[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp506),(
# 2889
(_tmpC00[1]=_tmp5C9,((_tmpC00[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp5E1),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC00,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2FB;_LL2FB:;}
# 2892
if(!_tmp5C1  && !(_tmp5B8.elt_tq).real_const)
# 2895
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2897
struct Cyc_Absyn_Exp*_tmp5E6=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B8.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC01[3];struct Cyc_Absyn_Exp*_tmp5E7=Cyc_Absyn_fncall_exp(_tmp5DF,((_tmpC01[2]=arg3,((_tmpC01[1]=_tmp5E6,((_tmpC01[0]=_tmp5E1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC01,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp5E8=Cyc_Absyn_exp_stmt(_tmp5E7,0);
_tmp5E8=Cyc_Absyn_declare_stmt(_tmp5E0,Cyc_Toc_typ_to_c(old_t2),_tmp506,_tmp5E8,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp5E8);};}else{
# 2904
struct Cyc_Absyn_Exp*_tmpC02[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5DF,(
(_tmpC02[2]=_tmp5C9,((_tmpC02[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B8.elt_typ),0),((_tmpC02[0]=_tmp506,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC02,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2910
goto _LL2F2;_LL2F7:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5CB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C3.f1;if(_tmp5CB->tag != 0)goto _LL2F9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5CC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C3.f2;if(_tmp5CC->tag != 1)goto _LL2F9;else{_tmp5CD=_tmp5CC->f1;}};_LL2F8:
# 2912
 if(!Cyc_Evexp_c_can_eval(_tmp5CD)){
const char*_tmpC05;void*_tmpC04;(_tmpC04=0,Cyc_Tcutil_terr(e->loc,((_tmpC05="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC05,sizeof(char),71))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC08;void*_tmpC07;(_tmpC07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC08="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC08,sizeof(char),45))),_tag_dyneither(_tmpC07,sizeof(void*),0)));}{
# 2924 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp5EF=_tmp5CD;
if(_tmp5C0  && !_tmp5C1)
_tmp5EF=Cyc_Absyn_add_exp(_tmp5CD,Cyc_Absyn_uint_exp(1,0),0);{
# 2931
struct Cyc_Absyn_Exp*_tmp5F0=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC09[3];struct Cyc_Absyn_Exp*_tmp5F1=Cyc_Absyn_fncall_exp(_tmp5F0,(
(_tmpC09[2]=_tmp5EF,((_tmpC09[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B6.elt_typ),0),((_tmpC09[0]=_tmp506,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC09,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2936
if(_tmp5BD){
struct Cyc_List_List*_tmpC0A;_tmp5F1->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC0A=_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A->hd=Cyc_Absyn_copy_exp(_tmp5F1),((_tmpC0A->tl=0,_tmpC0A)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp505,_tmp5F1);
goto _LL2F2;};};_LL2F9:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5CE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C3.f1;if(_tmp5CE->tag != 0)goto _LL2F2;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5CF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C3.f2;if(_tmp5CF->tag != 0)goto _LL2F2;};_LL2FA:
# 2944
 DynCast:
 if((_tmp5C0  && !_tmp5C1) && !(_tmp5B8.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC0D;void*_tmpC0C;(_tmpC0C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC0D="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC0D,sizeof(char),70))),_tag_dyneither(_tmpC0C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5F6=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC0E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5F6,(
(_tmpC0E[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC0E[1]=
# 2950
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5B6.elt_typ),0),((_tmpC0E[0]=_tmp506,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC0E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2953
goto _LL2F2;_LL2F2:;}
# 2955
goto _LL2EB;}_LL2EE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B4.f1;if(_tmp5B9->tag != 5)goto _LL2F0;else{_tmp5BA=_tmp5B9->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B4.f2;if(_tmp5BB->tag != 6)goto _LL2F0;};_LL2EF:
# 2957
{void*_tmp5F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5BA.ptr_atts).bounds);_LL303: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F8;if(_tmp5F9->tag != 0)goto _LL305;}_LL304:
# 2959
 _tmp506->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp506->r,_tmp506->loc),Cyc_Toc_curr_sp);goto _LL302;_LL305:;_LL306:
 goto _LL302;_LL302:;}
# 2962
goto _LL2EB;_LL2F0:;_LL2F1:
# 2964
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp506->r;
goto _LL2EB;_LL2EB:;}
# 2968
goto _LL25B;}_LL27C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp509->tag != 14)goto _LL27E;else{_tmp50A=_tmp509->f1;}}_LL27D:
# 2972
{void*_tmp5FA=_tmp50A->r;struct _tuple0*_tmp5FC;struct Cyc_List_List*_tmp5FD;struct Cyc_List_List*_tmp5FE;struct Cyc_List_List*_tmp600;_LL308: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5FB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5FA;if(_tmp5FB->tag != 27)goto _LL30A;else{_tmp5FC=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;_tmp5FE=_tmp5FB->f3;}}_LL309:
# 2974
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC12;void*_tmpC11[1];struct Cyc_String_pa_PrintArg_struct _tmpC10;(_tmpC10.tag=0,((_tmpC10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp50A->loc)),((_tmpC11[0]=& _tmpC10,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC12="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC12,sizeof(char),42))),_tag_dyneither(_tmpC11,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp50A->topt),_tmp5FD,1,0,_tmp5FE,_tmp5FC))->r;
goto _LL307;_LL30A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp5FF=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5FA;if(_tmp5FF->tag != 23)goto _LL30C;else{_tmp600=_tmp5FF->f1;}}_LL30B:
# 2980
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC16;void*_tmpC15[1];struct Cyc_String_pa_PrintArg_struct _tmpC14;(_tmpC14.tag=0,((_tmpC14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp50A->loc)),((_tmpC15[0]=& _tmpC14,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC16="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC16,sizeof(char),42))),_tag_dyneither(_tmpC15,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp600))->r;
goto _LL307;_LL30C:;_LL30D:
# 2986
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50A);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp50A)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp50A,0,Cyc_Toc_address_lvalue,1);
# 2992
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp50A);}
# 2994
goto _LL307;_LL307:;}
# 2996
goto _LL25B;_LL27E: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp50B=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp50B->tag != 15)goto _LL280;else{_tmp50C=_tmp50B->f1;_tmp50D=_tmp50B->f2;}}_LL27F:
# 2999
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC1A;void*_tmpC19[1];struct Cyc_String_pa_PrintArg_struct _tmpC18;(_tmpC18.tag=0,((_tmpC18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp50D->loc)),((_tmpC19[0]=& _tmpC18,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1A="%s: new at top-level",_tag_dyneither(_tmpC1A,sizeof(char),21))),_tag_dyneither(_tmpC19,sizeof(void*),1)))))));}
{void*_tmp60A=_tmp50D->r;struct Cyc_List_List*_tmp60C;struct Cyc_Absyn_Vardecl*_tmp60E;struct Cyc_Absyn_Exp*_tmp60F;struct Cyc_Absyn_Exp*_tmp610;int _tmp611;struct _tuple0*_tmp613;struct Cyc_List_List*_tmp614;struct Cyc_List_List*_tmp615;struct Cyc_Absyn_Aggrdecl*_tmp616;struct Cyc_List_List*_tmp618;_LL30F: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60B->tag != 25)goto _LL311;else{_tmp60C=_tmp60B->f1;}}_LL310: {
# 3006
struct _tuple0*_tmp619=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp61A=Cyc_Absyn_var_exp(_tmp619,0);
struct Cyc_Absyn_Stmt*_tmp61B=Cyc_Toc_init_array(nv,_tmp61A,_tmp60C,Cyc_Absyn_exp_stmt(_tmp61A,0));
void*old_elt_typ;
{void*_tmp61C=Cyc_Tcutil_compress(old_typ);void*_tmp61E;struct Cyc_Absyn_Tqual _tmp61F;union Cyc_Absyn_Constraint*_tmp620;_LL31A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp61D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp61C;if(_tmp61D->tag != 5)goto _LL31C;else{_tmp61E=(_tmp61D->f1).elt_typ;_tmp61F=(_tmp61D->f1).elt_tq;_tmp620=((_tmp61D->f1).ptr_atts).zero_term;}}_LL31B:
# 3012
 old_elt_typ=_tmp61E;goto _LL319;_LL31C:;_LL31D: {
# 3014
const char*_tmpC1D;void*_tmpC1C;old_elt_typ=(
(_tmpC1C=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1D="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC1D,sizeof(char),52))),_tag_dyneither(_tmpC1C,sizeof(void*),0))));}_LL319:;}{
# 3017
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp623=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp624=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp60C),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp50C == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp624);else{
# 3025
struct Cyc_Absyn_Exp*r=_tmp50C;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp624);}
# 3030
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp619,_tmp623,e1,_tmp61B,0));
goto _LL30E;};}_LL311: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp60D->tag != 26)goto _LL313;else{_tmp60E=_tmp60D->f1;_tmp60F=_tmp60D->f2;_tmp610=_tmp60D->f3;_tmp611=_tmp60D->f4;}}_LL312: {
# 3043 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp625=Cyc_Tcutil_compress(old_typ);void*_tmp627;struct Cyc_Absyn_Tqual _tmp628;union Cyc_Absyn_Constraint*_tmp629;union Cyc_Absyn_Constraint*_tmp62A;_LL31F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp626=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp625;if(_tmp626->tag != 5)goto _LL321;else{_tmp627=(_tmp626->f1).elt_typ;_tmp628=(_tmp626->f1).elt_tq;_tmp629=((_tmp626->f1).ptr_atts).bounds;_tmp62A=((_tmp626->f1).ptr_atts).zero_term;}}_LL320:
# 3046
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp629)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL31E;_LL321:;_LL322: {
const char*_tmpC20;void*_tmpC1F;(_tmpC1F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC20="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC20,sizeof(char),42))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}_LL31E:;}{
# 3050
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp610->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp60F);{
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Absyn_var_exp(max,0);
# 3058
if(_tmp611)
_tmp62D=Cyc_Absyn_add_exp(_tmp62D,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp60E,Cyc_Absyn_var_exp(max,0),_tmp610,_tmp611,
Cyc_Toc_skip_stmt_dl(),1);
# 3063
struct _RegionHandle _tmp62E=_new_region("r");struct _RegionHandle*r=& _tmp62E;_push_region(r);{
struct _tuple21*_tmpC23;struct Cyc_List_List*_tmpC22;struct Cyc_List_List*decls=
(_tmpC22=_region_malloc(r,sizeof(*_tmpC22)),((_tmpC22->hd=((_tmpC23=_region_malloc(r,sizeof(*_tmpC23)),((_tmpC23->f1=max,((_tmpC23->f2=Cyc_Absyn_uint_typ,((_tmpC23->f3=_tmp60F,_tmpC23)))))))),((_tmpC22->tl=0,_tmpC22)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp50C == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC24[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC24[1]=_tmp62D,((_tmpC24[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3073
struct Cyc_Absyn_Exp*r=_tmp50C;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC25[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC25[1]=_tmp62D,((_tmpC25[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC25,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3080
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpC28;struct Cyc_List_List*_tmpC27;decls=((_tmpC27=_region_malloc(r,sizeof(*_tmpC27)),((_tmpC27->hd=((_tmpC28=_region_malloc(r,sizeof(*_tmpC28)),((_tmpC28->f1=a,((_tmpC28->f2=ptr_typ,((_tmpC28->f3=ainit,_tmpC28)))))))),((_tmpC27->tl=decls,_tmpC27))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp633=Cyc_Toc_temp_var();
void*_tmp634=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp635=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC29[3];struct Cyc_Absyn_Exp*_tmp636=
Cyc_Absyn_fncall_exp(_tmp635,(
(_tmpC29[2]=_tmp62D,((_tmpC29[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC29[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC29,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3090
{struct _tuple21*_tmpC2C;struct Cyc_List_List*_tmpC2B;decls=((_tmpC2B=_region_malloc(r,sizeof(*_tmpC2B)),((_tmpC2B->hd=((_tmpC2C=_region_malloc(r,sizeof(*_tmpC2C)),((_tmpC2C->f1=_tmp633,((_tmpC2C->f2=_tmp634,((_tmpC2C->f3=_tmp636,_tmpC2C)))))))),((_tmpC2B->tl=decls,_tmpC2B))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp633,0),0),0);}else{
# 3093
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp63A=decls;for(0;_tmp63A != 0;_tmp63A=_tmp63A->tl){
struct _tuple0*_tmp63C;void*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;struct _tuple21 _tmp63B=*((struct _tuple21*)_tmp63A->hd);_tmp63C=_tmp63B.f1;_tmp63D=_tmp63B.f2;_tmp63E=_tmp63B.f3;
s=Cyc_Absyn_declare_stmt(_tmp63C,_tmp63D,_tmp63E,s,0);}}
# 3098
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3100
_npop_handler(0);goto _LL30E;
# 3063
;_pop_region(r);};};};}_LL313: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp612->tag != 27)goto _LL315;else{_tmp613=_tmp612->f1;_tmp614=_tmp612->f2;_tmp615=_tmp612->f3;_tmp616=_tmp612->f4;}}_LL314:
# 3103
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp50D->topt),_tmp614,1,_tmp50C,_tmp615,_tmp613))->r;
goto _LL30E;_LL315: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp60A;if(_tmp617->tag != 23)goto _LL317;else{_tmp618=_tmp617->f1;}}_LL316:
# 3107
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp50C,_tmp618))->r;
goto _LL30E;_LL317:;_LL318: {
# 3114
void*old_elt_typ=(void*)_check_null(_tmp50D->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3117
struct _tuple0*_tmp641=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp642=Cyc_Absyn_var_exp(_tmp641,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp642,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp50C == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3124
struct Cyc_Absyn_Exp*r=_tmp50C;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3132
int done=0;
{void*_tmp643=_tmp50D->r;void*_tmp645;struct Cyc_Absyn_Exp*_tmp646;_LL324: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp643;if(_tmp644->tag != 13)goto _LL326;else{_tmp645=(void*)_tmp644->f1;_tmp646=_tmp644->f2;}}_LL325:
# 3135
{struct _tuple19 _tmpC2D;struct _tuple19 _tmp648=(_tmpC2D.f1=Cyc_Tcutil_compress(_tmp645),((_tmpC2D.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp646->topt)),_tmpC2D)));void*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;union Cyc_Absyn_Constraint*_tmp64D;_LL329:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp649=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp648.f1;if(_tmp649->tag != 5)goto _LL32B;else{_tmp64A=(_tmp649->f1).elt_typ;_tmp64B=((_tmp649->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp648.f2;if(_tmp64C->tag != 5)goto _LL32B;else{_tmp64D=((_tmp64C->f1).ptr_atts).bounds;}};_LL32A:
# 3138
{struct _tuple19 _tmpC2E;struct _tuple19 _tmp64F=(_tmpC2E.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp64B),((_tmpC2E.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp64D),_tmpC2E)));struct Cyc_Absyn_Exp*_tmp652;_LL32E:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp650=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp64F.f1;if(_tmp650->tag != 0)goto _LL330;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp651=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp64F.f2;if(_tmp651->tag != 1)goto _LL330;else{_tmp652=_tmp651->f1;}};_LL32F:
# 3140
 Cyc_Toc_exp_to_c(nv,_tmp646);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp653=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpC2F[4];e->r=Cyc_Toc_fncall_exp_r(_tmp653,(
(_tmpC2F[3]=_tmp652,((_tmpC2F[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp64A),0),((_tmpC2F[1]=_tmp646,((_tmpC2F[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2F,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3148
goto _LL32D;};_LL330:;_LL331:
 goto _LL32D;_LL32D:;}
# 3151
goto _LL328;_LL32B:;_LL32C:
 goto _LL328;_LL328:;}
# 3154
goto _LL323;_LL326:;_LL327:
 goto _LL323;_LL323:;}
# 3157
if(!done){
struct Cyc_Absyn_Stmt*_tmp655=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp642),0);
struct Cyc_Absyn_Exp*_tmp656=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp50D);
_tmp655=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp642,_tmp656,0),_tmp50D,0),_tmp655,0);{
# 3163
void*_tmp657=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp641,_tmp657,mexp,_tmp655,0));};}
# 3166
goto _LL30E;};}_LL30E:;}
# 3168
goto _LL25B;_LL280: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp50E->tag != 17)goto _LL282;else{_tmp50F=_tmp50E->f1;}}_LL281: {
# 3171
int _tmp658=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50F);
Cyc_Toc_set_in_sizeof(nv,_tmp658);
goto _LL25B;}_LL282: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp510->tag != 16)goto _LL284;else{_tmp511=(void*)_tmp510->f1;}}_LL283:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC32;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC31;e->r=(void*)((_tmpC31=_cycalloc(sizeof(*_tmpC31)),((_tmpC31[0]=((_tmpC32.tag=16,((_tmpC32.f1=Cyc_Toc_typ_to_c(_tmp511),_tmpC32)))),_tmpC31))));}goto _LL25B;_LL284: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp512->tag != 18)goto _LL286;else{_tmp513=(void*)_tmp512->f1;_tmp514=_tmp512->f2;}}_LL285: {
# 3177
void*_tmp65B=_tmp513;
struct Cyc_List_List*_tmp65C=_tmp514;
for(0;_tmp65C != 0;_tmp65C=_tmp65C->tl){
void*_tmp65D=(void*)_tmp65C->hd;struct _dyneither_ptr*_tmp65F;unsigned int _tmp661;_LL333: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp65E=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp65D;if(_tmp65E->tag != 0)goto _LL335;else{_tmp65F=_tmp65E->f1;}}_LL334:
 goto _LL332;_LL335: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp660=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp65D;if(_tmp660->tag != 1)goto _LL332;else{_tmp661=_tmp660->f1;}}_LL336:
# 3183
{void*_tmp662=Cyc_Tcutil_compress(_tmp65B);union Cyc_Absyn_AggrInfoU _tmp664;struct Cyc_List_List*_tmp666;struct Cyc_List_List*_tmp668;struct Cyc_Absyn_Datatypefield*_tmp66A;_LL338: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp663=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp662;if(_tmp663->tag != 11)goto _LL33A;else{_tmp664=(_tmp663->f1).aggr_info;}}_LL339: {
# 3185
struct Cyc_Absyn_Aggrdecl*_tmp66B=Cyc_Absyn_get_known_aggrdecl(_tmp664);
if(_tmp66B->impl == 0){
const char*_tmpC35;void*_tmpC34;(_tmpC34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC35="struct fields must be known",_tag_dyneither(_tmpC35,sizeof(char),28))),_tag_dyneither(_tmpC34,sizeof(void*),0)));}
_tmp666=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp66B->impl))->fields;goto _LL33B;}_LL33A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp665=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp662;if(_tmp665->tag != 12)goto _LL33C;else{_tmp666=_tmp665->f2;}}_LL33B: {
# 3190
struct Cyc_Absyn_Aggrfield*_tmp66E=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp666,(int)_tmp661);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC38;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC37;_tmp65C->hd=(void*)((void*)((_tmpC37=_cycalloc(sizeof(*_tmpC37)),((_tmpC37[0]=((_tmpC38.tag=0,((_tmpC38.f1=_tmp66E->name,_tmpC38)))),_tmpC37)))));}
_tmp65B=_tmp66E->type;
goto _LL337;}_LL33C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp667=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp662;if(_tmp667->tag != 10)goto _LL33E;else{_tmp668=_tmp667->f1;}}_LL33D:
# 3195
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC3B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC3A;_tmp65C->hd=(void*)((void*)((_tmpC3A=_cycalloc(sizeof(*_tmpC3A)),((_tmpC3A[0]=((_tmpC3B.tag=0,((_tmpC3B.f1=Cyc_Absyn_fieldname((int)(_tmp661 + 1)),_tmpC3B)))),_tmpC3A)))));}
_tmp65B=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp668,(int)_tmp661)).f2;
goto _LL337;_LL33E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp662;if(_tmp669->tag != 4)goto _LL340;else{if((((_tmp669->f1).field_info).KnownDatatypefield).tag != 2)goto _LL340;_tmp66A=((struct _tuple1)(((_tmp669->f1).field_info).KnownDatatypefield).val).f2;}}_LL33F:
# 3199
 if(_tmp661 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC3E;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC3D;_tmp65C->hd=(void*)((void*)((_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D[0]=((_tmpC3E.tag=0,((_tmpC3E.f1=Cyc_Toc_tag_sp,_tmpC3E)))),_tmpC3D)))));}else{
# 3202
_tmp65B=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp66A->typs,(int)(_tmp661 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpC41;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpC40;_tmp65C->hd=(void*)((void*)((_tmpC40=_cycalloc(sizeof(*_tmpC40)),((_tmpC40[0]=((_tmpC41.tag=0,((_tmpC41.f1=Cyc_Absyn_fieldname((int)_tmp661),_tmpC41)))),_tmpC40)))));};}
# 3205
goto _LL337;_LL340:;_LL341:
{const char*_tmpC44;void*_tmpC43;(_tmpC43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC44="impossible type for offsetof tuple index",_tag_dyneither(_tmpC44,sizeof(char),41))),_tag_dyneither(_tmpC43,sizeof(void*),0)));}
goto _LL337;_LL337:;}
# 3209
goto _LL332;_LL332:;}
# 3212
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpC47;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC46;e->r=(void*)((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46[0]=((_tmpC47.tag=18,((_tmpC47.f1=Cyc_Toc_typ_to_c(_tmp513),((_tmpC47.f2=_tmp514,_tmpC47)))))),_tmpC46))));}
goto _LL25B;}_LL286: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp515->tag != 19)goto _LL288;else{_tmp516=_tmp515->f1;}}_LL287: {
# 3215
int _tmp67B=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp67C=Cyc_Tcutil_compress((void*)_check_null(_tmp516->topt));
{void*_tmp67D=_tmp67C;void*_tmp67F;struct Cyc_Absyn_Tqual _tmp680;void*_tmp681;union Cyc_Absyn_Constraint*_tmp682;union Cyc_Absyn_Constraint*_tmp683;union Cyc_Absyn_Constraint*_tmp684;_LL343: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67D;if(_tmp67E->tag != 5)goto _LL345;else{_tmp67F=(_tmp67E->f1).elt_typ;_tmp680=(_tmp67E->f1).elt_tq;_tmp681=((_tmp67E->f1).ptr_atts).rgn;_tmp682=((_tmp67E->f1).ptr_atts).nullable;_tmp683=((_tmp67E->f1).ptr_atts).bounds;_tmp684=((_tmp67E->f1).ptr_atts).zero_term;}}_LL344:
# 3220
{void*_tmp685=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp683);struct Cyc_Absyn_Exp*_tmp687;_LL348: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp686=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp685;if(_tmp686->tag != 1)goto _LL34A;else{_tmp687=_tmp686->f1;}}_LL349: {
# 3222
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp516);
Cyc_Toc_exp_to_c(nv,_tmp516);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC4A;void*_tmpC49;(_tmpC49=0,Cyc_Tcutil_warn(e->loc,((_tmpC4A="inserted null check due to dereference",_tag_dyneither(_tmpC4A,sizeof(char),39))),_tag_dyneither(_tmpC49,sizeof(void*),0)));}{
# 3228
struct Cyc_List_List*_tmpC4B;_tmp516->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp67C),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->hd=Cyc_Absyn_copy_exp(_tmp516),((_tmpC4B->tl=0,_tmpC4B)))))),0));};}
# 3236
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp684)){
unsigned int _tmp68D;int _tmp68E;struct _tuple11 _tmp68C=Cyc_Evexp_eval_const_uint_exp(_tmp687);_tmp68D=_tmp68C.f1;_tmp68E=_tmp68C.f2;
# 3242
if(!_tmp68E  || _tmp68D <= 0){
const char*_tmpC4E;void*_tmpC4D;(_tmpC4D=0,Cyc_Tcutil_terr(e->loc,((_tmpC4E="cannot determine dereference is in bounds",_tag_dyneither(_tmpC4E,sizeof(char),42))),_tag_dyneither(_tmpC4D,sizeof(void*),0)));}}
# 3245
goto _LL347;}_LL34A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp688=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp685;if(_tmp688->tag != 0)goto _LL347;}_LL34B: {
# 3248
struct Cyc_Absyn_Exp*_tmp691=Cyc_Absyn_uint_exp(0,0);
_tmp691->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp516,_tmp691);
Cyc_Toc_exp_to_c(nv,e);
goto _LL347;}_LL347:;}
# 3254
goto _LL342;_LL345:;_LL346: {
const char*_tmpC51;void*_tmpC50;(_tmpC50=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC51="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpC51,sizeof(char),29))),_tag_dyneither(_tmpC50,sizeof(void*),0)));}_LL342:;}
# 3257
Cyc_Toc_set_lhs(nv,_tmp67B);
goto _LL25B;};}_LL288: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp517->tag != 20)goto _LL28A;else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;_tmp51A=_tmp517->f3;_tmp51B=_tmp517->f4;}}_LL289: {
# 3260
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp518->topt);
Cyc_Toc_exp_to_c(nv,_tmp518);
if(_tmp51A  && _tmp51B)
e->r=Cyc_Toc_check_tagged_union(_tmp518,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp519,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3268
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL25B;}_LL28A: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp51C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp51C->tag != 21)goto _LL28C;else{_tmp51D=_tmp51C->f1;_tmp51E=_tmp51C->f2;_tmp51F=_tmp51C->f3;_tmp520=_tmp51C->f4;}}_LL28B: {
# 3272
int _tmp694=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp51D->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp51D);
Cyc_Toc_exp_to_c(nv,_tmp51D);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp696;struct Cyc_Absyn_Tqual _tmp697;void*_tmp698;union Cyc_Absyn_Constraint*_tmp699;union Cyc_Absyn_Constraint*_tmp69A;union Cyc_Absyn_Constraint*_tmp69B;struct Cyc_Absyn_PtrInfo _tmp695=Cyc_Toc_get_ptr_type(e1typ);_tmp696=_tmp695.elt_typ;_tmp697=_tmp695.elt_tq;_tmp698=(_tmp695.ptr_atts).rgn;_tmp699=(_tmp695.ptr_atts).nullable;_tmp69A=(_tmp695.ptr_atts).bounds;_tmp69B=(_tmp695.ptr_atts).zero_term;
{void*_tmp69C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69A);struct Cyc_Absyn_Exp*_tmp69E;_LL34D: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp69D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp69C;if(_tmp69D->tag != 1)goto _LL34F;else{_tmp69E=_tmp69D->f1;}}_LL34E: {
# 3283
unsigned int _tmp6A1;int _tmp6A2;struct _tuple11 _tmp6A0=Cyc_Evexp_eval_const_uint_exp(_tmp69E);_tmp6A1=_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;
if(_tmp6A2){
if(_tmp6A1 < 1){
const char*_tmpC54;void*_tmpC53;(_tmpC53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC54="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC54,sizeof(char),44))),_tag_dyneither(_tmpC53,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC57;void*_tmpC56;(_tmpC56=0,Cyc_Tcutil_warn(e->loc,((_tmpC57="inserted null check due to dereference",_tag_dyneither(_tmpC57,sizeof(char),39))),_tag_dyneither(_tmpC56,sizeof(void*),0)));}{
# 3291
struct Cyc_Absyn_Exp*_tmpC58[1];_tmp51D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC58[0]=Cyc_Absyn_new_exp(_tmp51D->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC58,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3296
if(!Cyc_Evexp_c_can_eval(_tmp69E)){
const char*_tmpC5B;void*_tmpC5A;(_tmpC5A=0,Cyc_Tcutil_terr(e->loc,((_tmpC5B="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpC5B,sizeof(char),47))),_tag_dyneither(_tmpC5A,sizeof(void*),0)));}{
# 3300
struct Cyc_Absyn_Exp*_tmpC5C[4];_tmp51D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC5C[3]=
# 3304
Cyc_Absyn_uint_exp(0,0),((_tmpC5C[2]=
# 3303
Cyc_Absyn_sizeoftyp_exp(_tmp696,0),((_tmpC5C[1]=_tmp69E,((_tmpC5C[0]=
# 3302
Cyc_Absyn_new_exp(_tmp51D->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5C,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3306
goto _LL34C;}_LL34F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp69F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp69C;if(_tmp69F->tag != 0)goto _LL34C;}_LL350: {
# 3309
void*ta1=Cyc_Toc_typ_to_c(_tmp696);
{struct Cyc_Absyn_Exp*_tmpC5D[3];_tmp51D->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp697),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpC5D[2]=
# 3314
Cyc_Absyn_uint_exp(0,0),((_tmpC5D[1]=
# 3313
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC5D[0]=
# 3312
Cyc_Absyn_new_exp(_tmp51D->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3317
goto _LL34C;}_LL34C:;}
# 3319
if(_tmp51F  && _tmp520)
e->r=Cyc_Toc_check_tagged_union(_tmp51D,Cyc_Toc_typ_to_c(e1typ),_tmp696,_tmp51E,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3322
if(is_poly  && _tmp520)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp694);
goto _LL25B;};};}_LL28C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp521=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp521->tag != 22)goto _LL28E;else{_tmp522=_tmp521->f1;_tmp523=_tmp521->f2;}}_LL28D: {
# 3327
int _tmp6AC=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6AD=Cyc_Tcutil_compress((void*)_check_null(_tmp522->topt));
# 3331
{void*_tmp6AE=_tmp6AD;struct Cyc_List_List*_tmp6B0;void*_tmp6B2;struct Cyc_Absyn_Tqual _tmp6B3;void*_tmp6B4;union Cyc_Absyn_Constraint*_tmp6B5;union Cyc_Absyn_Constraint*_tmp6B6;union Cyc_Absyn_Constraint*_tmp6B7;_LL352: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6AF=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6AE;if(_tmp6AF->tag != 10)goto _LL354;else{_tmp6B0=_tmp6AF->f1;}}_LL353:
# 3334
 Cyc_Toc_exp_to_c(nv,_tmp522);
Cyc_Toc_exp_to_c(nv,_tmp523);{
unsigned int _tmp6B9;int _tmp6BA;struct _tuple11 _tmp6B8=Cyc_Evexp_eval_const_uint_exp(_tmp523);_tmp6B9=_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;
if(!_tmp6BA){
const char*_tmpC60;void*_tmpC5F;(_tmpC5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC60="unknown tuple subscript in translation to C",_tag_dyneither(_tmpC60,sizeof(char),44))),_tag_dyneither(_tmpC5F,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp522,Cyc_Absyn_fieldname((int)(_tmp6B9 + 1)));
goto _LL351;};_LL354: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6AE;if(_tmp6B1->tag != 5)goto _LL356;else{_tmp6B2=(_tmp6B1->f1).elt_typ;_tmp6B3=(_tmp6B1->f1).elt_tq;_tmp6B4=((_tmp6B1->f1).ptr_atts).rgn;_tmp6B5=((_tmp6B1->f1).ptr_atts).nullable;_tmp6B6=((_tmp6B1->f1).ptr_atts).bounds;_tmp6B7=((_tmp6B1->f1).ptr_atts).zero_term;}}_LL355: {
# 3342
struct Cyc_List_List*_tmp6BD=Cyc_Toc_get_relns(_tmp522);
int _tmp6BE=Cyc_Toc_in_sizeof(nv);
# 3349
int in_bnds=_tmp6BE;
{void*_tmp6BF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6B6);_LL359:;_LL35A:
# 3352
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6AD,_tmp6BD,_tmp522,_tmp523);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpC64;void*_tmpC63[1];struct Cyc_String_pa_PrintArg_struct _tmpC62;(_tmpC62.tag=0,((_tmpC62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC63[0]=& _tmpC62,Cyc_Tcutil_warn(e->loc,((_tmpC64="bounds check necessary for %s",_tag_dyneither(_tmpC64,sizeof(char),30))),_tag_dyneither(_tmpC63,sizeof(void*),1)))))));}_LL358:;}
# 3359
Cyc_Toc_exp_to_c(nv,_tmp522);
Cyc_Toc_exp_to_c(nv,_tmp523);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6C3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6B6);struct Cyc_Absyn_Exp*_tmp6C5;_LL35C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6C4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6C3;if(_tmp6C4->tag != 1)goto _LL35E;else{_tmp6C5=_tmp6C4->f1;}}_LL35D: {
# 3364
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6B5);
void*ta1=Cyc_Toc_typ_to_c(_tmp6B2);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6B3);
if(in_bnds)
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3370
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6B7)){
# 3372
if(!Cyc_Evexp_c_can_eval(_tmp6C5)){
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,Cyc_Tcutil_terr(e->loc,((_tmpC67="cannot determine subscript is in bounds",_tag_dyneither(_tmpC67,sizeof(char),40))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp522);
struct Cyc_Absyn_Exp*_tmpC68[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpC68[2]=_tmp523,((_tmpC68[1]=_tmp6C5,((_tmpC68[0]=_tmp522,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC68,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6C5)){
const char*_tmpC6B;void*_tmpC6A;(_tmpC6A=0,Cyc_Tcutil_terr(e->loc,((_tmpC6B="cannot determine subscript is in bounds",_tag_dyneither(_tmpC6B,sizeof(char),40))),_tag_dyneither(_tmpC6A,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC6E;void*_tmpC6D;(_tmpC6D=0,Cyc_Tcutil_warn(e->loc,((_tmpC6E="inserted null check due to dereference",_tag_dyneither(_tmpC6E,sizeof(char),39))),_tag_dyneither(_tmpC6D,sizeof(void*),0)));}{
# 3384
struct Cyc_Absyn_Exp*_tmpC6F[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpC6F[3]=_tmp523,((_tmpC6F[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC6F[1]=_tmp6C5,((_tmpC6F[0]=_tmp522,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6F,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3390
if(!Cyc_Evexp_c_can_eval(_tmp6C5)){
const char*_tmpC72;void*_tmpC71;(_tmpC71=0,Cyc_Tcutil_terr(e->loc,((_tmpC72="cannot determine subscript is in bounds",_tag_dyneither(_tmpC72,sizeof(char),40))),_tag_dyneither(_tmpC71,sizeof(void*),0)));}{
# 3393
struct Cyc_Absyn_Exp*_tmpC73[2];_tmp523->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpC73[1]=Cyc_Absyn_copy_exp(_tmp523),((_tmpC73[0]=_tmp6C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC73,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3396
goto _LL35B;}_LL35E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6C6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6C3;if(_tmp6C6->tag != 0)goto _LL35B;}_LL35F: {
# 3398
void*ta1=Cyc_Toc_typ_to_c(_tmp6B2);
if(in_bnds){
# 3402
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6B3),
Cyc_Toc_member_exp(_tmp522,Cyc_Toc_curr_sp,0)),_tmp523);}else{
# 3407
struct Cyc_Absyn_Exp*_tmp6D2=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpC74[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6B3),
Cyc_Absyn_fncall_exp(_tmp6D2,(
(_tmpC74[2]=_tmp523,((_tmpC74[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC74[0]=_tmp522,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC74,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3413
goto _LL35B;}_LL35B:;}
# 3415
goto _LL351;}_LL356:;_LL357: {
const char*_tmpC77;void*_tmpC76;(_tmpC76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC77="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpC77,sizeof(char),49))),_tag_dyneither(_tmpC76,sizeof(void*),0)));}_LL351:;}
# 3418
Cyc_Toc_set_lhs(nv,_tmp6AC);
goto _LL25B;};}_LL28E: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp524=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp524->tag != 23)goto _LL290;else{_tmp525=_tmp524->f1;}}_LL28F:
# 3421
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp525))->r;else{
# 3426
struct Cyc_List_List*_tmp6D6=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp525);
void*_tmp6D7=Cyc_Toc_add_tuple_type(_tmp6D6);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp525 != 0;(_tmp525=_tmp525->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp525->hd);{
struct _tuple16*_tmpC7A;struct Cyc_List_List*_tmpC79;dles=((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((_tmpC79->hd=((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A->f1=0,((_tmpC7A->f2=(struct Cyc_Absyn_Exp*)_tmp525->hd,_tmpC7A)))))),((_tmpC79->tl=dles,_tmpC79))))));};}}
# 3433
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3436
goto _LL25B;_LL290: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp526->tag != 25)goto _LL292;else{_tmp527=_tmp526->f1;}}_LL291:
# 3440
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp527);
{struct Cyc_List_List*_tmp6DA=_tmp527;for(0;_tmp6DA != 0;_tmp6DA=_tmp6DA->tl){
struct Cyc_Absyn_Exp*_tmp6DC;struct _tuple16*_tmp6DB=(struct _tuple16*)_tmp6DA->hd;_tmp6DC=_tmp6DB->f2;
Cyc_Toc_exp_to_c(nv,_tmp6DC);}}
# 3445
goto _LL25B;_LL292: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp528->tag != 26)goto _LL294;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;_tmp52B=_tmp528->f3;_tmp52C=_tmp528->f4;}}_LL293: {
# 3449
unsigned int _tmp6DE;int _tmp6DF;struct _tuple11 _tmp6DD=Cyc_Evexp_eval_const_uint_exp(_tmp52A);_tmp6DE=_tmp6DD.f1;_tmp6DF=_tmp6DD.f2;{
void*_tmp6E0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp52B->topt));
Cyc_Toc_exp_to_c(nv,_tmp52B);{
struct Cyc_List_List*es=0;
# 3454
if(!Cyc_Toc_is_zero(_tmp52B)){
if(!_tmp6DF){
const char*_tmpC7D;void*_tmpC7C;(_tmpC7C=0,Cyc_Tcutil_terr(_tmp52A->loc,((_tmpC7D="cannot determine value of constant",_tag_dyneither(_tmpC7D,sizeof(char),35))),_tag_dyneither(_tmpC7C,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp6DE;++ i){
struct _tuple16*_tmpC80;struct Cyc_List_List*_tmpC7F;es=((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->hd=((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->f1=0,((_tmpC80->f2=_tmp52B,_tmpC80)))))),((_tmpC7F->tl=es,_tmpC7F))))));}}
# 3460
if(_tmp52C){
struct Cyc_Absyn_Exp*_tmp6E5=Cyc_Toc_cast_it(_tmp6E0,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpC83;struct Cyc_List_List*_tmpC82;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82->hd=((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->f1=0,((_tmpC83->f2=_tmp6E5,_tmpC83)))))),((_tmpC82->tl=0,_tmpC82)))))));}}
# 3465
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL25B;};};}_LL294: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp52D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp52D->tag != 27)goto _LL296;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;_tmp530=_tmp52D->f3;_tmp531=_tmp52D->f4;}}_LL295:
# 3470
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp52F,0,0,_tmp530,_tmp52E))->r;else{
# 3478
if(_tmp531 == 0){
const char*_tmpC86;void*_tmpC85;(_tmpC85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC86="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpC86,sizeof(char),38))),_tag_dyneither(_tmpC85,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp531;
# 3482
struct _RegionHandle _tmp6EA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6EA;_push_region(rgn);
{struct Cyc_List_List*_tmp6EB=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp530,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3486
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp531->impl))->tagged){
# 3488
struct Cyc_Absyn_Aggrfield*_tmp6ED;struct Cyc_Absyn_Exp*_tmp6EE;struct _tuple22*_tmp6EC=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6EB))->hd;_tmp6ED=_tmp6EC->f1;_tmp6EE=_tmp6EC->f2;{
void*_tmp6EF=(void*)_check_null(_tmp6EE->topt);
void*_tmp6F0=_tmp6ED->type;
Cyc_Toc_exp_to_c(nv,_tmp6EE);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6F0) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp6EF))
_tmp6EE->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp6EE->r,0));{
# 3497
int i=Cyc_Toc_get_member_offset(_tmp531,_tmp6ED->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpC8B;struct _tuple16*_tmpC8A;struct _tuple16*_tmpC89[2];struct Cyc_List_List*_tmp6F1=(_tmpC89[1]=((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->f1=0,((_tmpC8A->f2=_tmp6EE,_tmpC8A)))))),((_tmpC89[0]=((_tmpC8B=_cycalloc(sizeof(*_tmpC8B)),((_tmpC8B->f1=0,((_tmpC8B->f2=field_tag_exp,_tmpC8B)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC89,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6F1,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC91;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpC90;void*_tmpC8F[1];struct Cyc_List_List*ds=(_tmpC8F[0]=(void*)((_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91[0]=((_tmpC90.tag=1,((_tmpC90.f1=_tmp6ED->name,_tmpC90)))),_tmpC91)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8F,sizeof(void*),1)));
struct _tuple16*_tmpC94;struct _tuple16*_tmpC93[1];struct Cyc_List_List*dles=(_tmpC93[0]=((_tmpC94=_cycalloc(sizeof(*_tmpC94)),((_tmpC94->f1=ds,((_tmpC94->f2=umem,_tmpC94)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC93,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3506
struct Cyc_List_List*_tmp6FA=0;
struct Cyc_List_List*_tmp6FB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp6FB != 0;_tmp6FB=_tmp6FB->tl){
struct Cyc_List_List*_tmp6FC=_tmp6EB;for(0;_tmp6FC != 0;_tmp6FC=_tmp6FC->tl){
if((*((struct _tuple22*)_tmp6FC->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6FB->hd){
struct Cyc_Absyn_Aggrfield*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FF;struct _tuple22*_tmp6FD=(struct _tuple22*)_tmp6FC->hd;_tmp6FE=_tmp6FD->f1;_tmp6FF=_tmp6FD->f2;{
void*_tmp700=Cyc_Toc_typ_to_c(_tmp6FE->type);
void*_tmp701=Cyc_Toc_typ_to_c((void*)_check_null(_tmp6FF->topt));
Cyc_Toc_exp_to_c(nv,_tmp6FF);
# 3516
if(!Cyc_Tcutil_unify(_tmp700,_tmp701)){
# 3518
if(!Cyc_Tcutil_is_arithmetic_type(_tmp700) || !
Cyc_Tcutil_is_arithmetic_type(_tmp701))
_tmp6FF=Cyc_Toc_cast_it(_tmp700,Cyc_Absyn_copy_exp(_tmp6FF));}
{struct _tuple16*_tmpC97;struct Cyc_List_List*_tmpC96;_tmp6FA=((_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96->hd=((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97->f1=0,((_tmpC97->f2=_tmp6FF,_tmpC97)))))),((_tmpC96->tl=_tmp6FA,_tmpC96))))));}
break;};}}}
# 3525
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6FA));}}
# 3483
;_pop_region(rgn);};}
# 3529
goto _LL25B;_LL296: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp532->tag != 28)goto _LL298;else{_tmp533=(void*)_tmp532->f1;_tmp534=_tmp532->f2;}}_LL297: {
# 3531
struct Cyc_List_List*fs;
{void*_tmp704=Cyc_Tcutil_compress(_tmp533);struct Cyc_List_List*_tmp706;_LL361: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp705=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp704;if(_tmp705->tag != 12)goto _LL363;else{_tmp706=_tmp705->f2;}}_LL362:
 fs=_tmp706;goto _LL360;_LL363:;_LL364: {
const char*_tmpC9B;void*_tmpC9A[1];struct Cyc_String_pa_PrintArg_struct _tmpC99;(_tmpC99.tag=0,((_tmpC99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp533)),((_tmpC9A[0]=& _tmpC99,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC9B="anon struct has type %s",_tag_dyneither(_tmpC9B,sizeof(char),24))),_tag_dyneither(_tmpC9A,sizeof(void*),1)))))));}_LL360:;}{
# 3536
struct _RegionHandle _tmp70A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp70A;_push_region(rgn);{
struct Cyc_List_List*_tmp70B=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp534,Cyc_Absyn_StructA,fs);
for(0;_tmp70B != 0;_tmp70B=_tmp70B->tl){
struct Cyc_Absyn_Aggrfield*_tmp70D;struct Cyc_Absyn_Exp*_tmp70E;struct _tuple22*_tmp70C=(struct _tuple22*)_tmp70B->hd;_tmp70D=_tmp70C->f1;_tmp70E=_tmp70C->f2;{
void*_tmp70F=(void*)_check_null(_tmp70E->topt);
void*_tmp710=_tmp70D->type;
Cyc_Toc_exp_to_c(nv,_tmp70E);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp710) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp70F))
_tmp70E->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp70E->r,0));};}
# 3550
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp534);}
# 3552
_npop_handler(0);goto _LL25B;
# 3536
;_pop_region(rgn);};}_LL298: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp535->tag != 29)goto _LL29A;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;_tmp538=_tmp535->f3;}}_LL299: {
# 3555
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp711=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp712=Cyc_Absyn_var_exp(_tmp711,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp538->name,_tmp537->name));
tag_exp=_tmp537->is_extensible?Cyc_Absyn_var_exp(_tmp538->name,0):
 Cyc_Toc_datatype_tag(_tmp537,_tmp538->name);
mem_exp=_tmp712;{
struct Cyc_List_List*_tmp713=_tmp538->typs;
# 3566
if(Cyc_Toc_is_toplevel(nv)){
# 3568
struct Cyc_List_List*dles=0;
for(0;_tmp536 != 0;(_tmp536=_tmp536->tl,_tmp713=_tmp713->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp536->hd;
void*_tmp714=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp713))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp714))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpC9E;struct Cyc_List_List*_tmpC9D;dles=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=((_tmpC9E=_cycalloc(sizeof(*_tmpC9E)),((_tmpC9E->f1=0,((_tmpC9E->f2=cur_e,_tmpC9E)))))),((_tmpC9D->tl=dles,_tmpC9D))))));};}
# 3579
{struct _tuple16*_tmpCA1;struct Cyc_List_List*_tmpCA0;dles=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->hd=((_tmpCA1=_cycalloc(sizeof(*_tmpCA1)),((_tmpCA1->f1=0,((_tmpCA1->f2=tag_exp,_tmpCA1)))))),((_tmpCA0->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpCA0))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3585
struct Cyc_List_List*_tmpCA2;struct Cyc_List_List*_tmp719=
(_tmpCA2=_cycalloc(sizeof(*_tmpCA2)),((_tmpCA2->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpCA2->tl=0,_tmpCA2)))));
# 3588
{int i=1;for(0;_tmp536 != 0;(((_tmp536=_tmp536->tl,i ++)),_tmp713=_tmp713->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp536->hd;
void*_tmp71A=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp713))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp71A))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp71B=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3598
struct Cyc_List_List*_tmpCA3;_tmp719=((_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3->hd=_tmp71B,((_tmpCA3->tl=_tmp719,_tmpCA3))))));};}}{
# 3600
struct Cyc_Absyn_Stmt*_tmp71D=Cyc_Absyn_exp_stmt(_tmp712,0);
struct Cyc_List_List*_tmpCA4;struct Cyc_Absyn_Stmt*_tmp71E=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCA4=_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4->hd=_tmp71D,((_tmpCA4->tl=_tmp719,_tmpCA4))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp711,datatype_ctype,0,_tmp71E,0));};}
# 3604
goto _LL25B;};}_LL29A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp539=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp539->tag != 30)goto _LL29C;}_LL29B:
# 3606
 goto _LL29D;_LL29C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp53A=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp53A->tag != 31)goto _LL29E;}_LL29D:
 goto _LL25B;_LL29E: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp53B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp53B->tag != 32)goto _LL2A0;else{_tmp53C=(_tmp53B->f1).is_calloc;_tmp53D=(_tmp53B->f1).rgn;_tmp53E=(_tmp53B->f1).elt_type;_tmp53F=(_tmp53B->f1).num_elts;_tmp540=(_tmp53B->f1).fat_result;}}_LL29F: {
# 3610
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp53E)));
Cyc_Toc_exp_to_c(nv,_tmp53F);
# 3614
if(_tmp540){
struct _tuple0*_tmp721=Cyc_Toc_temp_var();
struct _tuple0*_tmp722=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp53C){
xexp=_tmp53F;
if(_tmp53D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp53D;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp721,0));}else{
# 3625
pexp=Cyc_Toc_calloc_exp(*_tmp53E,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp721,0));}{
# 3627
struct Cyc_Absyn_Exp*_tmpCA5[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpCA5[2]=
Cyc_Absyn_var_exp(_tmp721,0),((_tmpCA5[1]=
# 3628
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCA5[0]=Cyc_Absyn_var_exp(_tmp722,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3631
if(_tmp53D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp53D;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp721,0));}else{
# 3636
pexp=Cyc_Toc_malloc_exp(*_tmp53E,Cyc_Absyn_var_exp(_tmp721,0));}{
# 3638
struct Cyc_Absyn_Exp*_tmpCA6[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpCA6[2]=
Cyc_Absyn_var_exp(_tmp721,0),((_tmpCA6[1]=
# 3638
Cyc_Absyn_uint_exp(1,0),((_tmpCA6[0]=Cyc_Absyn_var_exp(_tmp722,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA6,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3641
struct Cyc_Absyn_Stmt*_tmp725=Cyc_Absyn_declare_stmt(_tmp721,Cyc_Absyn_uint_typ,_tmp53F,
Cyc_Absyn_declare_stmt(_tmp722,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp725);};}else{
# 3646
if(_tmp53C){
if(_tmp53D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp53D;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp53F))->r;}else{
# 3652
e->r=(Cyc_Toc_calloc_exp(*_tmp53E,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp53F))->r;}}else{
# 3655
if(_tmp53D != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp53D;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp53F))->r;}else{
# 3660
e->r=(Cyc_Toc_malloc_exp(*_tmp53E,_tmp53F))->r;}}}
# 3664
goto _LL25B;}_LL2A0: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp541->tag != 33)goto _LL2A2;else{_tmp542=_tmp541->f1;_tmp543=_tmp541->f2;}}_LL2A1: {
# 3673
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp542->topt);
void*e2_old_typ=(void*)_check_null(_tmp543->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpCA9;void*_tmpCA8;(_tmpCA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA9="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpCA9,sizeof(char),57))),_tag_dyneither(_tmpCA8,sizeof(void*),0)));}{
# 3680
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3684
swap_fn=Cyc_Toc__swap_word_e;}{
# 3688
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp542,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp543,& f2_tag,& tagged_mem_type2,1);
# 3695
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp542);
Cyc_Toc_exp_to_c(nv,_tmp543);
Cyc_Toc_set_lhs(nv,0);
# 3701
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp542,f1_tag,tagged_mem_type1);else{
# 3705
_tmp542=Cyc_Toc_push_address_exp(_tmp542);}
# 3707
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp543,f2_tag,tagged_mem_type2);else{
# 3711
_tmp543=Cyc_Toc_push_address_exp(_tmp543);}
# 3713
{struct Cyc_Absyn_Exp*_tmpCAA[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpCAA[1]=_tmp543,((_tmpCAA[0]=_tmp542,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3715
goto _LL25B;};};}_LL2A2: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp544=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp544->tag != 36)goto _LL2A4;else{_tmp545=_tmp544->f1;_tmp546=_tmp544->f2;}}_LL2A3: {
# 3718
void*_tmp729=Cyc_Tcutil_compress((void*)_check_null(_tmp545->topt));
Cyc_Toc_exp_to_c(nv,_tmp545);
{void*_tmp72A=_tmp729;struct Cyc_Absyn_Aggrdecl*_tmp72C;_LL366: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp72B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp72A;if(_tmp72B->tag != 11)goto _LL368;else{if((((_tmp72B->f1).aggr_info).KnownAggr).tag != 2)goto _LL368;_tmp72C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp72B->f1).aggr_info).KnownAggr).val);}}_LL367: {
# 3722
struct Cyc_Absyn_Exp*_tmp72D=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp72C,_tmp546),0);
struct Cyc_Absyn_Exp*_tmp72E=Cyc_Toc_member_exp(_tmp545,_tmp546,0);
struct Cyc_Absyn_Exp*_tmp72F=Cyc_Toc_member_exp(_tmp72E,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp72F,_tmp72D,0))->r;
goto _LL365;}_LL368:;_LL369: {
const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAD="non-aggregate type in tagcheck",_tag_dyneither(_tmpCAD,sizeof(char),31))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}_LL365:;}
# 3729
goto _LL25B;}_LL2A4: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp547=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp547->tag != 35)goto _LL2A6;else{_tmp548=_tmp547->f1;}}_LL2A5:
 Cyc_Toc_stmt_to_c(nv,_tmp548);goto _LL25B;_LL2A6: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp549=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp549->tag != 34)goto _LL2A8;}_LL2A7: {
const char*_tmpCB0;void*_tmpCAF;(_tmpCAF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB0="UnresolvedMem",_tag_dyneither(_tmpCB0,sizeof(char),14))),_tag_dyneither(_tmpCAF,sizeof(void*),0)));}_LL2A8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp54A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp54A->tag != 24)goto _LL2AA;}_LL2A9: {
const char*_tmpCB3;void*_tmpCB2;(_tmpCB2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB3="compoundlit",_tag_dyneither(_tmpCB3,sizeof(char),12))),_tag_dyneither(_tmpCB2,sizeof(void*),0)));}_LL2AA: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp54B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp54B->tag != 37)goto _LL2AC;}_LL2AB: {
const char*_tmpCB6;void*_tmpCB5;(_tmpCB5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB6="valueof(-)",_tag_dyneither(_tmpCB6,sizeof(char),11))),_tag_dyneither(_tmpCB5,sizeof(void*),0)));}_LL2AC: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp54C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4D7;if(_tmp54C->tag != 38)goto _LL25B;}_LL2AD: {
const char*_tmpCB9;void*_tmpCB8;(_tmpCB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCB9="__asm__",_tag_dyneither(_tmpCB9,sizeof(char),8))),_tag_dyneither(_tmpCB8,sizeof(void*),0)));}_LL25B:;};}
# 3766 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3769
Cyc_Toc_skip_stmt_dl(),0);}
# 3772
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp73A=e->r;struct Cyc_Absyn_Exp*_tmp73C;struct Cyc_Absyn_Exp*_tmp73E;struct Cyc_Absyn_Exp*_tmp740;_LL36B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp73B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp73A;if(_tmp73B->tag != 13)goto _LL36D;else{_tmp73C=_tmp73B->f2;}}_LL36C:
 return Cyc_Toc_path_length(_tmp73C);_LL36D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp73D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp73A;if(_tmp73D->tag != 19)goto _LL36F;else{_tmp73E=_tmp73D->f1;}}_LL36E:
 return 1 + Cyc_Toc_path_length(_tmp73E);_LL36F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp73F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp73A;if(_tmp73F->tag != 20)goto _LL371;else{_tmp740=_tmp73F->f1;}}_LL370:
 return Cyc_Toc_path_length(_tmp740);_LL371:;_LL372:
 return 0;_LL36A:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3781
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3786
struct Cyc_Absyn_Stmt*s;
{void*_tmp741=p->r;struct _tuple0*_tmp743;struct Cyc_Absyn_Pat*_tmp744;struct _tuple0*_tmp746;struct Cyc_Absyn_Vardecl*_tmp749;struct Cyc_Absyn_Pat*_tmp74A;enum Cyc_Absyn_Sign _tmp74D;int _tmp74E;char _tmp750;struct _dyneither_ptr _tmp752;int _tmp753;struct Cyc_Absyn_Enumdecl*_tmp755;struct Cyc_Absyn_Enumfield*_tmp756;void*_tmp758;struct Cyc_Absyn_Enumfield*_tmp759;struct Cyc_Absyn_Datatypedecl*_tmp75C;struct Cyc_Absyn_Datatypefield*_tmp75D;struct Cyc_List_List*_tmp75E;struct Cyc_List_List*_tmp760;struct Cyc_List_List*_tmp762;union Cyc_Absyn_AggrInfoU _tmp765;struct Cyc_List_List*_tmp766;struct Cyc_Absyn_Pat*_tmp768;_LL374: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp742=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp742->tag != 1)goto _LL376;else{_tmp743=(_tmp742->f1)->name;_tmp744=_tmp742->f2;}}_LL375: {
# 3790
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp76C=(void*)_check_null(_tmp744->topt);
{struct _tuple24*_tmpCBC;struct Cyc_List_List*_tmpCBB;decls=((_tmpCBB=_region_malloc(rgn,sizeof(*_tmpCBB)),((_tmpCBB->hd=((_tmpCBC=_region_malloc(rgn,sizeof(*_tmpCBC)),((_tmpCBC->f1=v,((_tmpCBC->f2=Cyc_Toc_typ_to_c_array(_tmp76C),_tmpCBC)))))),((_tmpCBB->tl=decls,_tmpCBB))))));}{
struct _tuple23 _tmp76F=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp743,Cyc_Absyn_var_exp(v,0)),rgn,_tmp76C,
Cyc_Absyn_var_exp(v,0),path,_tmp744,fail_stmt,decls);
nv=_tmp76F.f1;
decls=_tmp76F.f2;{
struct Cyc_Absyn_Stmt*_tmp770=_tmp76F.f3;
struct Cyc_Absyn_Stmt*_tmp771=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp771,_tmp770,0);
goto _LL373;};};}_LL376: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp745=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp745->tag != 3)goto _LL378;else{_tmp746=(_tmp745->f2)->name;}}_LL377: {
# 3803
struct _tuple0*_tmp772=Cyc_Toc_temp_var();
void*_tmp773=(void*)_check_null(p->topt);
{struct _tuple24*_tmpCBF;struct Cyc_List_List*_tmpCBE;decls=((_tmpCBE=_region_malloc(rgn,sizeof(*_tmpCBE)),((_tmpCBE->hd=((_tmpCBF=_region_malloc(rgn,sizeof(*_tmpCBF)),((_tmpCBF->f1=_tmp772,((_tmpCBF->f2=Cyc_Toc_typ_to_c_array(_tmp773),_tmpCBF)))))),((_tmpCBE->tl=decls,_tmpCBE))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp746,Cyc_Absyn_var_exp(_tmp772,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp772,0),r,0);
goto _LL373;}_LL378: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp747=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp747->tag != 0)goto _LL37A;}_LL379:
 s=Cyc_Toc_skip_stmt_dl();goto _LL373;_LL37A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp748=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp748->tag != 2)goto _LL37C;else{_tmp749=_tmp748->f1;_tmp74A=_tmp748->f2;}}_LL37B: {
# 3812
struct _tuple0*_tmp776=Cyc_Toc_temp_var();
{struct _tuple24*_tmpCC2;struct Cyc_List_List*_tmpCC1;decls=((_tmpCC1=_region_malloc(rgn,sizeof(*_tmpCC1)),((_tmpCC1->hd=((_tmpCC2=_region_malloc(rgn,sizeof(*_tmpCC2)),((_tmpCC2->f1=_tmp776,((_tmpCC2->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpCC2)))))),((_tmpCC1->tl=decls,_tmpCC1))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp749->name,Cyc_Absyn_var_exp(_tmp776,0));
# 3816
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp776,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp779=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp74A,fail_stmt,decls);
_tmp779.f3=Cyc_Absyn_seq_stmt(s,_tmp779.f3,0);
return _tmp779;};}_LL37C: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp74B=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp74B->tag != 8)goto _LL37E;}_LL37D:
# 3823
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL373;_LL37E: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp74C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp74C->tag != 9)goto _LL380;else{_tmp74D=_tmp74C->f1;_tmp74E=_tmp74C->f2;}}_LL37F:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp74D,_tmp74E,0),fail_stmt);goto _LL373;_LL380: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp74F=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp74F->tag != 10)goto _LL382;else{_tmp750=_tmp74F->f1;}}_LL381:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp750,0),fail_stmt);goto _LL373;_LL382: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp751=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp751->tag != 11)goto _LL384;else{_tmp752=_tmp751->f1;_tmp753=_tmp751->f2;}}_LL383:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp752,_tmp753,0),fail_stmt);goto _LL373;_LL384: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp754=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp754->tag != 12)goto _LL386;else{_tmp755=_tmp754->f1;_tmp756=_tmp754->f2;}}_LL385:
# 3828
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpCC5;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCC4;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4[0]=((_tmpCC5.tag=30,((_tmpCC5.f1=_tmp756->name,((_tmpCC5.f2=_tmp755,((_tmpCC5.f3=_tmp756,_tmpCC5)))))))),_tmpCC4)))),0),fail_stmt);}
goto _LL373;_LL386: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp757=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp757->tag != 13)goto _LL388;else{_tmp758=(void*)_tmp757->f1;_tmp759=_tmp757->f2;}}_LL387:
# 3831
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpCC8;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCC7;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=31,((_tmpCC8.f1=_tmp759->name,((_tmpCC8.f2=_tmp758,((_tmpCC8.f3=_tmp759,_tmpCC8)))))))),_tmpCC7)))),0),fail_stmt);}
goto _LL373;_LL388: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp75A=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp75A->tag != 5)goto _LL38A;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp75B=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp75A->f1)->r;if(_tmp75B->tag != 7)goto _LL38A;else{_tmp75C=_tmp75B->f1;_tmp75D=_tmp75B->f2;_tmp75E=_tmp75B->f3;}}}_LL389:
# 3841
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp77E=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp75D->name,_tmp75C->name);
void*_tmp77F=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp77F,r);
struct Cyc_List_List*_tmp780=_tmp75D->typs;
for(0;_tmp75E != 0;(((_tmp75E=_tmp75E->tl,_tmp780=((struct Cyc_List_List*)_check_null(_tmp780))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp781=(struct Cyc_Absyn_Pat*)_tmp75E->hd;
if(_tmp781->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp782=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp780))->hd)).f2;
void*_tmp783=(void*)_check_null(_tmp781->topt);
void*_tmp784=Cyc_Toc_typ_to_c_array(_tmp783);
struct Cyc_Absyn_Exp*_tmp785=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp77E,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp782)))
_tmp785=Cyc_Toc_cast_it(_tmp784,_tmp785);{
# 3859
struct _tuple23 _tmp786=Cyc_Toc_xlate_pat(nv,rgn,_tmp783,_tmp785,_tmp785,_tmp781,fail_stmt,decls);
# 3861
nv=_tmp786.f1;
decls=_tmp786.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp786.f3,0);};};}{
# 3865
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp77E,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp75C->is_extensible?Cyc_Absyn_var_exp(_tmp75D->name,0): Cyc_Toc_datatype_tag(_tmp75C,_tmp75D->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3873
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp77E,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3876
s=Cyc_Absyn_declare_stmt(_tmp77E,_tmp77F,rcast,s,0);
goto _LL373;};};_LL38A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp75F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp75F->tag != 7)goto _LL38C;else{_tmp760=_tmp75F->f3;}}_LL38B:
# 3879
 _tmp762=_tmp760;goto _LL38D;_LL38C: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp761=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp761->tag != 4)goto _LL38E;else{_tmp762=_tmp761->f1;}}_LL38D:
# 3887
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp762 != 0;(_tmp762=_tmp762->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp787=(struct Cyc_Absyn_Pat*)_tmp762->hd;
if(_tmp787->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp788=(void*)_check_null(_tmp787->topt);
struct _dyneither_ptr*_tmp789=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp78A=Cyc_Toc_xlate_pat(nv,rgn,_tmp788,Cyc_Toc_member_exp(r,_tmp789,0),
Cyc_Toc_member_exp(path,_tmp789,0),_tmp787,fail_stmt,decls);
nv=_tmp78A.f1;
decls=_tmp78A.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp78A.f3,0);};}
# 3901
goto _LL373;};_LL38E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp763=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp763->tag != 6)goto _LL390;else{if(_tmp763->f1 != 0)goto _LL390;}}_LL38F: {
# 3903
const char*_tmpCCB;void*_tmpCCA;(_tmpCCA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCB="unresolved aggregate pattern!",_tag_dyneither(_tmpCCB,sizeof(char),30))),_tag_dyneither(_tmpCCA,sizeof(void*),0)));}_LL390: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp764=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp764->tag != 6)goto _LL392;else{if(_tmp764->f1 == 0)goto _LL392;_tmp765=(_tmp764->f1)->aggr_info;_tmp766=_tmp764->f3;}}_LL391: {
# 3905
struct Cyc_Absyn_Aggrdecl*_tmp78D=Cyc_Absyn_get_known_aggrdecl(_tmp765);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp78D->impl))->tagged){
# 3909
struct Cyc_List_List*_tmp78F;struct Cyc_Absyn_Pat*_tmp790;struct _tuple25*_tmp78E=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp766))->hd;_tmp78F=_tmp78E->f1;_tmp790=_tmp78E->f2;{
struct _dyneither_ptr*f;
{void*_tmp791=(void*)((struct Cyc_List_List*)_check_null(_tmp78F))->hd;struct _dyneither_ptr*_tmp793;_LL39B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp792=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp791;if(_tmp792->tag != 1)goto _LL39D;else{_tmp793=_tmp792->f1;}}_LL39C:
 f=_tmp793;goto _LL39A;_LL39D:;_LL39E: {
const char*_tmpCCE;void*_tmpCCD;(_tmpCCD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCE="no field name in tagged union pattern",_tag_dyneither(_tmpCCE,sizeof(char),38))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}_LL39A:;}{
# 3916
void*_tmp796=(void*)_check_null(_tmp790->topt);
void*_tmp797=Cyc_Toc_typ_to_c_array(_tmp796);
# 3919
struct Cyc_Absyn_Exp*_tmp798=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp799=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp799=Cyc_Toc_cast_it(_tmp797,_tmp799);
_tmp798=Cyc_Toc_cast_it(_tmp797,_tmp798);{
struct _tuple23 _tmp79A=Cyc_Toc_xlate_pat(nv,rgn,_tmp796,_tmp799,_tmp798,_tmp790,fail_stmt,decls);
# 3927
nv=_tmp79A.f1;
decls=_tmp79A.f2;{
struct Cyc_Absyn_Stmt*_tmp79B=_tmp79A.f3;
struct Cyc_Absyn_Stmt*_tmp79C=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp78D,f),0),fail_stmt);
# 3935
s=Cyc_Absyn_seq_stmt(_tmp79C,_tmp79B,0);};};};};}else{
# 3938
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp766 != 0;_tmp766=_tmp766->tl){
struct _tuple25*_tmp79D=(struct _tuple25*)_tmp766->hd;
struct Cyc_Absyn_Pat*_tmp79E=(*_tmp79D).f2;
if(_tmp79E->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp79F=(void*)((struct Cyc_List_List*)_check_null((*_tmp79D).f1))->hd;struct _dyneither_ptr*_tmp7A1;_LL3A0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7A0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp79F;if(_tmp7A0->tag != 1)goto _LL3A2;else{_tmp7A1=_tmp7A0->f1;}}_LL3A1:
 f=_tmp7A1;goto _LL39F;_LL3A2:;_LL3A3: {
struct Cyc_Toc_Match_error_exn_struct _tmpCD1;struct Cyc_Toc_Match_error_exn_struct*_tmpCD0;(int)_throw((void*)((_tmpCD0=_cycalloc_atomic(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD1.tag=Cyc_Toc_Match_error,_tmpCD1)),_tmpCD0)))));}_LL39F:;}{
# 3949
void*_tmp7A4=(void*)_check_null(_tmp79E->topt);
void*_tmp7A5=Cyc_Toc_typ_to_c_array(_tmp7A4);
struct Cyc_Absyn_Exp*_tmp7A6=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp78D->impl))->fields,f)))->type))
_tmp7A6=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp7A5),_tmp7A6);{
struct _tuple23 _tmp7A7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7A4,_tmp7A6,
Cyc_Toc_member_exp(path,f,0),_tmp79E,fail_stmt,decls);
nv=_tmp7A7.f1;
decls=_tmp7A7.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7A7.f3,0);};};};}}
# 3961
goto _LL373;}_LL392: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp767=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp767->tag != 5)goto _LL394;else{_tmp768=_tmp767->f1;}}_LL393: {
# 3964
void*_tmp7A8=(void*)_check_null(_tmp768->topt);
# 3967
struct _tuple23 _tmp7A9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7A8,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp768,fail_stmt,decls);
# 3969
nv=_tmp7A9.f1;
decls=_tmp7A9.f2;{
struct Cyc_Absyn_Stmt*_tmp7AA=_tmp7A9.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3975
Cyc_Toc_skip_stmt_dl(),0),_tmp7AA,0);else{
# 3977
s=_tmp7AA;}
goto _LL373;};}_LL394: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp769=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp769->tag != 14)goto _LL396;}_LL395: {
const char*_tmpCD4;void*_tmpCD3;(_tmpCD3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD4="unknownid pat",_tag_dyneither(_tmpCD4,sizeof(char),14))),_tag_dyneither(_tmpCD3,sizeof(void*),0)));}_LL396: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp76A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp76A->tag != 15)goto _LL398;}_LL397: {
const char*_tmpCD7;void*_tmpCD6;(_tmpCD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD7="unknowncall pat",_tag_dyneither(_tmpCD7,sizeof(char),16))),_tag_dyneither(_tmpCD6,sizeof(void*),0)));}_LL398: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp76B=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp741;if(_tmp76B->tag != 16)goto _LL373;}_LL399: {
const char*_tmpCDA;void*_tmpCD9;(_tmpCD9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDA="exp pat",_tag_dyneither(_tmpCDA,sizeof(char),8))),_tag_dyneither(_tmpCD9,sizeof(void*),0)));}_LL373:;}{
# 3983
struct _tuple23 _tmpCDB;return(_tmpCDB.f1=nv,((_tmpCDB.f2=decls,((_tmpCDB.f3=s,_tmpCDB)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4020 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4022
struct _tuple26*_tmpCDC;return(_tmpCDC=_region_malloc(r,sizeof(*_tmpCDC)),((_tmpCDC->f1=Cyc_Toc_fresh_label(),((_tmpCDC->f2=Cyc_Toc_fresh_label(),((_tmpCDC->f3=sc,_tmpCDC)))))));}
# 4025
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4027
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7B3=(void*)_check_null(e->topt);
# 4032
int leave_as_switch;
{void*_tmp7B4=Cyc_Tcutil_compress(_tmp7B3);_LL3A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7B4;if(_tmp7B5->tag != 6)goto _LL3A7;}_LL3A6:
 goto _LL3A8;_LL3A7: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7B6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7B4;if(_tmp7B6->tag != 13)goto _LL3A9;}_LL3A8:
 leave_as_switch=1;goto _LL3A4;_LL3A9:;_LL3AA:
 leave_as_switch=0;goto _LL3A4;_LL3A4:;}
# 4038
{struct Cyc_List_List*_tmp7B7=scs;for(0;_tmp7B7 != 0;_tmp7B7=_tmp7B7->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7B7->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7B7->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4043
if(leave_as_switch){
# 4045
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7B8=scs;for(0;_tmp7B8 != 0;_tmp7B8=_tmp7B8->tl){
struct Cyc_Absyn_Stmt*_tmp7B9=((struct Cyc_Absyn_Switch_clause*)_tmp7B8->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7B8->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7B9,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7BA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7BA;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7B9);;_pop_region(rgn);};}}
# 4054
return;}{
# 4057
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4062
struct _RegionHandle _tmp7BB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7BB;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7BC=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4073
{struct Cyc_List_List*_tmp7BD=lscs;for(0;_tmp7BD != 0;_tmp7BD=_tmp7BD->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp7BD->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7BD->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp7BD->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp7BF;struct Cyc_List_List*_tmp7C0;struct Cyc_Absyn_Stmt*_tmp7C1;struct _tuple23 _tmp7BE=Cyc_Toc_xlate_pat(_tmp7BC,rgn,_tmp7B3,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp7BF=_tmp7BE.f1;_tmp7C0=_tmp7BE.f2;_tmp7C1=_tmp7BE.f3;
# 4079
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp7C2=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp7BF,_tmp7C2);
_tmp7C1=Cyc_Absyn_seq_stmt(_tmp7C1,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7C2,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4086
decls=_tmp7C0;
{struct Cyc_List_List*_tmpCDD;nvs=((_tmpCDD=_region_malloc(rgn,sizeof(*_tmpCDD)),((_tmpCDD->hd=_tmp7BF,((_tmpCDD->tl=nvs,_tmpCDD))))));}{
struct Cyc_List_List*_tmpCDE;test_stmts=((_tmpCDE=_region_malloc(rgn,sizeof(*_tmpCDE)),((_tmpCDE->hd=_tmp7C1,((_tmpCDE->tl=test_stmts,_tmpCDE))))));};}}
# 4090
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4095
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp7C6;struct _dyneither_ptr*_tmp7C7;struct Cyc_Absyn_Switch_clause*_tmp7C8;struct _tuple26*_tmp7C5=(struct _tuple26*)lscs->hd;_tmp7C6=_tmp7C5->f1;_tmp7C7=_tmp7C5->f2;_tmp7C8=_tmp7C5->f3;{
struct Cyc_Toc_Env*_tmp7C9=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp7C8->body;
struct _RegionHandle _tmp7CA=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp7CA;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp7CC;struct Cyc_Absyn_Switch_clause*_tmp7CD;struct _tuple26*_tmp7CB=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7CC=_tmp7CB->f2;_tmp7CD=_tmp7CB->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp7C9,end_l,_tmp7CC,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7CD->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4108
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp7C9,end_l),s);}
# 4110
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7C6,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp7C7,s,0),0);{
struct Cyc_List_List*_tmpCDF;stmts=((_tmpCDF=_region_malloc(rgn,sizeof(*_tmpCDF)),((_tmpCDF->hd=s,((_tmpCDF->tl=stmts,_tmpCDF))))));};
# 4100
;_pop_region(rgn2);};}{
# 4114
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4117
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp7D0;void*_tmp7D1;struct _tuple24*_tmp7CF=(struct _tuple24*)decls->hd;_tmp7D0=_tmp7CF->f1;_tmp7D1=_tmp7CF->f2;
res=Cyc_Absyn_declare_stmt(_tmp7D0,_tmp7D1,0,res,0);}
# 4122
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4063
;_pop_region(rgn);};};}
# 4127
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4133
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4135
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpCE0;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpCE0->tl=0,_tmpCE0)))))),0),0);}
# 4139
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4144
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4146
while(1){
void*_tmp7D3=s->r;struct Cyc_Absyn_Exp*_tmp7D6;struct Cyc_Absyn_Stmt*_tmp7D8;struct Cyc_Absyn_Stmt*_tmp7D9;struct Cyc_Absyn_Exp*_tmp7DB;struct Cyc_Absyn_Exp*_tmp7DD;struct Cyc_Absyn_Stmt*_tmp7DE;struct Cyc_Absyn_Stmt*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7E1;struct Cyc_Absyn_Stmt*_tmp7E2;struct Cyc_Absyn_Stmt*_tmp7E4;struct Cyc_Absyn_Stmt*_tmp7E6;struct Cyc_Absyn_Stmt*_tmp7E8;struct Cyc_Absyn_Exp*_tmp7EA;struct Cyc_Absyn_Exp*_tmp7EB;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Stmt*_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_List_List*_tmp7F0;struct Cyc_List_List*_tmp7F2;struct Cyc_Absyn_Switch_clause**_tmp7F3;struct Cyc_Absyn_Decl*_tmp7F5;struct Cyc_Absyn_Stmt*_tmp7F6;struct _dyneither_ptr*_tmp7F8;struct Cyc_Absyn_Stmt*_tmp7F9;struct Cyc_Absyn_Stmt*_tmp7FB;struct Cyc_Absyn_Exp*_tmp7FC;struct Cyc_Absyn_Stmt*_tmp7FE;struct Cyc_List_List*_tmp7FF;struct Cyc_Absyn_Exp*_tmp801;_LL3AC: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp7D4=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7D4->tag != 0)goto _LL3AE;}_LL3AD:
# 4149
 return;_LL3AE: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7D5=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7D5->tag != 1)goto _LL3B0;else{_tmp7D6=_tmp7D5->f1;}}_LL3AF:
# 4151
 Cyc_Toc_exp_to_c(nv,_tmp7D6);
return;_LL3B0: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7D7=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7D7->tag != 2)goto _LL3B2;else{_tmp7D8=_tmp7D7->f1;_tmp7D9=_tmp7D7->f2;}}_LL3B1:
# 4154
 Cyc_Toc_stmt_to_c(nv,_tmp7D8);
s=_tmp7D9;
continue;_LL3B2: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp7DA=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7DA->tag != 3)goto _LL3B4;else{_tmp7DB=_tmp7DA->f1;}}_LL3B3: {
# 4158
void*topt=0;
if(_tmp7DB != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp7DB->topt));
Cyc_Toc_exp_to_c(nv,_tmp7DB);}
# 4164
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp802=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp803=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp802,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp802,topt,_tmp7DB,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp803,0),0))->r;}else{
# 4172
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4174
return;}_LL3B4: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp7DC=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7DC->tag != 4)goto _LL3B6;else{_tmp7DD=_tmp7DC->f1;_tmp7DE=_tmp7DC->f2;_tmp7DF=_tmp7DC->f3;}}_LL3B5:
# 4176
 Cyc_Toc_exp_to_c(nv,_tmp7DD);
Cyc_Toc_stmt_to_c(nv,_tmp7DE);
s=_tmp7DF;
continue;_LL3B6: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp7E0=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7E0->tag != 5)goto _LL3B8;else{_tmp7E1=(_tmp7E0->f1).f1;_tmp7E2=_tmp7E0->f2;}}_LL3B7:
# 4181
 Cyc_Toc_exp_to_c(nv,_tmp7E1);{
struct _RegionHandle _tmp804=_new_region("temp");struct _RegionHandle*temp=& _tmp804;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7E2);
# 4185
_npop_handler(0);return;
# 4182
;_pop_region(temp);};_LL3B8: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp7E3=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7E3->tag != 6)goto _LL3BA;else{_tmp7E4=_tmp7E3->f1;}}_LL3B9: {
# 4187
struct _dyneither_ptr**_tmp806;struct Cyc_Toc_Env*_tmp805=nv;_tmp806=_tmp805->break_lab;
if(_tmp806 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp806,0);{
# 4191
int dest_depth=_tmp7E4 == 0?0: _tmp7E4->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL3BA: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp7E5=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7E5->tag != 7)goto _LL3BC;else{_tmp7E6=_tmp7E5->f1;}}_LL3BB: {
# 4195
struct _dyneither_ptr**_tmp808;struct Cyc_Toc_Env*_tmp807=nv;_tmp808=_tmp807->continue_lab;
if(_tmp808 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp808,0);
_tmp7E8=_tmp7E6;goto _LL3BD;}_LL3BC: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp7E7=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7E7->tag != 8)goto _LL3BE;else{_tmp7E8=_tmp7E7->f2;}}_LL3BD:
# 4201
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp7E8))->try_depth,s);
return;_LL3BE: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp7E9=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7E9->tag != 9)goto _LL3C0;else{_tmp7EA=_tmp7E9->f1;_tmp7EB=(_tmp7E9->f2).f1;_tmp7EC=(_tmp7E9->f3).f1;_tmp7ED=_tmp7E9->f4;}}_LL3BF:
# 4205
 Cyc_Toc_exp_to_c(nv,_tmp7EA);Cyc_Toc_exp_to_c(nv,_tmp7EB);Cyc_Toc_exp_to_c(nv,_tmp7EC);{
struct _RegionHandle _tmp809=_new_region("temp");struct _RegionHandle*temp=& _tmp809;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7ED);
# 4209
_npop_handler(0);return;
# 4206
;_pop_region(temp);};_LL3C0: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp7EE=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7EE->tag != 10)goto _LL3C2;else{_tmp7EF=_tmp7EE->f1;_tmp7F0=_tmp7EE->f2;}}_LL3C1:
# 4211
 Cyc_Toc_xlate_switch(nv,s,_tmp7EF,_tmp7F0);
return;_LL3C2: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp7F1=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7F1->tag != 11)goto _LL3C4;else{_tmp7F2=_tmp7F1->f1;_tmp7F3=_tmp7F1->f2;}}_LL3C3: {
# 4214
struct Cyc_Toc_FallthruInfo*_tmp80B;struct Cyc_Toc_Env*_tmp80A=nv;_tmp80B=_tmp80A->fallthru_info;
if(_tmp80B == 0){
const char*_tmpCE3;void*_tmpCE2;(_tmpCE2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE3="fallthru in unexpected place",_tag_dyneither(_tmpCE3,sizeof(char),29))),_tag_dyneither(_tmpCE2,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp80F;struct Cyc_List_List*_tmp810;struct Cyc_Dict_Dict _tmp811;struct Cyc_Toc_FallthruInfo _tmp80E=*_tmp80B;_tmp80F=_tmp80E.label;_tmp810=_tmp80E.binders;_tmp811=_tmp80E.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp80F,0);
# 4220
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp7F3)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp812=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp810);
struct Cyc_List_List*_tmp813=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp7F2);
for(0;_tmp812 != 0;(_tmp812=_tmp812->tl,_tmp813=_tmp813->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp813))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp811,(struct _tuple0*)_tmp812->hd),(struct Cyc_Absyn_Exp*)_tmp813->hd,0),s2,0);}
# 4228
s->r=s2->r;
return;};};};}_LL3C4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7F4=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7F4->tag != 12)goto _LL3C6;else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F4->f2;}}_LL3C5:
# 4234
{void*_tmp814=_tmp7F5->r;struct Cyc_Absyn_Vardecl*_tmp816;struct Cyc_Absyn_Pat*_tmp818;struct Cyc_Absyn_Exp*_tmp819;struct Cyc_List_List*_tmp81B;struct Cyc_Absyn_Fndecl*_tmp81D;struct Cyc_Absyn_Tvar*_tmp81F;struct Cyc_Absyn_Vardecl*_tmp820;int _tmp821;struct Cyc_Absyn_Exp*_tmp822;struct Cyc_Absyn_Tvar*_tmp824;struct Cyc_Absyn_Vardecl*_tmp825;_LL3CF: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp815=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp814;if(_tmp815->tag != 0)goto _LL3D1;else{_tmp816=_tmp815->f1;}}_LL3D0: {
# 4236
struct _RegionHandle _tmp826=_new_region("temp");struct _RegionHandle*temp=& _tmp826;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCE6;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCE5;struct Cyc_Toc_Env*_tmp827=Cyc_Toc_add_varmap(temp,nv,_tmp816->name,
Cyc_Absyn_varb_exp(_tmp816->name,(void*)((_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5[0]=((_tmpCE6.tag=4,((_tmpCE6.f1=_tmp816,_tmpCE6)))),_tmpCE5)))),0));
Cyc_Toc_local_decl_to_c(_tmp827,_tmp827,_tmp816,_tmp7F6);}
# 4241
_npop_handler(0);goto _LL3CE;
# 4236
;_pop_region(temp);}_LL3D1: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp817=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp814;if(_tmp817->tag != 2)goto _LL3D3;else{_tmp818=_tmp817->f1;_tmp819=_tmp817->f3;}}_LL3D2:
# 4245
{void*_tmp82A=_tmp818->r;struct Cyc_Absyn_Vardecl*_tmp82C;_LL3DE: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp82B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp82A;if(_tmp82B->tag != 1)goto _LL3E0;else{_tmp82C=_tmp82B->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp82D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp82B->f2)->r;if(_tmp82D->tag != 0)goto _LL3E0;};}}_LL3DF: {
# 4247
struct _tuple0*old_name=_tmp82C->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp82C->name=new_name;
_tmp82C->initializer=_tmp819;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCE9;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCE8;_tmp7F5->r=(void*)((_tmpCE8=_cycalloc(sizeof(*_tmpCE8)),((_tmpCE8[0]=((_tmpCE9.tag=0,((_tmpCE9.f1=_tmp82C,_tmpCE9)))),_tmpCE8))));}{
struct _RegionHandle _tmp830=_new_region("temp");struct _RegionHandle*temp=& _tmp830;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCEC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpCEB;struct Cyc_Toc_Env*_tmp831=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB[0]=((_tmpCEC.tag=4,((_tmpCEC.f1=_tmp82C,_tmpCEC)))),_tmpCEB)))),0));
Cyc_Toc_local_decl_to_c(_tmp831,nv,_tmp82C,_tmp7F6);}
# 4258
_npop_handler(0);goto _LL3DD;
# 4252
;_pop_region(temp);};}_LL3E0:;_LL3E1:
# 4263
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp818,(void*)_check_null(_tmp819->topt),_tmp819,_tmp7F6))->r;
goto _LL3DD;_LL3DD:;}
# 4266
goto _LL3CE;_LL3D3: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp81A=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp814;if(_tmp81A->tag != 3)goto _LL3D5;else{_tmp81B=_tmp81A->f1;}}_LL3D4: {
# 4279 "toc.cyc"
struct Cyc_List_List*_tmp834=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp81B);
if(_tmp834 == 0){
const char*_tmpCEF;void*_tmpCEE;(_tmpCEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCEF="empty Letv_d",_tag_dyneither(_tmpCEF,sizeof(char),13))),_tag_dyneither(_tmpCEE,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCF2;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCF1;_tmp7F5->r=(void*)((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1[0]=((_tmpCF2.tag=0,((_tmpCF2.f1=(struct Cyc_Absyn_Vardecl*)_tmp834->hd,_tmpCF2)))),_tmpCF1))));}
_tmp834=_tmp834->tl;
for(0;_tmp834 != 0;_tmp834=_tmp834->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpCF5;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCF4;struct Cyc_Absyn_Decl*_tmp839=Cyc_Absyn_new_decl((void*)((_tmpCF4=_cycalloc(sizeof(*_tmpCF4)),((_tmpCF4[0]=((_tmpCF5.tag=0,((_tmpCF5.f1=(struct Cyc_Absyn_Vardecl*)_tmp834->hd,_tmpCF5)))),_tmpCF4)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp839,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4288
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3CE;}_LL3D5: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp81C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp814;if(_tmp81C->tag != 1)goto _LL3D7;else{_tmp81D=_tmp81C->f1;}}_LL3D6: {
# 4291
struct _tuple0*_tmp83C=_tmp81D->name;
struct _RegionHandle _tmp83D=_new_region("temp");struct _RegionHandle*temp=& _tmp83D;_push_region(temp);{
struct Cyc_Toc_Env*_tmp83E=Cyc_Toc_add_varmap(temp,nv,_tmp81D->name,Cyc_Absyn_var_exp(_tmp83C,0));
Cyc_Toc_fndecl_to_c(_tmp83E,_tmp81D,0);
Cyc_Toc_stmt_to_c(_tmp83E,_tmp7F6);}
# 4297
_npop_handler(0);goto _LL3CE;
# 4292
;_pop_region(temp);}_LL3D7: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp81E=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp814;if(_tmp81E->tag != 4)goto _LL3D9;else{_tmp81F=_tmp81E->f1;_tmp820=_tmp81E->f2;_tmp821=_tmp81E->f3;_tmp822=_tmp81E->f4;}}_LL3D8: {
# 4299
struct Cyc_Absyn_Stmt*_tmp83F=_tmp7F6;
# 4315 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp820->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4322
struct _RegionHandle _tmp840=_new_region("temp");struct _RegionHandle*temp=& _tmp840;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp83F);
# 4325
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp83F,0))->r;else{
if(_tmp822 == 0){
struct Cyc_Absyn_Exp*_tmpCF8[1];struct Cyc_Absyn_Exp*_tmpCF7[1];struct Cyc_List_List*_tmpCF6;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpCF6->tl=0,_tmpCF6)))))),0),
# 4334
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpCF7[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF7,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp83F,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpCF8[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF8,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4340
Cyc_Toc_exp_to_c(nv,_tmp822);{
struct Cyc_Absyn_Exp*_tmpCFA[1];struct Cyc_Absyn_Exp*_tmpCF9[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpCF9[1]=_tmp822,((_tmpCF9[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4348
Cyc_Absyn_seq_stmt(_tmp83F,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpCFA[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFA,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4353
_npop_handler(0);return;
# 4322
;_pop_region(temp);}_LL3D9: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp823=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp814;if(_tmp823->tag != 5)goto _LL3DB;else{_tmp824=_tmp823->f1;_tmp825=_tmp823->f2;}}_LL3DA: {
# 4356
struct _tuple0*old_name=_tmp825->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp825->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD09;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD08;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD07;struct Cyc_Absyn_Decl*_tmpD06;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD05;s->r=(void*)((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=((_tmpD09.tag=12,((_tmpD09.f1=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->r=(void*)((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08[0]=((_tmpD07.tag=0,((_tmpD07.f1=_tmp825,_tmpD07)))),_tmpD08)))),((_tmpD06->loc=0,_tmpD06)))))),((_tmpD09.f2=_tmp7F6,_tmpD09)))))),_tmpD05))));}{
struct _RegionHandle _tmp84B=_new_region("temp");struct _RegionHandle*temp=& _tmp84B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD0C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD0B;struct Cyc_Toc_Env*_tmp84C=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B[0]=((_tmpD0C.tag=4,((_tmpD0C.f1=_tmp825,_tmpD0C)))),_tmpD0B)))),0));
Cyc_Toc_local_decl_to_c(_tmp84C,nv,_tmp825,_tmp7F6);}
# 4366
_npop_handler(0);return;
# 4360
;_pop_region(temp);};}_LL3DB:;_LL3DC: {
# 4368
const char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD0F="bad nested declaration within function",_tag_dyneither(_tmpD0F,sizeof(char),39))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));}_LL3CE:;}
# 4370
return;_LL3C6: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp7F7=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7F7->tag != 13)goto _LL3C8;else{_tmp7F8=_tmp7F7->f1;_tmp7F9=_tmp7F7->f2;}}_LL3C7:
# 4372
 s=_tmp7F9;continue;_LL3C8: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp7FA=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7FA->tag != 14)goto _LL3CA;else{_tmp7FB=_tmp7FA->f1;_tmp7FC=(_tmp7FA->f2).f1;}}_LL3C9: {
# 4374
struct _RegionHandle _tmp851=_new_region("temp");struct _RegionHandle*temp=& _tmp851;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp7FB);
Cyc_Toc_exp_to_c(nv,_tmp7FC);
# 4378
_npop_handler(0);return;
# 4374
;_pop_region(temp);}_LL3CA: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp7FD=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp7FD->tag != 15)goto _LL3CC;else{_tmp7FE=_tmp7FD->f1;_tmp7FF=_tmp7FD->f2;}}_LL3CB: {
# 4395 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4406
e_exp->topt=e_typ;{
struct _RegionHandle _tmp852=_new_region("temp");struct _RegionHandle*temp=& _tmp852;_push_region(temp);{
struct Cyc_Toc_Env*_tmp853=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4410
Cyc_Toc_stmt_to_c(_tmp853,_tmp7FE);{
struct Cyc_Absyn_Stmt*_tmp854=Cyc_Absyn_seq_stmt(_tmp7FE,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4416
struct _tuple0*_tmp855=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_var_exp(_tmp855,0);
struct Cyc_Absyn_Vardecl*_tmp857=Cyc_Absyn_new_vardecl(_tmp855,Cyc_Absyn_exn_typ(),0);
_tmp856->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD19;struct Cyc_Absyn_Pat*_tmpD18;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD17;struct Cyc_Absyn_Pat*_tmpD16;struct Cyc_Absyn_Pat*_tmp858=(_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->r=(void*)((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19[0]=((_tmpD17.tag=1,((_tmpD17.f1=_tmp857,((_tmpD17.f2=((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD18->topt=Cyc_Absyn_exn_typ(),((_tmpD18->loc=0,_tmpD18)))))))),_tmpD17)))))),_tmpD19)))),((_tmpD16->topt=Cyc_Absyn_exn_typ(),((_tmpD16->loc=0,_tmpD16)))))));
struct Cyc_Absyn_Exp*_tmp859=Cyc_Absyn_throw_exp(_tmp856,0);
_tmp859->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp85A=Cyc_Absyn_exp_stmt(_tmp859,0);
struct Cyc_Core_Opt*_tmpD1F;struct Cyc_List_List*_tmpD1E;struct Cyc_Absyn_Switch_clause*_tmpD1D;struct Cyc_Absyn_Switch_clause*_tmp85B=
(_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->pattern=_tmp858,((_tmpD1D->pat_vars=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->v=((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->hd=_tmp857,((_tmpD1E->tl=0,_tmpD1E)))))),_tmpD1F)))),((_tmpD1D->where_clause=0,((_tmpD1D->body=_tmp85A,((_tmpD1D->loc=0,_tmpD1D)))))))))));
# 4427
struct Cyc_List_List*_tmpD20;struct Cyc_Absyn_Stmt*_tmp85C=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp7FF,(
(_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->hd=_tmp85B,((_tmpD20->tl=0,_tmpD20))))))),0);
# 4431
Cyc_Toc_stmt_to_c(_tmp853,_tmp85C);{
# 4434
struct Cyc_List_List*_tmpD21;struct Cyc_Absyn_Exp*_tmp85D=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD21->tl=0,_tmpD21)))))),0);
# 4438
struct Cyc_List_List*_tmpD22;struct Cyc_Absyn_Stmt*_tmp85E=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD22->tl=0,_tmpD22)))))),0),0);
# 4442
struct Cyc_Absyn_Exp*_tmp85F=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp860=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp85E,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp85F,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp85D,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp860,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp854,
# 4453
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp85C,0),0),0),0),0),0))->r;};};};};}
# 4457
_npop_handler(0);return;
# 4407
;_pop_region(temp);};}_LL3CC: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp800=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp7D3;if(_tmp800->tag != 16)goto _LL3AB;else{_tmp801=_tmp800->f1;}}_LL3CD:
# 4459
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4462
Cyc_Toc_exp_to_c(nv,_tmp801);{
struct Cyc_List_List*_tmpD23;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->hd=_tmp801,((_tmpD23->tl=0,_tmpD23)))))),0));};}
# 4465
return;_LL3AB:;}}
# 4474
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp86C=_new_region("frgn");struct _RegionHandle*frgn=& _tmp86C;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp86D=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp86E=f->args;for(0;_tmp86E != 0;_tmp86E=_tmp86E->tl){
struct _tuple0*_tmpD24;struct _tuple0*_tmp86F=(_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->f1=Cyc_Absyn_Loc_n,((_tmpD24->f2=(*((struct _tuple8*)_tmp86E->hd)).f1,_tmpD24)))));
(*((struct _tuple8*)_tmp86E->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp86E->hd)).f3);
_tmp86D=Cyc_Toc_add_varmap(frgn,_tmp86D,_tmp86F,Cyc_Absyn_var_exp(_tmp86F,0));}}
# 4489
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4493
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp872;struct Cyc_Absyn_Tqual _tmp873;void*_tmp874;int _tmp875;struct Cyc_Absyn_VarargInfo _tmp871=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp872=_tmp871.name;_tmp873=_tmp871.tq;_tmp874=_tmp871.type;_tmp875=_tmp871.inject;{
void*_tmp876=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp874,(void*)& Cyc_Absyn_HeapRgn_val,_tmp873,Cyc_Absyn_false_conref));
struct _tuple0*_tmpD25;struct _tuple0*_tmp877=(_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->f1=Cyc_Absyn_Loc_n,((_tmpD25->f2=(struct _dyneither_ptr*)_check_null(_tmp872),_tmpD25)))));
{struct _tuple8*_tmpD28;struct Cyc_List_List*_tmpD27;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->hd=((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->f1=_tmp872,((_tmpD28->f2=_tmp873,((_tmpD28->f3=_tmp876,_tmpD28)))))))),((_tmpD27->tl=0,_tmpD27)))))));}
_tmp86D=Cyc_Toc_add_varmap(frgn,_tmp86D,_tmp877,Cyc_Absyn_var_exp(_tmp877,0));
f->cyc_varargs=0;};}
# 4502
{struct Cyc_List_List*_tmp87B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp87B != 0;_tmp87B=_tmp87B->tl){
((struct Cyc_Absyn_Vardecl*)_tmp87B->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp87B->hd)->type);}}
# 4505
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp86D),f->body);}
# 4481
;_pop_region(frgn);};}
# 4510
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL3E2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3E3:
 return Cyc_Absyn_Extern;default: _LL3E4:
 return s;}}
# 4526 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp87C=ad->name;
struct _DynRegionHandle*_tmp87E;struct Cyc_Dict_Dict*_tmp87F;struct Cyc_Toc_TocState _tmp87D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp87E=_tmp87D.dyn;_tmp87F=_tmp87D.aggrs_so_far;{
struct _DynRegionFrame _tmp880;struct _RegionHandle*d=_open_dynregion(& _tmp880,_tmp87E);
{int seen_defn_before;
struct _tuple15**_tmp881=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp87F,_tmp87C);
if(_tmp881 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpD29;v=((_tmpD29=_region_malloc(d,sizeof(*_tmpD29)),((_tmpD29->f1=ad,((_tmpD29->f2=Cyc_Absyn_strctq(_tmp87C),_tmpD29))))));}else{
# 4538
struct _tuple15*_tmpD2A;v=((_tmpD2A=_region_malloc(d,sizeof(*_tmpD2A)),((_tmpD2A->f1=ad,((_tmpD2A->f2=Cyc_Absyn_unionq_typ(_tmp87C),_tmpD2A))))));}
*_tmp87F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp87F,_tmp87C,v);};}else{
# 4541
struct Cyc_Absyn_Aggrdecl*_tmp885;void*_tmp886;struct _tuple15*_tmp884=*_tmp881;_tmp885=_tmp884->f1;_tmp886=_tmp884->f2;
if(_tmp885->impl == 0){
{struct _tuple15*_tmpD2B;*_tmp87F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp87F,_tmp87C,((_tmpD2B=_region_malloc(d,sizeof(*_tmpD2B)),((_tmpD2B->f1=ad,((_tmpD2B->f2=_tmp886,_tmpD2B)))))));}
seen_defn_before=0;}else{
# 4546
seen_defn_before=1;}}{
# 4548
struct Cyc_Absyn_Aggrdecl*_tmpD2C;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->kind=ad->kind,((_tmpD2C->sc=Cyc_Absyn_Public,((_tmpD2C->name=ad->name,((_tmpD2C->tvs=0,((_tmpD2C->impl=0,((_tmpD2C->attributes=ad->attributes,_tmpD2C)))))))))))));
# 4554
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD2D;new_ad->impl=((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->exist_vars=0,((_tmpD2D->rgn_po=0,((_tmpD2D->fields=0,((_tmpD2D->tagged=0,_tmpD2D))))))))));}{
# 4559
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp889=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp889 != 0;_tmp889=_tmp889->tl){
# 4563
struct Cyc_Absyn_Aggrfield*_tmp88A=(struct Cyc_Absyn_Aggrfield*)_tmp889->hd;
void*_tmp88B=_tmp88A->type;
if((ad->kind == Cyc_Absyn_StructA  && _tmp889->tl == 0) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp88B))){
# 4572
void*_tmp88C=Cyc_Tcutil_compress(_tmp88B);void*_tmp88E;struct Cyc_Absyn_Tqual _tmp88F;union Cyc_Absyn_Constraint*_tmp890;unsigned int _tmp891;_LL3E7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp88D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp88C;if(_tmp88D->tag != 8)goto _LL3E9;else{_tmp88E=(_tmp88D->f1).elt_type;_tmp88F=(_tmp88D->f1).tq;_tmp890=(_tmp88D->f1).zero_term;_tmp891=(_tmp88D->f1).zt_loc;}}_LL3E8:
# 4574
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD33;struct Cyc_Absyn_ArrayInfo _tmpD32;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD31;_tmp88B=(void*)((_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31[0]=((_tmpD33.tag=8,((_tmpD33.f1=((_tmpD32.elt_type=_tmp88E,((_tmpD32.tq=_tmp88F,((_tmpD32.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpD32.zero_term=_tmp890,((_tmpD32.zt_loc=_tmp891,_tmpD32)))))))))),_tmpD33)))),_tmpD31))));}
goto _LL3E6;_LL3E9:;_LL3EA:
# 4577
 if(Cyc_Toc_array_or_aggr_with_array(_tmp88B))goto _LL3E6;else{
continue;}_LL3E6:;}{
# 4581
struct Cyc_Absyn_Aggrfield*_tmpD34;struct Cyc_Absyn_Aggrfield*_tmp895=(_tmpD34=_cycalloc(sizeof(*_tmpD34)),((_tmpD34->name=_tmp88A->name,((_tmpD34->tq=Cyc_Toc_mt_tq,((_tmpD34->type=
# 4583
Cyc_Toc_typ_to_c(_tmp88B),((_tmpD34->width=_tmp88A->width,((_tmpD34->attributes=_tmp88A->attributes,_tmpD34)))))))))));
# 4589
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp896=_tmp895->type;
struct _dyneither_ptr*_tmp897=_tmp895->name;
const char*_tmpD39;void*_tmpD38[2];struct Cyc_String_pa_PrintArg_struct _tmpD37;struct Cyc_String_pa_PrintArg_struct _tmpD36;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD36.tag=0,((_tmpD36.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp897),((_tmpD37.tag=0,((_tmpD37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD38[0]=& _tmpD37,((_tmpD38[1]=& _tmpD36,Cyc_aprintf(((_tmpD39="_union_%s_%s",_tag_dyneither(_tmpD39,sizeof(char),13))),_tag_dyneither(_tmpD38,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD3A;struct _dyneither_ptr*str=(_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A[0]=s,_tmpD3A)));
struct Cyc_Absyn_Aggrfield*_tmpD3B;struct Cyc_Absyn_Aggrfield*_tmp898=(_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->name=Cyc_Toc_val_sp,((_tmpD3B->tq=Cyc_Toc_mt_tq,((_tmpD3B->type=_tmp896,((_tmpD3B->width=0,((_tmpD3B->attributes=0,_tmpD3B)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD3C;struct Cyc_Absyn_Aggrfield*_tmp899=(_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->name=Cyc_Toc_tag_sp,((_tmpD3C->tq=Cyc_Toc_mt_tq,((_tmpD3C->type=Cyc_Absyn_sint_typ,((_tmpD3C->width=0,((_tmpD3C->attributes=0,_tmpD3C)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpD3D[2];struct Cyc_List_List*_tmp89A=(_tmpD3D[1]=_tmp898,((_tmpD3D[0]=_tmp899,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3D,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpD42;struct _tuple0*_tmpD41;struct Cyc_Absyn_Aggrdecl*_tmpD40;struct Cyc_Absyn_Aggrdecl*_tmp89B=(_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->kind=Cyc_Absyn_StructA,((_tmpD40->sc=Cyc_Absyn_Public,((_tmpD40->name=(
(_tmpD41=_cycalloc(sizeof(*_tmpD41)),((_tmpD41->f1=Cyc_Absyn_Loc_n,((_tmpD41->f2=str,_tmpD41)))))),((_tmpD40->tvs=0,((_tmpD40->impl=(
(_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->exist_vars=0,((_tmpD42->rgn_po=0,((_tmpD42->fields=_tmp89A,((_tmpD42->tagged=0,_tmpD42)))))))))),((_tmpD40->attributes=0,_tmpD40)))))))))))));
# 4601
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD48;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD47;struct Cyc_List_List*_tmpD46;Cyc_Toc_result_decls=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=Cyc_Absyn_new_decl((void*)((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48[0]=((_tmpD47.tag=6,((_tmpD47.f1=_tmp89B,_tmpD47)))),_tmpD48)))),0),((_tmpD46->tl=Cyc_Toc_result_decls,_tmpD46))))));}
_tmp895->type=Cyc_Absyn_strct(str);}{
# 4604
struct Cyc_List_List*_tmpD49;new_fields=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=_tmp895,((_tmpD49->tl=new_fields,_tmpD49))))));};};}}
# 4606
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4608
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpD53;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD52;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD51;struct Cyc_List_List*_tmpD50;Cyc_Toc_result_decls=((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50->hd=((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->r=(void*)((_tmpD51=_cycalloc(sizeof(*_tmpD51)),((_tmpD51[0]=((_tmpD52.tag=6,((_tmpD52.f1=new_ad,_tmpD52)))),_tmpD51)))),((_tmpD53->loc=0,_tmpD53)))))),((_tmpD50->tl=Cyc_Toc_result_decls,_tmpD50))))));}};}
# 4530
;_pop_dynregion(d);};}
# 4637 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8B2;struct Cyc_Set_Set**_tmp8B3;struct Cyc_Toc_TocState _tmp8B1=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B2=_tmp8B1.dyn;_tmp8B3=_tmp8B1.datatypes_so_far;{
struct _DynRegionFrame _tmp8B4;struct _RegionHandle*d=_open_dynregion(& _tmp8B4,_tmp8B2);{
struct _tuple0*_tmp8B5=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp8B3,_tmp8B5)){
_npop_handler(0);return;}
*_tmp8B3=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp8B3,_tmp8B5);}{
# 4645
struct Cyc_List_List*_tmp8B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp8B6 != 0;_tmp8B6=_tmp8B6->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8B6->hd;
# 4648
struct Cyc_List_List*_tmp8B7=0;
int i=1;
{struct Cyc_List_List*_tmp8B8=f->typs;for(0;_tmp8B8 != 0;(_tmp8B8=_tmp8B8->tl,i ++)){
struct _dyneither_ptr*_tmp8B9=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpD54;struct Cyc_Absyn_Aggrfield*_tmp8BA=(_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54->name=_tmp8B9,((_tmpD54->tq=(*((struct _tuple10*)_tmp8B8->hd)).f1,((_tmpD54->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8B8->hd)).f2),((_tmpD54->width=0,((_tmpD54->attributes=0,_tmpD54)))))))))));
struct Cyc_List_List*_tmpD55;_tmp8B7=((_tmpD55=_cycalloc(sizeof(*_tmpD55)),((_tmpD55->hd=_tmp8BA,((_tmpD55->tl=_tmp8B7,_tmpD55))))));}}
# 4656
{struct Cyc_Absyn_Aggrfield*_tmpD58;struct Cyc_List_List*_tmpD57;_tmp8B7=((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57->hd=((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->name=Cyc_Toc_tag_sp,((_tmpD58->tq=Cyc_Toc_mt_tq,((_tmpD58->type=Cyc_Absyn_sint_typ,((_tmpD58->width=0,((_tmpD58->attributes=0,_tmpD58)))))))))))),((_tmpD57->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8B7),_tmpD57))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD5B;struct Cyc_Absyn_Aggrdecl*_tmpD5A;struct Cyc_Absyn_Aggrdecl*_tmp8BF=
(_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A->kind=Cyc_Absyn_StructA,((_tmpD5A->sc=Cyc_Absyn_Public,((_tmpD5A->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpD5A->tvs=0,((_tmpD5A->impl=(
# 4662
(_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B->exist_vars=0,((_tmpD5B->rgn_po=0,((_tmpD5B->fields=_tmp8B7,((_tmpD5B->tagged=0,_tmpD5B)))))))))),((_tmpD5A->attributes=0,_tmpD5A)))))))))))));
# 4664
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD61;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD60;struct Cyc_List_List*_tmpD5F;Cyc_Toc_result_decls=((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->hd=Cyc_Absyn_new_decl((void*)((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61[0]=((_tmpD60.tag=6,((_tmpD60.f1=_tmp8BF,_tmpD60)))),_tmpD61)))),0),((_tmpD5F->tl=Cyc_Toc_result_decls,_tmpD5F))))));};}};
# 4639
;_pop_dynregion(d);};}
# 4685 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp8C6;struct Cyc_Dict_Dict*_tmp8C7;struct Cyc_Toc_TocState _tmp8C5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8C6=_tmp8C5.dyn;_tmp8C7=_tmp8C5.xdatatypes_so_far;{
struct _DynRegionFrame _tmp8C8;struct _RegionHandle*d=_open_dynregion(& _tmp8C8,_tmp8C6);
{struct _tuple0*_tmp8C9=xd->name;
struct Cyc_List_List*_tmp8CA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp8CA != 0;_tmp8CA=_tmp8CA->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8CA->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8CB=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp8CC=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp8CB,Cyc_Absyn_false_conref,0);
# 4697
int*_tmp8CD=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8C7,f->name);_LL3EC: if(_tmp8CD != 0)goto _LL3EE;_LL3ED: {
# 4699
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4703
struct Cyc_Absyn_Vardecl*_tmp8CE=Cyc_Absyn_new_vardecl(f->name,_tmp8CC,initopt);
_tmp8CE->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD67;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD66;struct Cyc_List_List*_tmpD65;Cyc_Toc_result_decls=((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65->hd=Cyc_Absyn_new_decl((void*)((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67[0]=((_tmpD66.tag=0,((_tmpD66.f1=_tmp8CE,_tmpD66)))),_tmpD67)))),0),((_tmpD65->tl=Cyc_Toc_result_decls,_tmpD65))))));}
*_tmp8C7=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8C7,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp8D2=f->typs;for(0;_tmp8D2 != 0;(_tmp8D2=_tmp8D2->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpD6F;void*_tmpD6E[1];const char*_tmpD6D;struct _dyneither_ptr*_tmpD6C;struct _dyneither_ptr*_tmp8D3=(_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=(struct _dyneither_ptr)((_tmpD6F.tag=1,((_tmpD6F.f1=(unsigned long)i,((_tmpD6E[0]=& _tmpD6F,Cyc_aprintf(((_tmpD6D="f%d",_tag_dyneither(_tmpD6D,sizeof(char),4))),_tag_dyneither(_tmpD6E,sizeof(void*),1)))))))),_tmpD6C)));
struct Cyc_Absyn_Aggrfield*_tmpD70;struct Cyc_Absyn_Aggrfield*_tmp8D4=(_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->name=_tmp8D3,((_tmpD70->tq=(*((struct _tuple10*)_tmp8D2->hd)).f1,((_tmpD70->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp8D2->hd)).f2),((_tmpD70->width=0,((_tmpD70->attributes=0,_tmpD70)))))))))));
struct Cyc_List_List*_tmpD71;fields=((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71->hd=_tmp8D4,((_tmpD71->tl=fields,_tmpD71))))));}}
# 4716
{struct Cyc_Absyn_Aggrfield*_tmpD74;struct Cyc_List_List*_tmpD73;fields=((_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->name=Cyc_Toc_tag_sp,((_tmpD74->tq=Cyc_Toc_mt_tq,((_tmpD74->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpD74->width=0,((_tmpD74->attributes=0,_tmpD74)))))))))))),((_tmpD73->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpD73))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpD77;struct Cyc_Absyn_Aggrdecl*_tmpD76;struct Cyc_Absyn_Aggrdecl*_tmp8DD=
(_tmpD76=_cycalloc(sizeof(*_tmpD76)),((_tmpD76->kind=Cyc_Absyn_StructA,((_tmpD76->sc=Cyc_Absyn_Public,((_tmpD76->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpD76->tvs=0,((_tmpD76->impl=(
# 4723
(_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->exist_vars=0,((_tmpD77->rgn_po=0,((_tmpD77->fields=fields,((_tmpD77->tagged=0,_tmpD77)))))))))),((_tmpD76->attributes=0,_tmpD76)))))))))))));
# 4725
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpD7D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpD7C;struct Cyc_List_List*_tmpD7B;Cyc_Toc_result_decls=((_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((_tmpD7B->hd=Cyc_Absyn_new_decl((void*)((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7C.tag=6,((_tmpD7C.f1=_tmp8DD,_tmpD7C)))),_tmpD7D)))),0),((_tmpD7B->tl=Cyc_Toc_result_decls,_tmpD7B))))));}
# 4727
goto _LL3EB;};};};}_LL3EE: if(_tmp8CD == 0)goto _LL3F0;if(*_tmp8CD != 0)goto _LL3F0;_LL3EF:
# 4729
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp8E3=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp8E4=Cyc_Absyn_new_vardecl(f->name,_tmp8CC,_tmp8E3);
_tmp8E4->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD83;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD82;struct Cyc_List_List*_tmpD81;Cyc_Toc_result_decls=((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81->hd=Cyc_Absyn_new_decl((void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD82.tag=0,((_tmpD82.f1=_tmp8E4,_tmpD82)))),_tmpD83)))),0),((_tmpD81->tl=Cyc_Toc_result_decls,_tmpD81))))));}
*_tmp8C7=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp8C7,f->name,1);}
# 4736
goto _LL3EB;_LL3F0:;_LL3F1:
 goto _LL3EB;_LL3EB:;}}
# 4690
;_pop_dynregion(d);};};}
# 4743
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4751
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4755
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp8E8=init->r;struct Cyc_Absyn_Vardecl*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8EB;struct Cyc_Absyn_Exp*_tmp8EC;int _tmp8ED;_LL3F3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp8E9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp8E8;if(_tmp8E9->tag != 26)goto _LL3F5;else{_tmp8EA=_tmp8E9->f1;_tmp8EB=_tmp8E9->f2;_tmp8EC=_tmp8E9->f3;_tmp8ED=_tmp8E9->f4;}}_LL3F4:
# 4762
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp8EA,_tmp8EB,_tmp8EC,_tmp8ED,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3F2;_LL3F5:;_LL3F6:
# 4767
 if(vd->sc == Cyc_Absyn_Static){
# 4771
struct _RegionHandle _tmp8EE=_new_region("temp");struct _RegionHandle*temp=& _tmp8EE;_push_region(temp);
{struct Cyc_Toc_Env*_tmp8EF=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp8EF,init);}
# 4772
;_pop_region(temp);}else{
# 4776
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3F2;_LL3F2:;}else{
# 4781
void*_tmp8F0=Cyc_Tcutil_compress(old_typ);void*_tmp8F2;struct Cyc_Absyn_Exp*_tmp8F3;union Cyc_Absyn_Constraint*_tmp8F4;_LL3F8:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8F1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8F0;if(_tmp8F1->tag != 8)goto _LL3FA;else{_tmp8F2=(_tmp8F1->f1).elt_type;_tmp8F3=(_tmp8F1->f1).num_elts;_tmp8F4=(_tmp8F1->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp8F4))goto _LL3FA;_LL3F9:
 if(_tmp8F3 == 0){
const char*_tmpD86;void*_tmpD85;(_tmpD85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD86="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpD86,sizeof(char),55))),_tag_dyneither(_tmpD85,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp8F3;
struct Cyc_Absyn_Exp*_tmp8F7=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4789
struct Cyc_Absyn_Exp*_tmp8F8=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp8F7,_tmp8F8,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3F7;};_LL3FA:;_LL3FB:
 goto _LL3F7;_LL3F7:;}}
# 4803
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp8F9=_new_region("prgn");struct _RegionHandle*prgn=& _tmp8F9;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp8FA=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp8FB=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp8FD;struct Cyc_List_List*_tmp8FE;struct Cyc_Absyn_Stmt*_tmp8FF;struct _tuple23 _tmp8FC=
Cyc_Toc_xlate_pat(_tmp8FB,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp8FD=_tmp8FC.f1;_tmp8FE=_tmp8FC.f2;_tmp8FF=_tmp8FC.f3;
Cyc_Toc_stmt_to_c(_tmp8FD,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp8FF,s,0),0);
for(0;_tmp8FE != 0;_tmp8FE=_tmp8FE->tl){
struct _tuple0*_tmp901;void*_tmp902;struct _tuple24*_tmp900=(struct _tuple24*)_tmp8FE->hd;_tmp901=_tmp900->f1;_tmp902=_tmp900->f2;
s=Cyc_Absyn_declare_stmt(_tmp901,_tmp902,0,s,0);}}{
# 4819
struct Cyc_Absyn_Stmt*_tmp903=s;_npop_handler(0);return _tmp903;};
# 4806
;_pop_region(prgn);};}
# 4825
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp904=e->r;struct Cyc_Absyn_Exp*_tmp906;struct Cyc_Absyn_Exp*_tmp908;struct Cyc_Absyn_Exp*_tmp90A;struct Cyc_Absyn_Exp*_tmp90C;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_List_List*_tmp916;struct Cyc_Absyn_Exp*_tmp918;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Exp*_tmp921;struct Cyc_Absyn_Exp*_tmp922;struct Cyc_Absyn_Exp*_tmp924;struct Cyc_Absyn_Exp*_tmp925;struct Cyc_Absyn_Exp*_tmp927;struct Cyc_Absyn_Exp*_tmp928;struct Cyc_Absyn_Exp*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;struct Cyc_Absyn_Exp*_tmp92C;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_List_List*_tmp92F;void**_tmp931;struct Cyc_Absyn_Exp*_tmp932;void**_tmp934;struct Cyc_List_List*_tmp935;struct Cyc_List_List*_tmp937;struct Cyc_List_List*_tmp939;void**_tmp93B;void**_tmp93D;struct Cyc_Absyn_Stmt*_tmp93F;struct Cyc_Absyn_MallocInfo*_tmp941;_LL3FD: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp905=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp905->tag != 19)goto _LL3FF;else{_tmp906=_tmp905->f1;}}_LL3FE:
 _tmp908=_tmp906;goto _LL400;_LL3FF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp907=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp907->tag != 20)goto _LL401;else{_tmp908=_tmp907->f1;}}_LL400:
 _tmp90A=_tmp908;goto _LL402;_LL401: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp909=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp909->tag != 21)goto _LL403;else{_tmp90A=_tmp909->f1;}}_LL402:
 _tmp90C=_tmp90A;goto _LL404;_LL403: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp90B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp90B->tag != 14)goto _LL405;else{_tmp90C=_tmp90B->f1;}}_LL404:
 _tmp90E=_tmp90C;goto _LL406;_LL405: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp90D=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp90D->tag != 10)goto _LL407;else{_tmp90E=_tmp90D->f1;}}_LL406:
 _tmp910=_tmp90E;goto _LL408;_LL407: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp90F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp90F->tag != 11)goto _LL409;else{_tmp910=_tmp90F->f1;}}_LL408:
 _tmp912=_tmp910;goto _LL40A;_LL409: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp911=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp911->tag != 17)goto _LL40B;else{_tmp912=_tmp911->f1;}}_LL40A:
 _tmp914=_tmp912;goto _LL40C;_LL40B: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp913=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp913->tag != 4)goto _LL40D;else{_tmp914=_tmp913->f1;}}_LL40C:
 Cyc_Toc_exptypes_to_c(_tmp914);goto _LL3FC;_LL40D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp915=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp915->tag != 2)goto _LL40F;else{_tmp916=_tmp915->f2;}}_LL40E:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp916);goto _LL3FC;_LL40F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp917=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp917->tag != 6)goto _LL411;else{_tmp918=_tmp917->f1;_tmp919=_tmp917->f2;}}_LL410:
 _tmp91B=_tmp918;_tmp91C=_tmp919;goto _LL412;_LL411: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp91A=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp91A->tag != 7)goto _LL413;else{_tmp91B=_tmp91A->f1;_tmp91C=_tmp91A->f2;}}_LL412:
 _tmp91E=_tmp91B;_tmp91F=_tmp91C;goto _LL414;_LL413: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp91D=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp91D->tag != 8)goto _LL415;else{_tmp91E=_tmp91D->f1;_tmp91F=_tmp91D->f2;}}_LL414:
 _tmp921=_tmp91E;_tmp922=_tmp91F;goto _LL416;_LL415: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp920=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp920->tag != 22)goto _LL417;else{_tmp921=_tmp920->f1;_tmp922=_tmp920->f2;}}_LL416:
 _tmp924=_tmp921;_tmp925=_tmp922;goto _LL418;_LL417: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp923=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp923->tag != 33)goto _LL419;else{_tmp924=_tmp923->f1;_tmp925=_tmp923->f2;}}_LL418:
 _tmp927=_tmp924;_tmp928=_tmp925;goto _LL41A;_LL419: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp926=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp926->tag != 3)goto _LL41B;else{_tmp927=_tmp926->f1;_tmp928=_tmp926->f3;}}_LL41A:
 Cyc_Toc_exptypes_to_c(_tmp927);Cyc_Toc_exptypes_to_c(_tmp928);goto _LL3FC;_LL41B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp929=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp929->tag != 5)goto _LL41D;else{_tmp92A=_tmp929->f1;_tmp92B=_tmp929->f2;_tmp92C=_tmp929->f3;}}_LL41C:
# 4843
 Cyc_Toc_exptypes_to_c(_tmp92A);Cyc_Toc_exptypes_to_c(_tmp92B);Cyc_Toc_exptypes_to_c(_tmp92C);goto _LL3FC;_LL41D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp92D=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp92D->tag != 9)goto _LL41F;else{_tmp92E=_tmp92D->f1;_tmp92F=_tmp92D->f2;}}_LL41E:
# 4845
 Cyc_Toc_exptypes_to_c(_tmp92E);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp92F);goto _LL3FC;_LL41F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp930=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp930->tag != 13)goto _LL421;else{_tmp931=(void**)((void**)& _tmp930->f1);_tmp932=_tmp930->f2;}}_LL420:
*_tmp931=Cyc_Toc_typ_to_c(*_tmp931);Cyc_Toc_exptypes_to_c(_tmp932);goto _LL3FC;_LL421: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp933=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp933->tag != 24)goto _LL423;else{_tmp934=(void**)&(_tmp933->f1)->f3;_tmp935=_tmp933->f2;}}_LL422:
# 4848
*_tmp934=Cyc_Toc_typ_to_c(*_tmp934);
_tmp937=_tmp935;goto _LL424;_LL423: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp936=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp936->tag != 34)goto _LL425;else{_tmp937=_tmp936->f2;}}_LL424:
 _tmp939=_tmp937;goto _LL426;_LL425: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp938=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp938->tag != 25)goto _LL427;else{_tmp939=_tmp938->f1;}}_LL426:
# 4852
 for(0;_tmp939 != 0;_tmp939=_tmp939->tl){
struct Cyc_Absyn_Exp*_tmp951;struct _tuple16 _tmp950=*((struct _tuple16*)_tmp939->hd);_tmp951=_tmp950.f2;
Cyc_Toc_exptypes_to_c(_tmp951);}
# 4856
goto _LL3FC;_LL427: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp93A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp93A->tag != 18)goto _LL429;else{_tmp93B=(void**)((void**)& _tmp93A->f1);}}_LL428:
 _tmp93D=_tmp93B;goto _LL42A;_LL429: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp93C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp93C->tag != 16)goto _LL42B;else{_tmp93D=(void**)((void**)& _tmp93C->f1);}}_LL42A:
*_tmp93D=Cyc_Toc_typ_to_c(*_tmp93D);goto _LL3FC;_LL42B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp93E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp93E->tag != 35)goto _LL42D;else{_tmp93F=_tmp93E->f1;}}_LL42C:
 Cyc_Toc_stmttypes_to_c(_tmp93F);goto _LL3FC;_LL42D: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp940=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp940->tag != 32)goto _LL42F;else{_tmp941=(struct Cyc_Absyn_MallocInfo*)& _tmp940->f1;}}_LL42E:
# 4861
 if(_tmp941->elt_type != 0){
void**_tmpD87;_tmp941->elt_type=((_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp941->elt_type))),_tmpD87))));}
Cyc_Toc_exptypes_to_c(_tmp941->num_elts);
goto _LL3FC;_LL42F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp942=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp942->tag != 0)goto _LL431;}_LL430:
 goto _LL432;_LL431: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp943=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp943->tag != 1)goto _LL433;}_LL432:
 goto _LL434;_LL433: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp944=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp944->tag != 30)goto _LL435;}_LL434:
 goto _LL436;_LL435: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp945=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp945->tag != 38)goto _LL437;}_LL436:
 goto _LL438;_LL437: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp946=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp946->tag != 31)goto _LL439;}_LL438:
 goto _LL3FC;_LL439: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp947=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp947->tag != 28)goto _LL43B;}_LL43A:
# 4871
 goto _LL43C;_LL43B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp948=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp948->tag != 29)goto _LL43D;}_LL43C:
 goto _LL43E;_LL43D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp949=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp949->tag != 27)goto _LL43F;}_LL43E:
 goto _LL440;_LL43F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp94A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp94A->tag != 26)goto _LL441;}_LL440:
 goto _LL442;_LL441: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp94B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp94B->tag != 23)goto _LL443;}_LL442:
 goto _LL444;_LL443: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp94C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp94C->tag != 12)goto _LL445;}_LL444:
 goto _LL446;_LL445: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp94D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp94D->tag != 15)goto _LL447;}_LL446:
 goto _LL448;_LL447: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp94E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp94E->tag != 37)goto _LL449;}_LL448:
 goto _LL44A;_LL449: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp94F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp904;if(_tmp94F->tag != 36)goto _LL3FC;}_LL44A:
# 4880
{const char*_tmpD8A;void*_tmpD89;(_tmpD89=0,Cyc_Tcutil_terr(e->loc,((_tmpD8A="Cyclone expression within C code",_tag_dyneither(_tmpD8A,sizeof(char),33))),_tag_dyneither(_tmpD89,sizeof(void*),0)));}
goto _LL3FC;_LL3FC:;}
# 4885
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp955=d->r;struct Cyc_Absyn_Vardecl*_tmp957;struct Cyc_Absyn_Fndecl*_tmp959;struct Cyc_Absyn_Aggrdecl*_tmp95B;struct Cyc_Absyn_Enumdecl*_tmp95D;struct Cyc_Absyn_Typedefdecl*_tmp95F;_LL44C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp956=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp956->tag != 0)goto _LL44E;else{_tmp957=_tmp956->f1;}}_LL44D:
# 4888
 _tmp957->type=Cyc_Toc_typ_to_c(_tmp957->type);
if(_tmp957->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp957->initializer));
goto _LL44B;_LL44E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp958=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp958->tag != 1)goto _LL450;else{_tmp959=_tmp958->f1;}}_LL44F:
# 4892
 _tmp959->ret_type=Cyc_Toc_typ_to_c(_tmp959->ret_type);
{struct Cyc_List_List*_tmp96B=_tmp959->args;for(0;_tmp96B != 0;_tmp96B=_tmp96B->tl){
(*((struct _tuple8*)_tmp96B->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp96B->hd)).f3);}}
# 4896
goto _LL44B;_LL450: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp95A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp95A->tag != 6)goto _LL452;else{_tmp95B=_tmp95A->f1;}}_LL451:
 Cyc_Toc_aggrdecl_to_c(_tmp95B,1);goto _LL44B;_LL452: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp95C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp95C->tag != 8)goto _LL454;else{_tmp95D=_tmp95C->f1;}}_LL453:
# 4899
 if(_tmp95D->fields != 0){
struct Cyc_List_List*_tmp96C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp95D->fields))->v;for(0;_tmp96C != 0;_tmp96C=_tmp96C->tl){
struct Cyc_Absyn_Enumfield*_tmp96D=(struct Cyc_Absyn_Enumfield*)_tmp96C->hd;
if(_tmp96D->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp96D->tag));}}
# 4904
goto _LL44B;_LL454: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp95E=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp95E->tag != 9)goto _LL456;else{_tmp95F=_tmp95E->f1;}}_LL455:
 _tmp95F->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp95F->defn));goto _LL44B;_LL456: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp960=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp960->tag != 2)goto _LL458;}_LL457:
 goto _LL459;_LL458: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp961=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp961->tag != 3)goto _LL45A;}_LL459:
 goto _LL45B;_LL45A: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp962=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp962->tag != 7)goto _LL45C;}_LL45B:
 goto _LL45D;_LL45C: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp963=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp963->tag != 10)goto _LL45E;}_LL45D:
 goto _LL45F;_LL45E: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp964=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp964->tag != 11)goto _LL460;}_LL45F:
 goto _LL461;_LL460: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp965=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp965->tag != 12)goto _LL462;}_LL461:
 goto _LL463;_LL462: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp966=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp966->tag != 13)goto _LL464;}_LL463:
 goto _LL465;_LL464: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp967=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp967->tag != 4)goto _LL466;}_LL465:
 goto _LL467;_LL466: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp968=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp968->tag != 5)goto _LL468;}_LL467:
# 4915
{const char*_tmpD8D;void*_tmpD8C;(_tmpD8C=0,Cyc_Tcutil_terr(d->loc,((_tmpD8D="Cyclone declaration within C code",_tag_dyneither(_tmpD8D,sizeof(char),34))),_tag_dyneither(_tmpD8C,sizeof(void*),0)));}
goto _LL44B;_LL468: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp969=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp969->tag != 14)goto _LL46A;}_LL469:
 goto _LL46B;_LL46A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp96A=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp955;if(_tmp96A->tag != 15)goto _LL44B;}_LL46B:
# 4919
 goto _LL44B;_LL44B:;}
# 4923
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp970=s->r;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Stmt*_tmp974;struct Cyc_Absyn_Stmt*_tmp975;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Stmt*_tmp97A;struct Cyc_Absyn_Stmt*_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_Absyn_Stmt*_tmp97E;struct Cyc_Absyn_Exp*_tmp980;struct Cyc_Absyn_Exp*_tmp981;struct Cyc_Absyn_Exp*_tmp982;struct Cyc_Absyn_Stmt*_tmp983;struct Cyc_Absyn_Exp*_tmp985;struct Cyc_List_List*_tmp986;struct Cyc_Absyn_Decl*_tmp988;struct Cyc_Absyn_Stmt*_tmp989;struct Cyc_Absyn_Stmt*_tmp98B;struct Cyc_Absyn_Exp*_tmp98C;_LL46D: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp971=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp971->tag != 1)goto _LL46F;else{_tmp972=_tmp971->f1;}}_LL46E:
 Cyc_Toc_exptypes_to_c(_tmp972);goto _LL46C;_LL46F: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp973=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp973->tag != 2)goto _LL471;else{_tmp974=_tmp973->f1;_tmp975=_tmp973->f2;}}_LL470:
 Cyc_Toc_stmttypes_to_c(_tmp974);Cyc_Toc_stmttypes_to_c(_tmp975);goto _LL46C;_LL471: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp976=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp976->tag != 3)goto _LL473;else{_tmp977=_tmp976->f1;}}_LL472:
 if(_tmp977 != 0)Cyc_Toc_exptypes_to_c(_tmp977);goto _LL46C;_LL473: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp978=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp978->tag != 4)goto _LL475;else{_tmp979=_tmp978->f1;_tmp97A=_tmp978->f2;_tmp97B=_tmp978->f3;}}_LL474:
# 4929
 Cyc_Toc_exptypes_to_c(_tmp979);Cyc_Toc_stmttypes_to_c(_tmp97A);Cyc_Toc_stmttypes_to_c(_tmp97B);goto _LL46C;_LL475: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp97C=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp97C->tag != 5)goto _LL477;else{_tmp97D=(_tmp97C->f1).f1;_tmp97E=_tmp97C->f2;}}_LL476:
# 4931
 Cyc_Toc_exptypes_to_c(_tmp97D);Cyc_Toc_stmttypes_to_c(_tmp97E);goto _LL46C;_LL477: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp97F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp97F->tag != 9)goto _LL479;else{_tmp980=_tmp97F->f1;_tmp981=(_tmp97F->f2).f1;_tmp982=(_tmp97F->f3).f1;_tmp983=_tmp97F->f4;}}_LL478:
# 4933
 Cyc_Toc_exptypes_to_c(_tmp980);Cyc_Toc_exptypes_to_c(_tmp981);Cyc_Toc_exptypes_to_c(_tmp982);
Cyc_Toc_stmttypes_to_c(_tmp983);goto _LL46C;_LL479: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp984=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp984->tag != 10)goto _LL47B;else{_tmp985=_tmp984->f1;_tmp986=_tmp984->f2;}}_LL47A:
# 4936
 Cyc_Toc_exptypes_to_c(_tmp985);
for(0;_tmp986 != 0;_tmp986=_tmp986->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp986->hd)->body);}
goto _LL46C;_LL47B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp987=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp987->tag != 12)goto _LL47D;else{_tmp988=_tmp987->f1;_tmp989=_tmp987->f2;}}_LL47C:
 Cyc_Toc_decltypes_to_c(_tmp988);Cyc_Toc_stmttypes_to_c(_tmp989);goto _LL46C;_LL47D: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp98A=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp98A->tag != 14)goto _LL47F;else{_tmp98B=_tmp98A->f1;_tmp98C=(_tmp98A->f2).f1;}}_LL47E:
 Cyc_Toc_stmttypes_to_c(_tmp98B);Cyc_Toc_exptypes_to_c(_tmp98C);goto _LL46C;_LL47F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp98D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp98D->tag != 0)goto _LL481;}_LL480:
 goto _LL482;_LL481: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp98E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp98E->tag != 6)goto _LL483;}_LL482:
 goto _LL484;_LL483: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp98F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp98F->tag != 7)goto _LL485;}_LL484:
 goto _LL486;_LL485: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp990=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp990->tag != 8)goto _LL487;}_LL486:
 goto _LL46C;_LL487: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp991=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp991->tag != 11)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp992=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp992->tag != 13)goto _LL48B;}_LL48A:
 goto _LL48C;_LL48B: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp993=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp993->tag != 15)goto _LL48D;}_LL48C:
 goto _LL48E;_LL48D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp994=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp970;if(_tmp994->tag != 16)goto _LL46C;}_LL48E:
{const char*_tmpD90;void*_tmpD8F;(_tmpD8F=0,Cyc_Tcutil_terr(s->loc,((_tmpD90="Cyclone statement in C code",_tag_dyneither(_tmpD90,sizeof(char),28))),_tag_dyneither(_tmpD8F,sizeof(void*),0)));}
goto _LL46C;_LL46C:;}
# 4957
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpD93;void*_tmpD92;(_tmpD92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD93="decls_to_c: not at toplevel!",_tag_dyneither(_tmpD93,sizeof(char),29))),_tag_dyneither(_tmpD92,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp999=d->r;struct Cyc_Absyn_Vardecl*_tmp99B;struct Cyc_Absyn_Fndecl*_tmp99D;struct Cyc_Absyn_Aggrdecl*_tmp9A3;struct Cyc_Absyn_Datatypedecl*_tmp9A5;struct Cyc_Absyn_Enumdecl*_tmp9A7;struct Cyc_Absyn_Typedefdecl*_tmp9A9;struct Cyc_List_List*_tmp9AD;struct Cyc_List_List*_tmp9AF;struct Cyc_List_List*_tmp9B1;struct Cyc_List_List*_tmp9B3;_LL490: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp99A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp99A->tag != 0)goto _LL492;else{_tmp99B=_tmp99A->f1;}}_LL491: {
# 4964
struct _tuple0*_tmp9B4=_tmp99B->name;
# 4966
if(_tmp99B->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpD94;_tmp9B4=((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->f1=Cyc_Absyn_Rel_n(0),((_tmpD94->f2=(*_tmp9B4).f2,_tmpD94))))));}
if(_tmp99B->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp99B->initializer));else{
# 4972
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp99B->initializer));}}
# 4974
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpD97;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD96;nv=Cyc_Toc_add_varmap(r,nv,_tmp99B->name,Cyc_Absyn_varb_exp(_tmp9B4,(void*)((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=1,((_tmpD97.f1=_tmp99B,_tmpD97)))),_tmpD96)))),0));}
_tmp99B->name=_tmp9B4;
_tmp99B->sc=Cyc_Toc_scope_to_c(_tmp99B->sc);
_tmp99B->type=Cyc_Toc_typ_to_c(_tmp99B->type);
{struct Cyc_List_List*_tmpD98;Cyc_Toc_result_decls=((_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98->hd=d,((_tmpD98->tl=Cyc_Toc_result_decls,_tmpD98))))));}
goto _LL48F;}_LL492: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp99C=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp99C->tag != 1)goto _LL494;else{_tmp99D=_tmp99C->f1;}}_LL493: {
# 4981
struct _tuple0*_tmp9B9=_tmp99D->name;
# 4983
if(_tmp99D->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpD99;_tmp9B9=((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->f1=Cyc_Absyn_Abs_n(0,1),((_tmpD99->f2=(*_tmp9B9).f2,_tmpD99))))));}
_tmp99D->sc=Cyc_Absyn_Public;}
# 4987
nv=Cyc_Toc_add_varmap(r,nv,_tmp99D->name,Cyc_Absyn_var_exp(_tmp9B9,0));
_tmp99D->name=_tmp9B9;
Cyc_Toc_fndecl_to_c(nv,_tmp99D,cinclude);
{struct Cyc_List_List*_tmpD9A;Cyc_Toc_result_decls=((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A->hd=d,((_tmpD9A->tl=Cyc_Toc_result_decls,_tmpD9A))))));}
goto _LL48F;}_LL494: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp99E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp99E->tag != 2)goto _LL496;}_LL495:
 goto _LL497;_LL496: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp99F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp99F->tag != 3)goto _LL498;}_LL497: {
# 4994
const char*_tmpD9D;void*_tmpD9C;(_tmpD9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD9D="letdecl at toplevel",_tag_dyneither(_tmpD9D,sizeof(char),20))),_tag_dyneither(_tmpD9C,sizeof(void*),0)));}_LL498: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9A0=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9A0->tag != 4)goto _LL49A;}_LL499: {
# 4996
const char*_tmpDA0;void*_tmpD9F;(_tmpD9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA0="region decl at toplevel",_tag_dyneither(_tmpDA0,sizeof(char),24))),_tag_dyneither(_tmpD9F,sizeof(void*),0)));}_LL49A: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9A1=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9A1->tag != 5)goto _LL49C;}_LL49B: {
# 4998
const char*_tmpDA3;void*_tmpDA2;(_tmpDA2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDA3="alias decl at toplevel",_tag_dyneither(_tmpDA3,sizeof(char),23))),_tag_dyneither(_tmpDA2,sizeof(void*),0)));}_LL49C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9A2=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9A2->tag != 6)goto _LL49E;else{_tmp9A3=_tmp9A2->f1;}}_LL49D:
# 5000
 Cyc_Toc_aggrdecl_to_c(_tmp9A3,1);
goto _LL48F;_LL49E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9A4=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9A4->tag != 7)goto _LL4A0;else{_tmp9A5=_tmp9A4->f1;}}_LL49F:
# 5003
 if(_tmp9A5->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp9A5);else{
# 5006
Cyc_Toc_datatypedecl_to_c(_tmp9A5);}
goto _LL48F;_LL4A0: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9A6=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9A6->tag != 8)goto _LL4A2;else{_tmp9A7=_tmp9A6->f1;}}_LL4A1:
# 5009
 Cyc_Toc_enumdecl_to_c(_tmp9A7);
{struct Cyc_List_List*_tmpDA4;Cyc_Toc_result_decls=((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->hd=d,((_tmpDA4->tl=Cyc_Toc_result_decls,_tmpDA4))))));}
goto _LL48F;_LL4A2: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9A8=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9A8->tag != 9)goto _LL4A4;else{_tmp9A9=_tmp9A8->f1;}}_LL4A3:
# 5013
 _tmp9A9->name=_tmp9A9->name;
_tmp9A9->tvs=0;
if(_tmp9A9->defn != 0)
_tmp9A9->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9A9->defn));else{
# 5018
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp9A9->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL4B0:
 _tmp9A9->defn=Cyc_Absyn_void_star_typ();break;default: _LL4B1:
 _tmp9A9->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5023
{struct Cyc_List_List*_tmpDA5;Cyc_Toc_result_decls=((_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5->hd=d,((_tmpDA5->tl=Cyc_Toc_result_decls,_tmpDA5))))));}
goto _LL48F;_LL4A4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9AA=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9AA->tag != 14)goto _LL4A6;}_LL4A5:
 goto _LL4A7;_LL4A6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9AB=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9AB->tag != 15)goto _LL4A8;}_LL4A7:
# 5027
 goto _LL48F;_LL4A8: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9AC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9AC->tag != 10)goto _LL4AA;else{_tmp9AD=_tmp9AC->f2;}}_LL4A9:
 _tmp9AF=_tmp9AD;goto _LL4AB;_LL4AA: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9AE=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9AE->tag != 11)goto _LL4AC;else{_tmp9AF=_tmp9AE->f2;}}_LL4AB:
 _tmp9B1=_tmp9AF;goto _LL4AD;_LL4AC: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9B0=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9B0->tag != 12)goto _LL4AE;else{_tmp9B1=_tmp9B0->f1;}}_LL4AD:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9B1,top,cinclude);goto _LL48F;_LL4AE: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9B2=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp999;if(_tmp9B2->tag != 13)goto _LL48F;else{_tmp9B3=_tmp9B2->f1;}}_LL4AF:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmp9B3,top,1);goto _LL48F;_LL48F:;};}
# 5034
return nv;}
# 5038
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp9C5;struct Cyc_Core_NewRegion _tmp9C4=Cyc_Core__new_dynregion("internal-error",0);_tmp9C5=_tmp9C4.dynregion;{
struct _DynRegionFrame _tmp9C6;struct _RegionHandle*d=_open_dynregion(& _tmp9C6,_tmp9C5);{
struct Cyc_Dict_Dict*_tmpDB2;struct Cyc_Dict_Dict*_tmpDB1;struct Cyc_Set_Set**_tmpDB0;struct Cyc_List_List**_tmpDAF;struct Cyc_Dict_Dict*_tmpDAE;struct Cyc_List_List**_tmpDAD;struct Cyc_Toc_TocState*_tmpDAC;Cyc_Toc_toc_state=(
(_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->dyn=(struct _DynRegionHandle*)_tmp9C5,((_tmpDAC->tuple_types=(struct Cyc_List_List**)(
(_tmpDAD=_region_malloc(d,sizeof(*_tmpDAD)),((_tmpDAD[0]=0,_tmpDAD)))),((_tmpDAC->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDAE=_region_malloc(d,sizeof(*_tmpDAE)),((_tmpDAE[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDAE)))),((_tmpDAC->abs_struct_types=(struct Cyc_List_List**)(
(_tmpDAF=_region_malloc(d,sizeof(*_tmpDAF)),((_tmpDAF[0]=0,_tmpDAF)))),((_tmpDAC->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpDB0=_region_malloc(d,sizeof(*_tmpDB0)),((_tmpDB0[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDB0)))),((_tmpDAC->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpDB1=_region_malloc(d,sizeof(*_tmpDB1)),((_tmpDB1[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpDB1)))),((_tmpDAC->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpDB2=_region_malloc(d,sizeof(*_tmpDB2)),((_tmpDB2[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpDB2)))),((_tmpDAC->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpDAC))))))))))))))))));}
# 5052
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpDB3;Cyc_Toc_globals=_tag_dyneither(((_tmpDB3=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpDB3[0]=& Cyc_Toc__throw_str,((_tmpDB3[1]=& Cyc_Toc_setjmp_str,((_tmpDB3[2]=& Cyc_Toc__push_handler_str,((_tmpDB3[3]=& Cyc_Toc__pop_handler_str,((_tmpDB3[4]=& Cyc_Toc__exn_thrown_str,((_tmpDB3[5]=& Cyc_Toc__npop_handler_str,((_tmpDB3[6]=& Cyc_Toc__check_null_str,((_tmpDB3[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpDB3[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpDB3[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpDB3[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpDB3[11]=& Cyc_Toc__tag_dyneither_str,((_tmpDB3[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpDB3[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpDB3[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpDB3[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpDB3[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpDB3[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpDB3[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpDB3[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpDB3[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpDB3[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpDB3[22]=& Cyc_Toc__cycalloc_str,((_tmpDB3[23]=& Cyc_Toc__cyccalloc_str,((_tmpDB3[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpDB3[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpDB3[26]=& Cyc_Toc__region_malloc_str,((_tmpDB3[27]=& Cyc_Toc__region_calloc_str,((_tmpDB3[28]=& Cyc_Toc__check_times_str,((_tmpDB3[29]=& Cyc_Toc__new_region_str,((_tmpDB3[30]=& Cyc_Toc__push_region_str,((_tmpDB3[31]=& Cyc_Toc__pop_region_str,((_tmpDB3[32]=& Cyc_Toc__open_dynregion_str,((_tmpDB3[33]=& Cyc_Toc__push_dynregion_str,((_tmpDB3[34]=& Cyc_Toc__pop_dynregion_str,((_tmpDB3[35]=& Cyc_Toc__reset_region_str,((_tmpDB3[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpDB3[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpDB3[38]=& Cyc_Toc__throw_match_str,((_tmpDB3[39]=& Cyc_Toc__swap_word_str,((_tmpDB3[40]=& Cyc_Toc__swap_dyneither_str,_tmpDB3)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5040
;_pop_dynregion(d);};}
# 5105
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmp9CF=_new_region("start");struct _RegionHandle*start=& _tmp9CF;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5114
struct _DynRegionHandle*_tmp9D1;struct Cyc_Xarray_Xarray*_tmp9D2;struct Cyc_Toc_TocState _tmp9D0=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp9D1=_tmp9D0.dyn;_tmp9D2=_tmp9D0.temp_labels;
{struct _DynRegionFrame _tmp9D3;struct _RegionHandle*d=_open_dynregion(& _tmp9D3,_tmp9D1);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp9D2);;_pop_dynregion(d);}
# 5118
Cyc_Core_free_dynregion(_tmp9D1);};{
# 5120
struct Cyc_List_List*_tmp9D4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmp9D4;};
# 5107
;_pop_region(start);};}
