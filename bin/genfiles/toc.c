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
# 924
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
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
{const char*_tmpA2E;void*_tmpA2D;(_tmpA2D=0,Cyc_fprintf(Cyc_stderr,((_tmpA2E="\n",_tag_dyneither(_tmpA2E,sizeof(char),2))),_tag_dyneither(_tmpA2D,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA31;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA30;(int)_throw((void*)((_tmpA30=_cycalloc_atomic(sizeof(*_tmpA30)),((_tmpA30[0]=((_tmpA31.tag=Cyc_Toc_Toc_Unimplemented,_tmpA31)),_tmpA30)))));};}
# 144
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 147
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA34;void*_tmpA33;(_tmpA33=0,Cyc_fprintf(Cyc_stderr,((_tmpA34="\n",_tag_dyneither(_tmpA34,sizeof(char),2))),_tag_dyneither(_tmpA33,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA37;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA36;(int)_throw((void*)((_tmpA36=_cycalloc_atomic(sizeof(*_tmpA36)),((_tmpA36[0]=((_tmpA37.tag=Cyc_Toc_Toc_Impossible,_tmpA37)),_tmpA36)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp16[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmpA38;skip_stmt_opt=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=Cyc_Absyn_skip_stmt(0),_tmpA38))));}
return*skip_stmt_opt;}
# 273
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 276
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA3B;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA3A;return(void*)((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=13,((_tmpA3B.f1=t,((_tmpA3B.f2=e,((_tmpA3B.f3=0,((_tmpA3B.f4=Cyc_Absyn_No_coercion,_tmpA3B)))))))))),_tmpA3A))));}
# 279
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA3E;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA3D;return(void*)((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D[0]=((_tmpA3E.tag=19,((_tmpA3E.f1=e,_tmpA3E)))),_tmpA3D))));}
# 282
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA41;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA40;return(void*)((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=22,((_tmpA41.f1=e1,((_tmpA41.f2=e2,_tmpA41)))))),_tmpA40))));}
# 285
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA44;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA43;return(void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=36,((_tmpA44.f1=s,_tmpA44)))),_tmpA43))));}
# 288
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA47;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA46;return(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=16,((_tmpA47.f1=t,_tmpA47)))),_tmpA46))));}
# 291
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA4A;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA49;return(void*)((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=9,((_tmpA4A.f1=e,((_tmpA4A.f2=es,((_tmpA4A.f3=0,((_tmpA4A.f4=1,_tmpA4A)))))))))),_tmpA49))));}
# 294
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA4D;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA4C;return(void*)((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=1,((_tmpA4D.f1=e,_tmpA4D)))),_tmpA4C))));}
# 297
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA50;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA4F;return(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=2,((_tmpA50.f1=s1,((_tmpA50.f2=s2,_tmpA50)))))),_tmpA4F))));}
# 300
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA53;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA52;return(void*)((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=5,((_tmpA53.f1=e1,((_tmpA53.f2=e2,((_tmpA53.f3=e3,_tmpA53)))))))),_tmpA52))));}
# 303
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA56;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA55;return(void*)((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=20,((_tmpA56.f1=e,((_tmpA56.f2=n,((_tmpA56.f3=0,((_tmpA56.f4=0,_tmpA56)))))))))),_tmpA55))));}
# 306
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA59;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA58;return(void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=21,((_tmpA59.f1=e,((_tmpA59.f2=n,((_tmpA59.f3=0,((_tmpA59.f4=0,_tmpA59)))))))))),_tmpA58))));}
# 309
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 312
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA5C;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA5B;return(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=35,((_tmpA5C.f1=tdopt,((_tmpA5C.f2=ds,_tmpA5C)))))),_tmpA5B))));}
# 314
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA5F;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA5E;return(void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=8,((_tmpA5F.f1=v,((_tmpA5F.f2=s,_tmpA5F)))))),_tmpA5E))));}
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
struct Cyc_Core_Impossible_exn_struct _tmpA65;const char*_tmpA64;struct Cyc_Core_Impossible_exn_struct*_tmpA63;(int)_throw((void*)((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA65.tag=Cyc_Core_Impossible,((_tmpA65.f1=((_tmpA64="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA64,sizeof(char),44))),_tmpA65)))),_tmpA63)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA72;const char*_tmpA71;void*_tmpA70[1];struct Cyc_String_pa_PrintArg_struct _tmpA6F;struct Cyc_Core_Impossible_exn_struct*_tmpA6E;(int)_throw((void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA72.tag=Cyc_Core_Impossible,((_tmpA72.f1=(struct _dyneither_ptr)((_tmpA6F.tag=0,((_tmpA6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA70[0]=& _tmpA6F,Cyc_aprintf(((_tmpA71="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA71,sizeof(char),67))),_tag_dyneither(_tmpA70,sizeof(void*),1)))))))),_tmpA72)))),_tmpA6E)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA78;const char*_tmpA77;struct Cyc_Core_Impossible_exn_struct*_tmpA76;(int)_throw((void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA78.tag=Cyc_Core_Impossible,((_tmpA78.f1=((_tmpA77="impossible type (not pointer)",_tag_dyneither(_tmpA77,sizeof(char),30))),_tmpA78)))),_tmpA76)))));}_LL17:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpF4->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 451
static int Cyc_Toc_is_nullable(void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);union Cyc_Absyn_Constraint*_tmpF8;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF6;if(_tmpF7->tag != 5)goto _LL3A;else{_tmpF8=((_tmpF7->f1).ptr_atts).nullable;}}_LL39:
# 454
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8);_LL3A:;_LL3B: {
const char*_tmpA7B;void*_tmpA7A;(_tmpA7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA7B="is_nullable",_tag_dyneither(_tmpA7B,sizeof(char),12))),_tag_dyneither(_tmpA7A,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA7D;unsigned int _tmpA7C;struct _dyneither_ptr buf=(_tmpA7C=len,((_tmpA7D=_cyccalloc_atomic(sizeof(char),_tmpA7C),_tag_dyneither(_tmpA7D,sizeof(char),_tmpA7C))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr _tmp104;struct _dyneither_ptr*_tmp103=(struct _dyneither_ptr*)nms->hd;_tmp104=*_tmp103;
{char _tmpA80;char _tmpA7F;struct _dyneither_ptr _tmpA7E;(_tmpA7E=p,((_tmpA7F=*((char*)_check_dyneither_subscript(_tmpA7E,sizeof(char),0)),((_tmpA80='_',((_get_dyneither_size(_tmpA7E,sizeof(char))== 1  && (_tmpA7F == '\000'  && _tmpA80 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA7E.curr)=_tmpA80)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp104);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp104));}
# 484
{char _tmpA83;char _tmpA82;struct _dyneither_ptr _tmpA81;(_tmpA81=p,((_tmpA82=*((char*)_check_dyneither_subscript(_tmpA81,sizeof(char),0)),((_tmpA83='_',((_get_dyneither_size(_tmpA81,sizeof(char))== 1  && (_tmpA82 == '\000'  && _tmpA83 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA81.curr)=_tmpA83)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmpFD);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA84;Cyc_strcpy(p,((_tmpA84="_struct",_tag_dyneither(_tmpA84,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}static char _tmp11D[8]="*bogus*";
# 496
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
struct _DynRegionHandle*_tmp10F;struct Cyc_Dict_Dict*_tmp110;struct Cyc_Toc_TocState _tmp10E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10F=_tmp10E.dyn;_tmp110=_tmp10E.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp11D,_tmp11D,_tmp11D + 8};
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
{struct _tuple14 _tmpA85;pair=((_tmpA85.f1=fieldname,((_tmpA85.f2=dtname,_tmpA85))));}{
struct _DynRegionFrame _tmp112;struct _RegionHandle*d=_open_dynregion(& _tmp112,_tmp10F);
{struct _tuple0**_tmp113=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp110,& pair);
if(_tmp113 != 0){struct _tuple0*_tmp114=*_tmp113;_npop_handler(0);return _tmp114;}{
struct _tuple14*_tmpA86;struct _tuple14*_tmp115=(_tmpA86=_cycalloc(sizeof(*_tmpA86)),((_tmpA86->f1=fieldname,((_tmpA86->f2=dtname,_tmpA86)))));
union Cyc_Absyn_Nmspace _tmp117;struct _dyneither_ptr _tmp118;struct _tuple0*_tmp116=fieldname;_tmp117=_tmp116->f1;_tmp118=*_tmp116->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp118,dtname);
struct _dyneither_ptr*_tmpA89;struct _tuple0*_tmpA88;struct _tuple0*res=(_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->f1=_tmp117,((_tmpA88->f2=((_tmpA89=_cycalloc(sizeof(*_tmpA89)),((_tmpA89[0]=newvar,_tmpA89)))),_tmpA88)))));
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
struct Cyc_Int_pa_PrintArg_struct _tmpA91;void*_tmpA90[1];const char*_tmpA8F;struct _dyneither_ptr*_tmpA8E;struct _dyneither_ptr*xname=(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=(struct _dyneither_ptr)((_tmpA91.tag=1,((_tmpA91.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA90[0]=& _tmpA91,Cyc_aprintf(((_tmpA8F="_tuple%d",_tag_dyneither(_tmpA8F,sizeof(char),9))),_tag_dyneither(_tmpA90,sizeof(void*),1)))))))),_tmpA8E)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
struct Cyc_List_List*_tmp128=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA94;struct Cyc_List_List*_tmpA93;_tmp128=((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93->hd=((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->name=Cyc_Absyn_fieldname(i),((_tmpA94->tq=Cyc_Toc_mt_tq,((_tmpA94->type=(void*)ts2->hd,((_tmpA94->width=0,((_tmpA94->attributes=0,_tmpA94)))))))))))),((_tmpA93->tl=_tmp128,_tmpA93))))));}}
_tmp128=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp128);{
struct Cyc_Absyn_AggrdeclImpl*_tmpA99;struct _tuple0*_tmpA98;struct Cyc_Absyn_Aggrdecl*_tmpA97;struct Cyc_Absyn_Aggrdecl*_tmp12B=(_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->kind=Cyc_Absyn_StructA,((_tmpA97->sc=Cyc_Absyn_Public,((_tmpA97->name=(
(_tmpA98=_cycalloc(sizeof(*_tmpA98)),((_tmpA98->f1=Cyc_Absyn_Rel_n(0),((_tmpA98->f2=xname,_tmpA98)))))),((_tmpA97->tvs=0,((_tmpA97->impl=(
(_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->exist_vars=0,((_tmpA99->rgn_po=0,((_tmpA99->fields=_tmp128,((_tmpA99->tagged=0,_tmpA99)))))))))),((_tmpA97->attributes=0,_tmpA97)))))))))))));
# 547
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA9F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpA9E;struct Cyc_List_List*_tmpA9D;Cyc_Toc_result_decls=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->hd=Cyc_Absyn_new_decl((void*)((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpA9E.tag=6,((_tmpA9E.f1=_tmp12B,_tmpA9E)))),_tmpA9F)))),0),((_tmpA9D->tl=Cyc_Toc_result_decls,_tmpA9D))))));}
{struct _tuple17*_tmpAA2;struct Cyc_List_List*_tmpAA1;*_tmp120=((_tmpAA1=_region_malloc(d,sizeof(*_tmpAA1)),((_tmpAA1->hd=((_tmpAA2=_region_malloc(d,sizeof(*_tmpAA2)),((_tmpAA2->f1=x,((_tmpAA2->f2=ts,_tmpAA2)))))),((_tmpAA1->tl=*_tmp120,_tmpAA1))))));}{
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
struct Cyc_Int_pa_PrintArg_struct _tmpAAA;void*_tmpAA9[1];const char*_tmpAA8;struct _dyneither_ptr*_tmpAA7;struct _dyneither_ptr*xname=(_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=(struct _dyneither_ptr)((_tmpAAA.tag=1,((_tmpAAA.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAA9[0]=& _tmpAAA,Cyc_aprintf(((_tmpAA8="_tuple%d",_tag_dyneither(_tmpAA8,sizeof(char),9))),_tag_dyneither(_tmpAA9,sizeof(void*),1)))))))),_tmpAA7)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp147=0;
# 611
{struct _tuple18*_tmpAAD;struct Cyc_List_List*_tmpAAC;*_tmp13B=((_tmpAAC=_region_malloc(d,sizeof(*_tmpAAC)),((_tmpAAC->hd=((_tmpAAD=_region_malloc(d,sizeof(*_tmpAAD)),((_tmpAAD->f1=struct_name,((_tmpAAD->f2=type_args,((_tmpAAD->f3=x,_tmpAAD)))))))),((_tmpAAC->tl=*_tmp13B,_tmpAAC))))));}{
# 614
struct _RegionHandle _tmp14A=_new_region("r");struct _RegionHandle*r=& _tmp14A;_push_region(r);
{struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp14C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*t=_tmp14C->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp14B,t));
# 621
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpAB0;struct Cyc_List_List*_tmpAAF;_tmp147=((_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF->hd=((_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->name=_tmp14C->name,((_tmpAB0->tq=Cyc_Toc_mt_tq,((_tmpAB0->type=t,((_tmpAB0->width=_tmp14C->width,((_tmpAB0->attributes=_tmp14C->attributes,_tmpAB0)))))))))))),((_tmpAAF->tl=_tmp147,_tmpAAF))))));};}
# 625
_tmp147=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp147);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAB5;struct _tuple0*_tmpAB4;struct Cyc_Absyn_Aggrdecl*_tmpAB3;struct Cyc_Absyn_Aggrdecl*_tmp14F=(_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->kind=Cyc_Absyn_StructA,((_tmpAB3->sc=Cyc_Absyn_Public,((_tmpAB3->name=(
(_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->f1=Cyc_Absyn_Rel_n(0),((_tmpAB4->f2=xname,_tmpAB4)))))),((_tmpAB3->tvs=0,((_tmpAB3->impl=(
(_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->exist_vars=0,((_tmpAB5->rgn_po=0,((_tmpAB5->fields=_tmp147,((_tmpAB5->tagged=0,_tmpAB5)))))))))),((_tmpAB3->attributes=0,_tmpAB3)))))))))))));
# 633
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpABB;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpABA;struct Cyc_List_List*_tmpAB9;Cyc_Toc_result_decls=((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->hd=Cyc_Absyn_new_decl((void*)((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB[0]=((_tmpABA.tag=6,((_tmpABA.f1=_tmp14F,_tmpABA)))),_tmpABB)))),0),((_tmpAB9->tl=Cyc_Toc_result_decls,_tmpAB9))))));}{
void*_tmp153=x;_npop_handler(1);return _tmp153;};};}
# 615
;_pop_region(r);};};
# 569
;_pop_dynregion(d);};}
# 642
struct _tuple0*Cyc_Toc_temp_var(){
int _tmp15B=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpAC8;const char*_tmpAC7;void*_tmpAC6[1];struct Cyc_Int_pa_PrintArg_struct _tmpAC5;struct _tuple0*_tmpAC4;struct _tuple0*res=(_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->f1=Cyc_Absyn_Loc_n,((_tmpAC4->f2=((_tmpAC8=_cycalloc(sizeof(*_tmpAC8)),((_tmpAC8[0]=(struct _dyneither_ptr)((_tmpAC5.tag=1,((_tmpAC5.f1=(unsigned int)_tmp15B,((_tmpAC6[0]=& _tmpAC5,Cyc_aprintf(((_tmpAC7="_tmp%X",_tag_dyneither(_tmpAC7,sizeof(char),7))),_tag_dyneither(_tmpAC6,sizeof(void*),1)))))))),_tmpAC8)))),_tmpAC4)))));
return res;}
# 650
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp162;struct Cyc_Xarray_Xarray*_tmp163;struct Cyc_Toc_TocState _tmp161=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp162=_tmp161.dyn;_tmp163=_tmp161.temp_labels;{
struct _DynRegionFrame _tmp164;struct _RegionHandle*d=_open_dynregion(& _tmp164,_tmp162);
{int _tmp165=Cyc_Toc_fresh_label_counter ++;
if(_tmp165 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp163)){
struct _dyneither_ptr*_tmp166=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp163,_tmp165);_npop_handler(0);return _tmp166;}{
struct Cyc_Int_pa_PrintArg_struct _tmpAD0;void*_tmpACF[1];const char*_tmpACE;struct _dyneither_ptr*_tmpACD;struct _dyneither_ptr*res=(_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD[0]=(struct _dyneither_ptr)((_tmpAD0.tag=1,((_tmpAD0.f1=(unsigned int)_tmp165,((_tmpACF[0]=& _tmpAD0,Cyc_aprintf(((_tmpACE="_LL%X",_tag_dyneither(_tmpACE,sizeof(char),6))),_tag_dyneither(_tmpACF,sizeof(void*),1)))))))),_tmpACD)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp163,res)!= _tmp165){
const char*_tmpAD3;void*_tmpAD2;(_tmpAD2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD3="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpAD3,sizeof(char),43))),_tag_dyneither(_tmpAD2,sizeof(void*),0)));}{
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
struct _tuple8*_tmpAD4;return(_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->f1=_tmp170,((_tmpAD4->f2=_tmp171,((_tmpAD4->f3=Cyc_Toc_typ_to_c(_tmp172),_tmpAD4)))))));};}
# 687
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
struct Cyc_Absyn_Tqual _tmp175;void*_tmp176;struct _tuple10 _tmp174=*x;_tmp175=_tmp174.f1;_tmp176=_tmp174.f2;{
struct _tuple10*_tmpAD5;return(_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->f1=_tmp175,((_tmpAD5->f2=Cyc_Toc_typ_to_c(_tmp176),_tmpAD5)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpAD6;return(_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->name=f->name,((_tmpAD6->tq=Cyc_Toc_mt_tq,((_tmpAD6->type=
# 719
Cyc_Toc_typ_to_c(f->type),((_tmpAD6->width=f->width,((_tmpAD6->attributes=f->attributes,_tmpAD6)))))))))));}
# 723
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 725
return;}
# 728
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpAD7;cs=((_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAD7))));}
# 733
return*cs;}
# 735
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpAD8;r=((_tmpAD8=_cycalloc(sizeof(*_tmpAD8)),((_tmpAD8[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpAD8))));}
# 740
return*r;}
# 742
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpAD9;r=((_tmpAD9=_cycalloc(sizeof(*_tmpAD9)),((_tmpAD9[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAD9))));}
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
const char*_tmpADC;void*_tmpADB;(_tmpADB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADC="unresolved DatatypeFieldType",_tag_dyneither(_tmpADC,sizeof(char),29))),_tag_dyneither(_tmpADB,sizeof(void*),0)));}_LL6A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp189;if(_tmp194->tag != 5)goto _LL6C;else{_tmp195=(_tmp194->f1).elt_typ;_tmp196=(_tmp194->f1).elt_tq;_tmp197=((_tmp194->f1).ptr_atts).bounds;}}_LL6B:
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
{struct Cyc_List_List*_tmpADD;_tmp1D1=((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->hd=(void*)_tmp1A5->hd,((_tmpADD->tl=_tmp1D1,_tmpADD))))));}goto _LL9F;_LL9F:;}{
# 812
struct Cyc_List_List*_tmp1DA=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1A2);
if(_tmp1A4 != 0){
# 815
void*_tmp1DB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1A4->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpADE;struct _tuple8*_tmp1DC=(_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->f1=_tmp1A4->name,((_tmpADE->f2=_tmp1A4->tq,((_tmpADE->f3=_tmp1DB,_tmpADE)))))));
struct Cyc_List_List*_tmpADF;_tmp1DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1DA,((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->hd=_tmp1DC,((_tmpADF->tl=0,_tmpADF)))))));}{
# 819
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpAE5;struct Cyc_Absyn_FnInfo _tmpAE4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAE3;return(void*)((_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=((_tmpAE5.tag=9,((_tmpAE5.f1=((_tmpAE4.tvars=0,((_tmpAE4.effect=0,((_tmpAE4.ret_tqual=_tmp1A0,((_tmpAE4.ret_typ=Cyc_Toc_typ_to_c(_tmp1A1),((_tmpAE4.args=_tmp1DA,((_tmpAE4.c_varargs=_tmp1A3,((_tmpAE4.cyc_varargs=0,((_tmpAE4.rgn_po=0,((_tmpAE4.attributes=_tmp1D1,_tmpAE4)))))))))))))))))),_tmpAE5)))),_tmpAE3))));};};}_LL74: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp189;if(_tmp1A6->tag != 10)goto _LL76;else{_tmp1A7=_tmp1A6->f1;}}_LL75:
# 824
 _tmp1A7=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A7);
return Cyc_Toc_add_tuple_type(_tmp1A7);_LL76: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp189;if(_tmp1A8->tag != 12)goto _LL78;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f2;}}_LL77: {
# 829
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAE8;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE7;return(void*)((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE8.tag=12,((_tmpAE8.f1=_tmp1A9,((_tmpAE8.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1AA),_tmpAE8)))))),_tmpAE7))));}_LL78: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp189;if(_tmp1AB->tag != 11)goto _LL7A;else{_tmp1AC=(_tmp1AB->f1).aggr_info;_tmp1AD=(_tmp1AB->f1).targs;}}_LL79:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAEB;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAEA;return(void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEB.tag=17,((_tmpAEB.f1=_tmp1B3,((_tmpAEB.f2=0,((_tmpAEB.f3=_tmp1B5,((_tmpAEB.f4=0,_tmpAEB)))))))))),_tmpAEA))));}else{
return t;}}else{
# 866
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAEE;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAED;return(void*)((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=17,((_tmpAEE.f1=_tmp1B3,((_tmpAEE.f2=0,((_tmpAEE.f3=_tmp1B5,((_tmpAEE.f4=Cyc_Toc_typ_to_c(_tmp1B6),_tmpAEE)))))))))),_tmpAED))));}_LL80: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp189;if(_tmp1B7->tag != 19)goto _LL82;}_LL81:
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
const char*_tmpAF2;void*_tmpAF1[1];struct Cyc_String_pa_PrintArg_struct _tmpAF0;(_tmpAF0.tag=0,((_tmpAF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAF1[0]=& _tmpAF0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF2="atomic_typ:  bad type %s",_tag_dyneither(_tmpAF2,sizeof(char),25))),_tag_dyneither(_tmpAF1,sizeof(void*),1)))))));}_LLB8:;}
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
const char*_tmpAF5;void*_tmpAF4;(_tmpAF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF5="is_poly_field: type missing fields",_tag_dyneither(_tmpAF5,sizeof(char),35))),_tag_dyneither(_tmpAF4,sizeof(void*),0)));}
_tmp21C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21D->impl))->fields;goto _LLF0;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp218;if(_tmp21B->tag != 12)goto _LLF1;else{_tmp21C=_tmp21B->f2;}}_LLF0: {
# 982
struct Cyc_Absyn_Aggrfield*_tmp220=Cyc_Absyn_lookup_field(_tmp21C,f);
if(_tmp220 == 0){
const char*_tmpAF9;void*_tmpAF8[1];struct Cyc_String_pa_PrintArg_struct _tmpAF7;(_tmpAF7.tag=0,((_tmpAF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAF8[0]=& _tmpAF7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF9="is_poly_field: bad field %s",_tag_dyneither(_tmpAF9,sizeof(char),28))),_tag_dyneither(_tmpAF8,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp220->type);}_LLF1:;_LLF2: {
const char*_tmpAFD;void*_tmpAFC[1];struct Cyc_String_pa_PrintArg_struct _tmpAFB;(_tmpAFB.tag=0,((_tmpAFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAFC[0]=& _tmpAFB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFD="is_poly_field: bad type %s",_tag_dyneither(_tmpAFD,sizeof(char),27))),_tag_dyneither(_tmpAFC,sizeof(void*),1)))))));}_LLEC:;}
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
const char*_tmpB01;void*_tmpB00[1];struct Cyc_String_pa_PrintArg_struct _tmpAFF;(_tmpAFF.tag=0,((_tmpAFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22C->topt))),((_tmpB00[0]=& _tmpAFF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB01="is_poly_project: bad type %s",_tag_dyneither(_tmpB01,sizeof(char),29))),_tag_dyneither(_tmpB00,sizeof(void*),1)))))));}_LLFA:;}_LLF8:;_LLF9:
# 1004
 return 0;_LLF3:;}
# 1009
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB02;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->hd=s,((_tmpB02->tl=0,_tmpB02)))))),0);}
# 1013
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB03;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03->hd=s,((_tmpB03->tl=0,_tmpB03)))))),0);}
# 1017
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB04[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB04[1]=s,((_tmpB04[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB04,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB05[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB05[1]=n,((_tmpB05[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB05,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1031
struct Cyc_Absyn_Exp*_tmpB06[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB06[1]=n,((_tmpB06[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB06,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB07[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB07[2]=n,((_tmpB07[1]=s,((_tmpB07[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB07,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1039
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB08;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->hd=e,((_tmpB08->tl=0,_tmpB08)))))),0);}
# 1043
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB09[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB09,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB0F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB0E;struct Cyc_List_List*_tmpB0D;Cyc_Toc_result_decls=((_tmpB0D=_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D->hd=Cyc_Absyn_new_decl((void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB0E.tag=0,((_tmpB0E.f1=vd,_tmpB0E)))),_tmpB0F)))),0),((_tmpB0D->tl=Cyc_Toc_result_decls,_tmpB0D))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1073
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1075
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1077
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple16*_tmpB16;struct _tuple16*_tmpB15;struct _tuple16*_tmpB14;struct _tuple16*_tmpB13[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB13[2]=(
# 1080
(_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->f1=0,((_tmpB14->f2=xplussz,_tmpB14)))))),((_tmpB13[1]=(
# 1079
(_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->f1=0,((_tmpB15->f2=xexp,_tmpB15)))))),((_tmpB13[0]=(
# 1078
(_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->f1=0,((_tmpB16->f2=xexp,_tmpB16)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB13,sizeof(struct _tuple16*),3)))))))),0);}
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
int*_tmpB1E;struct Cyc_Absyn_Exp**_tmpB1D;int*_tmpB1C;struct Cyc_Toc_Env*_tmpB1B;return(_tmpB1B=_region_malloc(r,sizeof(*_tmpB1B)),((_tmpB1B->break_lab=(struct _dyneither_ptr**)0,((_tmpB1B->continue_lab=(struct _dyneither_ptr**)0,((_tmpB1B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB1B->varmap=(struct Cyc_Dict_Dict)
# 1145
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB1B->toplevel=(int)1,((_tmpB1B->in_lhs=(int*)(
# 1147
(_tmpB1C=_region_malloc(r,sizeof(*_tmpB1C)),((_tmpB1C[0]=0,_tmpB1C)))),((_tmpB1B->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB1B->struct_info).varsizeexp=((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=0,_tmpB1D)))),(((_tmpB1B->struct_info).lhs_exp=0,_tmpB1B->struct_info)))),((_tmpB1B->in_sizeof=(int*)(
(_tmpB1E=_region_malloc(r,sizeof(*_tmpB1E)),((_tmpB1E[0]=0,_tmpB1E)))),((_tmpB1B->rgn=(struct _RegionHandle*)r,_tmpB1B)))))))))))))))))));}
# 1153
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;int*_tmp256;struct Cyc_Toc_StructInfo _tmp257;int*_tmp258;struct Cyc_Toc_Env*_tmp250=e;_tmp251=_tmp250->break_lab;_tmp252=_tmp250->continue_lab;_tmp253=_tmp250->fallthru_info;_tmp254=_tmp250->varmap;_tmp255=_tmp250->toplevel;_tmp256=_tmp250->in_lhs;_tmp257=_tmp250->struct_info;_tmp258=_tmp250->in_sizeof;{
struct Cyc_Toc_Env*_tmpB1F;return(_tmpB1F=_region_malloc(r,sizeof(*_tmpB1F)),((_tmpB1F->break_lab=(struct _dyneither_ptr**)_tmp251,((_tmpB1F->continue_lab=(struct _dyneither_ptr**)_tmp252,((_tmpB1F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp253,((_tmpB1F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254),((_tmpB1F->toplevel=(int)_tmp255,((_tmpB1F->in_lhs=(int*)_tmp256,((_tmpB1F->struct_info=(struct Cyc_Toc_StructInfo)_tmp257,((_tmpB1F->in_sizeof=(int*)_tmp258,((_tmpB1F->rgn=(struct _RegionHandle*)r,_tmpB1F)))))))))))))))))));};}
# 1158
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp25B;struct _dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict _tmp25E;int _tmp25F;int*_tmp260;struct Cyc_Toc_StructInfo _tmp261;int*_tmp262;struct Cyc_Toc_Env*_tmp25A=e;_tmp25B=_tmp25A->break_lab;_tmp25C=_tmp25A->continue_lab;_tmp25D=_tmp25A->fallthru_info;_tmp25E=_tmp25A->varmap;_tmp25F=_tmp25A->toplevel;_tmp260=_tmp25A->in_lhs;_tmp261=_tmp25A->struct_info;_tmp262=_tmp25A->in_sizeof;{
struct Cyc_Toc_Env*_tmpB20;return(_tmpB20=_region_malloc(r,sizeof(*_tmpB20)),((_tmpB20->break_lab=(struct _dyneither_ptr**)_tmp25B,((_tmpB20->continue_lab=(struct _dyneither_ptr**)_tmp25C,((_tmpB20->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp25D,((_tmpB20->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25E),((_tmpB20->toplevel=(int)0,((_tmpB20->in_lhs=(int*)_tmp260,((_tmpB20->struct_info=(struct Cyc_Toc_StructInfo)_tmp261,((_tmpB20->in_sizeof=(int*)_tmp262,((_tmpB20->rgn=(struct _RegionHandle*)r,_tmpB20)))))))))))))))))));};}
# 1162
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp265;struct _dyneither_ptr**_tmp266;struct Cyc_Toc_FallthruInfo*_tmp267;struct Cyc_Dict_Dict _tmp268;int _tmp269;int*_tmp26A;struct Cyc_Toc_StructInfo _tmp26B;int*_tmp26C;struct Cyc_Toc_Env*_tmp264=e;_tmp265=_tmp264->break_lab;_tmp266=_tmp264->continue_lab;_tmp267=_tmp264->fallthru_info;_tmp268=_tmp264->varmap;_tmp269=_tmp264->toplevel;_tmp26A=_tmp264->in_lhs;_tmp26B=_tmp264->struct_info;_tmp26C=_tmp264->in_sizeof;{
struct Cyc_Toc_Env*_tmpB21;return(_tmpB21=_region_malloc(r,sizeof(*_tmpB21)),((_tmpB21->break_lab=(struct _dyneither_ptr**)_tmp265,((_tmpB21->continue_lab=(struct _dyneither_ptr**)_tmp266,((_tmpB21->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp267,((_tmpB21->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp268),((_tmpB21->toplevel=(int)1,((_tmpB21->in_lhs=(int*)_tmp26A,((_tmpB21->struct_info=(struct Cyc_Toc_StructInfo)_tmp26B,((_tmpB21->in_sizeof=(int*)_tmp26C,((_tmpB21->rgn=(struct _RegionHandle*)r,_tmpB21)))))))))))))))))));};}
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
struct Cyc_Toc_Env*_tmpB22;return(_tmpB22=_region_malloc(r,sizeof(*_tmpB22)),((_tmpB22->break_lab=(struct _dyneither_ptr**)_tmp276,((_tmpB22->continue_lab=(struct _dyneither_ptr**)_tmp277,((_tmpB22->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp278,((_tmpB22->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp279),((_tmpB22->toplevel=(int)_tmp27A,((_tmpB22->in_lhs=(int*)_tmp27B,((_tmpB22->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB22->struct_info).varsizeexp=_tmp27C,(((_tmpB22->struct_info).lhs_exp=exp,_tmpB22->struct_info)))),((_tmpB22->in_sizeof=(int*)_tmp27D,((_tmpB22->rgn=(struct _RegionHandle*)r,_tmpB22)))))))))))))))))));};}
# 1189
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp27F=(*x).f1;_LL107: if((_tmp27F.Rel_n).tag != 1)goto _LL109;_LL108: {
# 1192
const char*_tmpB26;void*_tmpB25[1];struct Cyc_String_pa_PrintArg_struct _tmpB24;(_tmpB24.tag=0,((_tmpB24.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB25[0]=& _tmpB24,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB26="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB26,sizeof(char),30))),_tag_dyneither(_tmpB25,sizeof(void*),1)))))));}_LL109:;_LL10A:
 goto _LL106;_LL106:;}{
# 1195
struct _dyneither_ptr**_tmp284;struct _dyneither_ptr**_tmp285;struct Cyc_Toc_FallthruInfo*_tmp286;struct Cyc_Dict_Dict _tmp287;int _tmp288;int*_tmp289;struct Cyc_Toc_StructInfo _tmp28A;int*_tmp28B;struct Cyc_Toc_Env*_tmp283=e;_tmp284=_tmp283->break_lab;_tmp285=_tmp283->continue_lab;_tmp286=_tmp283->fallthru_info;_tmp287=_tmp283->varmap;_tmp288=_tmp283->toplevel;_tmp289=_tmp283->in_lhs;_tmp28A=_tmp283->struct_info;_tmp28B=_tmp283->in_sizeof;{
struct Cyc_Dict_Dict _tmp28C=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp287),x,y);
struct Cyc_Toc_Env*_tmpB27;return(_tmpB27=_region_malloc(r,sizeof(*_tmpB27)),((_tmpB27->break_lab=(struct _dyneither_ptr**)_tmp284,((_tmpB27->continue_lab=(struct _dyneither_ptr**)_tmp285,((_tmpB27->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp286,((_tmpB27->varmap=(struct Cyc_Dict_Dict)_tmp28C,((_tmpB27->toplevel=(int)_tmp288,((_tmpB27->in_lhs=(int*)_tmp289,((_tmpB27->struct_info=(struct Cyc_Toc_StructInfo)_tmp28A,((_tmpB27->in_sizeof=(int*)_tmp28B,((_tmpB27->rgn=(struct _RegionHandle*)r,_tmpB27)))))))))))))))))));};};}
# 1202
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28F;struct _dyneither_ptr**_tmp290;struct Cyc_Toc_FallthruInfo*_tmp291;struct Cyc_Dict_Dict _tmp292;int _tmp293;int*_tmp294;struct Cyc_Toc_StructInfo _tmp295;int*_tmp296;struct Cyc_Toc_Env*_tmp28E=e;_tmp28F=_tmp28E->break_lab;_tmp290=_tmp28E->continue_lab;_tmp291=_tmp28E->fallthru_info;_tmp292=_tmp28E->varmap;_tmp293=_tmp28E->toplevel;_tmp294=_tmp28E->in_lhs;_tmp295=_tmp28E->struct_info;_tmp296=_tmp28E->in_sizeof;{
struct Cyc_Toc_Env*_tmpB28;return(_tmpB28=_region_malloc(r,sizeof(*_tmpB28)),((_tmpB28->break_lab=(struct _dyneither_ptr**)0,((_tmpB28->continue_lab=(struct _dyneither_ptr**)0,((_tmpB28->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp291,((_tmpB28->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp292),((_tmpB28->toplevel=(int)_tmp293,((_tmpB28->in_lhs=(int*)_tmp294,((_tmpB28->struct_info=(struct Cyc_Toc_StructInfo)_tmp295,((_tmpB28->in_sizeof=(int*)_tmp296,((_tmpB28->rgn=(struct _RegionHandle*)r,_tmpB28)))))))))))))))))));};}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1213
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB29;fallthru_vars=((_tmpB29=_region_malloc(r,sizeof(*_tmpB29)),((_tmpB29->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB29->tl=fallthru_vars,_tmpB29))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp29A;struct _dyneither_ptr**_tmp29B;struct Cyc_Toc_FallthruInfo*_tmp29C;struct Cyc_Dict_Dict _tmp29D;int _tmp29E;int*_tmp29F;struct Cyc_Toc_StructInfo _tmp2A0;int*_tmp2A1;struct Cyc_Toc_Env*_tmp299=e;_tmp29A=_tmp299->break_lab;_tmp29B=_tmp299->continue_lab;_tmp29C=_tmp299->fallthru_info;_tmp29D=_tmp299->varmap;_tmp29E=_tmp299->toplevel;_tmp29F=_tmp299->in_lhs;_tmp2A0=_tmp299->struct_info;_tmp2A1=_tmp299->in_sizeof;{
struct Cyc_Dict_Dict _tmp2A3;struct Cyc_Toc_Env*_tmp2A2=next_case_env;_tmp2A3=_tmp2A2->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB2A;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB2A=_region_malloc(r,sizeof(*_tmpB2A)),((_tmpB2A->label=fallthru_l,((_tmpB2A->binders=fallthru_vars,((_tmpB2A->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2A3),_tmpB2A)))))));
struct _dyneither_ptr**_tmpB2D;struct Cyc_Toc_Env*_tmpB2C;return(_tmpB2C=_region_malloc(r,sizeof(*_tmpB2C)),((_tmpB2C->break_lab=(struct _dyneither_ptr**)((_tmpB2D=_region_malloc(r,sizeof(*_tmpB2D)),((_tmpB2D[0]=break_l,_tmpB2D)))),((_tmpB2C->continue_lab=(struct _dyneither_ptr**)_tmp29B,((_tmpB2C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB2C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29D),((_tmpB2C->toplevel=(int)_tmp29E,((_tmpB2C->in_lhs=(int*)_tmp29F,((_tmpB2C->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A0,((_tmpB2C->in_sizeof=(int*)_tmp2A1,((_tmpB2C->rgn=(struct _RegionHandle*)r,_tmpB2C)))))))))))))))))));};};};}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1228
struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_StructInfo _tmp2AE;int*_tmp2AF;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->break_lab;_tmp2A9=_tmp2A7->continue_lab;_tmp2AA=_tmp2A7->fallthru_info;_tmp2AB=_tmp2A7->varmap;_tmp2AC=_tmp2A7->toplevel;_tmp2AD=_tmp2A7->in_lhs;_tmp2AE=_tmp2A7->struct_info;_tmp2AF=_tmp2A7->in_sizeof;{
struct _dyneither_ptr**_tmpB30;struct Cyc_Toc_Env*_tmpB2F;return(_tmpB2F=_region_malloc(r,sizeof(*_tmpB2F)),((_tmpB2F->break_lab=(struct _dyneither_ptr**)((_tmpB30=_region_malloc(r,sizeof(*_tmpB30)),((_tmpB30[0]=break_l,_tmpB30)))),((_tmpB2F->continue_lab=(struct _dyneither_ptr**)_tmp2A9,((_tmpB2F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB2F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),((_tmpB2F->toplevel=(int)_tmp2AC,((_tmpB2F->in_lhs=(int*)_tmp2AD,((_tmpB2F->struct_info=(struct Cyc_Toc_StructInfo)_tmp2AE,((_tmpB2F->in_sizeof=(int*)_tmp2AF,((_tmpB2F->rgn=(struct _RegionHandle*)r,_tmpB2F)))))))))))))))))));};}
# 1235
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1238
struct _dyneither_ptr**_tmp2B3;struct _dyneither_ptr**_tmp2B4;struct Cyc_Toc_FallthruInfo*_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;int _tmp2B7;int*_tmp2B8;struct Cyc_Toc_StructInfo _tmp2B9;int*_tmp2BA;struct Cyc_Toc_Env*_tmp2B2=e;_tmp2B3=_tmp2B2->break_lab;_tmp2B4=_tmp2B2->continue_lab;_tmp2B5=_tmp2B2->fallthru_info;_tmp2B6=_tmp2B2->varmap;_tmp2B7=_tmp2B2->toplevel;_tmp2B8=_tmp2B2->in_lhs;_tmp2B9=_tmp2B2->struct_info;_tmp2BA=_tmp2B2->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB33;struct Cyc_Toc_Env*_tmpB32;return(_tmpB32=_region_malloc(r,sizeof(*_tmpB32)),((_tmpB32->break_lab=(struct _dyneither_ptr**)0,((_tmpB32->continue_lab=(struct _dyneither_ptr**)_tmp2B4,((_tmpB32->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB33=_region_malloc(r,sizeof(*_tmpB33)),((_tmpB33->label=next_l,((_tmpB33->binders=0,((_tmpB33->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB33)))))))),((_tmpB32->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B6),((_tmpB32->toplevel=(int)_tmp2B7,((_tmpB32->in_lhs=(int*)_tmp2B8,((_tmpB32->struct_info=(struct Cyc_Toc_StructInfo)_tmp2B9,((_tmpB32->in_sizeof=(int*)_tmp2BA,((_tmpB32->rgn=(struct _RegionHandle*)r,_tmpB32)))))))))))))))))));};}
# 1253 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1256
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
void*_tmp2BD=e->annot;_LL10C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2BE=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BE->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10E;}_LL10D:
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL10E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2BF=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2BF->tag != Cyc_CfFlowInfo_NotZero)goto _LL110;}_LL10F:
 return 0;_LL110: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2C0=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C0->tag != Cyc_CfFlowInfo_IsZero)goto _LL112;}_LL111:
# 1261
{const char*_tmpB36;void*_tmpB35;(_tmpB35=0,Cyc_Tcutil_terr(e->loc,((_tmpB36="dereference of NULL pointer",_tag_dyneither(_tmpB36,sizeof(char),28))),_tag_dyneither(_tmpB35,sizeof(void*),0)));}
return 0;_LL112: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2C1=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2BD;if(_tmp2C1->tag != Cyc_Absyn_EmptyAnnot)goto _LL114;}_LL113:
# 1266
 return 1;_LL114:;_LL115: {
const char*_tmpB39;void*_tmpB38;(_tmpB38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB39="need_null_check",_tag_dyneither(_tmpB39,sizeof(char),16))),_tag_dyneither(_tmpB38,sizeof(void*),0)));}_LL10B:;}
# 1271
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2C6=e->annot;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2CA;_LL117: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2C7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL119;else{_tmp2C8=_tmp2C7->f1;}}_LL118:
 return _tmp2C8;_LL119: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2C9->tag != Cyc_CfFlowInfo_NotZero)goto _LL11B;else{_tmp2CA=_tmp2C9->f1;}}_LL11A:
 return _tmp2CA;_LL11B: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2CB=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CB->tag != Cyc_CfFlowInfo_IsZero)goto _LL11D;}_LL11C:
# 1276
{const char*_tmpB3C;void*_tmpB3B;(_tmpB3B=0,Cyc_Tcutil_terr(e->loc,((_tmpB3C="dereference of NULL pointer",_tag_dyneither(_tmpB3C,sizeof(char),28))),_tag_dyneither(_tmpB3B,sizeof(void*),0)));}
return 0;_LL11D: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2CC=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2C6;if(_tmp2CC->tag != Cyc_Absyn_EmptyAnnot)goto _LL11F;}_LL11E:
 return 0;_LL11F:;_LL120: {
const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="get_relns",_tag_dyneither(_tmpB3F,sizeof(char),10))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}_LL116:;}
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
{struct _tuple19 _tmpB40;struct _tuple19 _tmp338=(_tmpB40.f1=Cyc_Tcutil_compress(_tmp334),((_tmpB40.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpB40)));void*_tmp33A;union Cyc_Absyn_Constraint*_tmp33C;_LL17E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp338.f1;if(_tmp339->tag != 19)goto _LL180;else{_tmp33A=(void*)_tmp339->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp338.f2;if(_tmp33B->tag != 5)goto _LL180;else{_tmp33C=((_tmp33B->f1).ptr_atts).bounds;}};_LL17F:
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
if(e->topt == 0){const char*_tmpB43;void*_tmpB42;(_tmpB42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="Missing type in primop ",_tag_dyneither(_tmpB43,sizeof(char),24))),_tag_dyneither(_tmpB42,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1484
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB46;void*_tmpB45;(_tmpB45=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB46="Missing type in primop ",_tag_dyneither(_tmpB46,sizeof(char),24))),_tag_dyneither(_tmpB45,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1488
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple10*_tmpB47;return(_tmpB47=_cycalloc(sizeof(*_tmpB47)),((_tmpB47->f1=Cyc_Toc_mt_tq,((_tmpB47->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB47)))));}
# 1491
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple16*_tmpB48;return(_tmpB48=_cycalloc(sizeof(*_tmpB48)),((_tmpB48->f1=0,((_tmpB48->f2=e,_tmpB48)))));};}
# 1498
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1501
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp34F=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp34F=Cyc_Absyn_add_exp(_tmp34F,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1509
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp34F): Cyc_Toc_malloc_ptr(_tmp34F);else{
# 1513
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp34F);}}else{
# 1518
t=struct_typ;
eo=0;}
# 1521
if(do_declare){
void*_tmp350=x->r;struct _tuple0*_tmp352;_LL195: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp351->tag != 1)goto _LL197;else{_tmp352=_tmp351->f1;}}_LL196:
# 1524
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp352,t,eo,s,0),0);_LL197:;_LL198: {
# 1526
const char*_tmpB4B;void*_tmpB4A;(_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="bogus x to make_struct",_tag_dyneither(_tmpB4B,sizeof(char),23))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}_LL194:;}else{
# 1530
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1533
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1537
static int Cyc_Toc_array_or_aggr_with_array(void*typ){
void*_tmp355=Cyc_Tcutil_compress(typ);union Cyc_Absyn_AggrInfoU _tmp358;struct Cyc_List_List*_tmp359;struct Cyc_List_List*_tmp35B;_LL19A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp356=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp355;if(_tmp356->tag != 8)goto _LL19C;}_LL19B:
 return 1;_LL19C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp355;if(_tmp357->tag != 11)goto _LL19E;else{_tmp358=(_tmp357->f1).aggr_info;_tmp359=(_tmp357->f1).targs;}}_LL19D: {
# 1541
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp358);
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp35B=aggrfields;goto _LL19F;}_LL19E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp35A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp355;if(_tmp35A->tag != 12)goto _LL1A0;else{_tmp35B=_tmp35A->f2;}}_LL19F:
# 1545
 if(_tmp35B == 0)return 0;{
struct Cyc_List_List*fields;
for(fields=_tmp35B;fields->tl != 0;fields=fields->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp35C=(struct Cyc_Absyn_Aggrfield*)fields->hd;
return Cyc_Toc_array_or_aggr_with_array(_tmp35C->type);};};_LL1A0:;_LL1A1:
# 1551
 return 0;_LL199:;}
# 1555
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp35F;_LL1A3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35D;if(_tmp35E->tag != 8)goto _LL1A5;else{_tmp35F=(_tmp35E->f1).elt_type;}}_LL1A4:
 return _tmp35F;_LL1A5:;_LL1A6: {
const char*_tmpB4F;void*_tmpB4E[1];struct Cyc_String_pa_PrintArg_struct _tmpB4D;(_tmpB4D.tag=0,((_tmpB4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB4E[0]=& _tmpB4D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4F="array_elt_type called on %s",_tag_dyneither(_tmpB4F,sizeof(char),28))),_tag_dyneither(_tmpB4E,sizeof(void*),1)))))));}_LL1A2:;}
# 1562
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1568
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1573
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1578
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp363=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp363 != 0;_tmp363=_tmp363->tl){
# 1582
struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct _tuple16*_tmp364=(struct _tuple16*)_tmp363->hd;_tmp365=_tmp364->f1;_tmp366=_tmp364->f2;{
# 1586
struct Cyc_Absyn_Exp*e_index;
if(_tmp365 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1590
if(_tmp365->tl != 0){const char*_tmpB52;void*_tmpB51;(_tmpB51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB52="multiple designators in array",_tag_dyneither(_tmpB52,sizeof(char),30))),_tag_dyneither(_tmpB51,sizeof(void*),0)));}{
void*_tmp369=(void*)_tmp365->hd;
void*_tmp36A=_tmp369;struct Cyc_Absyn_Exp*_tmp36C;_LL1A8: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp36B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp36A;if(_tmp36B->tag != 0)goto _LL1AA;else{_tmp36C=_tmp36B->f1;}}_LL1A9:
# 1594
 Cyc_Toc_exp_to_c(nv,_tmp36C);
e_index=_tmp36C;
goto _LL1A7;_LL1AA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp36D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp36A;if(_tmp36D->tag != 1)goto _LL1A7;}_LL1AB: {
const char*_tmpB55;void*_tmpB54;(_tmpB54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB55="field name designators in array",_tag_dyneither(_tmpB55,sizeof(char),32))),_tag_dyneither(_tmpB54,sizeof(void*),0)));}_LL1A7:;};}{
# 1600
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp370=_tmp366->r;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Vardecl*_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*_tmp376;int _tmp377;void*_tmp379;struct Cyc_List_List*_tmp37A;_LL1AD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp371=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp371->tag != 25)goto _LL1AF;else{_tmp372=_tmp371->f1;}}_LL1AE:
# 1603
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp372,s);
goto _LL1AC;_LL1AF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp373->tag != 26)goto _LL1B1;else{_tmp374=_tmp373->f1;_tmp375=_tmp373->f2;_tmp376=_tmp373->f3;_tmp377=_tmp373->f4;}}_LL1B0:
# 1606
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp374,_tmp375,_tmp376,_tmp377,s,0);
# 1608
goto _LL1AC;_LL1B1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp378->tag != 29)goto _LL1B3;else{_tmp379=(void*)_tmp378->f1;_tmp37A=_tmp378->f2;}}_LL1B2:
# 1610
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp379,_tmp37A,s);
goto _LL1AC;_LL1B3: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp37B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp370;if(_tmp37B->tag != 27)goto _LL1B5;}_LL1B4:
# 1613
 goto _LL1AC;_LL1B5:;_LL1B6:
# 1615
 Cyc_Toc_exp_to_c(nv,_tmp366);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp366),0),s,0);
goto _LL1AC;_LL1AC:;};};}}
# 1621
return s;}
# 1626
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1631
struct _tuple0*_tmp37C=vd->name;
void*expected_elt_type;
{void*_tmp37D=Cyc_Tcutil_compress(comprehension_type);void*_tmp37F;void*_tmp381;_LL1B8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37D;if(_tmp37E->tag != 8)goto _LL1BA;else{_tmp37F=(_tmp37E->f1).elt_type;}}_LL1B9:
# 1635
 expected_elt_type=_tmp37F;goto _LL1B7;_LL1BA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37D;if(_tmp380->tag != 5)goto _LL1BC;else{_tmp381=(_tmp380->f1).elt_typ;}}_LL1BB:
# 1637
 expected_elt_type=_tmp381;goto _LL1B7;_LL1BC:;_LL1BD: {
const char*_tmpB58;void*_tmpB57;(_tmpB57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB58="init_comprehension passed a bad type!",_tag_dyneither(_tmpB58,sizeof(char),38))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}_LL1B7:;}{
# 1640
void*_tmp384=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1644
{void*_tmp385=e2->r;_LL1BF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp386=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp385;if(_tmp386->tag != 27)goto _LL1C1;}_LL1C0:
 return Cyc_Absyn_skip_stmt(0);_LL1C1:;_LL1C2:
 goto _LL1BE;_LL1BE:;}{
# 1648
struct _RegionHandle _tmp387=_new_region("r2");struct _RegionHandle*r2=& _tmp387;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB5B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB5A;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp37C,Cyc_Absyn_varb_exp(_tmp37C,(void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A[0]=((_tmpB5B.tag=4,((_tmpB5B.f1=vd,_tmpB5B)))),_tmpB5A)))),0));
struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp37C,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp37C,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp37C,0),0);
# 1656
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp37C,0),0);
struct Cyc_Absyn_Stmt*body;
# 1660
{void*_tmp388=e2->r;struct Cyc_List_List*_tmp38A;struct Cyc_Absyn_Vardecl*_tmp38C;struct Cyc_Absyn_Exp*_tmp38D;struct Cyc_Absyn_Exp*_tmp38E;int _tmp38F;void*_tmp391;struct Cyc_List_List*_tmp392;_LL1C4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp389=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp388;if(_tmp389->tag != 25)goto _LL1C6;else{_tmp38A=_tmp389->f1;}}_LL1C5:
# 1662
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp38A,Cyc_Toc_skip_stmt_dl());
goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp38B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp388;if(_tmp38B->tag != 26)goto _LL1C8;else{_tmp38C=_tmp38B->f1;_tmp38D=_tmp38B->f2;_tmp38E=_tmp38B->f3;_tmp38F=_tmp38B->f4;}}_LL1C7:
# 1665
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp38C,_tmp38D,_tmp38E,_tmp38F,Cyc_Toc_skip_stmt_dl(),0);
goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp390=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp388;if(_tmp390->tag != 29)goto _LL1CA;else{_tmp391=(void*)_tmp390->f1;_tmp392=_tmp390->f2;}}_LL1C9:
# 1668
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp391,_tmp392,Cyc_Toc_skip_stmt_dl());
goto _LL1C3;_LL1CA:;_LL1CB:
# 1671
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL1C3;_LL1C3:;}{
# 1675
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1677
if(zero_term){
# 1682
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp384,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1687
struct Cyc_Absyn_Stmt*_tmp393=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp37C,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp393;};};}
# 1649
;_pop_region(r2);};};}
# 1693
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1696
{struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp396 != 0;_tmp396=_tmp396->tl){
struct Cyc_List_List*_tmp398;struct Cyc_Absyn_Exp*_tmp399;struct _tuple16*_tmp397=(struct _tuple16*)_tmp396->hd;_tmp398=_tmp397->f1;_tmp399=_tmp397->f2;
if(_tmp398 == 0){
const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="empty designator list",_tag_dyneither(_tmpB5E,sizeof(char),22))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}
if(_tmp398->tl != 0){
const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="too many designators in anonymous struct",_tag_dyneither(_tmpB61,sizeof(char),41))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}{
void*_tmp39E=(void*)_tmp398->hd;struct _dyneither_ptr*_tmp3A0;_LL1CD: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp39F=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp39E;if(_tmp39F->tag != 1)goto _LL1CF;else{_tmp3A0=_tmp39F->f1;}}_LL1CE: {
# 1704
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp3A0,0);
{void*_tmp3A1=_tmp399->r;struct Cyc_List_List*_tmp3A3;struct Cyc_Absyn_Vardecl*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A7;int _tmp3A8;void*_tmp3AA;struct Cyc_List_List*_tmp3AB;_LL1D2: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3A2=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3A1;if(_tmp3A2->tag != 25)goto _LL1D4;else{_tmp3A3=_tmp3A2->f1;}}_LL1D3:
# 1707
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp399->topt),lval,_tmp3A3,s);goto _LL1D1;_LL1D4: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3A4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3A1;if(_tmp3A4->tag != 26)goto _LL1D6;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;_tmp3A7=_tmp3A4->f3;_tmp3A8=_tmp3A4->f4;}}_LL1D5:
# 1709
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp399->topt),lval,_tmp3A5,_tmp3A6,_tmp3A7,_tmp3A8,s,0);
goto _LL1D1;_LL1D6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3A9=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3A1;if(_tmp3A9->tag != 29)goto _LL1D8;else{_tmp3AA=(void*)_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;}}_LL1D7:
# 1712
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3AA,_tmp3AB,s);goto _LL1D1;_LL1D8: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3AC=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3A1;if(_tmp3AC->tag != 27)goto _LL1DA;}_LL1D9:
# 1714
 goto _LL1D1;_LL1DA:;_LL1DB:
# 1716
 Cyc_Toc_exp_to_c(nv,_tmp399);
# 1718
if(Cyc_Toc_is_poly_field(struct_type,_tmp3A0))
_tmp399=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp399);
# 1721
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp399,0),0),s,0);
goto _LL1D1;_LL1D1:;}
# 1724
goto _LL1CC;}_LL1CF:;_LL1D0: {
const char*_tmpB64;void*_tmpB63;(_tmpB63=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB64="array designator in struct",_tag_dyneither(_tmpB64,sizeof(char),27))),_tag_dyneither(_tmpB63,sizeof(void*),0)));}_LL1CC:;};}}
# 1728
return s;}
# 1733
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1737
struct _RegionHandle _tmp3AF=_new_region("r");struct _RegionHandle*r=& _tmp3AF;_push_region(r);
{struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp3B1=Cyc_Toc_add_tuple_type(_tmp3B0);
# 1741
struct _tuple0*_tmp3B2=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3B3=Cyc_Absyn_var_exp(_tmp3B2,0);
struct Cyc_Absyn_Stmt*_tmp3B4=Cyc_Absyn_exp_stmt(_tmp3B3,0);
# 1745
struct Cyc_Absyn_Exp*(*_tmp3B5)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1747
int is_atomic=1;
struct Cyc_List_List*_tmp3B6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3B6);for(0;_tmp3B6 != 0;(_tmp3B6=_tmp3B6->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3B6->hd;
struct Cyc_Absyn_Exp*lval=_tmp3B5(_tmp3B3,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp3B7=e->r;struct Cyc_List_List*_tmp3B9;struct Cyc_Absyn_Vardecl*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;int _tmp3BE;_LL1DD: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3B8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3B8->tag != 25)goto _LL1DF;else{_tmp3B9=_tmp3B8->f1;}}_LL1DE:
# 1755
 _tmp3B4=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp3B9,_tmp3B4);
goto _LL1DC;_LL1DF: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3BA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3BA->tag != 26)goto _LL1E1;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BA->f2;_tmp3BD=_tmp3BA->f3;_tmp3BE=_tmp3BA->f4;}}_LL1E0:
# 1758
 _tmp3B4=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp3BB,_tmp3BC,_tmp3BD,_tmp3BE,_tmp3B4,0);
# 1760
goto _LL1DC;_LL1E1: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3BF=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3B7;if(_tmp3BF->tag != 27)goto _LL1E3;}_LL1E2:
# 1762
 goto _LL1DC;_LL1E3:;_LL1E4:
# 1765
 Cyc_Toc_exp_to_c(nv,e);
_tmp3B4=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B5(_tmp3B3,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3B4,0);
# 1768
goto _LL1DC;_LL1DC:;};}}{
# 1771
struct Cyc_Absyn_Exp*_tmp3C0=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp3B2,0),_tmp3B1,_tmp3B4,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp3C0;};}
# 1738
;_pop_region(r);}
# 1775
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp3C1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3C1 != 0;_tmp3C1=_tmp3C1->tl){
struct Cyc_Absyn_Aggrfield*_tmp3C2=(struct Cyc_Absyn_Aggrfield*)_tmp3C1->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3C2->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1782
struct Cyc_String_pa_PrintArg_struct _tmpB6C;void*_tmpB6B[1];const char*_tmpB6A;void*_tmpB69;(_tmpB69=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB6C.tag=0,((_tmpB6C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB6B[0]=& _tmpB6C,Cyc_aprintf(((_tmpB6A="get_member_offset %s failed",_tag_dyneither(_tmpB6A,sizeof(char),28))),_tag_dyneither(_tmpB6B,sizeof(void*),1)))))))),_tag_dyneither(_tmpB69,sizeof(void*),0)));};}
# 1785
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3C7=Cyc_Tcutil_compress(*typ);void*_tmp3C9;struct Cyc_Absyn_Tqual _tmp3CA;struct Cyc_Absyn_Exp*_tmp3CB;union Cyc_Absyn_Constraint*_tmp3CC;unsigned int _tmp3CD;_LL1E6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3C8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C7;if(_tmp3C8->tag != 8)goto _LL1E8;else{_tmp3C9=(_tmp3C8->f1).elt_type;_tmp3CA=(_tmp3C8->f1).tq;_tmp3CB=(_tmp3C8->f1).num_elts;_tmp3CC=(_tmp3C8->f1).zero_term;_tmp3CD=(_tmp3C8->f1).zt_loc;}}_LL1E7:
# 1790
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3CB))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB72;struct Cyc_Absyn_ArrayInfo _tmpB71;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB70;*typ=(void*)((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70[0]=((_tmpB72.tag=8,((_tmpB72.f1=((_tmpB71.elt_type=_tmp3C9,((_tmpB71.tq=_tmp3CA,((_tmpB71.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB71.zero_term=_tmp3CC,((_tmpB71.zt_loc=_tmp3CD,_tmpB71)))))))))),_tmpB72)))),_tmpB70))));}
return 1;}
# 1794
goto _LL1E5;_LL1E8:;_LL1E9:
 goto _LL1E5;_LL1E5:;}
# 1797
return 0;}
# 1800
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1802
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1806
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3D1=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_Absyn_Aggrfield*_tmpB73;struct Cyc_Absyn_Aggrfield*_tmp3D2=(_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->name=_tmp3D1->name,((_tmpB73->tq=Cyc_Toc_mt_tq,((_tmpB73->type=new_field_type,((_tmpB73->width=_tmp3D1->width,((_tmpB73->attributes=_tmp3D1->attributes,_tmpB73)))))))))));
# 1813
struct Cyc_List_List*_tmpB74;new_fields=((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->hd=_tmp3D2,((_tmpB74->tl=new_fields,_tmpB74))))));}else{
# 1815
struct Cyc_List_List*_tmpB75;new_fields=((_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB75->tl=new_fields,_tmpB75))))));}}{
# 1817
struct Cyc_List_List*_tmp3D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB7D;void*_tmpB7C[1];const char*_tmpB7B;struct _dyneither_ptr*_tmpB7A;struct _dyneither_ptr*name=
(_tmpB7A=_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A[0]=(struct _dyneither_ptr)((_tmpB7D.tag=1,((_tmpB7D.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB7C[0]=& _tmpB7D,Cyc_aprintf(((_tmpB7B="_genStruct%d",_tag_dyneither(_tmpB7B,sizeof(char),13))),_tag_dyneither(_tmpB7C,sizeof(void*),1)))))))),_tmpB7A)));
struct _tuple0*_tmpB7E;struct _tuple0*qv=(_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB7E->f2=name,_tmpB7E)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB7F;struct Cyc_Absyn_AggrdeclImpl*_tmp3D7=(_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpB7F->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpB7F->fields=_tmp3D6,((_tmpB7F->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpB7F)))))))));
# 1825
struct Cyc_Absyn_Aggrdecl*_tmpB80;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80->kind=ad->kind,((_tmpB80->sc=ad->sc,((_tmpB80->name=qv,((_tmpB80->tvs=ad->tvs,((_tmpB80->impl=_tmp3D7,((_tmpB80->attributes=ad->attributes,_tmpB80)))))))))))));
# 1831
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1835
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1841
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple0*_tmp3DF=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3DF,0);
do_declare=1;}else{
# 1849
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1852
struct Cyc_Absyn_Stmt*_tmp3E0=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1856
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3E1=_tmp3E0->r;struct Cyc_Absyn_Exp**_tmp3E3;_LL1EB: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3E2=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3E1;if(_tmp3E2->tag != 1)goto _LL1ED;else{_tmp3E3=(struct Cyc_Absyn_Exp**)& _tmp3E2->f1;}}_LL1EC:
 varexp=_tmp3E3;goto _LL1EA;_LL1ED:;_LL1EE: {
const char*_tmpB83;void*_tmpB82;(_tmpB82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB83="stmt not an expression!",_tag_dyneither(_tmpB83,sizeof(char),24))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}_LL1EA:;}{
# 1862
struct Cyc_Absyn_Exp*(*_tmp3E6)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1866
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3E7=Cyc_Tcutil_compress(struct_type);union Cyc_Absyn_AggrInfoU _tmp3E9;struct Cyc_List_List*_tmp3EA;_LL1F0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E7;if(_tmp3E8->tag != 11)goto _LL1F2;else{_tmp3E9=(_tmp3E8->f1).aggr_info;_tmp3EA=(_tmp3E8->f1).targs;}}_LL1F1:
# 1873
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3E9);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3EA;
goto _LL1EF;_LL1F2:;_LL1F3: {
const char*_tmpB86;void*_tmpB85;(_tmpB85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB86="init_struct: bad struct type",_tag_dyneither(_tmpB86,sizeof(char),29))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}_LL1EF:;}
# 1880
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3ED=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3ED->tl != 0;_tmp3ED=_tmp3ED->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3EE=(struct Cyc_Absyn_Aggrfield*)_tmp3ED->hd;
struct _RegionHandle _tmp3EF=_new_region("temp");struct _RegionHandle*temp=& _tmp3EF;_push_region(temp);
{struct Cyc_List_List*_tmp3F0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1889
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3F0,_tmp3EE->type))){
struct Cyc_List_List*_tmp3F1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3F2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3F0,_tmp3F1);
# 1893
struct Cyc_List_List*new_fields=0;
for(_tmp3ED=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3ED != 0;_tmp3ED=_tmp3ED->tl){
struct Cyc_Absyn_Aggrfield*_tmp3F3=(struct Cyc_Absyn_Aggrfield*)_tmp3ED->hd;
struct Cyc_Absyn_Aggrfield*_tmpB87;struct Cyc_Absyn_Aggrfield*_tmp3F4=(_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->name=_tmp3F3->name,((_tmpB87->tq=Cyc_Toc_mt_tq,((_tmpB87->type=
# 1898
Cyc_Tcutil_rsubstitute(temp,_tmp3F2,_tmp3F3->type),((_tmpB87->width=_tmp3F3->width,((_tmpB87->attributes=_tmp3F3->attributes,_tmpB87)))))))))));
# 1905
if(_tmp3ED->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3F4->type)){
struct _dyneither_ptr**_tmpB88;index=((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88[0]=_tmp3F4->name,_tmpB88))));}}{
# 1912
struct Cyc_List_List*_tmpB89;new_fields=((_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=_tmp3F4,((_tmpB89->tl=new_fields,_tmpB89))))));};}
# 1914
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB91;void*_tmpB90[1];const char*_tmpB8F;struct _dyneither_ptr*_tmpB8E;struct _dyneither_ptr*name=
(_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E[0]=(struct _dyneither_ptr)((_tmpB91.tag=1,((_tmpB91.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB90[0]=& _tmpB91,Cyc_aprintf(((_tmpB8F="_genStruct%d",_tag_dyneither(_tmpB8F,sizeof(char),13))),_tag_dyneither(_tmpB90,sizeof(void*),1)))))))),_tmpB8E)));
struct _tuple0*_tmpB92;struct _tuple0*qv=(_tmpB92=_cycalloc(sizeof(*_tmpB92)),((_tmpB92->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB92->f2=name,_tmpB92)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB93;struct Cyc_Absyn_AggrdeclImpl*_tmp3F8=(_tmpB93=_cycalloc(sizeof(*_tmpB93)),((_tmpB93->exist_vars=0,((_tmpB93->rgn_po=0,((_tmpB93->fields=aggrfields,((_tmpB93->tagged=0,_tmpB93)))))))));
# 1923
struct Cyc_Absyn_Aggrdecl*_tmpB94;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94->kind=Cyc_Absyn_StructA,((_tmpB94->sc=Cyc_Absyn_Public,((_tmpB94->name=qv,((_tmpB94->tvs=0,((_tmpB94->impl=_tmp3F8,((_tmpB94->attributes=0,_tmpB94)))))))))))));
# 1929
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB9E;struct Cyc_Absyn_Aggrdecl**_tmpB9D;struct Cyc_Absyn_AggrInfo _tmpB9C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB9B;struct_type=(void*)((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=((_tmpB9E.tag=11,((_tmpB9E.f1=((_tmpB9C.aggr_info=Cyc_Absyn_KnownAggr(((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D[0]=new_ad,_tmpB9D))))),((_tmpB9C.targs=0,_tmpB9C)))),_tmpB9E)))),_tmpB9B))));}
# 1933
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1885
;_pop_region(temp);};}{
# 1937
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp404=_new_region("r");struct _RegionHandle*r=& _tmp404;_push_region(r);
{struct Cyc_List_List*_tmp405=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp405 != 0;_tmp405=_tmp405->tl){
struct Cyc_List_List*_tmp407;struct Cyc_Absyn_Exp*_tmp408;struct _tuple16*_tmp406=(struct _tuple16*)_tmp405->hd;_tmp407=_tmp406->f1;_tmp408=_tmp406->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp408->topt));
if(_tmp407 == 0){
const char*_tmpBA1;void*_tmpBA0;(_tmpBA0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA1="empty designator list",_tag_dyneither(_tmpBA1,sizeof(char),22))),_tag_dyneither(_tmpBA0,sizeof(void*),0)));}
if(_tmp407->tl != 0){
# 1947
struct _tuple0*_tmp40B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp40C=Cyc_Absyn_var_exp(_tmp40B,0);
for(0;_tmp407 != 0;_tmp407=_tmp407->tl){
void*_tmp40D=(void*)_tmp407->hd;struct _dyneither_ptr*_tmp40F;_LL1F5: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp40E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp40D;if(_tmp40E->tag != 1)goto _LL1F7;else{_tmp40F=_tmp40E->f1;}}_LL1F6:
# 1953
 if(Cyc_Toc_is_poly_field(struct_type,_tmp40F))
_tmp40C=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp40C);
_tmp3E0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E6(xexp,_tmp40F,0),_tmp40C,0),0),_tmp3E0,0);
# 1957
goto _LL1F4;_LL1F7:;_LL1F8: {
const char*_tmpBA4;void*_tmpBA3;(_tmpBA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA4="array designator in struct",_tag_dyneither(_tmpBA4,sizeof(char),27))),_tag_dyneither(_tmpBA3,sizeof(void*),0)));}_LL1F4:;}
# 1961
Cyc_Toc_exp_to_c(nv,_tmp408);
_tmp3E0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp40C,_tmp408,0),0),_tmp3E0,0);}else{
# 1964
void*_tmp412=(void*)_tmp407->hd;struct _dyneither_ptr*_tmp414;_LL1FA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp413=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp412;if(_tmp413->tag != 1)goto _LL1FC;else{_tmp414=_tmp413->f1;}}_LL1FB: {
# 1966
struct Cyc_Absyn_Exp*lval=_tmp3E6(xexp,_tmp414,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp414);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3E0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3E0,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}
# 1975
{void*_tmp415=_tmp408->r;struct Cyc_List_List*_tmp417;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_Absyn_Exp*_tmp41B;int _tmp41C;struct Cyc_Absyn_Exp*_tmp41E;void*_tmp420;struct Cyc_List_List*_tmp421;_LL1FF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp416=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp415;if(_tmp416->tag != 25)goto _LL201;else{_tmp417=_tmp416->f1;}}_LL200:
# 1977
 _tmp3E0=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp414)))->type,lval,_tmp417,_tmp3E0);
# 1979
goto _LL1FE;_LL201: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp418=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp415;if(_tmp418->tag != 26)goto _LL203;else{_tmp419=_tmp418->f1;_tmp41A=_tmp418->f2;_tmp41B=_tmp418->f3;_tmp41C=_tmp418->f4;}}_LL202:
# 1981
 _tmp3E0=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp414)))->type,lval,_tmp419,_tmp41A,_tmp41B,_tmp41C,_tmp3E0,0);
# 1985
_tmp41E=_tmp41A;goto _LL204;_LL203: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp41D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp415;if(_tmp41D->tag != 27)goto _LL205;else{_tmp41E=_tmp41D->f1;}}_LL204:
# 1989
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp414)){
void*_tmp422=Cyc_Toc_typ_to_c((void*)_check_null(_tmp41E->topt));
struct _tuple5 _tmpBA8;union Cyc_Absyn_Cnst _tmpBA7;struct Cyc_Absyn_Exp*_tmpBA5[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBA5[1]=Cyc_Absyn_sizeoftyp_exp(_tmp422,0),((_tmpBA5[0]=_tmp41E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA5,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpBA7.Int_c).val=((_tmpBA8.f1=Cyc_Absyn_Unsigned,((_tmpBA8.f2=(int)sizeof(double),_tmpBA8)))),(((_tmpBA7.Int_c).tag=5,_tmpBA7)))),0),0);}
# 1996
goto _LL1FE;_LL205: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp41F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp415;if(_tmp41F->tag != 29)goto _LL207;else{_tmp420=(void*)_tmp41F->f1;_tmp421=_tmp41F->f2;}}_LL206:
# 1998
 _tmp3E0=Cyc_Toc_init_anon_struct(nv,lval,_tmp420,_tmp421,_tmp3E0);goto _LL1FE;_LL207:;_LL208: {
# 2000
void*old_e_typ=(void*)_check_null(_tmp408->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp408);
{void*_tmp426=old_e_typ;_LL20A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp427=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp426;if(_tmp427->tag != 11)goto _LL20C;}_LL20B:
# 2007
 if(old_e_typ != _tmp408->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp408->topt))){
# 2011
ad=Cyc_Toc_update_aggr_type(ad,_tmp414,(void*)_check_null(_tmp408->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBB2;struct Cyc_Absyn_Aggrdecl**_tmpBB1;struct Cyc_Absyn_AggrInfo _tmpBB0;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBAF;struct_type=(void*)(
(_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF[0]=((_tmpBB2.tag=11,((_tmpBB2.f1=((_tmpBB0.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1[0]=ad,_tmpBB1))))),((_tmpBB0.targs=0,_tmpBB0)))),_tmpBB2)))),_tmpBAF))));}
# 2015
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 2017
goto _LL209;_LL20C:;_LL20D:
 goto _LL209;_LL209:;}{
# 2020
struct Cyc_Absyn_Aggrfield*_tmp42C=Cyc_Absyn_lookup_field(aggrfields,_tmp414);
# 2022
if(Cyc_Toc_is_poly_field(struct_type,_tmp414) && !was_ptr_type)
_tmp408=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp408);
# 2025
if(exist_types != 0)
_tmp408=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp42C))->type),_tmp408);
# 2028
_tmp3E0=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp408,0),0),_tmp3E0,0);
goto _LL1FE;};}_LL1FE:;}
# 2031
goto _LL1F9;}_LL1FC:;_LL1FD: {
const char*_tmpBB5;void*_tmpBB4;(_tmpBB4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB5="array designator in struct",_tag_dyneither(_tmpBB5,sizeof(char),27))),_tag_dyneither(_tmpBB4,sizeof(void*),0)));}_LL1F9:;}}}
# 2038
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2043
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3E0,pointer,rgnopt,is_atomic,do_declare);
# 2045
_tmp42F->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp430=_tmp42F;_npop_handler(0);return _tmp430;};};
# 1939
;_pop_region(r);};};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2051
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2054
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2057
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBB8;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBB7;return Cyc_Absyn_new_exp((void*)((_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBB8.tag=4,((_tmpBB8.f1=e1,((_tmpBB8.f2=incr,_tmpBB8)))))),_tmpBB7)))),0);}
# 2061
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2070
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2072
void*_tmp433=e1->r;struct Cyc_Absyn_Stmt*_tmp435;void*_tmp437;struct Cyc_Absyn_Exp*_tmp438;struct Cyc_Absyn_Exp*_tmp43A;struct _dyneither_ptr*_tmp43B;int _tmp43C;int _tmp43D;_LL20F: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp434=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp433;if(_tmp434->tag != 36)goto _LL211;else{_tmp435=_tmp434->f1;}}_LL210:
 Cyc_Toc_lvalue_assign_stmt(_tmp435,fs,f,f_env);goto _LL20E;_LL211: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp436=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp433;if(_tmp436->tag != 13)goto _LL213;else{_tmp437=(void*)_tmp436->f1;_tmp438=_tmp436->f2;}}_LL212:
 Cyc_Toc_lvalue_assign(_tmp438,fs,f,f_env);goto _LL20E;_LL213: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp439=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp433;if(_tmp439->tag != 20)goto _LL215;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;_tmp43C=_tmp439->f3;_tmp43D=_tmp439->f4;}}_LL214:
# 2077
 e1->r=_tmp43A->r;
{struct Cyc_List_List*_tmpBB9;Cyc_Toc_lvalue_assign(e1,((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->hd=_tmp43B,((_tmpBB9->tl=fs,_tmpBB9)))))),f,f_env);}
goto _LL20E;_LL215:;_LL216: {
# 2085
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2087
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL20E;}_LL20E:;}
# 2093
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2095
void*_tmp43F=s->r;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_Absyn_Decl*_tmp443;struct Cyc_Absyn_Stmt*_tmp444;struct Cyc_Absyn_Stmt*_tmp446;_LL218: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp440=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp43F;if(_tmp440->tag != 1)goto _LL21A;else{_tmp441=_tmp440->f1;}}_LL219:
 Cyc_Toc_lvalue_assign(_tmp441,fs,f,f_env);goto _LL217;_LL21A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp442=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43F;if(_tmp442->tag != 12)goto _LL21C;else{_tmp443=_tmp442->f1;_tmp444=_tmp442->f2;}}_LL21B:
# 2098
 Cyc_Toc_lvalue_assign_stmt(_tmp444,fs,f,f_env);goto _LL217;_LL21C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp445=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43F;if(_tmp445->tag != 2)goto _LL21E;else{_tmp446=_tmp445->f2;}}_LL21D:
 Cyc_Toc_lvalue_assign_stmt(_tmp446,fs,f,f_env);goto _LL217;_LL21E:;_LL21F: {
const char*_tmpBBD;void*_tmpBBC[1];struct Cyc_String_pa_PrintArg_struct _tmpBBB;(_tmpBBB.tag=0,((_tmpBBB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBBC[0]=& _tmpBBB,Cyc_Toc_toc_impos(((_tmpBBD="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBBD,sizeof(char),23))),_tag_dyneither(_tmpBBC,sizeof(void*),1)))))));}_LL217:;}
# 2104
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2108
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp44A=e->r;void**_tmp44C;struct Cyc_Absyn_Exp**_tmp44D;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Stmt*_tmp451;_LL221: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp44B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44A;if(_tmp44B->tag != 13)goto _LL223;else{_tmp44C=(void**)((void**)& _tmp44B->f1);_tmp44D=(struct Cyc_Absyn_Exp**)& _tmp44B->f2;}}_LL222:
# 2111
*_tmp44D=Cyc_Toc_push_address_exp(*_tmp44D);
*_tmp44C=Cyc_Absyn_cstar_typ(*_tmp44C,Cyc_Toc_mt_tq);
return e;_LL223: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp44E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44A;if(_tmp44E->tag != 19)goto _LL225;else{_tmp44F=_tmp44E->f1;}}_LL224:
# 2115
 return _tmp44F;_LL225: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp450=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp44A;if(_tmp450->tag != 36)goto _LL227;else{_tmp451=_tmp450->f1;}}_LL226:
# 2119
 Cyc_Toc_push_address_stmt(_tmp451);
return e;_LL227:;_LL228:
# 2122
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBC1;void*_tmpBC0[1];struct Cyc_String_pa_PrintArg_struct _tmpBBF;(_tmpBBF.tag=0,((_tmpBBF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBC0[0]=& _tmpBBF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC1="can't take & of exp %s",_tag_dyneither(_tmpBC1,sizeof(char),23))),_tag_dyneither(_tmpBC0,sizeof(void*),1)))))));};_LL220:;}
# 2127
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp455=s->r;struct Cyc_Absyn_Stmt*_tmp457;struct Cyc_Absyn_Stmt*_tmp459;struct Cyc_Absyn_Exp**_tmp45B;_LL22A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp456=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp455;if(_tmp456->tag != 2)goto _LL22C;else{_tmp457=_tmp456->f2;}}_LL22B:
 _tmp459=_tmp457;goto _LL22D;_LL22C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp458=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp455;if(_tmp458->tag != 12)goto _LL22E;else{_tmp459=_tmp458->f2;}}_LL22D:
 Cyc_Toc_push_address_stmt(_tmp459);goto _LL229;_LL22E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp45A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp455;if(_tmp45A->tag != 1)goto _LL230;else{_tmp45B=(struct Cyc_Absyn_Exp**)& _tmp45A->f1;}}_LL22F:
*_tmp45B=Cyc_Toc_push_address_exp(*_tmp45B);goto _LL229;_LL230:;_LL231: {
# 2133
const char*_tmpBC5;void*_tmpBC4[1];struct Cyc_String_pa_PrintArg_struct _tmpBC3;(_tmpBC3.tag=0,((_tmpBC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBC4[0]=& _tmpBC3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="can't take & of stmt %s",_tag_dyneither(_tmpBC5,sizeof(char),24))),_tag_dyneither(_tmpBC4,sizeof(void*),1)))))));}_LL229:;}
# 2138
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2140
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2142
if(x == 0)return 0;
{struct Cyc_List_List*_tmpBC6;result=((_tmpBC6=_region_malloc(r2,sizeof(*_tmpBC6)),((_tmpBC6->hd=f(x->hd,env),((_tmpBC6->tl=0,_tmpBC6))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpBC7;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBC7=_region_malloc(r2,sizeof(*_tmpBC7)),((_tmpBC7->hd=f(x->hd,env),((_tmpBC7->tl=0,_tmpBC7))))));}
prev=prev->tl;}
# 2149
return result;}
# 2151
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2155
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple16*_tmpBC8;return(_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->f1=0,((_tmpBC8->f2=e,_tmpBC8)))));}
# 2159
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp462=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp464;_LL233: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462;if(_tmp463->tag != 5)goto _LL235;else{_tmp464=_tmp463->f1;}}_LL234:
 return _tmp464;_LL235:;_LL236: {
const char*_tmpBCB;void*_tmpBCA;(_tmpBCA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCB="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBCB,sizeof(char),28))),_tag_dyneither(_tmpBCA,sizeof(void*),0)));}_LL232:;}
# 2169
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp467=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp46A;enum Cyc_Absyn_Sign _tmp46C;enum Cyc_Absyn_Sign _tmp470;enum Cyc_Absyn_Sign _tmp472;enum Cyc_Absyn_Sign _tmp474;int _tmp478;_LL238: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp467;if(_tmp468->tag != 5)goto _LL23A;}_LL239:
 res=Cyc_Absyn_null_exp(0);goto _LL237;_LL23A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp467;if(_tmp469->tag != 6)goto _LL23C;else{_tmp46A=_tmp469->f1;if(_tmp469->f2 != Cyc_Absyn_Char_sz)goto _LL23C;}}_LL23B:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp46A,'\000'),0);goto _LL237;_LL23C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp467;if(_tmp46B->tag != 6)goto _LL23E;else{_tmp46C=_tmp46B->f1;if(_tmp46B->f2 != Cyc_Absyn_Short_sz)goto _LL23E;}}_LL23D:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp46C,0),0);goto _LL237;_LL23E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp467;if(_tmp46D->tag != 13)goto _LL240;}_LL23F:
 goto _LL241;_LL240: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp467;if(_tmp46E->tag != 14)goto _LL242;}_LL241:
 _tmp470=Cyc_Absyn_Unsigned;goto _LL243;_LL242: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp467;if(_tmp46F->tag != 6)goto _LL244;else{_tmp470=_tmp46F->f1;if(_tmp46F->f2 != Cyc_Absyn_Int_sz)goto _LL244;}}_LL243:
 _tmp472=_tmp470;goto _LL245;_LL244: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp467;if(_tmp471->tag != 6)goto _LL246;else{_tmp472=_tmp471->f1;if(_tmp471->f2 != Cyc_Absyn_Long_sz)goto _LL246;}}_LL245:
# 2179
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp472,0),0);goto _LL237;_LL246: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp473=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp467;if(_tmp473->tag != 6)goto _LL248;else{_tmp474=_tmp473->f1;if(_tmp473->f2 != Cyc_Absyn_LongLong_sz)goto _LL248;}}_LL247:
# 2181
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp474,(long long)0),0);goto _LL237;_LL248: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp467;if(_tmp475->tag != 7)goto _LL24A;else{if(_tmp475->f1 != 0)goto _LL24A;}}_LL249:
{const char*_tmpBCC;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBCC="0.0F",_tag_dyneither(_tmpBCC,sizeof(char),5))),0),0);}goto _LL237;_LL24A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp476=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp467;if(_tmp476->tag != 7)goto _LL24C;else{if(_tmp476->f1 != 1)goto _LL24C;}}_LL24B:
{const char*_tmpBCD;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBCD="0.0",_tag_dyneither(_tmpBCD,sizeof(char),4))),1),0);}goto _LL237;_LL24C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp467;if(_tmp477->tag != 7)goto _LL24E;else{_tmp478=_tmp477->f1;}}_LL24D:
{const char*_tmpBCE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBCE="0.0L",_tag_dyneither(_tmpBCE,sizeof(char),5))),_tmp478),0);}goto _LL237;_LL24E:;_LL24F: {
# 2186
const char*_tmpBD2;void*_tmpBD1[1];struct Cyc_String_pa_PrintArg_struct _tmpBD0;(_tmpBD0.tag=0,((_tmpBD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBD1[0]=& _tmpBD0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD2="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBD2,sizeof(char),40))),_tag_dyneither(_tmpBD1,sizeof(void*),1)))))));}_LL237:;}
# 2188
res->topt=t;
return res;}
# 2195
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2208 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp47F=Cyc_Toc_typ_to_c(elt_type);
void*_tmp480=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp481=Cyc_Absyn_cstar_typ(_tmp47F,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpBD3;struct Cyc_Core_Opt*_tmp482=(_tmpBD3=_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3->v=_tmp481,_tmpBD3)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp483=e1->r;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp488;_LL251: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp484=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp483;if(_tmp484->tag != 19)goto _LL253;else{_tmp485=_tmp484->f1;}}_LL252:
# 2216
 if(!is_dyneither){
_tmp485=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp485,0,Cyc_Absyn_Other_coercion,0);
_tmp485->topt=fat_ptr_type;}
# 2220
Cyc_Toc_exp_to_c(nv,_tmp485);xinit=_tmp485;goto _LL250;_LL253: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp486=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp483;if(_tmp486->tag != 22)goto _LL255;else{_tmp487=_tmp486->f1;_tmp488=_tmp486->f2;}}_LL254:
# 2222
 if(!is_dyneither){
_tmp487=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp487,0,Cyc_Absyn_Other_coercion,0);
_tmp487->topt=fat_ptr_type;}
# 2226
Cyc_Toc_exp_to_c(nv,_tmp487);Cyc_Toc_exp_to_c(nv,_tmp488);
{struct Cyc_Absyn_Exp*_tmpBD4[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBD4[2]=_tmp488,((_tmpBD4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBD4[0]=_tmp487,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL250;_LL255:;_LL256: {
const char*_tmpBD7;void*_tmpBD6;(_tmpBD6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD7="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpBD7,sizeof(char),53))),_tag_dyneither(_tmpBD6,sizeof(void*),0)));}_LL250:;}{
# 2232
struct _tuple0*_tmp48C=Cyc_Toc_temp_var();
struct _RegionHandle _tmp48D=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp48D;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp48E=Cyc_Toc_add_varmap(rgn2,nv,_tmp48C,Cyc_Absyn_var_exp(_tmp48C,0));
struct Cyc_Absyn_Vardecl*_tmpBD8;struct Cyc_Absyn_Vardecl*_tmp48F=(_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8->sc=Cyc_Absyn_Public,((_tmpBD8->name=_tmp48C,((_tmpBD8->tq=Cyc_Toc_mt_tq,((_tmpBD8->type=_tmp480,((_tmpBD8->initializer=xinit,((_tmpBD8->rgn=0,((_tmpBD8->attributes=0,((_tmpBD8->escapes=0,_tmpBD8)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBDB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBDA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp490=(_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA[0]=((_tmpBDB.tag=4,((_tmpBDB.f1=_tmp48F,_tmpBDB)))),_tmpBDA)));
struct Cyc_Absyn_Exp*_tmp491=Cyc_Absyn_varb_exp(_tmp48C,(void*)_tmp490,0);
_tmp491->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp492=Cyc_Absyn_deref_exp(_tmp491,0);
_tmp492->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp48E,_tmp492);{
struct _tuple0*_tmp493=Cyc_Toc_temp_var();
_tmp48E=Cyc_Toc_add_varmap(rgn2,_tmp48E,_tmp493,Cyc_Absyn_var_exp(_tmp493,0));{
struct Cyc_Absyn_Vardecl*_tmpBDC;struct Cyc_Absyn_Vardecl*_tmp494=(_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC->sc=Cyc_Absyn_Public,((_tmpBDC->name=_tmp493,((_tmpBDC->tq=Cyc_Toc_mt_tq,((_tmpBDC->type=_tmp47F,((_tmpBDC->initializer=_tmp492,((_tmpBDC->rgn=0,((_tmpBDC->attributes=0,((_tmpBDC->escapes=0,_tmpBDC)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBDF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBDE;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp495=(_tmpBDE=_cycalloc(sizeof(*_tmpBDE)),((_tmpBDE[0]=((_tmpBDF.tag=4,((_tmpBDF.f1=_tmp494,_tmpBDF)))),_tmpBDE)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_varb_exp(_tmp493,(void*)_tmp495,0);
_tmp496->topt=_tmp492->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp496,e2,0);
z_init->topt=_tmp496->topt;}
# 2253
Cyc_Toc_exp_to_c(_tmp48E,z_init);{
struct _tuple0*_tmp497=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpBE0;struct Cyc_Absyn_Vardecl*_tmp498=(_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0->sc=Cyc_Absyn_Public,((_tmpBE0->name=_tmp497,((_tmpBE0->tq=Cyc_Toc_mt_tq,((_tmpBE0->type=_tmp47F,((_tmpBE0->initializer=z_init,((_tmpBE0->rgn=0,((_tmpBE0->attributes=0,((_tmpBE0->escapes=0,_tmpBE0)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBE3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBE2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp499=(_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE3.tag=4,((_tmpBE3.f1=_tmp498,_tmpBE3)))),_tmpBE2)));
_tmp48E=Cyc_Toc_add_varmap(rgn2,_tmp48E,_tmp497,Cyc_Absyn_var_exp(_tmp497,0));{
# 2260
struct Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_varb_exp(_tmp493,(void*)_tmp495,0);_tmp49A->topt=_tmp492->topt;{
struct Cyc_Absyn_Exp*_tmp49B=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp49A,_tmp49B,0);
_tmp49C->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp48E,_tmp49C);{
# 2266
struct Cyc_Absyn_Exp*_tmp49D=Cyc_Absyn_varb_exp(_tmp497,(void*)_tmp499,0);_tmp49D->topt=_tmp492->topt;{
struct Cyc_Absyn_Exp*_tmp49E=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp49F=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp49D,_tmp49E,0);
_tmp49F->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp48E,_tmp49F);{
# 2272
struct Cyc_Absyn_Exp*_tmpBE4[2];struct Cyc_List_List*_tmp4A0=(_tmpBE4[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBE4[0]=
# 2272
Cyc_Absyn_varb_exp(_tmp48C,(void*)_tmp490,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBE4,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2274
struct Cyc_Absyn_Exp*_tmp4A1=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4A0,0),_tmp4A1,0);{
# 2279
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp49C,_tmp49F,0),0);
struct Cyc_Absyn_Stmt*_tmp4A3=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp48C,(void*)_tmp490,0),Cyc_Toc_curr_sp,0);
_tmp4A4=Cyc_Toc_cast_it(_tmp481,_tmp4A4);{
struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_deref_exp(_tmp4A4,0);
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Absyn_assign_exp(_tmp4A5,Cyc_Absyn_var_exp(_tmp497,0),0);
struct Cyc_Absyn_Stmt*_tmp4A7=Cyc_Absyn_exp_stmt(_tmp4A6,0);
_tmp4A7=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4A2,_tmp4A3,Cyc_Absyn_skip_stmt(0),0),_tmp4A7,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBEA;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBE9;struct Cyc_Absyn_Decl*_tmpBE8;_tmp4A7=Cyc_Absyn_decl_stmt(((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->r=(void*)((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBE9.tag=0,((_tmpBE9.f1=_tmp498,_tmpBE9)))),_tmpBEA)))),((_tmpBE8->loc=0,_tmpBE8)))))),_tmp4A7,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBEF;struct Cyc_Absyn_Decl*_tmpBEE;_tmp4A7=Cyc_Absyn_decl_stmt(((_tmpBEE=_cycalloc(sizeof(*_tmpBEE)),((_tmpBEE->r=(void*)((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0[0]=((_tmpBEF.tag=0,((_tmpBEF.f1=_tmp494,_tmpBEF)))),_tmpBF0)))),((_tmpBEE->loc=0,_tmpBEE)))))),_tmp4A7,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBF5;struct Cyc_Absyn_Decl*_tmpBF4;_tmp4A7=Cyc_Absyn_decl_stmt(((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4->r=(void*)((_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF5.tag=0,((_tmpBF5.f1=_tmp48F,_tmpBF5)))),_tmpBF6)))),((_tmpBF4->loc=0,_tmpBF4)))))),_tmp4A7,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp4A7);};};};};};};};};};};};}
# 2234
;_pop_region(rgn2);};}
# 2305 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2309
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4BC=Cyc_Tcutil_compress(aggrtype);union Cyc_Absyn_AggrInfoU _tmp4BE;_LL258: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4BC;if(_tmp4BD->tag != 11)goto _LL25A;else{_tmp4BE=(_tmp4BD->f1).aggr_info;}}_LL259:
# 2312
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4BE);goto _LL257;_LL25A:;_LL25B: {
struct Cyc_String_pa_PrintArg_struct _tmpBFE;void*_tmpBFD[1];const char*_tmpBFC;void*_tmpBFB;(_tmpBFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpBFE.tag=0,((_tmpBFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBFD[0]=& _tmpBFE,Cyc_aprintf(((_tmpBFC="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpBFC,sizeof(char),51))),_tag_dyneither(_tmpBFD,sizeof(void*),1)))))))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}_LL257:;}{
# 2316
struct _tuple0*_tmp4C3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_var_exp(_tmp4C3,0);
struct Cyc_Absyn_Exp*_tmp4C5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_aggrarrow_exp(_tmp4C4,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Absyn_neq_exp(_tmp4C6,_tmp4C5,0);
struct Cyc_Absyn_Exp*_tmp4C8=Cyc_Absyn_aggrarrow_exp(_tmp4C4,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4C9=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4C8,0),0);
struct Cyc_Absyn_Stmt*_tmp4CA=Cyc_Absyn_ifthenelse_stmt(_tmp4C7,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4CB=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4CD=Cyc_Absyn_declare_stmt(_tmp4C3,_tmp4CB,_tmp4CC,Cyc_Absyn_seq_stmt(_tmp4CA,_tmp4C9,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4CD);}else{
# 2330
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_Toc_member_exp(aggrproj(_tmp4C4,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_Absyn_neq_exp(_tmp4CE,_tmp4C5,0);
struct Cyc_Absyn_Exp*_tmp4D0=Cyc_Toc_member_exp(aggrproj(_tmp4C4,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4D1=Cyc_Absyn_exp_stmt(_tmp4D0,0);
struct Cyc_Absyn_Stmt*_tmp4D2=Cyc_Absyn_ifthenelse_stmt(_tmp4CF,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4D3=Cyc_Absyn_declare_stmt(_tmp4C3,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4D2,_tmp4D1,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4D3);}};}
# 2343
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2346
void*_tmp4D4=e->r;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct _dyneither_ptr*_tmp4D9;int*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DC;struct _dyneither_ptr*_tmp4DD;int*_tmp4DE;_LL25D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D4;if(_tmp4D5->tag != 13)goto _LL25F;else{_tmp4D6=_tmp4D5->f2;}}_LL25E: {
const char*_tmpC01;void*_tmpC00;(_tmpC00=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC01="cast on lhs!",_tag_dyneither(_tmpC01,sizeof(char),13))),_tag_dyneither(_tmpC00,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D4;if(_tmp4D7->tag != 20)goto _LL261;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;_tmp4DA=(int*)& _tmp4D7->f4;}}_LL260: {
# 2349
void*_tmp4E1=Cyc_Tcutil_compress((void*)_check_null(_tmp4D8->topt));union Cyc_Absyn_AggrInfoU _tmp4E3;_LL266: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4E1;if(_tmp4E2->tag != 11)goto _LL268;else{_tmp4E3=(_tmp4E2->f1).aggr_info;}}_LL267: {
# 2351
struct Cyc_Absyn_Aggrdecl*_tmp4E4=Cyc_Absyn_get_known_aggrdecl(_tmp4E3);
*f_tag=Cyc_Toc_get_member_offset(_tmp4E4,_tmp4D9);{
# 2354
const char*_tmpC06;void*_tmpC05[2];struct Cyc_String_pa_PrintArg_struct _tmpC04;struct Cyc_String_pa_PrintArg_struct _tmpC03;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC03.tag=0,((_tmpC03.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D9),((_tmpC04.tag=0,((_tmpC04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4E4->name).f2),((_tmpC05[0]=& _tmpC04,((_tmpC05[1]=& _tmpC03,Cyc_aprintf(((_tmpC06="_union_%s_%s",_tag_dyneither(_tmpC06,sizeof(char),13))),_tag_dyneither(_tmpC05,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC07;*tagged_member_type=Cyc_Absyn_strct(((_tmpC07=_cycalloc(sizeof(*_tmpC07)),((_tmpC07[0]=str,_tmpC07)))));}
if(clear_read)*_tmp4DA=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E4->impl))->tagged;};}_LL268:;_LL269:
 return 0;_LL265:;}_LL261: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4DB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D4;if(_tmp4DB->tag != 21)goto _LL263;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;_tmp4DE=(int*)& _tmp4DB->f4;}}_LL262: {
# 2361
void*_tmp4EA=Cyc_Tcutil_compress((void*)_check_null(_tmp4DC->topt));void*_tmp4EC;_LL26B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EA;if(_tmp4EB->tag != 5)goto _LL26D;else{_tmp4EC=(_tmp4EB->f1).elt_typ;}}_LL26C: {
# 2363
void*_tmp4ED=Cyc_Tcutil_compress(_tmp4EC);union Cyc_Absyn_AggrInfoU _tmp4EF;_LL270: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4ED;if(_tmp4EE->tag != 11)goto _LL272;else{_tmp4EF=(_tmp4EE->f1).aggr_info;}}_LL271: {
# 2365
struct Cyc_Absyn_Aggrdecl*_tmp4F0=Cyc_Absyn_get_known_aggrdecl(_tmp4EF);
*f_tag=Cyc_Toc_get_member_offset(_tmp4F0,_tmp4DD);{
# 2368
const char*_tmpC0C;void*_tmpC0B[2];struct Cyc_String_pa_PrintArg_struct _tmpC0A;struct Cyc_String_pa_PrintArg_struct _tmpC09;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC09.tag=0,((_tmpC09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4DD),((_tmpC0A.tag=0,((_tmpC0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4F0->name).f2),((_tmpC0B[0]=& _tmpC0A,((_tmpC0B[1]=& _tmpC09,Cyc_aprintf(((_tmpC0C="_union_%s_%s",_tag_dyneither(_tmpC0C,sizeof(char),13))),_tag_dyneither(_tmpC0B,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC0D;*tagged_member_type=Cyc_Absyn_strct(((_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D[0]=str,_tmpC0D)))));}
if(clear_read)*_tmp4DE=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F0->impl))->tagged;};}_LL272:;_LL273:
 return 0;_LL26F:;}_LL26D:;_LL26E:
# 2374
 return 0;_LL26A:;}_LL263:;_LL264:
# 2376
 return 0;_LL25C:;}
# 2388 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple0*_tmp4F6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F6,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4F7=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F7,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F6,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2412 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2416
struct _tuple0*_tmp4F8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4F8,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2426
struct Cyc_Absyn_Exp*_tmp4F9=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4F9,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2429
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4F8,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC5C(unsigned int*_tmpC5B,unsigned int*_tmpC5A,struct _tuple0***_tmpC58){for(*_tmpC5B=0;*_tmpC5B < *_tmpC5A;(*_tmpC5B)++){(*_tmpC58)[*_tmpC5B]=(struct _tuple0*)
# 2806 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2435 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4FA=e->r;
if(e->topt == 0){
const char*_tmpC11;void*_tmpC10[1];struct Cyc_String_pa_PrintArg_struct _tmpC0F;(_tmpC0F.tag=0,((_tmpC0F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC10[0]=& _tmpC0F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC11="exp_to_c: no type for %s",_tag_dyneither(_tmpC11,sizeof(char),25))),_tag_dyneither(_tmpC10,sizeof(void*),1)))))));}
# 2441
if((nv->struct_info).lhs_exp != 0){
void*_tmp4FE=_tmp4FA;_LL275: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp4FF->tag != 28)goto _LL277;}_LL276:
 goto _LL274;_LL277:;_LL278:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL274:;}{
# 2449
void*old_typ=(void*)_check_null(e->topt);
void*_tmp500=_tmp4FA;struct _tuple0*_tmp504;void*_tmp505;enum Cyc_Absyn_Primop _tmp507;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Exp*_tmp50A;enum Cyc_Absyn_Incrementor _tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Core_Opt*_tmp50E;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_List_List*_tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_List_List*_tmp522;int _tmp523;struct Cyc_List_List*_tmp524;struct Cyc_Absyn_VarargInfo*_tmp525;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_List_List*_tmp52C;void**_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;int _tmp530;enum Cyc_Absyn_Coercion _tmp531;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp538;void*_tmp53A;void*_tmp53C;struct Cyc_List_List*_tmp53D;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp541;struct _dyneither_ptr*_tmp542;int _tmp543;int _tmp544;struct Cyc_Absyn_Exp*_tmp546;struct _dyneither_ptr*_tmp547;int _tmp548;int _tmp549;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_List_List*_tmp54E;struct Cyc_List_List*_tmp550;struct Cyc_Absyn_Vardecl*_tmp552;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp554;int _tmp555;struct Cyc_Absyn_Exp*_tmp557;void*_tmp558;int _tmp559;struct _tuple0*_tmp55B;struct Cyc_List_List*_tmp55C;struct Cyc_List_List*_tmp55D;struct Cyc_Absyn_Aggrdecl*_tmp55E;void*_tmp560;struct Cyc_List_List*_tmp561;struct Cyc_List_List*_tmp563;struct Cyc_Absyn_Datatypedecl*_tmp564;struct Cyc_Absyn_Datatypefield*_tmp565;int _tmp569;struct Cyc_Absyn_Exp*_tmp56A;void**_tmp56B;struct Cyc_Absyn_Exp*_tmp56C;int _tmp56D;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_Absyn_Exp*_tmp572;struct _dyneither_ptr*_tmp573;struct Cyc_Absyn_Stmt*_tmp575;_LL27A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp501=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp501->tag != 0)goto _LL27C;else{if(((_tmp501->f1).Null_c).tag != 1)goto _LL27C;}}_LL27B: {
# 2456
struct Cyc_Absyn_Exp*_tmp57A=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp57A,_tmp57A))->r;else{
# 2461
struct Cyc_Absyn_Exp*_tmpC12[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC12[2]=_tmp57A,((_tmpC12[1]=_tmp57A,((_tmpC12[0]=_tmp57A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC12,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2463
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2465
goto _LL279;}_LL27C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp502=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp502->tag != 0)goto _LL27E;}_LL27D:
 goto _LL279;_LL27E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp503=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp503->tag != 1)goto _LL280;else{_tmp504=_tmp503->f1;_tmp505=(void*)_tmp503->f2;}}_LL27F:
# 2468
{struct _handler_cons _tmp57C;_push_handler(& _tmp57C);{int _tmp57E=0;if(setjmp(_tmp57C.handler))_tmp57E=1;if(!_tmp57E){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp504))->r;;_pop_handler();}else{void*_tmp57D=(void*)_exn_thrown;void*_tmp580=_tmp57D;void*_tmp582;_LL2CF: {struct Cyc_Dict_Absent_exn_struct*_tmp581=(struct Cyc_Dict_Absent_exn_struct*)_tmp580;if(_tmp581->tag != Cyc_Dict_Absent)goto _LL2D1;}_LL2D0: {
# 2470
const char*_tmpC16;void*_tmpC15[1];struct Cyc_String_pa_PrintArg_struct _tmpC14;(_tmpC14.tag=0,((_tmpC14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp504)),((_tmpC15[0]=& _tmpC14,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC16="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC16,sizeof(char),32))),_tag_dyneither(_tmpC15,sizeof(void*),1)))))));}_LL2D1: _tmp582=_tmp580;_LL2D2:(void)_throw(_tmp582);_LL2CE:;}};}
# 2472
goto _LL279;_LL280: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp506=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp506->tag != 2)goto _LL282;else{_tmp507=_tmp506->f1;_tmp508=_tmp506->f2;}}_LL281: {
# 2475
struct Cyc_List_List*_tmp586=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp508);
# 2477
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp508);
switch(_tmp507){case Cyc_Absyn_Numelts: _LL2D3: {
# 2480
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508))->hd;
{void*_tmp587=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_Exp*_tmp589;void*_tmp58B;union Cyc_Absyn_Constraint*_tmp58C;union Cyc_Absyn_Constraint*_tmp58D;union Cyc_Absyn_Constraint*_tmp58E;_LL2D6: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp587;if(_tmp588->tag != 8)goto _LL2D8;else{_tmp589=(_tmp588->f1).num_elts;}}_LL2D7:
# 2484
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp589))){
const char*_tmpC19;void*_tmpC18;(_tmpC18=0,Cyc_Tcutil_terr(e->loc,((_tmpC19="can't calculate numelts",_tag_dyneither(_tmpC19,sizeof(char),24))),_tag_dyneither(_tmpC18,sizeof(void*),0)));}
e->r=_tmp589->r;goto _LL2D5;_LL2D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587;if(_tmp58A->tag != 5)goto _LL2DA;else{_tmp58B=(_tmp58A->f1).elt_typ;_tmp58C=((_tmp58A->f1).ptr_atts).nullable;_tmp58D=((_tmp58A->f1).ptr_atts).bounds;_tmp58E=((_tmp58A->f1).ptr_atts).zero_term;}}_LL2D9:
# 2488
{void*_tmp591=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp58D);struct Cyc_Absyn_Exp*_tmp594;_LL2DD: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp592=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp591;if(_tmp592->tag != 0)goto _LL2DF;}_LL2DE:
# 2490
{struct Cyc_Absyn_Exp*_tmpC1A[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC1A[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58B),0),((_tmpC1A[0]=(struct Cyc_Absyn_Exp*)_tmp508->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL2DC;_LL2DF: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp593=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp591;if(_tmp593->tag != 1)goto _LL2DC;else{_tmp594=_tmp593->f1;}}_LL2E0:
# 2496
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58E)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp508->hd);
# 2500
struct Cyc_Absyn_Exp*_tmpC1B[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC1B[1]=_tmp594,((_tmpC1B[0]=(struct Cyc_Absyn_Exp*)_tmp508->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58C)){
if(!Cyc_Evexp_c_can_eval(_tmp594)){
const char*_tmpC1E;void*_tmpC1D;(_tmpC1D=0,Cyc_Tcutil_terr(e->loc,((_tmpC1E="can't calculate numelts",_tag_dyneither(_tmpC1E,sizeof(char),24))),_tag_dyneither(_tmpC1D,sizeof(void*),0)));}
# 2505
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp594,Cyc_Absyn_uint_exp(0,0));}else{
# 2507
e->r=_tmp594->r;goto _LL2DC;}}
# 2509
goto _LL2DC;_LL2DC:;}
# 2511
goto _LL2D5;_LL2DA:;_LL2DB: {
# 2513
const char*_tmpC23;void*_tmpC22[2];struct Cyc_String_pa_PrintArg_struct _tmpC21;struct Cyc_String_pa_PrintArg_struct _tmpC20;(_tmpC20.tag=0,((_tmpC20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC21.tag=0,((_tmpC21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC22[0]=& _tmpC21,((_tmpC22[1]=& _tmpC20,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC23="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC23,sizeof(char),41))),_tag_dyneither(_tmpC22,sizeof(void*),2)))))))))))));}_LL2D5:;}
# 2516
break;}case Cyc_Absyn_Plus: _LL2D4:
# 2521
{void*_tmp59D=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp586))->hd);void*_tmp59F;union Cyc_Absyn_Constraint*_tmp5A0;union Cyc_Absyn_Constraint*_tmp5A1;_LL2E3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59D;if(_tmp59E->tag != 5)goto _LL2E5;else{_tmp59F=(_tmp59E->f1).elt_typ;_tmp5A0=((_tmp59E->f1).ptr_atts).bounds;_tmp5A1=((_tmp59E->f1).ptr_atts).zero_term;}}_LL2E4:
# 2523
{void*_tmp5A2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A0);struct Cyc_Absyn_Exp*_tmp5A5;_LL2E8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5A3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5A2;if(_tmp5A3->tag != 0)goto _LL2EA;}_LL2E9: {
# 2525
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC24[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC24[2]=e2,((_tmpC24[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp59F),0),((_tmpC24[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL2E7;}_LL2EA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5A4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A2;if(_tmp5A4->tag != 1)goto _LL2E7;else{_tmp5A5=_tmp5A4->f1;}}_LL2EB:
# 2531
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A1)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC25[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC25[2]=e2,((_tmpC25[1]=_tmp5A5,((_tmpC25[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC25,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2536
goto _LL2E7;_LL2E7:;}
# 2538
goto _LL2E2;_LL2E5:;_LL2E6:
# 2540
 goto _LL2E2;_LL2E2:;}
# 2542
break;case Cyc_Absyn_Minus: _LL2E1: {
# 2547
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp586))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp586->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2557
struct Cyc_Absyn_Exp*_tmpC26[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC26[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC26[1]=
# 2558
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC26[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC26,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2562
break;}case Cyc_Absyn_Eq: _LL2EC:
 goto _LL2ED;case Cyc_Absyn_Neq: _LL2ED:
 goto _LL2EE;case Cyc_Absyn_Gt: _LL2EE:
 goto _LL2EF;case Cyc_Absyn_Gte: _LL2EF:
 goto _LL2F0;case Cyc_Absyn_Lt: _LL2F0:
 goto _LL2F1;case Cyc_Absyn_Lte: _LL2F1: {
# 2570
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp508->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp586))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp586->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL2F2:
 break;}
# 2582
goto _LL279;}_LL282: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp509->tag != 4)goto _LL284;else{_tmp50A=_tmp509->f1;_tmp50B=_tmp509->f2;}}_LL283: {
# 2584
void*e2_cyc_typ=(void*)_check_null(_tmp50A->topt);
# 2593 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC27;struct _dyneither_ptr incr_str=(_tmpC27="increment",_tag_dyneither(_tmpC27,sizeof(char),10));
if(_tmp50B == Cyc_Absyn_PreDec  || _tmp50B == Cyc_Absyn_PostDec){const char*_tmpC28;incr_str=((_tmpC28="decrement",_tag_dyneither(_tmpC28,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp50A,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC2C;void*_tmpC2B[1];struct Cyc_String_pa_PrintArg_struct _tmpC2A;(_tmpC2A.tag=0,((_tmpC2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC2B[0]=& _tmpC2A,Cyc_Tcutil_terr(e->loc,((_tmpC2C="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC2C,sizeof(char),74))),_tag_dyneither(_tmpC2B,sizeof(void*),1)))))));}{
const char*_tmpC2F;void*_tmpC2E;(_tmpC2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC2F="in-place inc/dec on zero-term",_tag_dyneither(_tmpC2F,sizeof(char),30))),_tag_dyneither(_tmpC2E,sizeof(void*),0)));};}{
# 2602
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp50A,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5AF=Cyc_Absyn_signed_int_exp(1,0);
_tmp5AF->topt=Cyc_Absyn_sint_typ;
switch(_tmp50B){case Cyc_Absyn_PreInc: _LL2F4:
# 2609
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC35;struct Cyc_Core_Opt*_tmpC34;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC33;e->r=(void*)((_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33[0]=((_tmpC35.tag=3,((_tmpC35.f1=_tmp50A,((_tmpC35.f2=((_tmpC34=_cycalloc_atomic(sizeof(*_tmpC34)),((_tmpC34->v=(void*)Cyc_Absyn_Plus,_tmpC34)))),((_tmpC35.f3=_tmp5AF,_tmpC35)))))))),_tmpC33))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL2F5:
# 2613
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC3B;struct Cyc_Core_Opt*_tmpC3A;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC39;e->r=(void*)((_tmpC39=_cycalloc(sizeof(*_tmpC39)),((_tmpC39[0]=((_tmpC3B.tag=3,((_tmpC3B.f1=_tmp50A,((_tmpC3B.f2=((_tmpC3A=_cycalloc_atomic(sizeof(*_tmpC3A)),((_tmpC3A->v=(void*)Cyc_Absyn_Minus,_tmpC3A)))),((_tmpC3B.f3=_tmp5AF,_tmpC3B)))))))),_tmpC39))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL2F6:
# 2617
{const char*_tmpC3F;void*_tmpC3E[1];struct Cyc_String_pa_PrintArg_struct _tmpC3D;(_tmpC3D.tag=0,((_tmpC3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC3E[0]=& _tmpC3D,Cyc_Tcutil_terr(e->loc,((_tmpC3F="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC3F,sizeof(char),59))),_tag_dyneither(_tmpC3E,sizeof(void*),1)))))));}{
# 2619
const char*_tmpC42;void*_tmpC41;(_tmpC41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC42="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC42,sizeof(char),34))),_tag_dyneither(_tmpC41,sizeof(void*),0)));};}}
# 2622
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50A);
Cyc_Toc_set_lhs(nv,0);{
# 2627
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp50B == Cyc_Absyn_PostInc  || _tmp50B == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2633
if(_tmp50B == Cyc_Absyn_PreDec  || _tmp50B == Cyc_Absyn_PostDec)
change=- 1;{
struct Cyc_Absyn_Exp*_tmpC43[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC43[2]=
# 2637
Cyc_Absyn_signed_int_exp(change,0),((_tmpC43[1]=
# 2636
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC43[0]=
# 2635
Cyc_Toc_push_address_exp(_tmp50A),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC43,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2638
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2642
struct Cyc_Toc_functionSet*_tmp5BC=_tmp50B != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2644
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5BC,_tmp50A);
struct Cyc_Absyn_Exp*_tmpC44[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC44[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpC44[0]=
# 2645
Cyc_Toc_push_address_exp(_tmp50A),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2647
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp50A)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp50A,0,Cyc_Toc_incr_lvalue,_tmp50B);
e->r=_tmp50A->r;}}}
# 2651
goto _LL279;};};}_LL284: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp50C->tag != 3)goto _LL286;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;_tmp50F=_tmp50C->f3;}}_LL285: {
# 2670 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp50D->topt);
void*e2_old_typ=(void*)_check_null(_tmp50F->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp50D,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50D);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp50F);
e->r=Cyc_Toc_tagged_union_assignop(_tmp50D,e1_old_typ,_tmp50E,_tmp50F,e2_old_typ,f_tag,tagged_member_struct_type);
# 2681
return;}{
# 2683
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp50D,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp50D,_tmp50E,_tmp50F,ptr_type,is_dyneither,elt_type);
# 2689
return;}{
# 2693
int e1_poly=Cyc_Toc_is_poly_project(_tmp50D);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50D);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp50F);{
# 2699
int done=0;
if(_tmp50E != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
switch((enum Cyc_Absyn_Primop)_tmp50E->v){case Cyc_Absyn_Plus: _LL2F8:
 change=_tmp50F;break;case Cyc_Absyn_Minus: _LL2F9:
# 2707
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp50F,0);break;default: _LL2FA: {
const char*_tmpC47;void*_tmpC46;(_tmpC46=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC47="bad t ? pointer arithmetic",_tag_dyneither(_tmpC47,sizeof(char),27))),_tag_dyneither(_tmpC46,sizeof(void*),0)));}}
# 2710
done=1;{
# 2712
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpC48[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C1,((_tmpC48[2]=change,((_tmpC48[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC48[0]=
# 2713
Cyc_Toc_push_address_exp(_tmp50D),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC48,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2716
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2719
switch((enum Cyc_Absyn_Primop)_tmp50E->v){case Cyc_Absyn_Plus: _LL2FC:
# 2721
 done=1;
{struct Cyc_Absyn_Exp*_tmpC49[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp50D),((_tmpC49[1]=_tmp50F,((_tmpC49[0]=_tmp50D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC49,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL2FD: {
const char*_tmpC4C;void*_tmpC4B;(_tmpC4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4C="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC4C,sizeof(char),39))),_tag_dyneither(_tmpC4B,sizeof(void*),0)));}}}}
# 2728
if(!done){
# 2730
if(e1_poly)
_tmp50F->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp50F->r,0));
# 2736
if(!Cyc_Absyn_is_lvalue(_tmp50D)){
{struct _tuple20 _tmpC4F;struct _tuple20*_tmpC4E;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp50D,0,Cyc_Toc_assignop_lvalue,((_tmpC4E=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpC4E[0]=(struct _tuple20)((_tmpC4F.f1=_tmp50E,((_tmpC4F.f2=_tmp50F,_tmpC4F)))),_tmpC4E)))));}
e->r=_tmp50D->r;}}
# 2741
goto _LL279;};};};}_LL286: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp510=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp510->tag != 5)goto _LL288;else{_tmp511=_tmp510->f1;_tmp512=_tmp510->f2;_tmp513=_tmp510->f3;}}_LL287:
# 2743
 Cyc_Toc_exp_to_c(nv,_tmp511);
Cyc_Toc_exp_to_c(nv,_tmp512);
Cyc_Toc_exp_to_c(nv,_tmp513);
goto _LL279;_LL288: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp514=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp514->tag != 6)goto _LL28A;else{_tmp515=_tmp514->f1;_tmp516=_tmp514->f2;}}_LL289:
# 2748
 Cyc_Toc_exp_to_c(nv,_tmp515);
Cyc_Toc_exp_to_c(nv,_tmp516);
goto _LL279;_LL28A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp517->tag != 7)goto _LL28C;else{_tmp518=_tmp517->f1;_tmp519=_tmp517->f2;}}_LL28B:
# 2752
 Cyc_Toc_exp_to_c(nv,_tmp518);
Cyc_Toc_exp_to_c(nv,_tmp519);
goto _LL279;_LL28C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp51A->tag != 8)goto _LL28E;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL28D:
# 2756
 Cyc_Toc_exp_to_c(nv,_tmp51B);
Cyc_Toc_exp_to_c(nv,_tmp51C);
goto _LL279;_LL28E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp51D->tag != 9)goto _LL290;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;if(_tmp51D->f3 != 0)goto _LL290;}}_LL28F:
# 2760
 Cyc_Toc_exp_to_c(nv,_tmp51E);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp51F);
goto _LL279;_LL290: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp520->tag != 9)goto _LL292;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;if(_tmp520->f3 == 0)goto _LL292;_tmp523=(_tmp520->f3)->num_varargs;_tmp524=(_tmp520->f3)->injectors;_tmp525=(_tmp520->f3)->vai;}}_LL291: {
# 2772 "toc.cyc"
struct _RegionHandle _tmp5C8=_new_region("r");struct _RegionHandle*r=& _tmp5C8;_push_region(r);{
struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp523,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp525->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2780
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp522);
int num_normargs=num_args - _tmp523;
# 2784
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp522=_tmp522->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp522))->hd);{
struct Cyc_List_List*_tmpC50;new_args=((_tmpC50=_cycalloc(sizeof(*_tmpC50)),((_tmpC50->hd=(struct Cyc_Absyn_Exp*)_tmp522->hd,((_tmpC50->tl=new_args,_tmpC50))))));};}}
# 2789
{struct Cyc_Absyn_Exp*_tmpC53[3];struct Cyc_List_List*_tmpC52;new_args=((_tmpC52=_cycalloc(sizeof(*_tmpC52)),((_tmpC52->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC53[2]=num_varargs_exp,((_tmpC53[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC53[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC53,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC52->tl=new_args,_tmpC52))))));}
# 2794
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2796
Cyc_Toc_exp_to_c(nv,_tmp521);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp521,new_args,0),0);
# 2800
if(_tmp525->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5CC=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp525->type));struct Cyc_Absyn_Datatypedecl*_tmp5CE;_LL300: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CD->tag != 3)goto _LL302;else{if((((_tmp5CD->f1).datatype_info).KnownDatatype).tag != 2)goto _LL302;_tmp5CE=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5CD->f1).datatype_info).KnownDatatype).val);}}_LL301:
 tud=_tmp5CE;goto _LL2FF;_LL302:;_LL303: {
const char*_tmpC56;void*_tmpC55;(_tmpC55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC56="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC56,sizeof(char),44))),_tag_dyneither(_tmpC55,sizeof(void*),0)));}_LL2FF:;}{
# 2806
unsigned int _tmpC5B;unsigned int _tmpC5A;struct _dyneither_ptr _tmpC59;struct _tuple0**_tmpC58;unsigned int _tmpC57;struct _dyneither_ptr vs=(_tmpC57=(unsigned int)_tmp523,((_tmpC58=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpC57)),((_tmpC59=_tag_dyneither(_tmpC58,sizeof(struct _tuple0*),_tmpC57),((((_tmpC5A=_tmpC57,_tmpC5C(& _tmpC5B,& _tmpC5A,& _tmpC58))),_tmpC59)))))));
# 2808
if(_tmp523 != 0){
# 2810
struct Cyc_List_List*_tmp5D1=0;
{int i=_tmp523 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpC5D;_tmp5D1=((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpC5D->tl=_tmp5D1,_tmpC5D))))));}}
# 2814
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5D1,0),s,0);{
# 2817
int i=0;for(0;_tmp522 != 0;(((_tmp522=_tmp522->tl,_tmp524=_tmp524->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp522->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct _tuple0*_tmp5D4;struct Cyc_List_List*_tmp5D5;struct Cyc_Absyn_Datatypefield*_tmp5D3=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp524))->hd;_tmp5D4=_tmp5D3->name;_tmp5D5=_tmp5D3->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp5D5))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2832
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2835
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5D4),0),s,0);
# 2838
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5D4,tud->name)),0,s,0);};}};}else{
# 2845
struct _tuple16*_tmpC5E[3];struct Cyc_List_List*_tmp5D6=(_tmpC5E[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC5E[1]=
# 2845
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC5E[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5E,sizeof(struct _tuple16*),3)))))));
# 2847
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2857
{int i=0;for(0;_tmp522 != 0;(_tmp522=_tmp522->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp522->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp522->hd,0),s,0);}}
# 2863
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2876 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2878
_npop_handler(0);goto _LL279;
# 2772 "toc.cyc"
;_pop_region(r);}_LL292: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp526->tag != 10)goto _LL294;else{_tmp527=_tmp526->f1;}}_LL293:
# 2881 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp527);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp527),0))->r;
goto _LL279;_LL294: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp528->tag != 11)goto _LL296;else{_tmp529=_tmp528->f1;}}_LL295:
 Cyc_Toc_exp_to_c(nv,_tmp529);goto _LL279;_LL296: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp52A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp52A->tag != 12)goto _LL298;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL297:
# 2886
 Cyc_Toc_exp_to_c(nv,_tmp52B);
# 2895 "toc.cyc"
for(0;_tmp52C != 0;_tmp52C=_tmp52C->tl){
enum Cyc_Absyn_KindQual _tmp5DC=(Cyc_Tcutil_typ_kind((void*)_tmp52C->hd))->kind;
if(_tmp5DC != Cyc_Absyn_EffKind  && _tmp5DC != Cyc_Absyn_RgnKind){
{void*_tmp5DD=Cyc_Tcutil_compress((void*)_tmp52C->hd);_LL305: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5DD;if(_tmp5DE->tag != 2)goto _LL307;}_LL306:
 goto _LL308;_LL307: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5DD;if(_tmp5DF->tag != 3)goto _LL309;}_LL308:
 continue;_LL309:;_LL30A:
# 2902
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp52B,0))->r;
goto _LL304;_LL304:;}
# 2905
break;}}
# 2908
goto _LL279;_LL298: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp52D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp52D->tag != 13)goto _LL29A;else{_tmp52E=(void**)((void**)& _tmp52D->f1);_tmp52F=_tmp52D->f2;_tmp530=_tmp52D->f3;_tmp531=_tmp52D->f4;}}_LL299: {
# 2910
void*old_t2=(void*)_check_null(_tmp52F->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp52E;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp52E=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp52F);
# 2917
{struct _tuple19 _tmpC5F;struct _tuple19 _tmp5E1=(_tmpC5F.f1=Cyc_Tcutil_compress(old_t2),((_tmpC5F.f2=Cyc_Tcutil_compress(new_typ),_tmpC5F)));struct Cyc_Absyn_PtrInfo _tmp5E3;struct Cyc_Absyn_PtrInfo _tmp5E5;struct Cyc_Absyn_PtrInfo _tmp5E7;_LL30C:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E1.f1;if(_tmp5E2->tag != 5)goto _LL30E;else{_tmp5E3=_tmp5E2->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E1.f2;if(_tmp5E4->tag != 5)goto _LL30E;else{_tmp5E5=_tmp5E4->f1;}};_LL30D: {
# 2919
int _tmp5E9=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E3.ptr_atts).nullable);
int _tmp5EA=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E5.ptr_atts).nullable);
void*_tmp5EB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5E3.ptr_atts).bounds);
void*_tmp5EC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5E5.ptr_atts).bounds);
int _tmp5ED=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E3.ptr_atts).zero_term);
int _tmp5EE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E5.ptr_atts).zero_term);
{struct _tuple19 _tmpC60;struct _tuple19 _tmp5F0=(_tmpC60.f1=_tmp5EB,((_tmpC60.f2=_tmp5EC,_tmpC60)));struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Exp*_tmp5FA;_LL313:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F0.f1;if(_tmp5F1->tag != 1)goto _LL315;else{_tmp5F2=_tmp5F1->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F0.f2;if(_tmp5F3->tag != 1)goto _LL315;else{_tmp5F4=_tmp5F3->f1;}};_LL314:
# 2927
 if((!Cyc_Evexp_c_can_eval(_tmp5F2) || !Cyc_Evexp_c_can_eval(_tmp5F4)) && !
Cyc_Evexp_same_const_exp(_tmp5F2,_tmp5F4)){
const char*_tmpC63;void*_tmpC62;(_tmpC62=0,Cyc_Tcutil_terr(e->loc,((_tmpC63="can't validate cast due to potential size differences",_tag_dyneither(_tmpC63,sizeof(char),54))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}
if(_tmp5E9  && !_tmp5EA){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC66;void*_tmpC65;(_tmpC65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC66="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC66,sizeof(char),44))),_tag_dyneither(_tmpC65,sizeof(void*),0)));}
# 2936
if(_tmp531 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC6A;void*_tmpC69[1];struct Cyc_String_pa_PrintArg_struct _tmpC68;(_tmpC68.tag=0,((_tmpC68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC69[0]=& _tmpC68,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC6A="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC6A,sizeof(char),41))),_tag_dyneither(_tmpC69,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp52F);
if(do_null_check){
if(!_tmp530){
const char*_tmpC6D;void*_tmpC6C;(_tmpC6C=0,Cyc_Tcutil_warn(e->loc,((_tmpC6D="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC6D,sizeof(char),58))),_tag_dyneither(_tmpC6C,sizeof(void*),0)));}{
# 2943
struct Cyc_List_List*_tmpC6E;e->r=Cyc_Toc_cast_it_r(*_tmp52E,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E->hd=_tmp52F,((_tmpC6E->tl=0,_tmpC6E)))))),0));};}else{
# 2946
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp52F->r;}};}else{
# 2951
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp52F->r;}
# 2958
goto _LL312;_LL315:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F0.f1;if(_tmp5F5->tag != 1)goto _LL317;else{_tmp5F6=_tmp5F5->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F0.f2;if(_tmp5F7->tag != 0)goto _LL317;};_LL316:
# 2960
 if(!Cyc_Evexp_c_can_eval(_tmp5F6)){
const char*_tmpC71;void*_tmpC70;(_tmpC70=0,Cyc_Tcutil_terr(e->loc,((_tmpC71="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC71,sizeof(char),71))),_tag_dyneither(_tmpC70,sizeof(void*),0)));}
# 2963
if(_tmp531 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC75;void*_tmpC74[1];struct Cyc_String_pa_PrintArg_struct _tmpC73;(_tmpC73.tag=0,((_tmpC73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC74[0]=& _tmpC73,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC75="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC75,sizeof(char),44))),_tag_dyneither(_tmpC74,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 2967
if((_tmp5ED  && !(_tmp5E5.elt_tq).real_const) && !_tmp5EE)
# 2970
_tmp5F6=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5F6,Cyc_Absyn_uint_exp(1,0),0);
# 2972
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5F6,_tmp52F))->r;}else{
# 2974
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Toc__tag_dyneither_e;
# 2976
if(_tmp5ED){
# 2981
struct _tuple0*_tmp60D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp60E=Cyc_Absyn_var_exp(_tmp60D,0);
struct Cyc_Absyn_Exp*arg3;
# 2986
{void*_tmp60F=_tmp52F->r;_LL31C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp610->tag != 0)goto _LL31E;else{if(((_tmp610->f1).String_c).tag != 8)goto _LL31E;}}_LL31D:
# 2988
 arg3=_tmp5F6;goto _LL31B;_LL31E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp611=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60F;if(_tmp611->tag != 0)goto _LL320;else{if(((_tmp611->f1).Wstring_c).tag != 9)goto _LL320;}}_LL31F:
# 2990
 arg3=_tmp5F6;goto _LL31B;_LL320:;_LL321:
# 2992
{struct Cyc_Absyn_Exp*_tmpC76[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp52F),(
# 2994
(_tmpC76[1]=_tmp5F6,((_tmpC76[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp60E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC76,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL31B;_LL31B:;}
# 2997
if(!_tmp5EE  && !(_tmp5E5.elt_tq).real_const)
# 3000
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3002
struct Cyc_Absyn_Exp*_tmp613=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E5.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC77[3];struct Cyc_Absyn_Exp*_tmp614=Cyc_Absyn_fncall_exp(_tmp60C,((_tmpC77[2]=arg3,((_tmpC77[1]=_tmp613,((_tmpC77[0]=_tmp60E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC77,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp615=Cyc_Absyn_exp_stmt(_tmp614,0);
_tmp615=Cyc_Absyn_declare_stmt(_tmp60D,Cyc_Toc_typ_to_c(old_t2),_tmp52F,_tmp615,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp615);};}else{
# 3009
struct Cyc_Absyn_Exp*_tmpC78[3];e->r=Cyc_Toc_fncall_exp_r(_tmp60C,(
(_tmpC78[2]=_tmp5F6,((_tmpC78[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E5.elt_typ),0),((_tmpC78[0]=_tmp52F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC78,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3015
goto _LL312;_LL317:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F0.f1;if(_tmp5F8->tag != 0)goto _LL319;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F0.f2;if(_tmp5F9->tag != 1)goto _LL319;else{_tmp5FA=_tmp5F9->f1;}};_LL318:
# 3017
 if(!Cyc_Evexp_c_can_eval(_tmp5FA)){
const char*_tmpC7B;void*_tmpC7A;(_tmpC7A=0,Cyc_Tcutil_terr(e->loc,((_tmpC7B="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC7B,sizeof(char),71))),_tag_dyneither(_tmpC7A,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC7E;void*_tmpC7D;(_tmpC7D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC7E="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC7E,sizeof(char),45))),_tag_dyneither(_tmpC7D,sizeof(void*),0)));}{
# 3029 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp61C=_tmp5FA;
if(_tmp5ED  && !_tmp5EE)
_tmp61C=Cyc_Absyn_add_exp(_tmp5FA,Cyc_Absyn_uint_exp(1,0),0);{
# 3036
struct Cyc_Absyn_Exp*_tmp61D=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC7F[3];struct Cyc_Absyn_Exp*_tmp61E=Cyc_Absyn_fncall_exp(_tmp61D,(
(_tmpC7F[2]=_tmp61C,((_tmpC7F[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E3.elt_typ),0),((_tmpC7F[0]=_tmp52F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3041
if(_tmp5EA){
struct Cyc_List_List*_tmpC80;_tmp61E->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->hd=Cyc_Absyn_copy_exp(_tmp61E),((_tmpC80->tl=0,_tmpC80)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp52E,_tmp61E);
goto _LL312;};};_LL319:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F0.f1;if(_tmp5FB->tag != 0)goto _LL312;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F0.f2;if(_tmp5FC->tag != 0)goto _LL312;};_LL31A:
# 3049
 DynCast:
 if((_tmp5ED  && !_tmp5EE) && !(_tmp5E5.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC83;void*_tmpC82;(_tmpC82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC83="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC83,sizeof(char),70))),_tag_dyneither(_tmpC82,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp623=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC84[3];e->r=Cyc_Toc_fncall_exp_r(_tmp623,(
(_tmpC84[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC84[1]=
# 3055
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E3.elt_typ),0),((_tmpC84[0]=_tmp52F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC84,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3058
goto _LL312;_LL312:;}
# 3060
goto _LL30B;}_LL30E:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E1.f1;if(_tmp5E6->tag != 5)goto _LL310;else{_tmp5E7=_tmp5E6->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E1.f2;if(_tmp5E8->tag != 6)goto _LL310;};_LL30F:
# 3062
{void*_tmp625=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5E7.ptr_atts).bounds);_LL323: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp626=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp625;if(_tmp626->tag != 0)goto _LL325;}_LL324:
# 3064
 _tmp52F->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp52F->r,_tmp52F->loc),Cyc_Toc_curr_sp);goto _LL322;_LL325:;_LL326:
 goto _LL322;_LL322:;}
# 3067
goto _LL30B;_LL310:;_LL311:
# 3069
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp52F->r;
goto _LL30B;_LL30B:;}
# 3073
goto _LL279;}_LL29A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp532->tag != 14)goto _LL29C;else{_tmp533=_tmp532->f1;}}_LL29B:
# 3077
{void*_tmp627=_tmp533->r;struct _tuple0*_tmp629;struct Cyc_List_List*_tmp62A;struct Cyc_List_List*_tmp62B;struct Cyc_List_List*_tmp62D;_LL328: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp628=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp628->tag != 28)goto _LL32A;else{_tmp629=_tmp628->f1;_tmp62A=_tmp628->f2;_tmp62B=_tmp628->f3;}}_LL329:
# 3079
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC88;void*_tmpC87[1];struct Cyc_String_pa_PrintArg_struct _tmpC86;(_tmpC86.tag=0,((_tmpC86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp533->loc)),((_tmpC87[0]=& _tmpC86,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC88="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC88,sizeof(char),42))),_tag_dyneither(_tmpC87,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp631=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp533->topt),_tmp62A,1,0,_tmp62B,_tmp629);
e->r=_tmp631->r;
e->topt=_tmp631->topt;
goto _LL327;};_LL32A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp62C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp627;if(_tmp62C->tag != 23)goto _LL32C;else{_tmp62D=_tmp62C->f1;}}_LL32B:
# 3087
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC8C;void*_tmpC8B[1];struct Cyc_String_pa_PrintArg_struct _tmpC8A;(_tmpC8A.tag=0,((_tmpC8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp533->loc)),((_tmpC8B[0]=& _tmpC8A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC8C="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC8C,sizeof(char),42))),_tag_dyneither(_tmpC8B,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp62D))->r;
goto _LL327;_LL32C:;_LL32D:
# 3093
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp533);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp533)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp533,0,Cyc_Toc_address_lvalue,1);
# 3099
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp533);}
# 3101
goto _LL327;_LL327:;}
# 3103
goto _LL279;_LL29C: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp534=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp534->tag != 15)goto _LL29E;else{_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;}}_LL29D:
# 3106
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC90;void*_tmpC8F[1];struct Cyc_String_pa_PrintArg_struct _tmpC8E;(_tmpC8E.tag=0,((_tmpC8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp536->loc)),((_tmpC8F[0]=& _tmpC8E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC90="%s: new at top-level",_tag_dyneither(_tmpC90,sizeof(char),21))),_tag_dyneither(_tmpC8F,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp536->topt);
{void*_tmp638=_tmp536->r;struct Cyc_List_List*_tmp63A;struct Cyc_Absyn_Vardecl*_tmp63C;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63E;int _tmp63F;struct Cyc_Absyn_Exp*_tmp641;void*_tmp642;int _tmp643;struct _tuple0*_tmp645;struct Cyc_List_List*_tmp646;struct Cyc_List_List*_tmp647;struct Cyc_Absyn_Aggrdecl*_tmp648;struct Cyc_List_List*_tmp64A;_LL32F: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp639=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp639->tag != 25)goto _LL331;else{_tmp63A=_tmp639->f1;}}_LL330: {
# 3114
struct _tuple0*_tmp64B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp64C=Cyc_Absyn_var_exp(_tmp64B,0);
struct Cyc_Absyn_Stmt*_tmp64D=Cyc_Toc_init_array(nv,new_e_type,_tmp64C,_tmp63A,Cyc_Absyn_exp_stmt(_tmp64C,0));
void*old_elt_typ;
{void*_tmp64E=Cyc_Tcutil_compress(old_typ);void*_tmp650;struct Cyc_Absyn_Tqual _tmp651;union Cyc_Absyn_Constraint*_tmp652;_LL33C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp64F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64E;if(_tmp64F->tag != 5)goto _LL33E;else{_tmp650=(_tmp64F->f1).elt_typ;_tmp651=(_tmp64F->f1).elt_tq;_tmp652=((_tmp64F->f1).ptr_atts).zero_term;}}_LL33D:
# 3120
 old_elt_typ=_tmp650;goto _LL33B;_LL33E:;_LL33F: {
# 3122
const char*_tmpC93;void*_tmpC92;old_elt_typ=(
(_tmpC92=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC93="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC93,sizeof(char),52))),_tag_dyneither(_tmpC92,sizeof(void*),0))));}_LL33B:;}{
# 3125
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp655=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp656=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp63A),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp535 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp656);else{
# 3133
struct Cyc_Absyn_Exp*r=_tmp535;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp656);}
# 3138
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp64B,_tmp655,e1,_tmp64D,0));
goto _LL32E;};}_LL331: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp63B->tag != 26)goto _LL333;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;_tmp63E=_tmp63B->f3;_tmp63F=_tmp63B->f4;}}_LL332: {
# 3151 "toc.cyc"
int is_dyneither_ptr=0;
{void*_tmp657=Cyc_Tcutil_compress(old_typ);void*_tmp659;struct Cyc_Absyn_Tqual _tmp65A;union Cyc_Absyn_Constraint*_tmp65B;union Cyc_Absyn_Constraint*_tmp65C;_LL341: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp658=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp657;if(_tmp658->tag != 5)goto _LL343;else{_tmp659=(_tmp658->f1).elt_typ;_tmp65A=(_tmp658->f1).elt_tq;_tmp65B=((_tmp658->f1).ptr_atts).bounds;_tmp65C=((_tmp658->f1).ptr_atts).zero_term;}}_LL342:
# 3154
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp65B)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL340;_LL343:;_LL344: {
const char*_tmpC96;void*_tmpC95;(_tmpC95=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC96="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC96,sizeof(char),42))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}_LL340:;}{
# 3158
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)_check_null(_tmp63E->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp63D);{
struct Cyc_Absyn_Exp*_tmp65F=Cyc_Absyn_var_exp(max,0);
# 3166
if(_tmp63F)
_tmp65F=Cyc_Absyn_add_exp(_tmp65F,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),_tmp63C,Cyc_Absyn_var_exp(max,0),_tmp63E,_tmp63F,
Cyc_Toc_skip_stmt_dl(),1);
# 3172
struct _RegionHandle _tmp660=_new_region("r");struct _RegionHandle*r=& _tmp660;_push_region(r);{
struct _tuple21*_tmpC99;struct Cyc_List_List*_tmpC98;struct Cyc_List_List*decls=
(_tmpC98=_region_malloc(r,sizeof(*_tmpC98)),((_tmpC98->hd=((_tmpC99=_region_malloc(r,sizeof(*_tmpC99)),((_tmpC99->f1=max,((_tmpC99->f2=Cyc_Absyn_uint_typ,((_tmpC99->f3=_tmp63D,_tmpC99)))))))),((_tmpC98->tl=0,_tmpC98)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp535 == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC9A[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC9A[1]=_tmp65F,((_tmpC9A[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9A,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3182
struct Cyc_Absyn_Exp*r=_tmp535;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC9B[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC9B[1]=_tmp65F,((_tmpC9B[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3189
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpC9E;struct Cyc_List_List*_tmpC9D;decls=((_tmpC9D=_region_malloc(r,sizeof(*_tmpC9D)),((_tmpC9D->hd=((_tmpC9E=_region_malloc(r,sizeof(*_tmpC9E)),((_tmpC9E->f1=a,((_tmpC9E->f2=ptr_typ,((_tmpC9E->f3=ainit,_tmpC9E)))))))),((_tmpC9D->tl=decls,_tmpC9D))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp665=Cyc_Toc_temp_var();
void*_tmp666=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp667=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC9F[3];struct Cyc_Absyn_Exp*_tmp668=
Cyc_Absyn_fncall_exp(_tmp667,(
(_tmpC9F[2]=_tmp65F,((_tmpC9F[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC9F[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3199
{struct _tuple21*_tmpCA2;struct Cyc_List_List*_tmpCA1;decls=((_tmpCA1=_region_malloc(r,sizeof(*_tmpCA1)),((_tmpCA1->hd=((_tmpCA2=_region_malloc(r,sizeof(*_tmpCA2)),((_tmpCA2->f1=_tmp665,((_tmpCA2->f2=_tmp666,((_tmpCA2->f3=_tmp668,_tmpCA2)))))))),((_tmpCA1->tl=decls,_tmpCA1))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp665,0),0),0);}else{
# 3202
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp66C=decls;for(0;_tmp66C != 0;_tmp66C=_tmp66C->tl){
struct _tuple0*_tmp66E;void*_tmp66F;struct Cyc_Absyn_Exp*_tmp670;struct _tuple21 _tmp66D=*((struct _tuple21*)_tmp66C->hd);_tmp66E=_tmp66D.f1;_tmp66F=_tmp66D.f2;_tmp670=_tmp66D.f3;
s=Cyc_Absyn_declare_stmt(_tmp66E,_tmp66F,_tmp670,s,0);}}
# 3207
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3209
_npop_handler(0);goto _LL32E;
# 3172
;_pop_region(r);};};};}_LL333: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp640->tag != 27)goto _LL335;else{_tmp641=_tmp640->f1;_tmp642=(void*)_tmp640->f2;_tmp643=_tmp640->f3;}}_LL334: {
# 3213
int is_dyneither_ptr=0;
{void*_tmp673=Cyc_Tcutil_compress(old_typ);void*_tmp675;struct Cyc_Absyn_Tqual _tmp676;union Cyc_Absyn_Constraint*_tmp677;union Cyc_Absyn_Constraint*_tmp678;_LL346: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp674=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp673;if(_tmp674->tag != 5)goto _LL348;else{_tmp675=(_tmp674->f1).elt_typ;_tmp676=(_tmp674->f1).elt_tq;_tmp677=((_tmp674->f1).ptr_atts).bounds;_tmp678=((_tmp674->f1).ptr_atts).zero_term;}}_LL347:
# 3216
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp677)== (void*)& Cyc_Absyn_DynEither_b_val;
goto _LL345;_LL348:;_LL349: {
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA5="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpCA5,sizeof(char),42))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}_LL345:;}{
# 3220
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=_tmp642;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,_tmp641);{
struct Cyc_Absyn_Exp*_tmp67B=Cyc_Absyn_var_exp(max,0);
# 3228
struct Cyc_Absyn_Stmt*s;
if(_tmp643){
_tmp67B=Cyc_Absyn_add_exp(_tmp67B,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0);
s=Cyc_Absyn_exp_stmt(ex,0);};}else{
# 3236
s=Cyc_Absyn_skip_stmt(0);}{
# 3238
struct _RegionHandle _tmp67C=_new_region("r");struct _RegionHandle*r=& _tmp67C;_push_region(r);{
struct _tuple21*_tmpCA8;struct Cyc_List_List*_tmpCA7;struct Cyc_List_List*decls=
(_tmpCA7=_region_malloc(r,sizeof(*_tmpCA7)),((_tmpCA7->hd=((_tmpCA8=_region_malloc(r,sizeof(*_tmpCA8)),((_tmpCA8->f1=max,((_tmpCA8->f2=Cyc_Absyn_uint_typ,((_tmpCA8->f3=_tmp641,_tmpCA8)))))))),((_tmpCA7->tl=0,_tmpCA7)))));
struct Cyc_Absyn_Exp*ai;
if(_tmp535 == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpCA9[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpCA9[1]=_tmp67B,((_tmpCA9[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA9,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 3248
struct Cyc_Absyn_Exp*r=_tmp535;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpCAA[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpCAA[1]=_tmp67B,((_tmpCAA[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAA,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 3255
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple21*_tmpCAD;struct Cyc_List_List*_tmpCAC;decls=((_tmpCAC=_region_malloc(r,sizeof(*_tmpCAC)),((_tmpCAC->hd=((_tmpCAD=_region_malloc(r,sizeof(*_tmpCAD)),((_tmpCAD->f1=a,((_tmpCAD->f2=ptr_typ,((_tmpCAD->f3=ainit,_tmpCAD)))))))),((_tmpCAC->tl=decls,_tmpCAC))))));}
if(is_dyneither_ptr){
struct _tuple0*_tmp681=Cyc_Toc_temp_var();
void*_tmp682=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp683=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpCAE[3];struct Cyc_Absyn_Exp*_tmp684=
Cyc_Absyn_fncall_exp(_tmp683,(
(_tmpCAE[2]=_tmp67B,((_tmpCAE[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpCAE[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3265
{struct _tuple21*_tmpCB1;struct Cyc_List_List*_tmpCB0;decls=((_tmpCB0=_region_malloc(r,sizeof(*_tmpCB0)),((_tmpCB0->hd=((_tmpCB1=_region_malloc(r,sizeof(*_tmpCB1)),((_tmpCB1->f1=_tmp681,((_tmpCB1->f2=_tmp682,((_tmpCB1->f3=_tmp684,_tmpCB1)))))))),((_tmpCB0->tl=decls,_tmpCB0))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp681,0),0),0);}else{
# 3268
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp688=decls;for(0;_tmp688 != 0;_tmp688=_tmp688->tl){
struct _tuple0*_tmp68A;void*_tmp68B;struct Cyc_Absyn_Exp*_tmp68C;struct _tuple21 _tmp689=*((struct _tuple21*)_tmp688->hd);_tmp68A=_tmp689.f1;_tmp68B=_tmp689.f2;_tmp68C=_tmp689.f3;
s=Cyc_Absyn_declare_stmt(_tmp68A,_tmp68B,_tmp68C,s,0);}}
# 3273
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 3275
_npop_handler(0);goto _LL32E;
# 3238
;_pop_region(r);};};};}_LL335: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp644->tag != 28)goto _LL337;else{_tmp645=_tmp644->f1;_tmp646=_tmp644->f2;_tmp647=_tmp644->f3;_tmp648=_tmp644->f4;}}_LL336: {
# 3279
struct Cyc_Absyn_Exp*_tmp68F=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp536->topt),_tmp646,1,_tmp535,_tmp647,_tmp645);
e->r=_tmp68F->r;
e->topt=_tmp68F->topt;
goto _LL32E;}_LL337: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp649=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp638;if(_tmp649->tag != 23)goto _LL339;else{_tmp64A=_tmp649->f1;}}_LL338:
# 3285
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp535,_tmp64A))->r;
goto _LL32E;_LL339:;_LL33A: {
# 3292
void*old_elt_typ=(void*)_check_null(_tmp536->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3295
struct _tuple0*_tmp690=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp691=Cyc_Absyn_var_exp(_tmp690,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp691,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp535 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3302
struct Cyc_Absyn_Exp*r=_tmp535;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3310
int done=0;
{void*_tmp692=_tmp536->r;void*_tmp694;struct Cyc_Absyn_Exp*_tmp695;_LL34B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp693=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp692;if(_tmp693->tag != 13)goto _LL34D;else{_tmp694=(void*)_tmp693->f1;_tmp695=_tmp693->f2;}}_LL34C:
# 3313
{struct _tuple19 _tmpCB2;struct _tuple19 _tmp697=(_tmpCB2.f1=Cyc_Tcutil_compress(_tmp694),((_tmpCB2.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp695->topt)),_tmpCB2)));void*_tmp699;union Cyc_Absyn_Constraint*_tmp69A;union Cyc_Absyn_Constraint*_tmp69C;_LL350:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp698=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp697.f1;if(_tmp698->tag != 5)goto _LL352;else{_tmp699=(_tmp698->f1).elt_typ;_tmp69A=((_tmp698->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp697.f2;if(_tmp69B->tag != 5)goto _LL352;else{_tmp69C=((_tmp69B->f1).ptr_atts).bounds;}};_LL351:
# 3316
{struct _tuple19 _tmpCB3;struct _tuple19 _tmp69E=(_tmpCB3.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69A),((_tmpCB3.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp69C),_tmpCB3)));struct Cyc_Absyn_Exp*_tmp6A1;_LL355:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp69F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp69E.f1;if(_tmp69F->tag != 0)goto _LL357;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp69E.f2;if(_tmp6A0->tag != 1)goto _LL357;else{_tmp6A1=_tmp6A0->f1;}};_LL356:
# 3318
 Cyc_Toc_exp_to_c(nv,_tmp695);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp6A2=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCB4[4];e->r=Cyc_Toc_fncall_exp_r(_tmp6A2,(
(_tmpCB4[3]=_tmp6A1,((_tmpCB4[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp699),0),((_tmpCB4[1]=_tmp695,((_tmpCB4[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB4,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3326
goto _LL354;};_LL357:;_LL358:
 goto _LL354;_LL354:;}
# 3329
goto _LL34F;_LL352:;_LL353:
 goto _LL34F;_LL34F:;}
# 3332
goto _LL34A;_LL34D:;_LL34E:
 goto _LL34A;_LL34A:;}
# 3335
if(!done){
struct Cyc_Absyn_Stmt*_tmp6A4=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp691),0);
struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp536);
_tmp6A4=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp691,_tmp6A5,0),_tmp536,0),_tmp6A4,0);{
# 3341
void*_tmp6A6=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp690,_tmp6A6,mexp,_tmp6A4,0));};}
# 3344
goto _LL32E;};}_LL32E:;}
# 3346
goto _LL279;};_LL29E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp537=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp537->tag != 17)goto _LL2A0;else{_tmp538=_tmp537->f1;}}_LL29F: {
# 3349
int _tmp6A7=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp538);
Cyc_Toc_set_in_sizeof(nv,_tmp6A7);
goto _LL279;}_LL2A0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp539=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp539->tag != 16)goto _LL2A2;else{_tmp53A=(void*)_tmp539->f1;}}_LL2A1:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCB7;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCB6;e->r=(void*)((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB7.tag=16,((_tmpCB7.f1=Cyc_Toc_typ_to_c(_tmp53A),_tmpCB7)))),_tmpCB6))));}goto _LL279;_LL2A2: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp53B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp53B->tag != 18)goto _LL2A4;else{_tmp53C=(void*)_tmp53B->f1;_tmp53D=_tmp53B->f2;}}_LL2A3: {
# 3355
void*_tmp6AA=_tmp53C;
struct Cyc_List_List*_tmp6AB=_tmp53D;
for(0;_tmp6AB != 0;_tmp6AB=_tmp6AB->tl){
void*_tmp6AC=(void*)_tmp6AB->hd;struct _dyneither_ptr*_tmp6AE;unsigned int _tmp6B0;_LL35A: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp6AD=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp6AC;if(_tmp6AD->tag != 0)goto _LL35C;else{_tmp6AE=_tmp6AD->f1;}}_LL35B:
 goto _LL359;_LL35C: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp6AF=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp6AC;if(_tmp6AF->tag != 1)goto _LL359;else{_tmp6B0=_tmp6AF->f1;}}_LL35D:
# 3361
{void*_tmp6B1=Cyc_Tcutil_compress(_tmp6AA);union Cyc_Absyn_AggrInfoU _tmp6B3;struct Cyc_List_List*_tmp6B5;struct Cyc_List_List*_tmp6B7;struct Cyc_Absyn_Datatypefield*_tmp6B9;_LL35F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6B2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6B1;if(_tmp6B2->tag != 11)goto _LL361;else{_tmp6B3=(_tmp6B2->f1).aggr_info;}}_LL360: {
# 3363
struct Cyc_Absyn_Aggrdecl*_tmp6BA=Cyc_Absyn_get_known_aggrdecl(_tmp6B3);
if(_tmp6BA->impl == 0){
const char*_tmpCBA;void*_tmpCB9;(_tmpCB9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBA="struct fields must be known",_tag_dyneither(_tmpCBA,sizeof(char),28))),_tag_dyneither(_tmpCB9,sizeof(void*),0)));}
_tmp6B5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6BA->impl))->fields;goto _LL362;}_LL361: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6B4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6B1;if(_tmp6B4->tag != 12)goto _LL363;else{_tmp6B5=_tmp6B4->f2;}}_LL362: {
# 3368
struct Cyc_Absyn_Aggrfield*_tmp6BD=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6B5,(int)_tmp6B0);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCBD;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCBC;_tmp6AB->hd=(void*)((void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBD.tag=0,((_tmpCBD.f1=_tmp6BD->name,_tmpCBD)))),_tmpCBC)))));}
_tmp6AA=_tmp6BD->type;
goto _LL35E;}_LL363: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6B6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6B1;if(_tmp6B6->tag != 10)goto _LL365;else{_tmp6B7=_tmp6B6->f1;}}_LL364:
# 3373
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC0;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCBF;_tmp6AB->hd=(void*)((void*)((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCC0.tag=0,((_tmpCC0.f1=Cyc_Absyn_fieldname((int)(_tmp6B0 + 1)),_tmpCC0)))),_tmpCBF)))));}
_tmp6AA=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6B7,(int)_tmp6B0)).f2;
goto _LL35E;_LL365: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6B8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6B1;if(_tmp6B8->tag != 4)goto _LL367;else{if((((_tmp6B8->f1).field_info).KnownDatatypefield).tag != 2)goto _LL367;_tmp6B9=((struct _tuple1)(((_tmp6B8->f1).field_info).KnownDatatypefield).val).f2;}}_LL366:
# 3377
 if(_tmp6B0 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCC2;_tmp6AB->hd=(void*)((void*)((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC3.tag=0,((_tmpCC3.f1=Cyc_Toc_tag_sp,_tmpCC3)))),_tmpCC2)))));}else{
# 3380
_tmp6AA=(*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6B9->typs,(int)(_tmp6B0 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC6;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCC5;_tmp6AB->hd=(void*)((void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC6.tag=0,((_tmpCC6.f1=Cyc_Absyn_fieldname((int)_tmp6B0),_tmpCC6)))),_tmpCC5)))));};}
# 3383
goto _LL35E;_LL367:;_LL368:
{const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC9="impossible type for offsetof tuple index",_tag_dyneither(_tmpCC9,sizeof(char),41))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}
goto _LL35E;_LL35E:;}
# 3387
goto _LL359;_LL359:;}
# 3390
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCCC;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCCB;e->r=(void*)((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCC.tag=18,((_tmpCCC.f1=Cyc_Toc_typ_to_c(_tmp53C),((_tmpCCC.f2=_tmp53D,_tmpCCC)))))),_tmpCCB))));}
goto _LL279;}_LL2A4: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp53E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp53E->tag != 19)goto _LL2A6;else{_tmp53F=_tmp53E->f1;}}_LL2A5: {
# 3393
int _tmp6CA=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6CB=Cyc_Tcutil_compress((void*)_check_null(_tmp53F->topt));
{void*_tmp6CC=_tmp6CB;void*_tmp6CE;struct Cyc_Absyn_Tqual _tmp6CF;void*_tmp6D0;union Cyc_Absyn_Constraint*_tmp6D1;union Cyc_Absyn_Constraint*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D3;_LL36A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6CD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6CC;if(_tmp6CD->tag != 5)goto _LL36C;else{_tmp6CE=(_tmp6CD->f1).elt_typ;_tmp6CF=(_tmp6CD->f1).elt_tq;_tmp6D0=((_tmp6CD->f1).ptr_atts).rgn;_tmp6D1=((_tmp6CD->f1).ptr_atts).nullable;_tmp6D2=((_tmp6CD->f1).ptr_atts).bounds;_tmp6D3=((_tmp6CD->f1).ptr_atts).zero_term;}}_LL36B:
# 3398
{void*_tmp6D4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6D2);struct Cyc_Absyn_Exp*_tmp6D6;_LL36F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D4;if(_tmp6D5->tag != 1)goto _LL371;else{_tmp6D6=_tmp6D5->f1;}}_LL370: {
# 3400
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp53F);
Cyc_Toc_exp_to_c(nv,_tmp53F);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,Cyc_Tcutil_warn(e->loc,((_tmpCCF="inserted null check due to dereference",_tag_dyneither(_tmpCCF,sizeof(char),39))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}{
# 3406
struct Cyc_List_List*_tmpCD0;_tmp53F->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6CB),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=Cyc_Absyn_copy_exp(_tmp53F),((_tmpCD0->tl=0,_tmpCD0)))))),0));};}
# 3414
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D3)){
unsigned int _tmp6DC;int _tmp6DD;struct _tuple11 _tmp6DB=Cyc_Evexp_eval_const_uint_exp(_tmp6D6);_tmp6DC=_tmp6DB.f1;_tmp6DD=_tmp6DB.f2;
# 3420
if(!_tmp6DD  || _tmp6DC <= 0){
const char*_tmpCD3;void*_tmpCD2;(_tmpCD2=0,Cyc_Tcutil_terr(e->loc,((_tmpCD3="cannot determine dereference is in bounds",_tag_dyneither(_tmpCD3,sizeof(char),42))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}}
# 3423
goto _LL36E;}_LL371: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D4;if(_tmp6D7->tag != 0)goto _LL36E;}_LL372: {
# 3426
struct Cyc_Absyn_Exp*_tmp6E0=Cyc_Absyn_uint_exp(0,0);
_tmp6E0->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp53F,_tmp6E0);
Cyc_Toc_exp_to_c(nv,e);
goto _LL36E;}_LL36E:;}
# 3432
goto _LL369;_LL36C:;_LL36D: {
const char*_tmpCD6;void*_tmpCD5;(_tmpCD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD6="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCD6,sizeof(char),29))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}_LL369:;}
# 3435
Cyc_Toc_set_lhs(nv,_tmp6CA);
goto _LL279;};}_LL2A6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp540=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp540->tag != 20)goto _LL2A8;else{_tmp541=_tmp540->f1;_tmp542=_tmp540->f2;_tmp543=_tmp540->f3;_tmp544=_tmp540->f4;}}_LL2A7: {
# 3438
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp541->topt);
Cyc_Toc_exp_to_c(nv,_tmp541);
if(_tmp543  && _tmp544)
e->r=Cyc_Toc_check_tagged_union(_tmp541,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp542,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3446
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL279;}_LL2A8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp545=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp545->tag != 21)goto _LL2AA;else{_tmp546=_tmp545->f1;_tmp547=_tmp545->f2;_tmp548=_tmp545->f3;_tmp549=_tmp545->f4;}}_LL2A9: {
# 3450
int _tmp6E3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp546->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp546);
Cyc_Toc_exp_to_c(nv,_tmp546);{
int is_poly=Cyc_Toc_is_poly_project(e);
void*_tmp6E5;struct Cyc_Absyn_Tqual _tmp6E6;void*_tmp6E7;union Cyc_Absyn_Constraint*_tmp6E8;union Cyc_Absyn_Constraint*_tmp6E9;union Cyc_Absyn_Constraint*_tmp6EA;struct Cyc_Absyn_PtrInfo _tmp6E4=Cyc_Toc_get_ptr_type(e1typ);_tmp6E5=_tmp6E4.elt_typ;_tmp6E6=_tmp6E4.elt_tq;_tmp6E7=(_tmp6E4.ptr_atts).rgn;_tmp6E8=(_tmp6E4.ptr_atts).nullable;_tmp6E9=(_tmp6E4.ptr_atts).bounds;_tmp6EA=(_tmp6E4.ptr_atts).zero_term;
{void*_tmp6EB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6E9);struct Cyc_Absyn_Exp*_tmp6ED;_LL374: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6EB;if(_tmp6EC->tag != 1)goto _LL376;else{_tmp6ED=_tmp6EC->f1;}}_LL375: {
# 3461
unsigned int _tmp6F0;int _tmp6F1;struct _tuple11 _tmp6EF=Cyc_Evexp_eval_const_uint_exp(_tmp6ED);_tmp6F0=_tmp6EF.f1;_tmp6F1=_tmp6EF.f2;
if(_tmp6F1){
if(_tmp6F0 < 1){
const char*_tmpCD9;void*_tmpCD8;(_tmpCD8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD9="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCD9,sizeof(char),44))),_tag_dyneither(_tmpCD8,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCDC;void*_tmpCDB;(_tmpCDB=0,Cyc_Tcutil_warn(e->loc,((_tmpCDC="inserted null check due to dereference",_tag_dyneither(_tmpCDC,sizeof(char),39))),_tag_dyneither(_tmpCDB,sizeof(void*),0)));}{
# 3469
struct Cyc_Absyn_Exp*_tmpCDD[1];_tmp546->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCDD[0]=Cyc_Absyn_new_exp(_tmp546->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCDD,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3474
if(!Cyc_Evexp_c_can_eval(_tmp6ED)){
const char*_tmpCE0;void*_tmpCDF;(_tmpCDF=0,Cyc_Tcutil_terr(e->loc,((_tmpCE0="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCE0,sizeof(char),47))),_tag_dyneither(_tmpCDF,sizeof(void*),0)));}{
# 3478
struct Cyc_Absyn_Exp*_tmpCE1[4];_tmp546->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCE1[3]=
# 3482
Cyc_Absyn_uint_exp(0,0),((_tmpCE1[2]=
# 3481
Cyc_Absyn_sizeoftyp_exp(_tmp6E5,0),((_tmpCE1[1]=_tmp6ED,((_tmpCE1[0]=
# 3480
Cyc_Absyn_new_exp(_tmp546->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE1,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3484
goto _LL373;}_LL376: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6EE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6EB;if(_tmp6EE->tag != 0)goto _LL373;}_LL377: {
# 3487
void*ta1=Cyc_Toc_typ_to_c(_tmp6E5);
{struct Cyc_Absyn_Exp*_tmpCE2[3];_tmp546->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6E6),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpCE2[2]=
# 3492
Cyc_Absyn_uint_exp(0,0),((_tmpCE2[1]=
# 3491
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCE2[0]=
# 3490
Cyc_Absyn_new_exp(_tmp546->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3495
goto _LL373;}_LL373:;}
# 3497
if(_tmp548  && _tmp549)
e->r=Cyc_Toc_check_tagged_union(_tmp546,Cyc_Toc_typ_to_c(e1typ),_tmp6E5,_tmp547,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3500
if(is_poly  && _tmp549)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6E3);
goto _LL279;};};}_LL2AA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp54A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp54A->tag != 22)goto _LL2AC;else{_tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;}}_LL2AB: {
# 3505
int _tmp6FB=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6FC=Cyc_Tcutil_compress((void*)_check_null(_tmp54B->topt));
# 3509
{void*_tmp6FD=_tmp6FC;struct Cyc_List_List*_tmp6FF;void*_tmp701;struct Cyc_Absyn_Tqual _tmp702;void*_tmp703;union Cyc_Absyn_Constraint*_tmp704;union Cyc_Absyn_Constraint*_tmp705;union Cyc_Absyn_Constraint*_tmp706;_LL379: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6FE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6FD;if(_tmp6FE->tag != 10)goto _LL37B;else{_tmp6FF=_tmp6FE->f1;}}_LL37A:
# 3512
 Cyc_Toc_exp_to_c(nv,_tmp54B);
Cyc_Toc_exp_to_c(nv,_tmp54C);{
unsigned int _tmp708;int _tmp709;struct _tuple11 _tmp707=Cyc_Evexp_eval_const_uint_exp(_tmp54C);_tmp708=_tmp707.f1;_tmp709=_tmp707.f2;
if(!_tmp709){
const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE5="unknown tuple subscript in translation to C",_tag_dyneither(_tmpCE5,sizeof(char),44))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp54B,Cyc_Absyn_fieldname((int)(_tmp708 + 1)));
goto _LL378;};_LL37B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp700=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6FD;if(_tmp700->tag != 5)goto _LL37D;else{_tmp701=(_tmp700->f1).elt_typ;_tmp702=(_tmp700->f1).elt_tq;_tmp703=((_tmp700->f1).ptr_atts).rgn;_tmp704=((_tmp700->f1).ptr_atts).nullable;_tmp705=((_tmp700->f1).ptr_atts).bounds;_tmp706=((_tmp700->f1).ptr_atts).zero_term;}}_LL37C: {
# 3520
struct Cyc_List_List*_tmp70C=Cyc_Toc_get_relns(_tmp54B);
int _tmp70D=Cyc_Toc_need_null_check(_tmp54B);
int _tmp70E=Cyc_Toc_in_sizeof(nv);
# 3528
int in_bnds=_tmp70E;
{void*_tmp70F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp705);_LL380:;_LL381:
# 3531
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6FC,_tmp70C,_tmp54B,_tmp54C);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpCE9;void*_tmpCE8[1];struct Cyc_String_pa_PrintArg_struct _tmpCE7;(_tmpCE7.tag=0,((_tmpCE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCE8[0]=& _tmpCE7,Cyc_Tcutil_warn(e->loc,((_tmpCE9="bounds check necessary for %s",_tag_dyneither(_tmpCE9,sizeof(char),30))),_tag_dyneither(_tmpCE8,sizeof(void*),1)))))));}_LL37F:;}
# 3538
Cyc_Toc_exp_to_c(nv,_tmp54B);
Cyc_Toc_exp_to_c(nv,_tmp54C);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp713=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp705);struct Cyc_Absyn_Exp*_tmp715;_LL383: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp714=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp713;if(_tmp714->tag != 1)goto _LL385;else{_tmp715=_tmp714->f1;}}_LL384: {
# 3543
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp704) && (unsigned int)Cyc_Toc_need_null_check;
void*ta1=Cyc_Toc_typ_to_c(_tmp701);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp702);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCEC;void*_tmpCEB;(_tmpCEB=0,Cyc_Tcutil_warn(e->loc,((_tmpCEC="inserted null check due to dereference",_tag_dyneither(_tmpCEC,sizeof(char),39))),_tag_dyneither(_tmpCEB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCED[1];_tmp54B->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCED[0]=Cyc_Absyn_copy_exp(_tmp54B),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCED,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3554
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp706)){
# 3556
if(!Cyc_Evexp_c_can_eval(_tmp715)){
const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,Cyc_Tcutil_terr(e->loc,((_tmpCF0="cannot determine subscript is in bounds",_tag_dyneither(_tmpCF0,sizeof(char),40))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp54B);
struct Cyc_Absyn_Exp*_tmpCF1[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpCF1[2]=_tmp54C,((_tmpCF1[1]=_tmp715,((_tmpCF1[0]=_tmp54B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp715)){
const char*_tmpCF4;void*_tmpCF3;(_tmpCF3=0,Cyc_Tcutil_terr(e->loc,((_tmpCF4="cannot determine subscript is in bounds",_tag_dyneither(_tmpCF4,sizeof(char),40))),_tag_dyneither(_tmpCF3,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCF7;void*_tmpCF6;(_tmpCF6=0,Cyc_Tcutil_warn(e->loc,((_tmpCF7="inserted null check due to dereference",_tag_dyneither(_tmpCF7,sizeof(char),39))),_tag_dyneither(_tmpCF6,sizeof(void*),0)));}{
# 3568
struct Cyc_Absyn_Exp*_tmpCF8[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCF8[3]=_tmp54C,((_tmpCF8[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCF8[1]=_tmp715,((_tmpCF8[0]=_tmp54B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF8,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3574
if(!Cyc_Evexp_c_can_eval(_tmp715)){
const char*_tmpCFB;void*_tmpCFA;(_tmpCFA=0,Cyc_Tcutil_terr(e->loc,((_tmpCFB="cannot determine subscript is in bounds",_tag_dyneither(_tmpCFB,sizeof(char),40))),_tag_dyneither(_tmpCFA,sizeof(void*),0)));}{
# 3577
struct Cyc_Absyn_Exp*_tmpCFC[2];_tmp54C->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpCFC[1]=Cyc_Absyn_copy_exp(_tmp54C),((_tmpCFC[0]=_tmp715,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFC,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3580
goto _LL382;}_LL385: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp716=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp713;if(_tmp716->tag != 0)goto _LL382;}_LL386: {
# 3582
void*ta1=Cyc_Toc_typ_to_c(_tmp701);
if(in_bnds){
# 3586
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp702),
Cyc_Toc_member_exp(_tmp54B,Cyc_Toc_curr_sp,0)),_tmp54C);}else{
# 3591
struct Cyc_Absyn_Exp*_tmp725=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpCFD[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp702),
Cyc_Absyn_fncall_exp(_tmp725,(
(_tmpCFD[2]=_tmp54C,((_tmpCFD[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCFD[0]=_tmp54B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3597
goto _LL382;}_LL382:;}
# 3599
goto _LL378;}_LL37D:;_LL37E: {
const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD00="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD00,sizeof(char),49))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}_LL378:;}
# 3602
Cyc_Toc_set_lhs(nv,_tmp6FB);
goto _LL279;};}_LL2AC: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp54D=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp54D->tag != 23)goto _LL2AE;else{_tmp54E=_tmp54D->f1;}}_LL2AD:
# 3605
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp54E))->r;else{
# 3610
struct Cyc_List_List*_tmp729=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp54E);
void*_tmp72A=Cyc_Toc_add_tuple_type(_tmp729);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp54E != 0;(_tmp54E=_tmp54E->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp54E->hd);{
struct _tuple16*_tmpD03;struct Cyc_List_List*_tmpD02;dles=((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02->hd=((_tmpD03=_cycalloc(sizeof(*_tmpD03)),((_tmpD03->f1=0,((_tmpD03->f2=(struct Cyc_Absyn_Exp*)_tmp54E->hd,_tmpD03)))))),((_tmpD02->tl=dles,_tmpD02))))));};}}
# 3617
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3620
goto _LL279;_LL2AE: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp54F->tag != 25)goto _LL2B0;else{_tmp550=_tmp54F->f1;}}_LL2AF:
# 3624
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp550);
{struct Cyc_List_List*_tmp72D=_tmp550;for(0;_tmp72D != 0;_tmp72D=_tmp72D->tl){
struct Cyc_Absyn_Exp*_tmp72F;struct _tuple16*_tmp72E=(struct _tuple16*)_tmp72D->hd;_tmp72F=_tmp72E->f2;
Cyc_Toc_exp_to_c(nv,_tmp72F);}}
# 3629
goto _LL279;_LL2B0: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp551=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp551->tag != 26)goto _LL2B2;else{_tmp552=_tmp551->f1;_tmp553=_tmp551->f2;_tmp554=_tmp551->f3;_tmp555=_tmp551->f4;}}_LL2B1: {
# 3633
unsigned int _tmp731;int _tmp732;struct _tuple11 _tmp730=Cyc_Evexp_eval_const_uint_exp(_tmp553);_tmp731=_tmp730.f1;_tmp732=_tmp730.f2;{
void*_tmp733=Cyc_Toc_typ_to_c((void*)_check_null(_tmp554->topt));
Cyc_Toc_exp_to_c(nv,_tmp554);{
struct Cyc_List_List*es=0;
# 3638
if(!Cyc_Toc_is_zero(_tmp554)){
if(!_tmp732){
const char*_tmpD06;void*_tmpD05;(_tmpD05=0,Cyc_Tcutil_terr(_tmp553->loc,((_tmpD06="cannot determine value of constant",_tag_dyneither(_tmpD06,sizeof(char),35))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp731;++ i){
struct _tuple16*_tmpD09;struct Cyc_List_List*_tmpD08;es=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->f1=0,((_tmpD09->f2=_tmp554,_tmpD09)))))),((_tmpD08->tl=es,_tmpD08))))));}}
# 3644
if(_tmp555){
struct Cyc_Absyn_Exp*_tmp738=Cyc_Toc_cast_it(_tmp733,Cyc_Absyn_uint_exp(0,0));
struct _tuple16*_tmpD0C;struct Cyc_List_List*_tmpD0B;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD0B=_cycalloc(sizeof(*_tmpD0B)),((_tmpD0B->hd=((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->f1=0,((_tmpD0C->f2=_tmp738,_tmpD0C)))))),((_tmpD0B->tl=0,_tmpD0B)))))));}}
# 3649
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL279;};};}_LL2B2: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp556=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp556->tag != 27)goto _LL2B4;else{_tmp557=_tmp556->f1;_tmp558=(void*)_tmp556->f2;_tmp559=_tmp556->f3;}}_LL2B3:
# 3654
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL279;_LL2B4: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp55A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp55A->tag != 28)goto _LL2B6;else{_tmp55B=_tmp55A->f1;_tmp55C=_tmp55A->f2;_tmp55D=_tmp55A->f3;_tmp55E=_tmp55A->f4;}}_LL2B5:
# 3659
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp73B=Cyc_Toc_init_struct(nv,old_typ,_tmp55C,0,0,_tmp55D,_tmp55B);
e->r=_tmp73B->r;
e->topt=_tmp73B->topt;}else{
# 3670
if(_tmp55E == 0){
const char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0F="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD0F,sizeof(char),38))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp55E;
# 3674
struct _RegionHandle _tmp73E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp73E;_push_region(rgn);
{struct Cyc_List_List*_tmp73F=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp55D,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3678
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55E->impl))->tagged){
# 3680
struct Cyc_Absyn_Aggrfield*_tmp741;struct Cyc_Absyn_Exp*_tmp742;struct _tuple22*_tmp740=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp73F))->hd;_tmp741=_tmp740->f1;_tmp742=_tmp740->f2;{
void*_tmp743=(void*)_check_null(_tmp742->topt);
void*_tmp744=_tmp741->type;
Cyc_Toc_exp_to_c(nv,_tmp742);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp744) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp743))
_tmp742->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp742->r,0));{
# 3689
int i=Cyc_Toc_get_member_offset(_tmp55E,_tmp741->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple16*_tmpD14;struct _tuple16*_tmpD13;struct _tuple16*_tmpD12[2];struct Cyc_List_List*_tmp745=(_tmpD12[1]=((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->f1=0,((_tmpD13->f2=_tmp742,_tmpD13)))))),((_tmpD12[0]=((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->f1=0,((_tmpD14->f2=field_tag_exp,_tmpD14)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD12,sizeof(struct _tuple16*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp745,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD1A;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD19;void*_tmpD18[1];struct Cyc_List_List*ds=(_tmpD18[0]=(void*)((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A[0]=((_tmpD19.tag=1,((_tmpD19.f1=_tmp741->name,_tmpD19)))),_tmpD1A)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD18,sizeof(void*),1)));
struct _tuple16*_tmpD1D;struct _tuple16*_tmpD1C[1];struct Cyc_List_List*dles=(_tmpD1C[0]=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->f1=ds,((_tmpD1D->f2=umem,_tmpD1D)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1C,sizeof(struct _tuple16*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3698
struct Cyc_List_List*_tmp74E=0;
struct Cyc_List_List*_tmp74F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp74F != 0;_tmp74F=_tmp74F->tl){
struct Cyc_List_List*_tmp750=_tmp73F;for(0;_tmp750 != 0;_tmp750=_tmp750->tl){
if((*((struct _tuple22*)_tmp750->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp74F->hd){
struct Cyc_Absyn_Aggrfield*_tmp752;struct Cyc_Absyn_Exp*_tmp753;struct _tuple22*_tmp751=(struct _tuple22*)_tmp750->hd;_tmp752=_tmp751->f1;_tmp753=_tmp751->f2;{
void*_tmp754=Cyc_Toc_typ_to_c(_tmp752->type);
void*_tmp755=Cyc_Toc_typ_to_c((void*)_check_null(_tmp753->topt));
Cyc_Toc_exp_to_c(nv,_tmp753);
# 3708
if(!Cyc_Tcutil_unify(_tmp754,_tmp755)){
# 3710
if(!Cyc_Tcutil_is_arithmetic_type(_tmp754) || !
Cyc_Tcutil_is_arithmetic_type(_tmp755))
_tmp753=Cyc_Toc_cast_it(_tmp754,Cyc_Absyn_copy_exp(_tmp753));}
{struct _tuple16*_tmpD20;struct Cyc_List_List*_tmpD1F;_tmp74E=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->hd=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->f1=0,((_tmpD20->f2=_tmp753,_tmpD20)))))),((_tmpD1F->tl=_tmp74E,_tmpD1F))))));}
break;};}}}
# 3717
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp74E));}}
# 3675
;_pop_region(rgn);};}
# 3721
goto _LL279;_LL2B6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp55F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp55F->tag != 29)goto _LL2B8;else{_tmp560=(void*)_tmp55F->f1;_tmp561=_tmp55F->f2;}}_LL2B7: {
# 3723
struct Cyc_List_List*fs;
{void*_tmp758=Cyc_Tcutil_compress(_tmp560);struct Cyc_List_List*_tmp75A;_LL388: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp758;if(_tmp759->tag != 12)goto _LL38A;else{_tmp75A=_tmp759->f2;}}_LL389:
 fs=_tmp75A;goto _LL387;_LL38A:;_LL38B: {
const char*_tmpD24;void*_tmpD23[1];struct Cyc_String_pa_PrintArg_struct _tmpD22;(_tmpD22.tag=0,((_tmpD22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp560)),((_tmpD23[0]=& _tmpD22,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD24="anon struct has type %s",_tag_dyneither(_tmpD24,sizeof(char),24))),_tag_dyneither(_tmpD23,sizeof(void*),1)))))));}_LL387:;}{
# 3728
struct _RegionHandle _tmp75E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp75E;_push_region(rgn);{
struct Cyc_List_List*_tmp75F=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp561,Cyc_Absyn_StructA,fs);
for(0;_tmp75F != 0;_tmp75F=_tmp75F->tl){
struct Cyc_Absyn_Aggrfield*_tmp761;struct Cyc_Absyn_Exp*_tmp762;struct _tuple22*_tmp760=(struct _tuple22*)_tmp75F->hd;_tmp761=_tmp760->f1;_tmp762=_tmp760->f2;{
void*_tmp763=(void*)_check_null(_tmp762->topt);
void*_tmp764=_tmp761->type;
Cyc_Toc_exp_to_c(nv,_tmp762);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp764) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp763))
_tmp762->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp762->r,0));};}
# 3742
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp561);}
# 3744
_npop_handler(0);goto _LL279;
# 3728
;_pop_region(rgn);};}_LL2B8: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp562=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp562->tag != 30)goto _LL2BA;else{_tmp563=_tmp562->f1;_tmp564=_tmp562->f2;_tmp565=_tmp562->f3;}}_LL2B9: {
# 3747
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple0*_tmp765=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp766=Cyc_Absyn_var_exp(_tmp765,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp565->name,_tmp564->name));
tag_exp=_tmp564->is_extensible?Cyc_Absyn_var_exp(_tmp565->name,0):
 Cyc_Toc_datatype_tag(_tmp564,_tmp565->name);
mem_exp=_tmp766;{
struct Cyc_List_List*_tmp767=_tmp565->typs;
# 3758
if(Cyc_Toc_is_toplevel(nv)){
# 3760
struct Cyc_List_List*dles=0;
for(0;_tmp563 != 0;(_tmp563=_tmp563->tl,_tmp767=_tmp767->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp563->hd;
void*_tmp768=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp767))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp768))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple16*_tmpD27;struct Cyc_List_List*_tmpD26;dles=((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->hd=((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->f1=0,((_tmpD27->f2=cur_e,_tmpD27)))))),((_tmpD26->tl=dles,_tmpD26))))));};}
# 3771
{struct _tuple16*_tmpD2A;struct Cyc_List_List*_tmpD29;dles=((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->hd=((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A->f1=0,((_tmpD2A->f2=tag_exp,_tmpD2A)))))),((_tmpD29->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD29))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3777
struct Cyc_List_List*_tmpD2B;struct Cyc_List_List*_tmp76D=
(_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD2B->tl=0,_tmpD2B)))));
# 3780
{int i=1;for(0;_tmp563 != 0;(((_tmp563=_tmp563->tl,i ++)),_tmp767=_tmp767->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp563->hd;
void*_tmp76E=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp767))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp76E))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp76F=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3790
struct Cyc_List_List*_tmpD2C;_tmp76D=((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->hd=_tmp76F,((_tmpD2C->tl=_tmp76D,_tmpD2C))))));};}}{
# 3792
struct Cyc_Absyn_Stmt*_tmp771=Cyc_Absyn_exp_stmt(_tmp766,0);
struct Cyc_List_List*_tmpD2D;struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->hd=_tmp771,((_tmpD2D->tl=_tmp76D,_tmpD2D))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp765,datatype_ctype,0,_tmp772,0));};}
# 3796
goto _LL279;};}_LL2BA: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp566=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp566->tag != 31)goto _LL2BC;}_LL2BB:
# 3798
 goto _LL2BD;_LL2BC: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp567=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp567->tag != 32)goto _LL2BE;}_LL2BD:
 goto _LL279;_LL2BE: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp568=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp568->tag != 33)goto _LL2C0;else{_tmp569=(_tmp568->f1).is_calloc;_tmp56A=(_tmp568->f1).rgn;_tmp56B=(_tmp568->f1).elt_type;_tmp56C=(_tmp568->f1).num_elts;_tmp56D=(_tmp568->f1).fat_result;}}_LL2BF: {
# 3802
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp56B)));
Cyc_Toc_exp_to_c(nv,_tmp56C);
# 3806
if(_tmp56D){
struct _tuple0*_tmp775=Cyc_Toc_temp_var();
struct _tuple0*_tmp776=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp569){
xexp=_tmp56C;
if(_tmp56A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp56A;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp775,0));}else{
# 3817
pexp=Cyc_Toc_calloc_exp(*_tmp56B,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp775,0));}{
# 3819
struct Cyc_Absyn_Exp*_tmpD2E[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD2E[2]=
Cyc_Absyn_var_exp(_tmp775,0),((_tmpD2E[1]=
# 3820
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD2E[0]=Cyc_Absyn_var_exp(_tmp776,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3823
if(_tmp56A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp56A;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp775,0));}else{
# 3828
pexp=Cyc_Toc_malloc_exp(*_tmp56B,Cyc_Absyn_var_exp(_tmp775,0));}{
# 3830
struct Cyc_Absyn_Exp*_tmpD2F[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD2F[2]=
Cyc_Absyn_var_exp(_tmp775,0),((_tmpD2F[1]=
# 3830
Cyc_Absyn_uint_exp(1,0),((_tmpD2F[0]=Cyc_Absyn_var_exp(_tmp776,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3833
struct Cyc_Absyn_Stmt*_tmp779=Cyc_Absyn_declare_stmt(_tmp775,Cyc_Absyn_uint_typ,_tmp56C,
Cyc_Absyn_declare_stmt(_tmp776,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp779);};}else{
# 3838
if(_tmp569){
if(_tmp56A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp56A;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp56C))->r;}else{
# 3844
e->r=(Cyc_Toc_calloc_exp(*_tmp56B,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp56C))->r;}}else{
# 3847
if(_tmp56A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp56A;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp56C))->r;}else{
# 3852
e->r=(Cyc_Toc_malloc_exp(*_tmp56B,_tmp56C))->r;}}}
# 3856
goto _LL279;}_LL2C0: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp56E=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp56E->tag != 34)goto _LL2C2;else{_tmp56F=_tmp56E->f1;_tmp570=_tmp56E->f2;}}_LL2C1: {
# 3865
int is_dyneither_ptr=0;
void*e1_old_typ=(void*)_check_null(_tmp56F->topt);
void*e2_old_typ=(void*)_check_null(_tmp570->topt);
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
const char*_tmpD32;void*_tmpD31;(_tmpD31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD32="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD32,sizeof(char),57))),_tag_dyneither(_tmpD31,sizeof(void*),0)));}{
# 3872
struct Cyc_Absyn_Exp*swap_fn;
if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3876
swap_fn=Cyc_Toc__swap_word_e;}{
# 3880
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp56F,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp570,& f2_tag,& tagged_mem_type2,1);
# 3887
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp56F);
Cyc_Toc_exp_to_c(nv,_tmp570);
Cyc_Toc_set_lhs(nv,0);
# 3893
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp56F,f1_tag,tagged_mem_type1);else{
# 3897
_tmp56F=Cyc_Toc_push_address_exp(_tmp56F);}
# 3899
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp570,f2_tag,tagged_mem_type2);else{
# 3903
_tmp570=Cyc_Toc_push_address_exp(_tmp570);}
# 3905
{struct Cyc_Absyn_Exp*_tmpD33[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD33[1]=_tmp570,((_tmpD33[0]=_tmp56F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD33,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3907
goto _LL279;};};}_LL2C2: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp571=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp571->tag != 37)goto _LL2C4;else{_tmp572=_tmp571->f1;_tmp573=_tmp571->f2;}}_LL2C3: {
# 3910
void*_tmp77D=Cyc_Tcutil_compress((void*)_check_null(_tmp572->topt));
Cyc_Toc_exp_to_c(nv,_tmp572);
{void*_tmp77E=_tmp77D;struct Cyc_Absyn_Aggrdecl*_tmp780;_LL38D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp77F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp77E;if(_tmp77F->tag != 11)goto _LL38F;else{if((((_tmp77F->f1).aggr_info).KnownAggr).tag != 2)goto _LL38F;_tmp780=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp77F->f1).aggr_info).KnownAggr).val);}}_LL38E: {
# 3914
struct Cyc_Absyn_Exp*_tmp781=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp780,_tmp573),0);
struct Cyc_Absyn_Exp*_tmp782=Cyc_Toc_member_exp(_tmp572,_tmp573,0);
struct Cyc_Absyn_Exp*_tmp783=Cyc_Toc_member_exp(_tmp782,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp783,_tmp781,0))->r;
goto _LL38C;}_LL38F:;_LL390: {
const char*_tmpD36;void*_tmpD35;(_tmpD35=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD36="non-aggregate type in tagcheck",_tag_dyneither(_tmpD36,sizeof(char),31))),_tag_dyneither(_tmpD35,sizeof(void*),0)));}_LL38C:;}
# 3921
goto _LL279;}_LL2C4: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp574=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp574->tag != 36)goto _LL2C6;else{_tmp575=_tmp574->f1;}}_LL2C5:
 Cyc_Toc_stmt_to_c(nv,_tmp575);goto _LL279;_LL2C6: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp576=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp576->tag != 35)goto _LL2C8;}_LL2C7: {
const char*_tmpD39;void*_tmpD38;(_tmpD38=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD39="UnresolvedMem",_tag_dyneither(_tmpD39,sizeof(char),14))),_tag_dyneither(_tmpD38,sizeof(void*),0)));}_LL2C8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp577=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp577->tag != 24)goto _LL2CA;}_LL2C9: {
const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD3C="compoundlit",_tag_dyneither(_tmpD3C,sizeof(char),12))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}_LL2CA: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp578=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp578->tag != 38)goto _LL2CC;}_LL2CB: {
const char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3F="valueof(-)",_tag_dyneither(_tmpD3F,sizeof(char),11))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}_LL2CC: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp579=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp500;if(_tmp579->tag != 39)goto _LL279;}_LL2CD: {
const char*_tmpD42;void*_tmpD41;(_tmpD41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD42="__asm__",_tag_dyneither(_tmpD42,sizeof(char),8))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}_LL279:;};}
# 3958 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3961
Cyc_Toc_skip_stmt_dl(),0);}
# 3964
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp78E=e->r;struct Cyc_Absyn_Exp*_tmp790;struct Cyc_Absyn_Exp*_tmp792;struct Cyc_Absyn_Exp*_tmp794;_LL392: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp78F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp78E;if(_tmp78F->tag != 13)goto _LL394;else{_tmp790=_tmp78F->f2;}}_LL393:
 return Cyc_Toc_path_length(_tmp790);_LL394: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp791=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp78E;if(_tmp791->tag != 19)goto _LL396;else{_tmp792=_tmp791->f1;}}_LL395:
 return 1 + Cyc_Toc_path_length(_tmp792);_LL396: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp793=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp78E;if(_tmp793->tag != 20)goto _LL398;else{_tmp794=_tmp793->f1;}}_LL397:
 return Cyc_Toc_path_length(_tmp794);_LL398:;_LL399:
 return 0;_LL391:;}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3973
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3978
struct Cyc_Absyn_Stmt*s;
{void*_tmp795=p->r;struct _tuple0*_tmp797;struct Cyc_Absyn_Pat*_tmp798;struct _tuple0*_tmp79A;struct Cyc_Absyn_Vardecl*_tmp79D;struct Cyc_Absyn_Pat*_tmp79E;enum Cyc_Absyn_Sign _tmp7A1;int _tmp7A2;char _tmp7A4;struct _dyneither_ptr _tmp7A6;int _tmp7A7;struct Cyc_Absyn_Enumdecl*_tmp7A9;struct Cyc_Absyn_Enumfield*_tmp7AA;void*_tmp7AC;struct Cyc_Absyn_Enumfield*_tmp7AD;struct Cyc_Absyn_Datatypedecl*_tmp7B0;struct Cyc_Absyn_Datatypefield*_tmp7B1;struct Cyc_List_List*_tmp7B2;struct Cyc_List_List*_tmp7B4;struct Cyc_List_List*_tmp7B6;union Cyc_Absyn_AggrInfoU _tmp7B9;struct Cyc_List_List*_tmp7BA;struct Cyc_Absyn_Pat*_tmp7BC;_LL39B: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp796=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp796->tag != 1)goto _LL39D;else{_tmp797=(_tmp796->f1)->name;_tmp798=_tmp796->f2;}}_LL39C: {
# 3982
struct _tuple0*v=Cyc_Toc_temp_var();
void*_tmp7C0=(void*)_check_null(_tmp798->topt);
{struct _tuple24*_tmpD45;struct Cyc_List_List*_tmpD44;decls=((_tmpD44=_region_malloc(rgn,sizeof(*_tmpD44)),((_tmpD44->hd=((_tmpD45=_region_malloc(rgn,sizeof(*_tmpD45)),((_tmpD45->f1=v,((_tmpD45->f2=Cyc_Toc_typ_to_c_array(_tmp7C0),_tmpD45)))))),((_tmpD44->tl=decls,_tmpD44))))));}{
struct _tuple23 _tmp7C3=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp797,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7C0,
Cyc_Absyn_var_exp(v,0),path,_tmp798,fail_stmt,decls);
nv=_tmp7C3.f1;
decls=_tmp7C3.f2;{
struct Cyc_Absyn_Stmt*_tmp7C4=_tmp7C3.f3;
struct Cyc_Absyn_Stmt*_tmp7C5=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7C5,_tmp7C4,0);
goto _LL39A;};};}_LL39D: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp799=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp799->tag != 3)goto _LL39F;else{_tmp79A=(_tmp799->f2)->name;}}_LL39E: {
# 3995
struct _tuple0*_tmp7C6=Cyc_Toc_temp_var();
void*_tmp7C7=(void*)_check_null(p->topt);
{struct _tuple24*_tmpD48;struct Cyc_List_List*_tmpD47;decls=((_tmpD47=_region_malloc(rgn,sizeof(*_tmpD47)),((_tmpD47->hd=((_tmpD48=_region_malloc(rgn,sizeof(*_tmpD48)),((_tmpD48->f1=_tmp7C6,((_tmpD48->f2=Cyc_Toc_typ_to_c_array(_tmp7C7),_tmpD48)))))),((_tmpD47->tl=decls,_tmpD47))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp79A,Cyc_Absyn_var_exp(_tmp7C6,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C6,0),r,0);
goto _LL39A;}_LL39F: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp79B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp79B->tag != 0)goto _LL3A1;}_LL3A0:
 s=Cyc_Toc_skip_stmt_dl();goto _LL39A;_LL3A1: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp79C=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp79C->tag != 2)goto _LL3A3;else{_tmp79D=_tmp79C->f1;_tmp79E=_tmp79C->f2;}}_LL3A2: {
# 4004
struct _tuple0*_tmp7CA=Cyc_Toc_temp_var();
{struct _tuple24*_tmpD4B;struct Cyc_List_List*_tmpD4A;decls=((_tmpD4A=_region_malloc(rgn,sizeof(*_tmpD4A)),((_tmpD4A->hd=((_tmpD4B=_region_malloc(rgn,sizeof(*_tmpD4B)),((_tmpD4B->f1=_tmp7CA,((_tmpD4B->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD4B)))))),((_tmpD4A->tl=decls,_tmpD4A))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp79D->name,Cyc_Absyn_var_exp(_tmp7CA,0));
# 4008
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7CA,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple23 _tmp7CD=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp79E,fail_stmt,decls);
_tmp7CD.f3=Cyc_Absyn_seq_stmt(s,_tmp7CD.f3,0);
return _tmp7CD;};}_LL3A3: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp79F=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp79F->tag != 8)goto _LL3A5;}_LL3A4:
# 4015
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL39A;_LL3A5: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp7A0=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7A0->tag != 9)goto _LL3A7;else{_tmp7A1=_tmp7A0->f1;_tmp7A2=_tmp7A0->f2;}}_LL3A6:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp7A1,_tmp7A2,0),fail_stmt);goto _LL39A;_LL3A7: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp7A3=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7A3->tag != 10)goto _LL3A9;else{_tmp7A4=_tmp7A3->f1;}}_LL3A8:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp7A4,0),fail_stmt);goto _LL39A;_LL3A9: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp7A5=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7A5->tag != 11)goto _LL3AB;else{_tmp7A6=_tmp7A5->f1;_tmp7A7=_tmp7A5->f2;}}_LL3AA:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7A6,_tmp7A7,0),fail_stmt);goto _LL39A;_LL3AB: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7A8=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7A8->tag != 12)goto _LL3AD;else{_tmp7A9=_tmp7A8->f1;_tmp7AA=_tmp7A8->f2;}}_LL3AC:
# 4020
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD4E;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD4D;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD4E.tag=31,((_tmpD4E.f1=_tmp7AA->name,((_tmpD4E.f2=_tmp7A9,((_tmpD4E.f3=_tmp7AA,_tmpD4E)))))))),_tmpD4D)))),0),fail_stmt);}
goto _LL39A;_LL3AD: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7AB=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7AB->tag != 13)goto _LL3AF;else{_tmp7AC=(void*)_tmp7AB->f1;_tmp7AD=_tmp7AB->f2;}}_LL3AE:
# 4023
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD51;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD50;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD51.tag=32,((_tmpD51.f1=_tmp7AD->name,((_tmpD51.f2=_tmp7AC,((_tmpD51.f3=_tmp7AD,_tmpD51)))))))),_tmpD50)))),0),fail_stmt);}
goto _LL39A;_LL3AF: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7AE=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7AE->tag != 5)goto _LL3B1;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7AF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp7AE->f1)->r;if(_tmp7AF->tag != 7)goto _LL3B1;else{_tmp7B0=_tmp7AF->f1;_tmp7B1=_tmp7AF->f2;_tmp7B2=_tmp7AF->f3;}}}_LL3B0:
# 4033
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple0*_tmp7D2=Cyc_Toc_temp_var();
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp7B1->name,_tmp7B0->name);
void*_tmp7D3=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7D3,r);
struct Cyc_List_List*_tmp7D4=_tmp7B1->typs;
for(0;_tmp7B2 != 0;(((_tmp7B2=_tmp7B2->tl,_tmp7D4=((struct Cyc_List_List*)_check_null(_tmp7D4))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7D5=(struct Cyc_Absyn_Pat*)_tmp7B2->hd;
if(_tmp7D5->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7D6=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp7D4))->hd)).f2;
void*_tmp7D7=(void*)_check_null(_tmp7D5->topt);
void*_tmp7D8=Cyc_Toc_typ_to_c_array(_tmp7D7);
struct Cyc_Absyn_Exp*_tmp7D9=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7D2,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7D6)))
_tmp7D9=Cyc_Toc_cast_it(_tmp7D8,_tmp7D9);{
# 4051
struct _tuple23 _tmp7DA=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D7,_tmp7D9,_tmp7D9,_tmp7D5,fail_stmt,decls);
# 4053
nv=_tmp7DA.f1;
decls=_tmp7DA.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7DA.f3,0);};};}{
# 4057
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7D2,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp7B0->is_extensible?Cyc_Absyn_var_exp(_tmp7B1->name,0): Cyc_Toc_datatype_tag(_tmp7B0,_tmp7B1->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 4065
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7D2,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 4068
s=Cyc_Absyn_declare_stmt(_tmp7D2,_tmp7D3,rcast,s,0);
goto _LL39A;};};_LL3B1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7B3=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7B3->tag != 7)goto _LL3B3;else{_tmp7B4=_tmp7B3->f3;}}_LL3B2:
# 4071
 _tmp7B6=_tmp7B4;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7B5=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7B5->tag != 4)goto _LL3B5;else{_tmp7B6=_tmp7B5->f1;}}_LL3B4:
# 4079
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp7B6 != 0;(_tmp7B6=_tmp7B6->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7DB=(struct Cyc_Absyn_Pat*)_tmp7B6->hd;
if(_tmp7DB->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7DC=(void*)_check_null(_tmp7DB->topt);
struct _dyneither_ptr*_tmp7DD=Cyc_Absyn_fieldname(cnt);
struct _tuple23 _tmp7DE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7DC,Cyc_Toc_member_exp(r,_tmp7DD,0),
Cyc_Toc_member_exp(path,_tmp7DD,0),_tmp7DB,fail_stmt,decls);
nv=_tmp7DE.f1;
decls=_tmp7DE.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7DE.f3,0);};}
# 4093
goto _LL39A;};_LL3B5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7B7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7B7->tag != 6)goto _LL3B7;else{if(_tmp7B7->f1 != 0)goto _LL3B7;}}_LL3B6: {
# 4095
const char*_tmpD54;void*_tmpD53;(_tmpD53=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD54="unresolved aggregate pattern!",_tag_dyneither(_tmpD54,sizeof(char),30))),_tag_dyneither(_tmpD53,sizeof(void*),0)));}_LL3B7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7B8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7B8->tag != 6)goto _LL3B9;else{if(_tmp7B8->f1 == 0)goto _LL3B9;_tmp7B9=(_tmp7B8->f1)->aggr_info;_tmp7BA=_tmp7B8->f3;}}_LL3B8: {
# 4097
struct Cyc_Absyn_Aggrdecl*_tmp7E1=Cyc_Absyn_get_known_aggrdecl(_tmp7B9);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E1->impl))->tagged){
# 4101
struct Cyc_List_List*_tmp7E3;struct Cyc_Absyn_Pat*_tmp7E4;struct _tuple25*_tmp7E2=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7BA))->hd;_tmp7E3=_tmp7E2->f1;_tmp7E4=_tmp7E2->f2;{
struct _dyneither_ptr*f;
{void*_tmp7E5=(void*)((struct Cyc_List_List*)_check_null(_tmp7E3))->hd;struct _dyneither_ptr*_tmp7E7;_LL3C2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7E6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7E5;if(_tmp7E6->tag != 1)goto _LL3C4;else{_tmp7E7=_tmp7E6->f1;}}_LL3C3:
 f=_tmp7E7;goto _LL3C1;_LL3C4:;_LL3C5: {
const char*_tmpD57;void*_tmpD56;(_tmpD56=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD57="no field name in tagged union pattern",_tag_dyneither(_tmpD57,sizeof(char),38))),_tag_dyneither(_tmpD56,sizeof(void*),0)));}_LL3C1:;}{
# 4108
void*_tmp7EA=(void*)_check_null(_tmp7E4->topt);
void*_tmp7EB=Cyc_Toc_typ_to_c_array(_tmp7EA);
# 4111
struct Cyc_Absyn_Exp*_tmp7EC=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7ED=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7ED=Cyc_Toc_cast_it(_tmp7EB,_tmp7ED);
_tmp7EC=Cyc_Toc_cast_it(_tmp7EB,_tmp7EC);{
struct _tuple23 _tmp7EE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7EA,_tmp7ED,_tmp7EC,_tmp7E4,fail_stmt,decls);
# 4119
nv=_tmp7EE.f1;
decls=_tmp7EE.f2;{
struct Cyc_Absyn_Stmt*_tmp7EF=_tmp7EE.f3;
struct Cyc_Absyn_Stmt*_tmp7F0=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7E1,f),0),fail_stmt);
# 4127
s=Cyc_Absyn_seq_stmt(_tmp7F0,_tmp7EF,0);};};};};}else{
# 4130
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7BA != 0;_tmp7BA=_tmp7BA->tl){
struct _tuple25*_tmp7F1=(struct _tuple25*)_tmp7BA->hd;
struct Cyc_Absyn_Pat*_tmp7F2=(*_tmp7F1).f2;
if(_tmp7F2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7F3=(void*)((struct Cyc_List_List*)_check_null((*_tmp7F1).f1))->hd;struct _dyneither_ptr*_tmp7F5;_LL3C7: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7F4=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7F3;if(_tmp7F4->tag != 1)goto _LL3C9;else{_tmp7F5=_tmp7F4->f1;}}_LL3C8:
 f=_tmp7F5;goto _LL3C6;_LL3C9:;_LL3CA: {
struct Cyc_Toc_Match_error_exn_struct _tmpD5A;struct Cyc_Toc_Match_error_exn_struct*_tmpD59;(int)_throw((void*)((_tmpD59=_cycalloc_atomic(sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD5A.tag=Cyc_Toc_Match_error,_tmpD5A)),_tmpD59)))));}_LL3C6:;}{
# 4141
void*_tmp7F8=(void*)_check_null(_tmp7F2->topt);
void*_tmp7F9=Cyc_Toc_typ_to_c_array(_tmp7F8);
struct Cyc_Absyn_Exp*_tmp7FA=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E1->impl))->fields,f)))->type))
_tmp7FA=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp7F9),_tmp7FA);{
struct _tuple23 _tmp7FB=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F8,_tmp7FA,
Cyc_Toc_member_exp(path,f,0),_tmp7F2,fail_stmt,decls);
nv=_tmp7FB.f1;
decls=_tmp7FB.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7FB.f3,0);};};};}}
# 4153
goto _LL39A;}_LL3B9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7BB=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7BB->tag != 5)goto _LL3BB;else{_tmp7BC=_tmp7BB->f1;}}_LL3BA: {
# 4156
void*_tmp7FC=(void*)_check_null(_tmp7BC->topt);
# 4159
struct _tuple23 _tmp7FD=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FC,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7BC,fail_stmt,decls);
# 4161
nv=_tmp7FD.f1;
decls=_tmp7FD.f2;{
struct Cyc_Absyn_Stmt*_tmp7FE=_tmp7FD.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4167
Cyc_Toc_skip_stmt_dl(),0),_tmp7FE,0);else{
# 4169
s=_tmp7FE;}
goto _LL39A;};}_LL3BB: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7BD=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7BD->tag != 14)goto _LL3BD;}_LL3BC: {
const char*_tmpD5D;void*_tmpD5C;(_tmpD5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5D="unknownid pat",_tag_dyneither(_tmpD5D,sizeof(char),14))),_tag_dyneither(_tmpD5C,sizeof(void*),0)));}_LL3BD: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7BE=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7BE->tag != 15)goto _LL3BF;}_LL3BE: {
const char*_tmpD60;void*_tmpD5F;(_tmpD5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD60="unknowncall pat",_tag_dyneither(_tmpD60,sizeof(char),16))),_tag_dyneither(_tmpD5F,sizeof(void*),0)));}_LL3BF: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7BF=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp795;if(_tmp7BF->tag != 16)goto _LL39A;}_LL3C0: {
const char*_tmpD63;void*_tmpD62;(_tmpD62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD63="exp pat",_tag_dyneither(_tmpD63,sizeof(char),8))),_tag_dyneither(_tmpD62,sizeof(void*),0)));}_LL39A:;}{
# 4175
struct _tuple23 _tmpD64;return(_tmpD64.f1=nv,((_tmpD64.f2=decls,((_tmpD64.f3=s,_tmpD64)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4212 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4214
struct _tuple26*_tmpD65;return(_tmpD65=_region_malloc(r,sizeof(*_tmpD65)),((_tmpD65->f1=Cyc_Toc_fresh_label(),((_tmpD65->f2=Cyc_Toc_fresh_label(),((_tmpD65->f3=sc,_tmpD65)))))));}
# 4217
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4219
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp807=(void*)_check_null(e->topt);
# 4224
int leave_as_switch;
{void*_tmp808=Cyc_Tcutil_compress(_tmp807);_LL3CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp809=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp808;if(_tmp809->tag != 6)goto _LL3CE;}_LL3CD:
 goto _LL3CF;_LL3CE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp80A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp808;if(_tmp80A->tag != 13)goto _LL3D0;}_LL3CF:
 leave_as_switch=1;goto _LL3CB;_LL3D0:;_LL3D1:
 leave_as_switch=0;goto _LL3CB;_LL3CB:;}
# 4230
{struct Cyc_List_List*_tmp80B=scs;for(0;_tmp80B != 0;_tmp80B=_tmp80B->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp80B->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp80B->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4235
if(leave_as_switch){
# 4237
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp80C=scs;for(0;_tmp80C != 0;_tmp80C=_tmp80C->tl){
struct Cyc_Absyn_Stmt*_tmp80D=((struct Cyc_Absyn_Switch_clause*)_tmp80C->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp80C->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp80D,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp80E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp80E;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp80D);;_pop_region(rgn);};}}
# 4246
return;}{
# 4249
struct _tuple0*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4254
struct _RegionHandle _tmp80F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp80F;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp810=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4265
{struct Cyc_List_List*_tmp811=lscs;for(0;_tmp811 != 0;_tmp811=_tmp811->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp811->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp811->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp811->tl))->hd)).f1;
struct Cyc_Toc_Env*_tmp813;struct Cyc_List_List*_tmp814;struct Cyc_Absyn_Stmt*_tmp815;struct _tuple23 _tmp812=Cyc_Toc_xlate_pat(_tmp810,rgn,_tmp807,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp813=_tmp812.f1;_tmp814=_tmp812.f2;_tmp815=_tmp812.f3;
# 4271
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp816=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp813,_tmp816);
_tmp815=Cyc_Absyn_seq_stmt(_tmp815,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp816,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4278
decls=_tmp814;
{struct Cyc_List_List*_tmpD66;nvs=((_tmpD66=_region_malloc(rgn,sizeof(*_tmpD66)),((_tmpD66->hd=_tmp813,((_tmpD66->tl=nvs,_tmpD66))))));}{
struct Cyc_List_List*_tmpD67;test_stmts=((_tmpD67=_region_malloc(rgn,sizeof(*_tmpD67)),((_tmpD67->hd=_tmp815,((_tmpD67->tl=test_stmts,_tmpD67))))));};}}
# 4282
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4287
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _dyneither_ptr*_tmp81A;struct _dyneither_ptr*_tmp81B;struct Cyc_Absyn_Switch_clause*_tmp81C;struct _tuple26*_tmp819=(struct _tuple26*)lscs->hd;_tmp81A=_tmp819->f1;_tmp81B=_tmp819->f2;_tmp81C=_tmp819->f3;{
struct Cyc_Toc_Env*_tmp81D=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp81C->body;
struct _RegionHandle _tmp81E=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp81E;_push_region(rgn2);
if(lscs->tl != 0){
struct _dyneither_ptr*_tmp820;struct Cyc_Absyn_Switch_clause*_tmp821;struct _tuple26*_tmp81F=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp820=_tmp81F->f2;_tmp821=_tmp81F->f3;
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp81D,end_l,_tmp820,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp821->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 4300
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp81D,end_l),s);}
# 4302
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp81A,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp81B,s,0),0);{
struct Cyc_List_List*_tmpD68;stmts=((_tmpD68=_region_malloc(rgn,sizeof(*_tmpD68)),((_tmpD68->hd=s,((_tmpD68->tl=stmts,_tmpD68))))));};
# 4292
;_pop_region(rgn2);};}{
# 4306
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4309
for(decls;decls != 0;decls=decls->tl){
struct _tuple0*_tmp824;void*_tmp825;struct _tuple24*_tmp823=(struct _tuple24*)decls->hd;_tmp824=_tmp823->f1;_tmp825=_tmp823->f2;
res=Cyc_Absyn_declare_stmt(_tmp824,_tmp825,0,res,0);}
# 4314
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4255
;_pop_region(rgn);};};}
# 4319
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4325
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4327
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD69;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD69->tl=0,_tmpD69)))))),0),0);}
# 4331
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 4336
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4338
while(1){
void*_tmp827=s->r;struct Cyc_Absyn_Exp*_tmp82A;struct Cyc_Absyn_Stmt*_tmp82C;struct Cyc_Absyn_Stmt*_tmp82D;struct Cyc_Absyn_Exp*_tmp82F;struct Cyc_Absyn_Exp*_tmp831;struct Cyc_Absyn_Stmt*_tmp832;struct Cyc_Absyn_Stmt*_tmp833;struct Cyc_Absyn_Exp*_tmp835;struct Cyc_Absyn_Stmt*_tmp836;struct Cyc_Absyn_Stmt*_tmp838;struct Cyc_Absyn_Stmt*_tmp83A;struct Cyc_Absyn_Stmt*_tmp83C;struct Cyc_Absyn_Exp*_tmp83E;struct Cyc_Absyn_Exp*_tmp83F;struct Cyc_Absyn_Exp*_tmp840;struct Cyc_Absyn_Stmt*_tmp841;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_List_List*_tmp844;struct Cyc_List_List*_tmp846;struct Cyc_Absyn_Switch_clause**_tmp847;struct Cyc_Absyn_Decl*_tmp849;struct Cyc_Absyn_Stmt*_tmp84A;struct _dyneither_ptr*_tmp84C;struct Cyc_Absyn_Stmt*_tmp84D;struct Cyc_Absyn_Stmt*_tmp84F;struct Cyc_Absyn_Exp*_tmp850;struct Cyc_Absyn_Stmt*_tmp852;struct Cyc_List_List*_tmp853;struct Cyc_Absyn_Exp*_tmp855;_LL3D3: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp828=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp828->tag != 0)goto _LL3D5;}_LL3D4:
# 4341
 return;_LL3D5: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp829=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp829->tag != 1)goto _LL3D7;else{_tmp82A=_tmp829->f1;}}_LL3D6:
# 4343
 Cyc_Toc_exp_to_c(nv,_tmp82A);
return;_LL3D7: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp82B=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp82B->tag != 2)goto _LL3D9;else{_tmp82C=_tmp82B->f1;_tmp82D=_tmp82B->f2;}}_LL3D8:
# 4346
 Cyc_Toc_stmt_to_c(nv,_tmp82C);
s=_tmp82D;
continue;_LL3D9: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp82E=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp82E->tag != 3)goto _LL3DB;else{_tmp82F=_tmp82E->f1;}}_LL3DA: {
# 4350
void*topt=0;
if(_tmp82F != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp82F->topt));
Cyc_Toc_exp_to_c(nv,_tmp82F);}
# 4356
if(s->try_depth > 0){
if(topt != 0){
struct _tuple0*_tmp856=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp857=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp856,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp856,topt,_tmp82F,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp857,0),0))->r;}else{
# 4364
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4366
return;}_LL3DB: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp830=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp830->tag != 4)goto _LL3DD;else{_tmp831=_tmp830->f1;_tmp832=_tmp830->f2;_tmp833=_tmp830->f3;}}_LL3DC:
# 4368
 Cyc_Toc_exp_to_c(nv,_tmp831);
Cyc_Toc_stmt_to_c(nv,_tmp832);
s=_tmp833;
continue;_LL3DD: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp834=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp834->tag != 5)goto _LL3DF;else{_tmp835=(_tmp834->f1).f1;_tmp836=_tmp834->f2;}}_LL3DE:
# 4373
 Cyc_Toc_exp_to_c(nv,_tmp835);{
struct _RegionHandle _tmp858=_new_region("temp");struct _RegionHandle*temp=& _tmp858;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp836);
# 4377
_npop_handler(0);return;
# 4374
;_pop_region(temp);};_LL3DF: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp837=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp837->tag != 6)goto _LL3E1;else{_tmp838=_tmp837->f1;}}_LL3E0: {
# 4379
struct _dyneither_ptr**_tmp85A;struct Cyc_Toc_Env*_tmp859=nv;_tmp85A=_tmp859->break_lab;
if(_tmp85A != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp85A,0);{
# 4383
int dest_depth=_tmp838 == 0?0: _tmp838->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL3E1: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp839=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp839->tag != 7)goto _LL3E3;else{_tmp83A=_tmp839->f1;}}_LL3E2: {
# 4387
struct _dyneither_ptr**_tmp85C;struct Cyc_Toc_Env*_tmp85B=nv;_tmp85C=_tmp85B->continue_lab;
if(_tmp85C != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp85C,0);
_tmp83C=_tmp83A;goto _LL3E4;}_LL3E3: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp83B=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp83B->tag != 8)goto _LL3E5;else{_tmp83C=_tmp83B->f2;}}_LL3E4:
# 4393
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp83C))->try_depth,s);
return;_LL3E5: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp83D=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp83D->tag != 9)goto _LL3E7;else{_tmp83E=_tmp83D->f1;_tmp83F=(_tmp83D->f2).f1;_tmp840=(_tmp83D->f3).f1;_tmp841=_tmp83D->f4;}}_LL3E6:
# 4397
 Cyc_Toc_exp_to_c(nv,_tmp83E);Cyc_Toc_exp_to_c(nv,_tmp83F);Cyc_Toc_exp_to_c(nv,_tmp840);{
struct _RegionHandle _tmp85D=_new_region("temp");struct _RegionHandle*temp=& _tmp85D;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp841);
# 4401
_npop_handler(0);return;
# 4398
;_pop_region(temp);};_LL3E7: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp842=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp842->tag != 10)goto _LL3E9;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;}}_LL3E8:
# 4403
 Cyc_Toc_xlate_switch(nv,s,_tmp843,_tmp844);
return;_LL3E9: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp845=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp845->tag != 11)goto _LL3EB;else{_tmp846=_tmp845->f1;_tmp847=_tmp845->f2;}}_LL3EA: {
# 4406
struct Cyc_Toc_FallthruInfo*_tmp85F;struct Cyc_Toc_Env*_tmp85E=nv;_tmp85F=_tmp85E->fallthru_info;
if(_tmp85F == 0){
const char*_tmpD6C;void*_tmpD6B;(_tmpD6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6C="fallthru in unexpected place",_tag_dyneither(_tmpD6C,sizeof(char),29))),_tag_dyneither(_tmpD6B,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp863;struct Cyc_List_List*_tmp864;struct Cyc_Dict_Dict _tmp865;struct Cyc_Toc_FallthruInfo _tmp862=*_tmp85F;_tmp863=_tmp862.label;_tmp864=_tmp862.binders;_tmp865=_tmp862.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp863,0);
# 4412
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp847)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp866=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp864);
struct Cyc_List_List*_tmp867=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp846);
for(0;_tmp866 != 0;(_tmp866=_tmp866->tl,_tmp867=_tmp867->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp867))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp865,(struct _tuple0*)_tmp866->hd),(struct Cyc_Absyn_Exp*)_tmp867->hd,0),s2,0);}
# 4420
s->r=s2->r;
return;};};};}_LL3EB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp848=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp848->tag != 12)goto _LL3ED;else{_tmp849=_tmp848->f1;_tmp84A=_tmp848->f2;}}_LL3EC:
# 4426
{void*_tmp868=_tmp849->r;struct Cyc_Absyn_Vardecl*_tmp86A;struct Cyc_Absyn_Pat*_tmp86C;struct Cyc_Absyn_Exp*_tmp86D;struct Cyc_List_List*_tmp86F;struct Cyc_Absyn_Fndecl*_tmp871;struct Cyc_Absyn_Tvar*_tmp873;struct Cyc_Absyn_Vardecl*_tmp874;int _tmp875;struct Cyc_Absyn_Exp*_tmp876;struct Cyc_Absyn_Tvar*_tmp878;struct Cyc_Absyn_Vardecl*_tmp879;_LL3F6: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp869=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp868;if(_tmp869->tag != 0)goto _LL3F8;else{_tmp86A=_tmp869->f1;}}_LL3F7: {
# 4428
struct _RegionHandle _tmp87A=_new_region("temp");struct _RegionHandle*temp=& _tmp87A;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD6F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD6E;struct Cyc_Toc_Env*_tmp87B=Cyc_Toc_add_varmap(temp,nv,_tmp86A->name,
Cyc_Absyn_varb_exp(_tmp86A->name,(void*)((_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD6F.tag=4,((_tmpD6F.f1=_tmp86A,_tmpD6F)))),_tmpD6E)))),0));
Cyc_Toc_local_decl_to_c(_tmp87B,_tmp87B,_tmp86A,_tmp84A);}
# 4433
_npop_handler(0);goto _LL3F5;
# 4428
;_pop_region(temp);}_LL3F8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp86B=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp868;if(_tmp86B->tag != 2)goto _LL3FA;else{_tmp86C=_tmp86B->f1;_tmp86D=_tmp86B->f3;}}_LL3F9:
# 4437
{void*_tmp87E=_tmp86C->r;struct Cyc_Absyn_Vardecl*_tmp880;_LL405: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp87F=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp87E;if(_tmp87F->tag != 1)goto _LL407;else{_tmp880=_tmp87F->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp881=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp87F->f2)->r;if(_tmp881->tag != 0)goto _LL407;};}}_LL406: {
# 4439
struct _tuple0*old_name=_tmp880->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp880->name=new_name;
_tmp880->initializer=_tmp86D;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD72;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD71;_tmp849->r=(void*)((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71[0]=((_tmpD72.tag=0,((_tmpD72.f1=_tmp880,_tmpD72)))),_tmpD71))));}{
struct _RegionHandle _tmp884=_new_region("temp");struct _RegionHandle*temp=& _tmp884;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD75;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD74;struct Cyc_Toc_Env*_tmp885=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74[0]=((_tmpD75.tag=4,((_tmpD75.f1=_tmp880,_tmpD75)))),_tmpD74)))),0));
Cyc_Toc_local_decl_to_c(_tmp885,nv,_tmp880,_tmp84A);}
# 4450
_npop_handler(0);goto _LL404;
# 4444
;_pop_region(temp);};}_LL407:;_LL408:
# 4455
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp86C,(void*)_check_null(_tmp86D->topt),_tmp86D,_tmp84A))->r;
goto _LL404;_LL404:;}
# 4458
goto _LL3F5;_LL3FA: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp86E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp868;if(_tmp86E->tag != 3)goto _LL3FC;else{_tmp86F=_tmp86E->f1;}}_LL3FB: {
# 4471 "toc.cyc"
struct Cyc_List_List*_tmp888=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp86F);
if(_tmp888 == 0){
const char*_tmpD78;void*_tmpD77;(_tmpD77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD78="empty Letv_d",_tag_dyneither(_tmpD78,sizeof(char),13))),_tag_dyneither(_tmpD77,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD7B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD7A;_tmp849->r=(void*)((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A[0]=((_tmpD7B.tag=0,((_tmpD7B.f1=(struct Cyc_Absyn_Vardecl*)_tmp888->hd,_tmpD7B)))),_tmpD7A))));}
_tmp888=_tmp888->tl;
for(0;_tmp888 != 0;_tmp888=_tmp888->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD7E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD7D;struct Cyc_Absyn_Decl*_tmp88D=Cyc_Absyn_new_decl((void*)((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7E.tag=0,((_tmpD7E.f1=(struct Cyc_Absyn_Vardecl*)_tmp888->hd,_tmpD7E)))),_tmpD7D)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp88D,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4480
Cyc_Toc_stmt_to_c(nv,s);
goto _LL3F5;}_LL3FC: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp870=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp868;if(_tmp870->tag != 1)goto _LL3FE;else{_tmp871=_tmp870->f1;}}_LL3FD: {
# 4483
struct _tuple0*_tmp890=_tmp871->name;
struct _RegionHandle _tmp891=_new_region("temp");struct _RegionHandle*temp=& _tmp891;_push_region(temp);{
struct Cyc_Toc_Env*_tmp892=Cyc_Toc_add_varmap(temp,nv,_tmp871->name,Cyc_Absyn_var_exp(_tmp890,0));
Cyc_Toc_fndecl_to_c(_tmp892,_tmp871,0);
Cyc_Toc_stmt_to_c(_tmp892,_tmp84A);}
# 4489
_npop_handler(0);goto _LL3F5;
# 4484
;_pop_region(temp);}_LL3FE: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp872=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp868;if(_tmp872->tag != 4)goto _LL400;else{_tmp873=_tmp872->f1;_tmp874=_tmp872->f2;_tmp875=_tmp872->f3;_tmp876=_tmp872->f4;}}_LL3FF: {
# 4491
struct Cyc_Absyn_Stmt*_tmp893=_tmp84A;
# 4507 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple0*rh_var=Cyc_Toc_temp_var();
struct _tuple0*x_var=_tmp874->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4514
struct _RegionHandle _tmp894=_new_region("temp");struct _RegionHandle*temp=& _tmp894;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp893);
# 4517
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp893,0))->r;else{
if(_tmp876 == 0){
struct Cyc_Absyn_Exp*_tmpD81[1];struct Cyc_Absyn_Exp*_tmpD80[1];struct Cyc_List_List*_tmpD7F;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD7F->tl=0,_tmpD7F)))))),0),
# 4526
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD80[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD80,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp893,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD81[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD81,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4532
Cyc_Toc_exp_to_c(nv,_tmp876);{
struct Cyc_Absyn_Exp*_tmpD83[1];struct Cyc_Absyn_Exp*_tmpD82[2];s->r=(
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
(_tmpD82[1]=_tmp876,((_tmpD82[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD82,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4540
Cyc_Absyn_seq_stmt(_tmp893,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
(_tmpD83[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD83,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4545
_npop_handler(0);return;
# 4514
;_pop_region(temp);}_LL400: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp877=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp868;if(_tmp877->tag != 5)goto _LL402;else{_tmp878=_tmp877->f1;_tmp879=_tmp877->f2;}}_LL401: {
# 4548
struct _tuple0*old_name=_tmp879->name;
struct _tuple0*new_name=Cyc_Toc_temp_var();
_tmp879->name=new_name;
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpD92;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD91;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD90;struct Cyc_Absyn_Decl*_tmpD8F;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD8E;s->r=(void*)((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[0]=((_tmpD92.tag=12,((_tmpD92.f1=((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->r=(void*)((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91[0]=((_tmpD90.tag=0,((_tmpD90.f1=_tmp879,_tmpD90)))),_tmpD91)))),((_tmpD8F->loc=0,_tmpD8F)))))),((_tmpD92.f2=_tmp84A,_tmpD92)))))),_tmpD8E))));}{
struct _RegionHandle _tmp89F=_new_region("temp");struct _RegionHandle*temp=& _tmp89F;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD95;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD94;struct Cyc_Toc_Env*_tmp8A0=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94[0]=((_tmpD95.tag=4,((_tmpD95.f1=_tmp879,_tmpD95)))),_tmpD94)))),0));
Cyc_Toc_local_decl_to_c(_tmp8A0,nv,_tmp879,_tmp84A);}
# 4558
_npop_handler(0);return;
# 4552
;_pop_region(temp);};}_LL402:;_LL403: {
# 4560
const char*_tmpD98;void*_tmpD97;(_tmpD97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD98="bad nested declaration within function",_tag_dyneither(_tmpD98,sizeof(char),39))),_tag_dyneither(_tmpD97,sizeof(void*),0)));}_LL3F5:;}
# 4562
return;_LL3ED: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp84B=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp84B->tag != 13)goto _LL3EF;else{_tmp84C=_tmp84B->f1;_tmp84D=_tmp84B->f2;}}_LL3EE:
# 4564
 s=_tmp84D;continue;_LL3EF: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp84E=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp84E->tag != 14)goto _LL3F1;else{_tmp84F=_tmp84E->f1;_tmp850=(_tmp84E->f2).f1;}}_LL3F0: {
# 4566
struct _RegionHandle _tmp8A5=_new_region("temp");struct _RegionHandle*temp=& _tmp8A5;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp84F);
Cyc_Toc_exp_to_c(nv,_tmp850);
# 4570
_npop_handler(0);return;
# 4566
;_pop_region(temp);}_LL3F1: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp851=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp851->tag != 15)goto _LL3F3;else{_tmp852=_tmp851->f1;_tmp853=_tmp851->f2;}}_LL3F2: {
# 4587 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
struct _tuple0*e_var=Cyc_Toc_temp_var();
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4598
e_exp->topt=e_typ;{
struct _RegionHandle _tmp8A6=_new_region("temp");struct _RegionHandle*temp=& _tmp8A6;_push_region(temp);{
struct Cyc_Toc_Env*_tmp8A7=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4602
Cyc_Toc_stmt_to_c(_tmp8A7,_tmp852);{
struct Cyc_Absyn_Stmt*_tmp8A8=Cyc_Absyn_seq_stmt(_tmp852,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4608
struct _tuple0*_tmp8A9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp8AA=Cyc_Absyn_var_exp(_tmp8A9,0);
struct Cyc_Absyn_Vardecl*_tmp8AB=Cyc_Absyn_new_vardecl(_tmp8A9,Cyc_Absyn_exn_typ(),0);
_tmp8AA->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDA2;struct Cyc_Absyn_Pat*_tmpDA1;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDA0;struct Cyc_Absyn_Pat*_tmpD9F;struct Cyc_Absyn_Pat*_tmp8AC=(_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->r=(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA0.tag=1,((_tmpDA0.f1=_tmp8AB,((_tmpDA0.f2=((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDA1->topt=Cyc_Absyn_exn_typ(),((_tmpDA1->loc=0,_tmpDA1)))))))),_tmpDA0)))))),_tmpDA2)))),((_tmpD9F->topt=Cyc_Absyn_exn_typ(),((_tmpD9F->loc=0,_tmpD9F)))))));
struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Absyn_throw_exp(_tmp8AA,0);
_tmp8AD->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp8AE=Cyc_Absyn_exp_stmt(_tmp8AD,0);
struct Cyc_Core_Opt*_tmpDA8;struct Cyc_List_List*_tmpDA7;struct Cyc_Absyn_Switch_clause*_tmpDA6;struct Cyc_Absyn_Switch_clause*_tmp8AF=
(_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->pattern=_tmp8AC,((_tmpDA6->pat_vars=((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->v=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->hd=_tmp8AB,((_tmpDA7->tl=0,_tmpDA7)))))),_tmpDA8)))),((_tmpDA6->where_clause=0,((_tmpDA6->body=_tmp8AE,((_tmpDA6->loc=0,_tmpDA6)))))))))));
# 4619
struct Cyc_List_List*_tmpDA9;struct Cyc_Absyn_Stmt*_tmp8B0=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp853,(
(_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->hd=_tmp8AF,((_tmpDA9->tl=0,_tmpDA9))))))),0);
# 4623
Cyc_Toc_stmt_to_c(_tmp8A7,_tmp8B0);{
# 4626
struct Cyc_List_List*_tmpDAA;struct Cyc_Absyn_Exp*_tmp8B1=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDAA->tl=0,_tmpDAA)))))),0);
# 4630
struct Cyc_List_List*_tmpDAB;struct Cyc_Absyn_Stmt*_tmp8B2=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDAB->tl=0,_tmpDAB)))))),0),0);
# 4634
struct Cyc_Absyn_Exp*_tmp8B3=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp8B2,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp8B3,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp8B1,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8B4,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8A8,
# 4645
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8B0,0),0),0),0),0),0))->r;};};};};}
# 4649
_npop_handler(0);return;
# 4599
;_pop_region(temp);};}_LL3F3: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp854=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp827;if(_tmp854->tag != 16)goto _LL3D2;else{_tmp855=_tmp854->f1;}}_LL3F4:
# 4651
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4654
Cyc_Toc_exp_to_c(nv,_tmp855);{
struct Cyc_List_List*_tmpDAC;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=_tmp855,((_tmpDAC->tl=0,_tmpDAC)))))),0));};}
# 4657
return;_LL3D2:;}}
# 4666
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8C0=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8C0;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8C1=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8C2=f->args;for(0;_tmp8C2 != 0;_tmp8C2=_tmp8C2->tl){
struct _tuple0*_tmpDAD;struct _tuple0*_tmp8C3=(_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->f1=Cyc_Absyn_Loc_n,((_tmpDAD->f2=(*((struct _tuple8*)_tmp8C2->hd)).f1,_tmpDAD)))));
(*((struct _tuple8*)_tmp8C2->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp8C2->hd)).f3);
_tmp8C1=Cyc_Toc_add_varmap(frgn,_tmp8C1,_tmp8C3,Cyc_Absyn_var_exp(_tmp8C3,0));}}
# 4681
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4685
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct _dyneither_ptr*_tmp8C6;struct Cyc_Absyn_Tqual _tmp8C7;void*_tmp8C8;int _tmp8C9;struct Cyc_Absyn_VarargInfo _tmp8C5=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp8C6=_tmp8C5.name;_tmp8C7=_tmp8C5.tq;_tmp8C8=_tmp8C5.type;_tmp8C9=_tmp8C5.inject;{
void*_tmp8CA=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8C8,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C7,Cyc_Absyn_false_conref));
struct _tuple0*_tmpDAE;struct _tuple0*_tmp8CB=(_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->f1=Cyc_Absyn_Loc_n,((_tmpDAE->f2=(struct _dyneither_ptr*)_check_null(_tmp8C6),_tmpDAE)))));
{struct _tuple8*_tmpDB1;struct Cyc_List_List*_tmpDB0;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->hd=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->f1=_tmp8C6,((_tmpDB1->f2=_tmp8C7,((_tmpDB1->f3=_tmp8CA,_tmpDB1)))))))),((_tmpDB0->tl=0,_tmpDB0)))))));}
_tmp8C1=Cyc_Toc_add_varmap(frgn,_tmp8C1,_tmp8CB,Cyc_Absyn_var_exp(_tmp8CB,0));
f->cyc_varargs=0;};}
# 4694
{struct Cyc_List_List*_tmp8CF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8CF != 0;_tmp8CF=_tmp8CF->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8CF->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8CF->hd)->type);}}
# 4697
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8C1),f->body);}
# 4673
;_pop_region(frgn);};}
# 4702
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL409:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL40A:
 return Cyc_Absyn_Extern;default: _LL40B:
 return s;}}
# 4718 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple0*_tmp8D0=ad->name;
struct _DynRegionHandle*_tmp8D2;struct Cyc_Dict_Dict*_tmp8D3;struct Cyc_Toc_TocState _tmp8D1=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8D2=_tmp8D1.dyn;_tmp8D3=_tmp8D1.aggrs_so_far;{
struct _DynRegionFrame _tmp8D4;struct _RegionHandle*d=_open_dynregion(& _tmp8D4,_tmp8D2);
{int seen_defn_before;
struct _tuple15**_tmp8D5=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8D3,_tmp8D0);
if(_tmp8D5 == 0){
seen_defn_before=0;{
struct _tuple15*v;
if(ad->kind == Cyc_Absyn_StructA){
struct _tuple15*_tmpDB2;v=((_tmpDB2=_region_malloc(d,sizeof(*_tmpDB2)),((_tmpDB2->f1=ad,((_tmpDB2->f2=Cyc_Absyn_strctq(_tmp8D0),_tmpDB2))))));}else{
# 4730
struct _tuple15*_tmpDB3;v=((_tmpDB3=_region_malloc(d,sizeof(*_tmpDB3)),((_tmpDB3->f1=ad,((_tmpDB3->f2=Cyc_Absyn_unionq_typ(_tmp8D0),_tmpDB3))))));}
*_tmp8D3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8D3,_tmp8D0,v);};}else{
# 4733
struct Cyc_Absyn_Aggrdecl*_tmp8D9;void*_tmp8DA;struct _tuple15*_tmp8D8=*_tmp8D5;_tmp8D9=_tmp8D8->f1;_tmp8DA=_tmp8D8->f2;
if(_tmp8D9->impl == 0){
{struct _tuple15*_tmpDB4;*_tmp8D3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8D3,_tmp8D0,((_tmpDB4=_region_malloc(d,sizeof(*_tmpDB4)),((_tmpDB4->f1=ad,((_tmpDB4->f2=_tmp8DA,_tmpDB4)))))));}
seen_defn_before=0;}else{
# 4738
seen_defn_before=1;}}{
# 4740
struct Cyc_Absyn_Aggrdecl*_tmpDB5;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->kind=ad->kind,((_tmpDB5->sc=Cyc_Absyn_Public,((_tmpDB5->name=ad->name,((_tmpDB5->tvs=0,((_tmpDB5->impl=0,((_tmpDB5->attributes=ad->attributes,_tmpDB5)))))))))))));
# 4746
if(ad->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpDB6;new_ad->impl=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->exist_vars=0,((_tmpDB6->rgn_po=0,((_tmpDB6->fields=0,((_tmpDB6->tagged=0,_tmpDB6))))))))));}{
# 4751
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8DD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp8DD != 0;_tmp8DD=_tmp8DD->tl){
# 4755
struct Cyc_Absyn_Aggrfield*_tmp8DE=(struct Cyc_Absyn_Aggrfield*)_tmp8DD->hd;
void*_tmp8DF=_tmp8DE->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8DF)) && (
ad->kind == Cyc_Absyn_StructA  && _tmp8DD->tl == 0  || ad->kind == Cyc_Absyn_UnionA)){
# 4765
void*_tmp8E0=Cyc_Tcutil_compress(_tmp8DF);void*_tmp8E2;struct Cyc_Absyn_Tqual _tmp8E3;union Cyc_Absyn_Constraint*_tmp8E4;unsigned int _tmp8E5;_LL40E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8E1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E0;if(_tmp8E1->tag != 8)goto _LL410;else{_tmp8E2=(_tmp8E1->f1).elt_type;_tmp8E3=(_tmp8E1->f1).tq;_tmp8E4=(_tmp8E1->f1).zero_term;_tmp8E5=(_tmp8E1->f1).zt_loc;}}_LL40F:
# 4767
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDBC;struct Cyc_Absyn_ArrayInfo _tmpDBB;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDBA;_tmp8DF=(void*)((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA[0]=((_tmpDBC.tag=8,((_tmpDBC.f1=((_tmpDBB.elt_type=_tmp8E2,((_tmpDBB.tq=_tmp8E3,((_tmpDBB.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDBB.zero_term=_tmp8E4,((_tmpDBB.zt_loc=_tmp8E5,_tmpDBB)))))))))),_tmpDBC)))),_tmpDBA))));}
goto _LL40D;_LL410:;_LL411:
# 4770
 if(Cyc_Toc_array_or_aggr_with_array(_tmp8DF))goto _LL40D;else{
if(ad->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpDBF;void*_tmpDBE;(_tmpDBE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpDBF="union with non-array fields of kind TA",_tag_dyneither(_tmpDBF,sizeof(char),39))),_tag_dyneither(_tmpDBE,sizeof(void*),0)));}}_LL40D:;}{
# 4775
struct Cyc_Absyn_Aggrfield*_tmpDC0;struct Cyc_Absyn_Aggrfield*_tmp8EB=(_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->name=_tmp8DE->name,((_tmpDC0->tq=Cyc_Toc_mt_tq,((_tmpDC0->type=
# 4777
Cyc_Toc_typ_to_c(_tmp8DF),((_tmpDC0->width=_tmp8DE->width,((_tmpDC0->attributes=_tmp8DE->attributes,_tmpDC0)))))))))));
# 4783
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8EC=_tmp8EB->type;
struct _dyneither_ptr*_tmp8ED=_tmp8EB->name;
const char*_tmpDC5;void*_tmpDC4[2];struct Cyc_String_pa_PrintArg_struct _tmpDC3;struct Cyc_String_pa_PrintArg_struct _tmpDC2;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC2.tag=0,((_tmpDC2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8ED),((_tmpDC3.tag=0,((_tmpDC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDC4[0]=& _tmpDC3,((_tmpDC4[1]=& _tmpDC2,Cyc_aprintf(((_tmpDC5="_union_%s_%s",_tag_dyneither(_tmpDC5,sizeof(char),13))),_tag_dyneither(_tmpDC4,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDC6;struct _dyneither_ptr*str=(_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6[0]=s,_tmpDC6)));
struct Cyc_Absyn_Aggrfield*_tmpDC7;struct Cyc_Absyn_Aggrfield*_tmp8EE=(_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->name=Cyc_Toc_val_sp,((_tmpDC7->tq=Cyc_Toc_mt_tq,((_tmpDC7->type=_tmp8EC,((_tmpDC7->width=0,((_tmpDC7->attributes=0,_tmpDC7)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDC8;struct Cyc_Absyn_Aggrfield*_tmp8EF=(_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->name=Cyc_Toc_tag_sp,((_tmpDC8->tq=Cyc_Toc_mt_tq,((_tmpDC8->type=Cyc_Absyn_sint_typ,((_tmpDC8->width=0,((_tmpDC8->attributes=0,_tmpDC8)))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDC9[2];struct Cyc_List_List*_tmp8F0=(_tmpDC9[1]=_tmp8EE,((_tmpDC9[0]=_tmp8EF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDC9,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpDCE;struct _tuple0*_tmpDCD;struct Cyc_Absyn_Aggrdecl*_tmpDCC;struct Cyc_Absyn_Aggrdecl*_tmp8F1=(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->kind=Cyc_Absyn_StructA,((_tmpDCC->sc=Cyc_Absyn_Public,((_tmpDCC->name=(
(_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->f1=Cyc_Absyn_Loc_n,((_tmpDCD->f2=str,_tmpDCD)))))),((_tmpDCC->tvs=0,((_tmpDCC->impl=(
(_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->exist_vars=0,((_tmpDCE->rgn_po=0,((_tmpDCE->fields=_tmp8F0,((_tmpDCE->tagged=0,_tmpDCE)))))))))),((_tmpDCC->attributes=0,_tmpDCC)))))))))))));
# 4795
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDD4;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDD3;struct Cyc_List_List*_tmpDD2;Cyc_Toc_result_decls=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->hd=Cyc_Absyn_new_decl((void*)((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4[0]=((_tmpDD3.tag=6,((_tmpDD3.f1=_tmp8F1,_tmpDD3)))),_tmpDD4)))),0),((_tmpDD2->tl=Cyc_Toc_result_decls,_tmpDD2))))));}
_tmp8EB->type=Cyc_Absyn_strct(str);}{
# 4798
struct Cyc_List_List*_tmpDD5;new_fields=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->hd=_tmp8EB,((_tmpDD5->tl=new_fields,_tmpDD5))))));};};}}
# 4800
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4802
if(add_to_decls){
struct Cyc_Absyn_Decl*_tmpDDF;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDDE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDDD;struct Cyc_List_List*_tmpDDC;Cyc_Toc_result_decls=((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->hd=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->r=(void*)((_tmpDDD=_cycalloc(sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDDE.tag=6,((_tmpDDE.f1=new_ad,_tmpDDE)))),_tmpDDD)))),((_tmpDDF->loc=0,_tmpDDF)))))),((_tmpDDC->tl=Cyc_Toc_result_decls,_tmpDDC))))));}};}
# 4722
;_pop_dynregion(d);};}
# 4831 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp908;struct Cyc_Set_Set**_tmp909;struct Cyc_Toc_TocState _tmp907=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp908=_tmp907.dyn;_tmp909=_tmp907.datatypes_so_far;{
struct _DynRegionFrame _tmp90A;struct _RegionHandle*d=_open_dynregion(& _tmp90A,_tmp908);{
struct _tuple0*_tmp90B=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp909,_tmp90B)){
_npop_handler(0);return;}
*_tmp909=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp909,_tmp90B);}{
# 4839
struct Cyc_List_List*_tmp90C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp90C != 0;_tmp90C=_tmp90C->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp90C->hd;
# 4842
struct Cyc_List_List*_tmp90D=0;
int i=1;
{struct Cyc_List_List*_tmp90E=f->typs;for(0;_tmp90E != 0;(_tmp90E=_tmp90E->tl,i ++)){
struct _dyneither_ptr*_tmp90F=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpDE0;struct Cyc_Absyn_Aggrfield*_tmp910=(_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->name=_tmp90F,((_tmpDE0->tq=(*((struct _tuple10*)_tmp90E->hd)).f1,((_tmpDE0->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp90E->hd)).f2),((_tmpDE0->width=0,((_tmpDE0->attributes=0,_tmpDE0)))))))))));
struct Cyc_List_List*_tmpDE1;_tmp90D=((_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->hd=_tmp910,((_tmpDE1->tl=_tmp90D,_tmpDE1))))));}}
# 4850
{struct Cyc_Absyn_Aggrfield*_tmpDE4;struct Cyc_List_List*_tmpDE3;_tmp90D=((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3->hd=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->name=Cyc_Toc_tag_sp,((_tmpDE4->tq=Cyc_Toc_mt_tq,((_tmpDE4->type=Cyc_Absyn_sint_typ,((_tmpDE4->width=0,((_tmpDE4->attributes=0,_tmpDE4)))))))))))),((_tmpDE3->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp90D),_tmpDE3))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDE7;struct Cyc_Absyn_Aggrdecl*_tmpDE6;struct Cyc_Absyn_Aggrdecl*_tmp915=
(_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->kind=Cyc_Absyn_StructA,((_tmpDE6->sc=Cyc_Absyn_Public,((_tmpDE6->name=
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpDE6->tvs=0,((_tmpDE6->impl=(
# 4856
(_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->exist_vars=0,((_tmpDE7->rgn_po=0,((_tmpDE7->fields=_tmp90D,((_tmpDE7->tagged=0,_tmpDE7)))))))))),((_tmpDE6->attributes=0,_tmpDE6)))))))))))));
# 4858
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDED;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDEC;struct Cyc_List_List*_tmpDEB;Cyc_Toc_result_decls=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->hd=Cyc_Absyn_new_decl((void*)((_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED[0]=((_tmpDEC.tag=6,((_tmpDEC.f1=_tmp915,_tmpDEC)))),_tmpDED)))),0),((_tmpDEB->tl=Cyc_Toc_result_decls,_tmpDEB))))));};}};
# 4833
;_pop_dynregion(d);};}
# 4879 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
if(xd->fields == 0)
return;{
struct _DynRegionHandle*_tmp91C;struct Cyc_Dict_Dict*_tmp91D;struct Cyc_Toc_TocState _tmp91B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp91C=_tmp91B.dyn;_tmp91D=_tmp91B.xdatatypes_so_far;{
struct _DynRegionFrame _tmp91E;struct _RegionHandle*d=_open_dynregion(& _tmp91E,_tmp91C);
{struct _tuple0*_tmp91F=xd->name;
struct Cyc_List_List*_tmp920=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp920 != 0;_tmp920=_tmp920->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp920->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp921=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp922=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp921,Cyc_Absyn_false_conref,0);
# 4891
int*_tmp923=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp91D,f->name);_LL413: if(_tmp923 != 0)goto _LL415;_LL414: {
# 4893
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4897
struct Cyc_Absyn_Vardecl*_tmp924=Cyc_Absyn_new_vardecl(f->name,_tmp922,initopt);
_tmp924->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDF3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDF2;struct Cyc_List_List*_tmpDF1;Cyc_Toc_result_decls=((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->hd=Cyc_Absyn_new_decl((void*)((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF2.tag=0,((_tmpDF2.f1=_tmp924,_tmpDF2)))),_tmpDF3)))),0),((_tmpDF1->tl=Cyc_Toc_result_decls,_tmpDF1))))));}
*_tmp91D=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp91D,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp928=f->typs;for(0;_tmp928 != 0;(_tmp928=_tmp928->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpDFB;void*_tmpDFA[1];const char*_tmpDF9;struct _dyneither_ptr*_tmpDF8;struct _dyneither_ptr*_tmp929=(_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=(struct _dyneither_ptr)((_tmpDFB.tag=1,((_tmpDFB.f1=(unsigned long)i,((_tmpDFA[0]=& _tmpDFB,Cyc_aprintf(((_tmpDF9="f%d",_tag_dyneither(_tmpDF9,sizeof(char),4))),_tag_dyneither(_tmpDFA,sizeof(void*),1)))))))),_tmpDF8)));
struct Cyc_Absyn_Aggrfield*_tmpDFC;struct Cyc_Absyn_Aggrfield*_tmp92A=(_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->name=_tmp929,((_tmpDFC->tq=(*((struct _tuple10*)_tmp928->hd)).f1,((_tmpDFC->type=
Cyc_Toc_typ_to_c((*((struct _tuple10*)_tmp928->hd)).f2),((_tmpDFC->width=0,((_tmpDFC->attributes=0,_tmpDFC)))))))))));
struct Cyc_List_List*_tmpDFD;fields=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->hd=_tmp92A,((_tmpDFD->tl=fields,_tmpDFD))))));}}
# 4910
{struct Cyc_Absyn_Aggrfield*_tmpE00;struct Cyc_List_List*_tmpDFF;fields=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->hd=((_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->name=Cyc_Toc_tag_sp,((_tmpE00->tq=Cyc_Toc_mt_tq,((_tmpE00->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE00->width=0,((_tmpE00->attributes=0,_tmpE00)))))))))))),((_tmpDFF->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpDFF))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE03;struct Cyc_Absyn_Aggrdecl*_tmpE02;struct Cyc_Absyn_Aggrdecl*_tmp933=
(_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->kind=Cyc_Absyn_StructA,((_tmpE02->sc=Cyc_Absyn_Public,((_tmpE02->name=
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpE02->tvs=0,((_tmpE02->impl=(
# 4917
(_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->exist_vars=0,((_tmpE03->rgn_po=0,((_tmpE03->fields=fields,((_tmpE03->tagged=0,_tmpE03)))))))))),((_tmpE02->attributes=0,_tmpE02)))))))))))));
# 4919
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE09;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE08;struct Cyc_List_List*_tmpE07;Cyc_Toc_result_decls=((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->hd=Cyc_Absyn_new_decl((void*)((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09[0]=((_tmpE08.tag=6,((_tmpE08.f1=_tmp933,_tmpE08)))),_tmpE09)))),0),((_tmpE07->tl=Cyc_Toc_result_decls,_tmpE07))))));}
# 4921
goto _LL412;};};};}_LL415: if(_tmp923 == 0)goto _LL417;if(*_tmp923 != 0)goto _LL417;_LL416:
# 4923
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp939=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp93A=Cyc_Absyn_new_vardecl(f->name,_tmp922,_tmp939);
_tmp93A->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE0F;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE0E;struct Cyc_List_List*_tmpE0D;Cyc_Toc_result_decls=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->hd=Cyc_Absyn_new_decl((void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE0E.tag=0,((_tmpE0E.f1=_tmp93A,_tmpE0E)))),_tmpE0F)))),0),((_tmpE0D->tl=Cyc_Toc_result_decls,_tmpE0D))))));}
*_tmp91D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp91D,f->name,1);}
# 4930
goto _LL412;_LL417:;_LL418:
 goto _LL412;_LL412:;}}
# 4884
;_pop_dynregion(d);};};}
# 4937
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4945
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4949
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp93E=init->r;struct Cyc_Absyn_Vardecl*_tmp940;struct Cyc_Absyn_Exp*_tmp941;struct Cyc_Absyn_Exp*_tmp942;int _tmp943;_LL41A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp93F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp93E;if(_tmp93F->tag != 26)goto _LL41C;else{_tmp940=_tmp93F->f1;_tmp941=_tmp93F->f2;_tmp942=_tmp93F->f3;_tmp943=_tmp93F->f4;}}_LL41B:
# 4956
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp940,_tmp941,_tmp942,_tmp943,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL419;_LL41C:;_LL41D:
# 4961
 if(vd->sc == Cyc_Absyn_Static){
# 4965
struct _RegionHandle _tmp944=_new_region("temp");struct _RegionHandle*temp=& _tmp944;_push_region(temp);
{struct Cyc_Toc_Env*_tmp945=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp945,init);}
# 4966
;_pop_region(temp);}else{
# 4970
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL419;_LL419:;}else{
# 4975
void*_tmp946=Cyc_Tcutil_compress(old_typ);void*_tmp948;struct Cyc_Absyn_Exp*_tmp949;union Cyc_Absyn_Constraint*_tmp94A;_LL41F:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp947=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp946;if(_tmp947->tag != 8)goto _LL421;else{_tmp948=(_tmp947->f1).elt_type;_tmp949=(_tmp947->f1).num_elts;_tmp94A=(_tmp947->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp94A))goto _LL421;_LL420:
 if(_tmp949 == 0){
const char*_tmpE12;void*_tmpE11;(_tmpE11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE12="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE12,sizeof(char),55))),_tag_dyneither(_tmpE11,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp949;
struct Cyc_Absyn_Exp*_tmp94D=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4983
struct Cyc_Absyn_Exp*_tmp94E=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp94D,_tmp94E,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL41E;};_LL421:;_LL422:
 goto _LL41E;_LL41E:;}}
# 4997
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple0*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp94F=_new_region("prgn");struct _RegionHandle*prgn=& _tmp94F;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp950=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp951=Cyc_Toc_share_env(prgn,nv);
struct Cyc_Toc_Env*_tmp953;struct Cyc_List_List*_tmp954;struct Cyc_Absyn_Stmt*_tmp955;struct _tuple23 _tmp952=
Cyc_Toc_xlate_pat(_tmp951,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);_tmp953=_tmp952.f1;_tmp954=_tmp952.f2;_tmp955=_tmp952.f3;
Cyc_Toc_stmt_to_c(_tmp953,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp955,s,0),0);
for(0;_tmp954 != 0;_tmp954=_tmp954->tl){
struct _tuple0*_tmp957;void*_tmp958;struct _tuple24*_tmp956=(struct _tuple24*)_tmp954->hd;_tmp957=_tmp956->f1;_tmp958=_tmp956->f2;
s=Cyc_Absyn_declare_stmt(_tmp957,_tmp958,0,s,0);}}{
# 5013
struct Cyc_Absyn_Stmt*_tmp959=s;_npop_handler(0);return _tmp959;};
# 5000
;_pop_region(prgn);};}
# 5019
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp95A=e->r;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_List_List*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*_tmp980;struct Cyc_Absyn_Exp*_tmp981;struct Cyc_Absyn_Exp*_tmp982;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_List_List*_tmp985;void**_tmp987;struct Cyc_Absyn_Exp*_tmp988;void**_tmp98A;struct Cyc_List_List*_tmp98B;struct Cyc_List_List*_tmp98D;struct Cyc_List_List*_tmp98F;void**_tmp991;void**_tmp993;struct Cyc_Absyn_Stmt*_tmp995;struct Cyc_Absyn_MallocInfo*_tmp997;_LL424: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp95B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp95B->tag != 19)goto _LL426;else{_tmp95C=_tmp95B->f1;}}_LL425:
 _tmp95E=_tmp95C;goto _LL427;_LL426: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp95D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp95D->tag != 20)goto _LL428;else{_tmp95E=_tmp95D->f1;}}_LL427:
 _tmp960=_tmp95E;goto _LL429;_LL428: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp95F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp95F->tag != 21)goto _LL42A;else{_tmp960=_tmp95F->f1;}}_LL429:
 _tmp962=_tmp960;goto _LL42B;_LL42A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp961=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp961->tag != 14)goto _LL42C;else{_tmp962=_tmp961->f1;}}_LL42B:
 _tmp964=_tmp962;goto _LL42D;_LL42C: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp963=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp963->tag != 10)goto _LL42E;else{_tmp964=_tmp963->f1;}}_LL42D:
 _tmp966=_tmp964;goto _LL42F;_LL42E: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp965=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp965->tag != 11)goto _LL430;else{_tmp966=_tmp965->f1;}}_LL42F:
 _tmp968=_tmp966;goto _LL431;_LL430: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp967=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp967->tag != 17)goto _LL432;else{_tmp968=_tmp967->f1;}}_LL431:
 _tmp96A=_tmp968;goto _LL433;_LL432: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp969=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp969->tag != 4)goto _LL434;else{_tmp96A=_tmp969->f1;}}_LL433:
 Cyc_Toc_exptypes_to_c(_tmp96A);goto _LL423;_LL434: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp96B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp96B->tag != 2)goto _LL436;else{_tmp96C=_tmp96B->f2;}}_LL435:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp96C);goto _LL423;_LL436: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp96D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp96D->tag != 6)goto _LL438;else{_tmp96E=_tmp96D->f1;_tmp96F=_tmp96D->f2;}}_LL437:
 _tmp971=_tmp96E;_tmp972=_tmp96F;goto _LL439;_LL438: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp970=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp970->tag != 7)goto _LL43A;else{_tmp971=_tmp970->f1;_tmp972=_tmp970->f2;}}_LL439:
 _tmp974=_tmp971;_tmp975=_tmp972;goto _LL43B;_LL43A: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp973=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp973->tag != 8)goto _LL43C;else{_tmp974=_tmp973->f1;_tmp975=_tmp973->f2;}}_LL43B:
 _tmp977=_tmp974;_tmp978=_tmp975;goto _LL43D;_LL43C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp976=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp976->tag != 22)goto _LL43E;else{_tmp977=_tmp976->f1;_tmp978=_tmp976->f2;}}_LL43D:
 _tmp97A=_tmp977;_tmp97B=_tmp978;goto _LL43F;_LL43E: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp979=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp979->tag != 34)goto _LL440;else{_tmp97A=_tmp979->f1;_tmp97B=_tmp979->f2;}}_LL43F:
 _tmp97D=_tmp97A;_tmp97E=_tmp97B;goto _LL441;_LL440: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp97C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp97C->tag != 3)goto _LL442;else{_tmp97D=_tmp97C->f1;_tmp97E=_tmp97C->f3;}}_LL441:
 Cyc_Toc_exptypes_to_c(_tmp97D);Cyc_Toc_exptypes_to_c(_tmp97E);goto _LL423;_LL442: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp97F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp97F->tag != 5)goto _LL444;else{_tmp980=_tmp97F->f1;_tmp981=_tmp97F->f2;_tmp982=_tmp97F->f3;}}_LL443:
# 5037
 Cyc_Toc_exptypes_to_c(_tmp980);Cyc_Toc_exptypes_to_c(_tmp981);Cyc_Toc_exptypes_to_c(_tmp982);goto _LL423;_LL444: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp983=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp983->tag != 9)goto _LL446;else{_tmp984=_tmp983->f1;_tmp985=_tmp983->f2;}}_LL445:
# 5039
 Cyc_Toc_exptypes_to_c(_tmp984);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp985);goto _LL423;_LL446: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp986=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp986->tag != 13)goto _LL448;else{_tmp987=(void**)((void**)& _tmp986->f1);_tmp988=_tmp986->f2;}}_LL447:
*_tmp987=Cyc_Toc_typ_to_c(*_tmp987);Cyc_Toc_exptypes_to_c(_tmp988);goto _LL423;_LL448: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp989=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp989->tag != 24)goto _LL44A;else{_tmp98A=(void**)&(_tmp989->f1)->f3;_tmp98B=_tmp989->f2;}}_LL449:
# 5042
*_tmp98A=Cyc_Toc_typ_to_c(*_tmp98A);
_tmp98D=_tmp98B;goto _LL44B;_LL44A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp98C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp98C->tag != 35)goto _LL44C;else{_tmp98D=_tmp98C->f2;}}_LL44B:
 _tmp98F=_tmp98D;goto _LL44D;_LL44C: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp98E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp98E->tag != 25)goto _LL44E;else{_tmp98F=_tmp98E->f1;}}_LL44D:
# 5046
 for(0;_tmp98F != 0;_tmp98F=_tmp98F->tl){
struct Cyc_Absyn_Exp*_tmp9A8;struct _tuple16 _tmp9A7=*((struct _tuple16*)_tmp98F->hd);_tmp9A8=_tmp9A7.f2;
Cyc_Toc_exptypes_to_c(_tmp9A8);}
# 5050
goto _LL423;_LL44E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp990=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp990->tag != 18)goto _LL450;else{_tmp991=(void**)((void**)& _tmp990->f1);}}_LL44F:
 _tmp993=_tmp991;goto _LL451;_LL450: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp992=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp992->tag != 16)goto _LL452;else{_tmp993=(void**)((void**)& _tmp992->f1);}}_LL451:
*_tmp993=Cyc_Toc_typ_to_c(*_tmp993);goto _LL423;_LL452: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp994=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp994->tag != 36)goto _LL454;else{_tmp995=_tmp994->f1;}}_LL453:
 Cyc_Toc_stmttypes_to_c(_tmp995);goto _LL423;_LL454: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp996=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp996->tag != 33)goto _LL456;else{_tmp997=(struct Cyc_Absyn_MallocInfo*)& _tmp996->f1;}}_LL455:
# 5055
 if(_tmp997->elt_type != 0){
void**_tmpE13;_tmp997->elt_type=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp997->elt_type))),_tmpE13))));}
Cyc_Toc_exptypes_to_c(_tmp997->num_elts);
goto _LL423;_LL456: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp998=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp998->tag != 0)goto _LL458;}_LL457:
 goto _LL459;_LL458: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp999=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp999->tag != 1)goto _LL45A;}_LL459:
 goto _LL45B;_LL45A: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp99A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp99A->tag != 31)goto _LL45C;}_LL45B:
 goto _LL45D;_LL45C: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp99B->tag != 39)goto _LL45E;}_LL45D:
 goto _LL45F;_LL45E: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp99C->tag != 32)goto _LL460;}_LL45F:
 goto _LL423;_LL460: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp99D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp99D->tag != 29)goto _LL462;}_LL461:
# 5065
 goto _LL463;_LL462: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp99E->tag != 30)goto _LL464;}_LL463:
 goto _LL465;_LL464: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp99F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp99F->tag != 28)goto _LL466;}_LL465:
 goto _LL467;_LL466: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9A0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A0->tag != 26)goto _LL468;}_LL467:
 goto _LL469;_LL468: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A1->tag != 27)goto _LL46A;}_LL469:
 goto _LL46B;_LL46A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9A2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A2->tag != 23)goto _LL46C;}_LL46B:
 goto _LL46D;_LL46C: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9A3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A3->tag != 12)goto _LL46E;}_LL46D:
 goto _LL46F;_LL46E: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9A4=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A4->tag != 15)goto _LL470;}_LL46F:
 goto _LL471;_LL470: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9A5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A5->tag != 38)goto _LL472;}_LL471:
 goto _LL473;_LL472: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9A6=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp95A;if(_tmp9A6->tag != 37)goto _LL423;}_LL473:
# 5075
{const char*_tmpE16;void*_tmpE15;(_tmpE15=0,Cyc_Tcutil_terr(e->loc,((_tmpE16="Cyclone expression within C code",_tag_dyneither(_tmpE16,sizeof(char),33))),_tag_dyneither(_tmpE15,sizeof(void*),0)));}
goto _LL423;_LL423:;}
# 5080
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9AC=d->r;struct Cyc_Absyn_Vardecl*_tmp9AE;struct Cyc_Absyn_Fndecl*_tmp9B0;struct Cyc_Absyn_Aggrdecl*_tmp9B2;struct Cyc_Absyn_Enumdecl*_tmp9B4;struct Cyc_Absyn_Typedefdecl*_tmp9B6;_LL475: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9AD=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9AD->tag != 0)goto _LL477;else{_tmp9AE=_tmp9AD->f1;}}_LL476:
# 5083
 _tmp9AE->type=Cyc_Toc_typ_to_c(_tmp9AE->type);
if(_tmp9AE->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9AE->initializer));
goto _LL474;_LL477: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9AF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9AF->tag != 1)goto _LL479;else{_tmp9B0=_tmp9AF->f1;}}_LL478:
# 5087
 _tmp9B0->ret_type=Cyc_Toc_typ_to_c(_tmp9B0->ret_type);
{struct Cyc_List_List*_tmp9C2=_tmp9B0->args;for(0;_tmp9C2 != 0;_tmp9C2=_tmp9C2->tl){
(*((struct _tuple8*)_tmp9C2->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp9C2->hd)).f3);}}
# 5091
goto _LL474;_LL479: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9B1=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9B1->tag != 6)goto _LL47B;else{_tmp9B2=_tmp9B1->f1;}}_LL47A:
 Cyc_Toc_aggrdecl_to_c(_tmp9B2,1);goto _LL474;_LL47B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9B3=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9B3->tag != 8)goto _LL47D;else{_tmp9B4=_tmp9B3->f1;}}_LL47C:
# 5094
 if(_tmp9B4->fields != 0){
struct Cyc_List_List*_tmp9C3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9B4->fields))->v;for(0;_tmp9C3 != 0;_tmp9C3=_tmp9C3->tl){
struct Cyc_Absyn_Enumfield*_tmp9C4=(struct Cyc_Absyn_Enumfield*)_tmp9C3->hd;
if(_tmp9C4->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C4->tag));}}
# 5099
goto _LL474;_LL47D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9B5=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9B5->tag != 9)goto _LL47F;else{_tmp9B6=_tmp9B5->f1;}}_LL47E:
 _tmp9B6->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9B6->defn));goto _LL474;_LL47F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9B7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9B7->tag != 2)goto _LL481;}_LL480:
 goto _LL482;_LL481: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9B8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9B8->tag != 3)goto _LL483;}_LL482:
 goto _LL484;_LL483: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9B9=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9B9->tag != 7)goto _LL485;}_LL484:
 goto _LL486;_LL485: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9BA=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9BA->tag != 10)goto _LL487;}_LL486:
 goto _LL488;_LL487: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9BB=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9BB->tag != 11)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9BC=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9BC->tag != 12)goto _LL48B;}_LL48A:
 goto _LL48C;_LL48B: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9BD=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9BD->tag != 13)goto _LL48D;}_LL48C:
 goto _LL48E;_LL48D: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9BE=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9BE->tag != 4)goto _LL48F;}_LL48E:
 goto _LL490;_LL48F: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9BF=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9BF->tag != 5)goto _LL491;}_LL490:
# 5110
{const char*_tmpE19;void*_tmpE18;(_tmpE18=0,Cyc_Tcutil_terr(d->loc,((_tmpE19="Cyclone declaration within C code",_tag_dyneither(_tmpE19,sizeof(char),34))),_tag_dyneither(_tmpE18,sizeof(void*),0)));}
goto _LL474;_LL491: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9C0=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9C0->tag != 14)goto _LL493;}_LL492:
 goto _LL494;_LL493: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9C1=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9AC;if(_tmp9C1->tag != 15)goto _LL474;}_LL494:
# 5114
 goto _LL474;_LL474:;}
# 5118
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9C7=s->r;struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Stmt*_tmp9CB;struct Cyc_Absyn_Stmt*_tmp9CC;struct Cyc_Absyn_Exp*_tmp9CE;struct Cyc_Absyn_Exp*_tmp9D0;struct Cyc_Absyn_Stmt*_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D2;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Stmt*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D7;struct Cyc_Absyn_Exp*_tmp9D8;struct Cyc_Absyn_Exp*_tmp9D9;struct Cyc_Absyn_Stmt*_tmp9DA;struct Cyc_Absyn_Exp*_tmp9DC;struct Cyc_List_List*_tmp9DD;struct Cyc_Absyn_Decl*_tmp9DF;struct Cyc_Absyn_Stmt*_tmp9E0;struct Cyc_Absyn_Stmt*_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E3;_LL496: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9C8=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9C8->tag != 1)goto _LL498;else{_tmp9C9=_tmp9C8->f1;}}_LL497:
 Cyc_Toc_exptypes_to_c(_tmp9C9);goto _LL495;_LL498: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9CA=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9CA->tag != 2)goto _LL49A;else{_tmp9CB=_tmp9CA->f1;_tmp9CC=_tmp9CA->f2;}}_LL499:
 Cyc_Toc_stmttypes_to_c(_tmp9CB);Cyc_Toc_stmttypes_to_c(_tmp9CC);goto _LL495;_LL49A: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9CD=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9CD->tag != 3)goto _LL49C;else{_tmp9CE=_tmp9CD->f1;}}_LL49B:
 if(_tmp9CE != 0)Cyc_Toc_exptypes_to_c(_tmp9CE);goto _LL495;_LL49C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9CF=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9CF->tag != 4)goto _LL49E;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;_tmp9D2=_tmp9CF->f3;}}_LL49D:
# 5124
 Cyc_Toc_exptypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(_tmp9D1);Cyc_Toc_stmttypes_to_c(_tmp9D2);goto _LL495;_LL49E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9D3=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9D3->tag != 5)goto _LL4A0;else{_tmp9D4=(_tmp9D3->f1).f1;_tmp9D5=_tmp9D3->f2;}}_LL49F:
# 5126
 Cyc_Toc_exptypes_to_c(_tmp9D4);Cyc_Toc_stmttypes_to_c(_tmp9D5);goto _LL495;_LL4A0: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9D6=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9D6->tag != 9)goto _LL4A2;else{_tmp9D7=_tmp9D6->f1;_tmp9D8=(_tmp9D6->f2).f1;_tmp9D9=(_tmp9D6->f3).f1;_tmp9DA=_tmp9D6->f4;}}_LL4A1:
# 5128
 Cyc_Toc_exptypes_to_c(_tmp9D7);Cyc_Toc_exptypes_to_c(_tmp9D8);Cyc_Toc_exptypes_to_c(_tmp9D9);
Cyc_Toc_stmttypes_to_c(_tmp9DA);goto _LL495;_LL4A2: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9DB=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9DB->tag != 10)goto _LL4A4;else{_tmp9DC=_tmp9DB->f1;_tmp9DD=_tmp9DB->f2;}}_LL4A3:
# 5131
 Cyc_Toc_exptypes_to_c(_tmp9DC);
for(0;_tmp9DD != 0;_tmp9DD=_tmp9DD->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9DD->hd)->body);}
goto _LL495;_LL4A4: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9DE=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9DE->tag != 12)goto _LL4A6;else{_tmp9DF=_tmp9DE->f1;_tmp9E0=_tmp9DE->f2;}}_LL4A5:
 Cyc_Toc_decltypes_to_c(_tmp9DF);Cyc_Toc_stmttypes_to_c(_tmp9E0);goto _LL495;_LL4A6: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp9E1=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E1->tag != 14)goto _LL4A8;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=(_tmp9E1->f2).f1;}}_LL4A7:
 Cyc_Toc_stmttypes_to_c(_tmp9E2);Cyc_Toc_exptypes_to_c(_tmp9E3);goto _LL495;_LL4A8: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp9E4=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E4->tag != 0)goto _LL4AA;}_LL4A9:
 goto _LL4AB;_LL4AA: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp9E5=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E5->tag != 6)goto _LL4AC;}_LL4AB:
 goto _LL4AD;_LL4AC: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp9E6=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E6->tag != 7)goto _LL4AE;}_LL4AD:
 goto _LL4AF;_LL4AE: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9E7=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E7->tag != 8)goto _LL4B0;}_LL4AF:
 goto _LL495;_LL4B0: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp9E8=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E8->tag != 11)goto _LL4B2;}_LL4B1:
 goto _LL4B3;_LL4B2: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp9E9=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9E9->tag != 13)goto _LL4B4;}_LL4B3:
 goto _LL4B5;_LL4B4: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp9EA=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9EA->tag != 15)goto _LL4B6;}_LL4B5:
 goto _LL4B7;_LL4B6: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp9EB=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9C7;if(_tmp9EB->tag != 16)goto _LL495;}_LL4B7:
{const char*_tmpE1C;void*_tmpE1B;(_tmpE1B=0,Cyc_Tcutil_terr(s->loc,((_tmpE1C="Cyclone statement in C code",_tag_dyneither(_tmpE1C,sizeof(char),28))),_tag_dyneither(_tmpE1B,sizeof(void*),0)));}
goto _LL495;_LL495:;}
# 5152
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE1F;void*_tmpE1E;(_tmpE1E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE1F="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE1F,sizeof(char),29))),_tag_dyneither(_tmpE1E,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp9F0=d->r;struct Cyc_Absyn_Vardecl*_tmp9F2;struct Cyc_Absyn_Fndecl*_tmp9F4;struct Cyc_Absyn_Aggrdecl*_tmp9FA;struct Cyc_Absyn_Datatypedecl*_tmp9FC;struct Cyc_Absyn_Enumdecl*_tmp9FE;struct Cyc_Absyn_Typedefdecl*_tmpA00;struct Cyc_List_List*_tmpA04;struct Cyc_List_List*_tmpA06;struct Cyc_List_List*_tmpA08;struct Cyc_List_List*_tmpA0A;_LL4B9: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9F1=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F1->tag != 0)goto _LL4BB;else{_tmp9F2=_tmp9F1->f1;}}_LL4BA: {
# 5159
struct _tuple0*_tmpA0B=_tmp9F2->name;
# 5161
if(_tmp9F2->sc == Cyc_Absyn_ExternC){
struct _tuple0*_tmpE20;_tmpA0B=((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20->f1=Cyc_Absyn_Rel_n(0),((_tmpE20->f2=(*_tmpA0B).f2,_tmpE20))))));}
if(_tmp9F2->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9F2->initializer));else{
# 5167
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9F2->initializer));}}
# 5169
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE23;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE22;nv=Cyc_Toc_add_varmap(r,nv,_tmp9F2->name,Cyc_Absyn_varb_exp(_tmpA0B,(void*)((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=((_tmpE23.tag=1,((_tmpE23.f1=_tmp9F2,_tmpE23)))),_tmpE22)))),0));}
_tmp9F2->name=_tmpA0B;
_tmp9F2->sc=Cyc_Toc_scope_to_c(_tmp9F2->sc);
_tmp9F2->type=Cyc_Toc_typ_to_c(_tmp9F2->type);
{struct Cyc_List_List*_tmpE24;Cyc_Toc_result_decls=((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24->hd=d,((_tmpE24->tl=Cyc_Toc_result_decls,_tmpE24))))));}
goto _LL4B8;}_LL4BB: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9F3=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F3->tag != 1)goto _LL4BD;else{_tmp9F4=_tmp9F3->f1;}}_LL4BC: {
# 5176
struct _tuple0*_tmpA10=_tmp9F4->name;
# 5178
if(_tmp9F4->sc == Cyc_Absyn_ExternC){
{struct _tuple0*_tmpE25;_tmpA10=((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE25->f2=(*_tmpA10).f2,_tmpE25))))));}
_tmp9F4->sc=Cyc_Absyn_Public;}
# 5182
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F4->name,Cyc_Absyn_var_exp(_tmpA10,0));
_tmp9F4->name=_tmpA10;
Cyc_Toc_fndecl_to_c(nv,_tmp9F4,cinclude);
{struct Cyc_List_List*_tmpE26;Cyc_Toc_result_decls=((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->hd=d,((_tmpE26->tl=Cyc_Toc_result_decls,_tmpE26))))));}
goto _LL4B8;}_LL4BD: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9F5=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F5->tag != 2)goto _LL4BF;}_LL4BE:
 goto _LL4C0;_LL4BF: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9F6=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F6->tag != 3)goto _LL4C1;}_LL4C0: {
# 5189
const char*_tmpE29;void*_tmpE28;(_tmpE28=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE29="letdecl at toplevel",_tag_dyneither(_tmpE29,sizeof(char),20))),_tag_dyneither(_tmpE28,sizeof(void*),0)));}_LL4C1: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9F7=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F7->tag != 4)goto _LL4C3;}_LL4C2: {
# 5191
const char*_tmpE2C;void*_tmpE2B;(_tmpE2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE2C="region decl at toplevel",_tag_dyneither(_tmpE2C,sizeof(char),24))),_tag_dyneither(_tmpE2B,sizeof(void*),0)));}_LL4C3: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9F8=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F8->tag != 5)goto _LL4C5;}_LL4C4: {
# 5193
const char*_tmpE2F;void*_tmpE2E;(_tmpE2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE2F="alias decl at toplevel",_tag_dyneither(_tmpE2F,sizeof(char),23))),_tag_dyneither(_tmpE2E,sizeof(void*),0)));}_LL4C5: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9F9=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9F9->tag != 6)goto _LL4C7;else{_tmp9FA=_tmp9F9->f1;}}_LL4C6:
# 5195
 Cyc_Toc_aggrdecl_to_c(_tmp9FA,1);
goto _LL4B8;_LL4C7: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9FB=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9FB->tag != 7)goto _LL4C9;else{_tmp9FC=_tmp9FB->f1;}}_LL4C8:
# 5198
 if(_tmp9FC->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp9FC);else{
# 5201
Cyc_Toc_datatypedecl_to_c(_tmp9FC);}
goto _LL4B8;_LL4C9: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9FD=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9FD->tag != 8)goto _LL4CB;else{_tmp9FE=_tmp9FD->f1;}}_LL4CA:
# 5204
 Cyc_Toc_enumdecl_to_c(_tmp9FE);
{struct Cyc_List_List*_tmpE30;Cyc_Toc_result_decls=((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30->hd=d,((_tmpE30->tl=Cyc_Toc_result_decls,_tmpE30))))));}
goto _LL4B8;_LL4CB: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9FF=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmp9FF->tag != 9)goto _LL4CD;else{_tmpA00=_tmp9FF->f1;}}_LL4CC:
# 5208
 _tmpA00->name=_tmpA00->name;
_tmpA00->tvs=0;
if(_tmpA00->defn != 0)
_tmpA00->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA00->defn));else{
# 5213
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA00->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL4D9:
 _tmpA00->defn=Cyc_Absyn_void_star_typ();break;default: _LL4DA:
 _tmpA00->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5218
{struct Cyc_List_List*_tmpE31;Cyc_Toc_result_decls=((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->hd=d,((_tmpE31->tl=Cyc_Toc_result_decls,_tmpE31))))));}
goto _LL4B8;_LL4CD: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA01=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmpA01->tag != 14)goto _LL4CF;}_LL4CE:
 goto _LL4D0;_LL4CF: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA02=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmpA02->tag != 15)goto _LL4D1;}_LL4D0:
# 5222
 goto _LL4B8;_LL4D1: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA03=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmpA03->tag != 10)goto _LL4D3;else{_tmpA04=_tmpA03->f2;}}_LL4D2:
 _tmpA06=_tmpA04;goto _LL4D4;_LL4D3: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA05=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmpA05->tag != 11)goto _LL4D5;else{_tmpA06=_tmpA05->f2;}}_LL4D4:
 _tmpA08=_tmpA06;goto _LL4D6;_LL4D5: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA07=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmpA07->tag != 12)goto _LL4D7;else{_tmpA08=_tmpA07->f1;}}_LL4D6:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA08,top,cinclude);goto _LL4B8;_LL4D7: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA09=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9F0;if(_tmpA09->tag != 13)goto _LL4B8;else{_tmpA0A=_tmpA09->f1;}}_LL4D8:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA0A,top,1);goto _LL4B8;_LL4B8:;};}
# 5229
return nv;}
# 5233
static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA1C;struct Cyc_Core_NewRegion _tmpA1B=Cyc_Core__new_dynregion("internal-error",0);_tmpA1C=_tmpA1B.dynregion;{
struct _DynRegionFrame _tmpA1D;struct _RegionHandle*d=_open_dynregion(& _tmpA1D,_tmpA1C);{
struct Cyc_Dict_Dict*_tmpE3E;struct Cyc_Dict_Dict*_tmpE3D;struct Cyc_Set_Set**_tmpE3C;struct Cyc_List_List**_tmpE3B;struct Cyc_Dict_Dict*_tmpE3A;struct Cyc_List_List**_tmpE39;struct Cyc_Toc_TocState*_tmpE38;Cyc_Toc_toc_state=(
(_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->dyn=(struct _DynRegionHandle*)_tmpA1C,((_tmpE38->tuple_types=(struct Cyc_List_List**)(
(_tmpE39=_region_malloc(d,sizeof(*_tmpE39)),((_tmpE39[0]=0,_tmpE39)))),((_tmpE38->aggrs_so_far=(struct Cyc_Dict_Dict*)(
(_tmpE3A=_region_malloc(d,sizeof(*_tmpE3A)),((_tmpE3A[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE3A)))),((_tmpE38->abs_struct_types=(struct Cyc_List_List**)(
(_tmpE3B=_region_malloc(d,sizeof(*_tmpE3B)),((_tmpE3B[0]=0,_tmpE3B)))),((_tmpE38->datatypes_so_far=(struct Cyc_Set_Set**)(
(_tmpE3C=_region_malloc(d,sizeof(*_tmpE3C)),((_tmpE3C[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE3C)))),((_tmpE38->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
(_tmpE3D=_region_malloc(d,sizeof(*_tmpE3D)),((_tmpE3D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE3D)))),((_tmpE38->qvar_tags=(struct Cyc_Dict_Dict*)(
(_tmpE3E=_region_malloc(d,sizeof(*_tmpE3E)),((_tmpE3E[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE3E)))),((_tmpE38->temp_labels=(struct Cyc_Xarray_Xarray*)
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpE38))))))))))))))))));}
# 5247
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE3F;Cyc_Toc_globals=_tag_dyneither(((_tmpE3F=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE3F[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE3F[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE3F[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE3F[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE3F[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE3F[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE3F[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE3F[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE3F[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE3F[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE3F[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE3F[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE3F[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE3F[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE3F[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE3F[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE3F[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE3F[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE3F[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE3F[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE3F[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE3F[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE3F[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE3F[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE3F[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE3F[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE3F[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE3F[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE3F[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE3F[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE3F[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE3F[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE3F[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE3F[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE3F[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE3F[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE3F[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE3F[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE3F[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE3F[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE3F[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE3F)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 5235
;_pop_dynregion(d);};}
# 5300
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA26=_new_region("start");struct _RegionHandle*start=& _tmpA26;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5309
struct _DynRegionHandle*_tmpA28;struct Cyc_Xarray_Xarray*_tmpA29;struct Cyc_Toc_TocState _tmpA27=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA28=_tmpA27.dyn;_tmpA29=_tmpA27.temp_labels;
{struct _DynRegionFrame _tmpA2A;struct _RegionHandle*d=_open_dynregion(& _tmpA2A,_tmpA28);
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA29);;_pop_dynregion(d);}
# 5313
Cyc_Core_free_dynregion(_tmpA28);};{
# 5315
struct Cyc_List_List*_tmpA2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA2B;};
# 5302
;_pop_region(start);};}
