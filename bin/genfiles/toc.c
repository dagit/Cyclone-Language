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
# 36
typedef char*Cyc_CstringNN;
# 37
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
# 250
return 0;{
# 251
unsigned char*ptrbd=dyn.curr + bd * sz;
# 252
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
# 257
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
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 104
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
# 105
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
# 106
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
# 107
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
# 108
typedef void*Cyc_Absyn_kindbound_t;
# 109
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
# 110
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
# 111
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
# 112
typedef void*Cyc_Absyn_bounds_t;
# 113
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
# 114
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
# 115
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
# 116
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
# 117
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
# 118
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
# 119
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
# 120
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
# 121
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
# 122
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
# 123
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
# 124
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
# 125
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
# 126
typedef void*Cyc_Absyn_raw_exp_t;
# 127
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
# 128
typedef void*Cyc_Absyn_raw_stmt_t;
# 129
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
# 130
typedef void*Cyc_Absyn_raw_pat_t;
# 131
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
# 132
typedef void*Cyc_Absyn_binding_t;
# 133
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
# 134
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
# 135
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
# 136
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
# 137
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
# 138
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
# 139
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
# 140
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
# 141
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
# 142
typedef void*Cyc_Absyn_raw_decl_t;
# 143
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
# 144
typedef void*Cyc_Absyn_designator_t;
# 145
typedef void*Cyc_Absyn_absyn_annot_t;
# 146
typedef void*Cyc_Absyn_attribute_t;
# 147
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
# 148
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
# 149
typedef void*Cyc_Absyn_offsetof_field_t;
# 150
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
# 151
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
# 152
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
# 281
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
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 529
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
# 929
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
# 990
void*Cyc_Absyn_strctq(struct _tuple0*name);
# 991
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
# 1074
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1075
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1103
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
# 69
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
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
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
# 94
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
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
# 42
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
# 95
typedef struct Cyc_List_List*Cyc_CfFlowInfo_relns_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 108
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 113
typedef void*Cyc_CfFlowInfo_absRval_t;
# 114
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
# 115
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 120
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};
# 135
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 142
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple0*Cyc_Toc_temp_var();
# 36
extern struct _dyneither_ptr Cyc_Toc_globals;
# 65 "toc.cyc"
extern int Cyc_noexpand_r;
# 73
int Cyc_Toc_warn_bounds_checks=0;
# 74
int Cyc_Toc_warn_all_null_deref=0;
# 75
unsigned int Cyc_Toc_total_bounds_checks=0;
# 76
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 82
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};
# 103
static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{struct _tuple0*f1;struct _tuple0*f2;};
# 105
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
# 106
struct _tuple0*_tmp1;struct _tuple0*_tmp2;struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{
# 107
struct _tuple0*_tmp4;struct _tuple0*_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{
# 108
int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);
# 109
if(i != 0)return i;
# 110
return Cyc_Absyn_qvar_cmp(_tmp2,_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 113
void*Cyc_Toc_aggrdecl_type(struct _tuple0*q,void*(*type_maker)(struct _tuple0*)){
# 114
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{
# 115
struct _DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);
# 116
{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp8,q);
# 117
if(v == 0){
# 118
void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
# 120
struct _tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
# 121
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}}
# 116
;_pop_dynregion(d);};}
# 126
static int Cyc_Toc_tuple_type_counter=0;
# 127
static int Cyc_Toc_temp_var_counter=0;
# 128
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 133
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 136
Cyc_vfprintf(Cyc_stderr,fmt,ap);
# 137
{const char*_tmpA6C;void*_tmpA6B;(_tmpA6B=0,Cyc_fprintf(Cyc_stderr,((_tmpA6C="\n",_tag_dyneither(_tmpA6C,sizeof(char),2))),_tag_dyneither(_tmpA6B,sizeof(void*),0)));}
# 138
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 139
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA6F;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA6E;(int)_throw((void*)((_tmpA6E=_cycalloc_atomic(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6F.tag=Cyc_Toc_Toc_Unimplemented,_tmpA6F)),_tmpA6E)))));};}
# 141
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 144
Cyc_vfprintf(Cyc_stderr,fmt,ap);
# 145
{const char*_tmpA72;void*_tmpA71;(_tmpA71=0,Cyc_fprintf(Cyc_stderr,((_tmpA72="\n",_tag_dyneither(_tmpA72,sizeof(char),2))),_tag_dyneither(_tmpA71,sizeof(void*),0)));}
# 146
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 147
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA75;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA74;(int)_throw((void*)((_tmpA74=_cycalloc_atomic(sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA75.tag=Cyc_Toc_Toc_Impossible,_tmpA75)),_tmpA74)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp17[5]="curr";
# 159 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp18[4]="tag";
# 160
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp19[4]="val";
# 161
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp1A[14]="_handler_cons";
# 162
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";
# 163
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
# 164
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp1D[17]="_DynRegionHandle";
# 165
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";
# 166
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 172
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp1F[7]="_throw";
# 181 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,_tmp1F + 7};static struct _tuple0 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";
# 182
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple0 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp23[14]="_push_handler";
# 183
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";
# 184
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp27[12]="_exn_thrown";
# 185
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp27,_tmp27,_tmp27 + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";
# 186
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp2B[12]="_check_null";
# 187
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple0 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";
# 188
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,_tmp2D + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp2F[31]="_check_known_subscript_notnull";
# 189
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp31[27]="_check_dyneither_subscript";
# 190
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp31,_tmp31,_tmp31 + 27};static struct _tuple0 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp33[15]="_dyneither_ptr";
# 191
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";
# 192
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct _tuple0 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp37[20]="_init_dyneither_ptr";
# 193
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 20};static struct _tuple0 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
# 194
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39 + 21};static struct _tuple0 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3B[20]="_get_dyneither_size";
# 195
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple0 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";
# 196
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};static struct _tuple0 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3F[24]="_get_zero_arr_size_char";
# 197
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple0 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp41[25]="_get_zero_arr_size_short";
# 198
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp41,_tmp41,_tmp41 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp43[23]="_get_zero_arr_size_int";
# 199
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp43,_tmp43,_tmp43 + 23};static struct _tuple0 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
# 200
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,_tmp45 + 25};static struct _tuple0 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
# 201
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,_tmp47 + 26};static struct _tuple0 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp49[30]="_get_zero_arr_size_longdouble";
# 202
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static struct _tuple0 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
# 203
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,_tmp4B + 28};static struct _tuple0 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp4D[20]="_dyneither_ptr_plus";
# 204
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple0 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";
# 205
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static struct _tuple0 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp51[20]="_zero_arr_plus_char";
# 206
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp51,_tmp51,_tmp51 + 20};static struct _tuple0 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
# 207
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp55[19]="_zero_arr_plus_int";
# 208
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp55,_tmp55,_tmp55 + 19};static struct _tuple0 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";
# 209
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};static struct _tuple0 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp59[22]="_zero_arr_plus_double";
# 210
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp59,_tmp59,_tmp59 + 22};static struct _tuple0 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
# 211
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,_tmp5B + 26};static struct _tuple0 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp5D[24]="_zero_arr_plus_voidstar";
# 212
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple0 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp5F[28]="_dyneither_ptr_inplace_plus";
# 213
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp61[23]="_zero_arr_inplace_plus";
# 214
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp61,_tmp61,_tmp61 + 23};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
# 215
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,_tmp63 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp65[29]="_zero_arr_inplace_plus_short";
# 216
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
# 217
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,_tmp67 + 27};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp69[29]="_zero_arr_inplace_plus_float";
# 218
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
# 219
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,_tmp6B,_tmp6B + 30};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";
# 220
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,_tmp6D + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";
# 221
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";
# 222
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71 + 33};static struct _tuple0 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp73[28]="_zero_arr_inplace_plus_post";
# 223
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp73,_tmp73,_tmp73 + 28};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp75[33]="_zero_arr_inplace_plus_post_char";
# 224
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75 + 33};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp77[34]="_zero_arr_inplace_plus_post_short";
# 225
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,_tmp77 + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp79[32]="_zero_arr_inplace_plus_post_int";
# 226
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 32};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";
# 227
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,_tmp7B + 34};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";
# 228
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,_tmp7D + 35};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";
# 229
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,_tmp7F + 39};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";
# 230
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,_tmp81 + 37};static struct _tuple0 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp83[10]="_cycalloc";
# 231
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp83,_tmp83,_tmp83 + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp85[11]="_cyccalloc";
# 232
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,_tmp85,_tmp85 + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
# 233
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp89[18]="_cyccalloc_atomic";
# 234
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp89,_tmp89,_tmp89 + 18};static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";
# 235
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct _tuple0 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp8D[15]="_region_calloc";
# 236
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple0 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";
# 237
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct _tuple0 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp91[12]="_new_region";
# 238
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp91,_tmp91,_tmp91 + 12};static struct _tuple0 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";
# 239
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct _tuple0 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp95[12]="_pop_region";
# 240
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp95,_tmp95,_tmp95 + 12};static struct _tuple0 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";
# 241
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static struct _tuple0 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp99[16]="_push_dynregion";
# 242
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp99,_tmp99,_tmp99 + 16};static struct _tuple0 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";
# 243
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct _tuple0 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp9D[14]="_reset_region";
# 244
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple0 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";
# 245
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static struct _tuple0 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpA1[29]="_dyneither_ptr_decrease_size";
# 246
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct _tuple0 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";
# 247
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static struct _tuple0 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
# 248
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};static struct _tuple0 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpA7[13]="_throw_match";
# 249
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpA7,_tmpA7,_tmpA7 + 13};static struct _tuple0 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
# 254
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 257
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 259
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 261
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
# 262
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
# 263
if(skip_stmt_opt == 0){
# 264
struct Cyc_Absyn_Stmt**_tmpA76;skip_stmt_opt=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=Cyc_Absyn_skip_stmt(0),_tmpA76))));}
# 265
return*skip_stmt_opt;}
# 270
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
# 271
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 273
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
# 274
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA79;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA78;return(void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA79.tag=13,((_tmpA79.f1=(void*)t,((_tmpA79.f2=e,((_tmpA79.f3=0,((_tmpA79.f4=Cyc_Absyn_No_coercion,_tmpA79)))))))))),_tmpA78))));}
# 276
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
# 277
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA7C;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA7B;return(void*)((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7C.tag=19,((_tmpA7C.f1=e,_tmpA7C)))),_tmpA7B))));}
# 279
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 280
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA7F;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA7E;return(void*)((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=22,((_tmpA7F.f1=e1,((_tmpA7F.f2=e2,_tmpA7F)))))),_tmpA7E))));}
# 282
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
# 283
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA82;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA81;return(void*)((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA82.tag=35,((_tmpA82.f1=s,_tmpA82)))),_tmpA81))));}
# 285
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
# 286
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA85;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA84;return(void*)((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=16,((_tmpA85.f1=(void*)t,_tmpA85)))),_tmpA84))));}
# 288
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
# 289
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA88;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA87;return(void*)((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA88.tag=9,((_tmpA88.f1=e,((_tmpA88.f2=es,((_tmpA88.f3=0,((_tmpA88.f4=1,_tmpA88)))))))))),_tmpA87))));}
# 291
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
# 292
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA8B;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA8A;return(void*)((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=1,((_tmpA8B.f1=e,_tmpA8B)))),_tmpA8A))));}
# 294
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
# 295
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA8E;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA8D;return(void*)((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA8E.tag=2,((_tmpA8E.f1=s1,((_tmpA8E.f2=s2,_tmpA8E)))))),_tmpA8D))));}
# 297
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 298
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA91;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA90;return(void*)((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=5,((_tmpA91.f1=e1,((_tmpA91.f2=e2,((_tmpA91.f3=e3,_tmpA91)))))))),_tmpA90))));}
# 300
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
# 301
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA94;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA93;return(void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=((_tmpA94.tag=20,((_tmpA94.f1=e,((_tmpA94.f2=n,((_tmpA94.f3=0,((_tmpA94.f4=0,_tmpA94)))))))))),_tmpA93))));}
# 303
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
# 304
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA97;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA96;return(void*)((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=21,((_tmpA97.f1=e,((_tmpA97.f2=n,((_tmpA97.f3=0,((_tmpA97.f4=0,_tmpA97)))))))))),_tmpA96))));}
# 306
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 309
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA9A;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA99;return(void*)((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99[0]=((_tmpA9A.tag=34,((_tmpA9A.f1=tdopt,((_tmpA9A.f2=ds,_tmpA9A)))))),_tmpA99))));}
# 311
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
# 312
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA9D;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA9C;return(void*)((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=8,((_tmpA9D.f1=v,((_tmpA9D.f2=s,_tmpA9D)))))),_tmpA9C))));}
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
# 366
struct Cyc_Absyn_Exp*function;
# 367
{void*_tmpC6=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmpC8;int _tmpCA;int _tmpCC;_LL1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC6;if(_tmpC7->tag != 6)goto _LL3;else{_tmpC8=_tmpC7->f2;}}_LL2:
# 369
 switch(_tmpC8){case Cyc_Absyn_Char_sz: _LLD:
# 370
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LLE:
# 371
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LLF:
# 372
 function=fS->fint;break;default: _LL10: {
# 373
struct Cyc_Core_Impossible_exn_struct _tmpAA3;const char*_tmpAA2;struct Cyc_Core_Impossible_exn_struct*_tmpAA1;(int)_throw((void*)((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=((_tmpAA3.tag=Cyc_Core_Impossible,((_tmpAA3.f1=((_tmpAA2="impossible IntType (not char, short or int)",_tag_dyneither(_tmpAA2,sizeof(char),44))),_tmpAA3)))),_tmpAA1)))));}}
# 375
goto _LL0;_LL3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpC9->tag != 7)goto _LL5;else{_tmpCA=_tmpC9->f1;if(_tmpCA != 0)goto _LL5;}}_LL4:
# 377
 function=fS->ffloat;
# 378
goto _LL0;_LL5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpCB->tag != 7)goto _LL7;else{_tmpCC=_tmpCB->f1;if(_tmpCC != 1)goto _LL7;}}_LL6:
# 380
 function=fS->fdouble;
# 381
goto _LL0;_LL7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpC6;if(_tmpCD->tag != 7)goto _LL9;}_LL8:
# 383
 function=fS->flongdouble;
# 384
goto _LL0;_LL9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC6;if(_tmpCE->tag != 5)goto _LLB;}_LLA:
# 386
 function=fS->fvoidstar;
# 387
goto _LL0;_LLB:;_LLC: {
# 389
struct Cyc_Core_Impossible_exn_struct _tmpAB0;const char*_tmpAAF;void*_tmpAAE[1];struct Cyc_String_pa_PrintArg_struct _tmpAAD;struct Cyc_Core_Impossible_exn_struct*_tmpAAC;(int)_throw((void*)((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((_tmpAB0.tag=Cyc_Core_Impossible,((_tmpAB0.f1=(struct _dyneither_ptr)((_tmpAAD.tag=0,((_tmpAAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAAE[0]=& _tmpAAD,Cyc_aprintf(((_tmpAAF="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpAAF,sizeof(char),67))),_tag_dyneither(_tmpAAE,sizeof(void*),1)))))))),_tmpAB0)))),_tmpAAC)))));}_LL0:;}
# 391
return function;}
# 393
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
# 394
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 396
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
# 397
void*_tmpD7=Cyc_Tcutil_compress((void*)_check_null(arr->topt));struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;_LL13: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 5)goto _LL15;else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD9.elt_typ;}}_LL14:
# 399
 return Cyc_Toc_getFunctionType(fS,_tmpDA);_LL15:;_LL16: {
# 400
struct Cyc_Core_Impossible_exn_struct _tmpAB6;const char*_tmpAB5;struct Cyc_Core_Impossible_exn_struct*_tmpAB4;(int)_throw((void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB6.tag=Cyc_Core_Impossible,((_tmpAB6.f1=((_tmpAB5="impossible type (not pointer)",_tag_dyneither(_tmpAB5,sizeof(char),30))),_tmpAB6)))),_tmpAB4)))));}_LL12:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 407
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
# 408
void*_tmpDE=e->r;union Cyc_Absyn_Cnst _tmpE0;struct _tuple3 _tmpE1;char _tmpE2;union Cyc_Absyn_Cnst _tmpE4;struct _dyneither_ptr _tmpE5;union Cyc_Absyn_Cnst _tmpE7;struct _tuple4 _tmpE8;short _tmpE9;union Cyc_Absyn_Cnst _tmpEB;struct _tuple5 _tmpEC;int _tmpED;union Cyc_Absyn_Cnst _tmpEF;struct _tuple6 _tmpF0;long long _tmpF1;union Cyc_Absyn_Cnst _tmpF3;int _tmpF4;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*_tmp100;_LL18: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpDF->tag != 0)goto _LL1A;else{_tmpE0=_tmpDF->f1;if((_tmpE0.Char_c).tag != 2)goto _LL1A;_tmpE1=(struct _tuple3)(_tmpE0.Char_c).val;_tmpE2=_tmpE1.f2;}}_LL19:
# 409
 return _tmpE2 == '\000';_LL1A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE3->tag != 0)goto _LL1C;else{_tmpE4=_tmpE3->f1;if((_tmpE4.Wchar_c).tag != 3)goto _LL1C;_tmpE5=(struct _dyneither_ptr)(_tmpE4.Wchar_c).val;}}_LL1B: {
# 411
unsigned long _tmp101=Cyc_strlen((struct _dyneither_ptr)_tmpE5);
# 412
int i=0;
# 413
if(_tmp101 >= 2  && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),0))== '\\'){
# 414
if(*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== '0')i=2;else{
# 415
if((*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),1))== 'x'  && _tmp101 >= 3) && *((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),2))== '0')i=3;else{
# 416
return 0;}}
# 417
for(0;i < _tmp101;++ i){
# 418
if(*((const char*)_check_dyneither_subscript(_tmpE5,sizeof(char),i))!= '0')return 0;}
# 419
return 1;}else{
# 421
return 0;}}_LL1C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpE6->tag != 0)goto _LL1E;else{_tmpE7=_tmpE6->f1;if((_tmpE7.Short_c).tag != 4)goto _LL1E;_tmpE8=(struct _tuple4)(_tmpE7.Short_c).val;_tmpE9=_tmpE8.f2;}}_LL1D:
# 422
 return _tmpE9 == 0;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpEA->tag != 0)goto _LL20;else{_tmpEB=_tmpEA->f1;if((_tmpEB.Int_c).tag != 5)goto _LL20;_tmpEC=(struct _tuple5)(_tmpEB.Int_c).val;_tmpED=_tmpEC.f2;}}_LL1F:
# 423
 return _tmpED == 0;_LL20: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpEE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpEE->tag != 0)goto _LL22;else{_tmpEF=_tmpEE->f1;if((_tmpEF.LongLong_c).tag != 6)goto _LL22;_tmpF0=(struct _tuple6)(_tmpEF.LongLong_c).val;_tmpF1=_tmpF0.f2;}}_LL21:
# 424
 return _tmpF1 == 0;_LL22: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF2->tag != 0)goto _LL24;else{_tmpF3=_tmpF2->f1;if((_tmpF3.Null_c).tag != 1)goto _LL24;_tmpF4=(int)(_tmpF3.Null_c).val;}}_LL23:
# 425
 return 1;_LL24: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF5->tag != 13)goto _LL26;else{_tmpF6=_tmpF5->f2;}}_LL25:
# 426
 return Cyc_Toc_is_zero(_tmpF6);_LL26: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF7->tag != 23)goto _LL28;else{_tmpF8=_tmpF7->f1;}}_LL27:
# 427
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpF8);_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpF9->tag != 25)goto _LL2A;else{_tmpFA=_tmpF9->f1;}}_LL29:
# 428
 _tmpFC=_tmpFA;goto _LL2B;_LL2A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFB->tag != 27)goto _LL2C;else{_tmpFC=_tmpFB->f3;}}_LL2B:
# 429
 _tmpFE=_tmpFC;goto _LL2D;_LL2C: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFD->tag != 24)goto _LL2E;else{_tmpFE=_tmpFD->f2;}}_LL2D:
# 430
 _tmp100=_tmpFE;goto _LL2F;_LL2E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDE;if(_tmpFF->tag != 34)goto _LL30;else{_tmp100=_tmpFF->f2;}}_LL2F:
# 432
 for(0;_tmp100 != 0;_tmp100=_tmp100->tl){
# 433
if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmp100->hd)).f2))return 0;}
# 434
return 1;_LL30:;_LL31:
# 435
 return 0;_LL17:;}
# 440
static int Cyc_Toc_is_nullable(void*t){
# 441
void*_tmp102=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp104;struct Cyc_Absyn_PtrAtts _tmp105;union Cyc_Absyn_Constraint*_tmp106;_LL33: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag != 5)goto _LL35;else{_tmp104=_tmp103->f1;_tmp105=_tmp104.ptr_atts;_tmp106=_tmp105.nullable;}}_LL34:
# 443
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp106);_LL35:;_LL36: {
# 444
const char*_tmpAB9;void*_tmpAB8;(_tmpAB8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAB9="is_nullable",_tag_dyneither(_tmpAB9,sizeof(char),12))),_tag_dyneither(_tmpAB8,sizeof(void*),0)));}_LL32:;}
# 449
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple0*x){
# 450
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
# 451
unsigned int len=9 + slen;
# 452
struct _tuple0 _tmp10A;union Cyc_Absyn_Nmspace _tmp10B;struct _dyneither_ptr*_tmp10C;struct _dyneither_ptr _tmp10D;struct _tuple0*_tmp109=x;_tmp10A=*_tmp109;_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;_tmp10D=*_tmp10C;{
# 453
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp10D);
# 454
len +=1 + vlen;{
# 455
struct Cyc_List_List*nms;
# 456
{union Cyc_Absyn_Nmspace _tmp10E=_tmp10B;int _tmp10F;struct Cyc_List_List*_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_List_List*_tmp112;_LL38: if((_tmp10E.Loc_n).tag != 4)goto _LL3A;_tmp10F=(int)(_tmp10E.Loc_n).val;_LL39:
# 457
 nms=0;goto _LL37;_LL3A: if((_tmp10E.Rel_n).tag != 1)goto _LL3C;_tmp110=(struct Cyc_List_List*)(_tmp10E.Rel_n).val;_LL3B:
# 458
 nms=_tmp110;goto _LL37;_LL3C: if((_tmp10E.Abs_n).tag != 2)goto _LL3E;_tmp111=(struct Cyc_List_List*)(_tmp10E.Abs_n).val;_LL3D:
# 459
 nms=_tmp111;goto _LL37;_LL3E: if((_tmp10E.C_n).tag != 3)goto _LL37;_tmp112=(struct Cyc_List_List*)(_tmp10E.C_n).val;_LL3F:
# 460
 nms=_tmp112;goto _LL37;_LL37:;}
# 462
{struct Cyc_List_List*_tmp113=nms;for(0;_tmp113 != 0;_tmp113=_tmp113->tl){
# 463
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp113->hd));}}{
# 464
char*_tmpABB;unsigned int _tmpABA;struct _dyneither_ptr buf=(_tmpABA=len,((_tmpABB=_cyccalloc_atomic(sizeof(char),_tmpABA),_tag_dyneither(_tmpABB,sizeof(char),_tmpABA))));
# 465
struct _dyneither_ptr p=buf;
# 466
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
# 467
for(0;nms != 0;nms=nms->tl){
# 468
struct _dyneither_ptr _tmp115;struct _dyneither_ptr*_tmp114=(struct _dyneither_ptr*)nms->hd;_tmp115=*_tmp114;
# 469
{char _tmpABE;char _tmpABD;struct _dyneither_ptr _tmpABC;(_tmpABC=p,((_tmpABD=*((char*)_check_dyneither_subscript(_tmpABC,sizeof(char),0)),((_tmpABE='_',((_get_dyneither_size(_tmpABC,sizeof(char))== 1  && (_tmpABD == '\000'  && _tmpABE != '\000')?_throw_arraybounds(): 1,*((char*)_tmpABC.curr)=_tmpABE)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
# 470
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp115);
# 471
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp115));}
# 473
{char _tmpAC1;char _tmpAC0;struct _dyneither_ptr _tmpABF;(_tmpABF=p,((_tmpAC0=*((char*)_check_dyneither_subscript(_tmpABF,sizeof(char),0)),((_tmpAC1='_',((_get_dyneither_size(_tmpABF,sizeof(char))== 1  && (_tmpAC0 == '\000'  && _tmpAC1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpABF.curr)=_tmpAC1)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
# 474
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp10D);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
# 475
{const char*_tmpAC2;Cyc_strcpy(p,((_tmpAC2="_struct",_tag_dyneither(_tmpAC2,sizeof(char),8))));}
# 476
return(struct _dyneither_ptr)buf;};};};}static char _tmp130[8]="*bogus*";
# 485
static struct _tuple0*Cyc_Toc_collapse_qvars(struct _tuple0*fieldname,struct _tuple0*dtname){
# 486
struct _DynRegionHandle*_tmp120;struct Cyc_Dict_Dict*_tmp121;struct Cyc_Toc_TocState _tmp11F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp120=_tmp11F.dyn;_tmp121=_tmp11F.qvar_tags;{
# 487
static struct _dyneither_ptr bogus_string={_tmp130,_tmp130,_tmp130 + 8};
# 488
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 489
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 490
{struct _tuple14 _tmpAC3;pair=((_tmpAC3.f1=fieldname,((_tmpAC3.f2=dtname,_tmpAC3))));}{
# 491
struct _DynRegionFrame _tmp123;struct _RegionHandle*d=_open_dynregion(& _tmp123,_tmp120);
# 492
{struct _tuple0**_tmp124=((struct _tuple0**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp121,(struct _tuple14*)& pair);
# 493
if(_tmp124 != 0){struct _tuple0*_tmp125=*_tmp124;_npop_handler(0);return _tmp125;}{
# 494
struct _tuple14*_tmpAC4;struct _tuple14*_tmp126=(_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->f1=fieldname,((_tmpAC4->f2=dtname,_tmpAC4)))));
# 495
struct _tuple0 _tmp128;union Cyc_Absyn_Nmspace _tmp129;struct _dyneither_ptr*_tmp12A;struct _dyneither_ptr _tmp12B;struct _tuple0*_tmp127=fieldname;_tmp128=*_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;_tmp12B=*_tmp12A;{
# 496
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp12B,dtname);
# 497
struct _dyneither_ptr*_tmpAC7;struct _tuple0*_tmpAC6;struct _tuple0*res=(_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->f1=_tmp129,((_tmpAC6->f2=((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7[0]=newvar,_tmpAC7)))),_tmpAC6)))));
# 498
*_tmp121=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple0*v))Cyc_Dict_insert)(*_tmp121,(struct _tuple14*)_tmp126,res);{
# 499
struct _tuple0*_tmp12C=res;_npop_handler(0);return _tmp12C;};};};}
# 492
;_pop_dynregion(d);};};}
# 503
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple17{void*f1;struct Cyc_List_List*f2;};
# 507
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
# 508
struct _DynRegionHandle*_tmp132;struct Cyc_List_List**_tmp133;struct Cyc_Toc_TocState _tmp131=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp132=_tmp131.dyn;_tmp133=_tmp131.tuple_types;{
# 509
struct _DynRegionFrame _tmp134;struct _RegionHandle*d=_open_dynregion(& _tmp134,_tmp132);
# 511
{struct Cyc_List_List*_tmp135=*_tmp133;for(0;_tmp135 != 0;_tmp135=_tmp135->tl){
# 512
struct _tuple17 _tmp137;void*_tmp138;struct Cyc_List_List*_tmp139;struct _tuple17*_tmp136=(struct _tuple17*)_tmp135->hd;_tmp137=*_tmp136;_tmp138=_tmp137.f1;_tmp139=_tmp137.f2;{
# 513
struct Cyc_List_List*_tmp13A=tqs0;
# 514
for(0;_tmp13A != 0  && _tmp139 != 0;(_tmp13A=_tmp13A->tl,_tmp139=_tmp139->tl)){
# 515
if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp13A->hd)).f2,(void*)_tmp139->hd))
# 516
break;}
# 517
if(_tmp13A == 0  && _tmp139 == 0){
# 518
void*_tmp13B=_tmp138;_npop_handler(0);return _tmp13B;}};}}{
# 521
struct Cyc_Int_pa_PrintArg_struct _tmpACF;void*_tmpACE[1];const char*_tmpACD;struct _dyneither_ptr*_tmpACC;struct _dyneither_ptr*xname=(_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=(struct _dyneither_ptr)((_tmpACF.tag=1,((_tmpACF.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpACE[0]=& _tmpACF,Cyc_aprintf(((_tmpACD="_tuple%d",_tag_dyneither(_tmpACD,sizeof(char),9))),_tag_dyneither(_tmpACE,sizeof(void*),1)))))))),_tmpACC)));
# 522
void*x=Cyc_Absyn_strct(xname);
# 523
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,tqs0);
# 524
struct Cyc_List_List*_tmp13C=0;
# 525
struct Cyc_List_List*ts2=ts;
# 526
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
# 527
struct Cyc_Absyn_Aggrfield*_tmpAD2;struct Cyc_List_List*_tmpAD1;_tmp13C=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->hd=((_tmpAD2=_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->name=Cyc_Absyn_fieldname(i),((_tmpAD2->tq=Cyc_Toc_mt_tq,((_tmpAD2->type=(void*)ts2->hd,((_tmpAD2->width=0,((_tmpAD2->attributes=0,_tmpAD2)))))))))))),((_tmpAD1->tl=_tmp13C,_tmpAD1))))));}}
# 528
_tmp13C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13C);{
# 529
struct Cyc_Absyn_AggrdeclImpl*_tmpAD7;struct _tuple0*_tmpAD6;struct Cyc_Absyn_Aggrdecl*_tmpAD5;struct Cyc_Absyn_Aggrdecl*_tmp13F=(_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->kind=Cyc_Absyn_StructA,((_tmpAD5->sc=Cyc_Absyn_Public,((_tmpAD5->name=(
# 530
(_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->f1=Cyc_Absyn_Rel_n(0),((_tmpAD6->f2=xname,_tmpAD6)))))),((_tmpAD5->tvs=0,((_tmpAD5->impl=(
# 531
(_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7->exist_vars=0,((_tmpAD7->rgn_po=0,((_tmpAD7->fields=_tmp13C,((_tmpAD7->tagged=0,_tmpAD7)))))))))),((_tmpAD5->attributes=0,_tmpAD5)))))))))))));
# 536
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpADD;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpADC;struct Cyc_List_List*_tmpADB;Cyc_Toc_result_decls=((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB->hd=Cyc_Absyn_new_decl((void*)((_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD[0]=((_tmpADC.tag=6,((_tmpADC.f1=_tmp13F,_tmpADC)))),_tmpADD)))),0),((_tmpADB->tl=Cyc_Toc_result_decls,_tmpADB))))));}
# 537
{struct _tuple17*_tmpAE0;struct Cyc_List_List*_tmpADF;*_tmp133=((_tmpADF=_region_malloc(d,sizeof(*_tmpADF)),((_tmpADF->hd=((_tmpAE0=_region_malloc(d,sizeof(*_tmpAE0)),((_tmpAE0->f1=x,((_tmpAE0->f2=ts,_tmpAE0)))))),((_tmpADF->tl=*_tmp133,_tmpADF))))));}{
# 538
void*_tmp145=x;_npop_handler(0);return _tmp145;};};};
# 511
;_pop_dynregion(d);};}struct _tuple18{struct _tuple0*f1;struct Cyc_List_List*f2;void*f3;};
# 547
static void*Cyc_Toc_add_struct_type(struct _tuple0*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 551
struct _DynRegionHandle*_tmp14E;struct Cyc_List_List**_tmp14F;struct Cyc_Toc_TocState _tmp14D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp14E=_tmp14D.dyn;_tmp14F=_tmp14D.abs_struct_types;{
# 554
struct _DynRegionFrame _tmp150;struct _RegionHandle*d=_open_dynregion(& _tmp150,_tmp14E);
# 556
{struct Cyc_List_List*_tmp151=*_tmp14F;for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
# 557
struct _tuple18 _tmp153;struct _tuple0*_tmp154;struct Cyc_List_List*_tmp155;void*_tmp156;struct _tuple18*_tmp152=(struct _tuple18*)_tmp151->hd;_tmp153=*_tmp152;_tmp154=_tmp153.f1;_tmp155=_tmp153.f2;_tmp156=_tmp153.f3;
# 559
if(Cyc_Absyn_qvar_cmp(_tmp154,struct_name)== 0  && 
# 560
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(type_args)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp155)){
# 561
int okay=1;
# 562
{struct Cyc_List_List*_tmp157=type_args;for(0;_tmp157 != 0;(_tmp157=_tmp157->tl,_tmp155=_tmp155->tl)){
# 563
void*_tmp158=(void*)_tmp157->hd;
# 564
void*_tmp159=(void*)((struct Cyc_List_List*)_check_null(_tmp155))->hd;
# 566
if(Cyc_Tcutil_unify(_tmp158,_tmp159))
# 568
continue;
# 571
okay=0;
# 572
break;}}
# 574
if(okay){
# 576
void*_tmp15A=_tmp156;_npop_handler(0);return _tmp15A;}}}}{
# 584
struct Cyc_Int_pa_PrintArg_struct _tmpAE8;void*_tmpAE7[1];const char*_tmpAE6;struct _dyneither_ptr*_tmpAE5;struct _dyneither_ptr*xname=(_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5[0]=(struct _dyneither_ptr)((_tmpAE8.tag=1,((_tmpAE8.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAE7[0]=& _tmpAE8,Cyc_aprintf(((_tmpAE6="_tuple%d",_tag_dyneither(_tmpAE6,sizeof(char),9))),_tag_dyneither(_tmpAE7,sizeof(void*),1)))))))),_tmpAE5)));
# 585
void*x=Cyc_Absyn_strct(xname);
# 586
struct Cyc_List_List*_tmp15B=0;
# 588
{struct _tuple18*_tmpAEB;struct Cyc_List_List*_tmpAEA;*_tmp14F=((_tmpAEA=_region_malloc(d,sizeof(*_tmpAEA)),((_tmpAEA->hd=((_tmpAEB=_region_malloc(d,sizeof(*_tmpAEB)),((_tmpAEB->f1=struct_name,((_tmpAEB->f2=type_args,((_tmpAEB->f3=x,_tmpAEB)))))))),((_tmpAEA->tl=*_tmp14F,_tmpAEA))))));}{
# 591
struct _RegionHandle _tmp15E=_new_region("r");struct _RegionHandle*r=& _tmp15E;_push_region(r);
# 592
{struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,type_vars,type_args);
# 593
for(0;fields != 0;fields=fields->tl){
# 594
struct Cyc_Absyn_Aggrfield*_tmp160=(struct Cyc_Absyn_Aggrfield*)fields->hd;
# 595
void*t=_tmp160->type;
# 596
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp15F,t));
# 598
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
# 599
struct Cyc_Absyn_Aggrfield*_tmpAEE;struct Cyc_List_List*_tmpAED;_tmp15B=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->hd=((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->name=_tmp160->name,((_tmpAEE->tq=Cyc_Toc_mt_tq,((_tmpAEE->type=t,((_tmpAEE->width=_tmp160->width,((_tmpAEE->attributes=_tmp160->attributes,_tmpAEE)))))))))))),((_tmpAED->tl=_tmp15B,_tmpAED))))));};}
# 602
_tmp15B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15B);{
# 603
struct Cyc_Absyn_AggrdeclImpl*_tmpAF3;struct _tuple0*_tmpAF2;struct Cyc_Absyn_Aggrdecl*_tmpAF1;struct Cyc_Absyn_Aggrdecl*_tmp163=(_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->kind=Cyc_Absyn_StructA,((_tmpAF1->sc=Cyc_Absyn_Public,((_tmpAF1->name=(
# 604
(_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->f1=Cyc_Absyn_Rel_n(0),((_tmpAF2->f2=xname,_tmpAF2)))))),((_tmpAF1->tvs=0,((_tmpAF1->impl=(
# 605
(_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->exist_vars=0,((_tmpAF3->rgn_po=0,((_tmpAF3->fields=_tmp15B,((_tmpAF3->tagged=0,_tmpAF3)))))))))),((_tmpAF1->attributes=0,_tmpAF1)))))))))))));
# 610
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAF9;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAF8;struct Cyc_List_List*_tmpAF7;Cyc_Toc_result_decls=((_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->hd=Cyc_Absyn_new_decl((void*)((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAF8.tag=6,((_tmpAF8.f1=_tmp163,_tmpAF8)))),_tmpAF9)))),0),((_tmpAF7->tl=Cyc_Toc_result_decls,_tmpAF7))))));}{
# 611
void*_tmp167=x;_npop_handler(1);return _tmp167;};};}
# 592
;_pop_region(r);};};
# 556
;_pop_dynregion(d);};}
# 619
struct _tuple0*Cyc_Toc_temp_var(){
# 620
int _tmp16F=Cyc_Toc_temp_var_counter ++;
# 621
struct _dyneither_ptr*_tmpB06;const char*_tmpB05;void*_tmpB04[1];struct Cyc_Int_pa_PrintArg_struct _tmpB03;struct _tuple0*_tmpB02;struct _tuple0*res=(_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->f1=Cyc_Absyn_Loc_n,((_tmpB02->f2=((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((_tmpB06[0]=(struct _dyneither_ptr)((_tmpB03.tag=1,((_tmpB03.f1=(unsigned int)_tmp16F,((_tmpB04[0]=& _tmpB03,Cyc_aprintf(((_tmpB05="_tmp%X",_tag_dyneither(_tmpB05,sizeof(char),7))),_tag_dyneither(_tmpB04,sizeof(void*),1)))))))),_tmpB06)))),_tmpB02)))));
# 622
return res;}
# 627
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
# 628
struct _DynRegionHandle*_tmp176;struct Cyc_Xarray_Xarray*_tmp177;struct Cyc_Toc_TocState _tmp175=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp176=_tmp175.dyn;_tmp177=_tmp175.temp_labels;{
# 629
struct _DynRegionFrame _tmp178;struct _RegionHandle*d=_open_dynregion(& _tmp178,_tmp176);
# 630
{int _tmp179=Cyc_Toc_fresh_label_counter ++;
# 631
if(_tmp179 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp177)){
# 632
struct _dyneither_ptr*_tmp17A=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp177,_tmp179);_npop_handler(0);return _tmp17A;}{
# 633
struct Cyc_Int_pa_PrintArg_struct _tmpB0E;void*_tmpB0D[1];const char*_tmpB0C;struct _dyneither_ptr*_tmpB0B;struct _dyneither_ptr*res=(_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=(struct _dyneither_ptr)((_tmpB0E.tag=1,((_tmpB0E.f1=(unsigned int)_tmp179,((_tmpB0D[0]=& _tmpB0E,Cyc_aprintf(((_tmpB0C="_LL%X",_tag_dyneither(_tmpB0C,sizeof(char),6))),_tag_dyneither(_tmpB0D,sizeof(void*),1)))))))),_tmpB0B)));
# 634
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp177,res)!= _tmp179){
# 635
const char*_tmpB11;void*_tmpB10;(_tmpB10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB11="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpB11,sizeof(char),43))),_tag_dyneither(_tmpB10,sizeof(void*),0)));}{
# 636
struct _dyneither_ptr*_tmp17D=res;_npop_handler(0);return _tmp17D;};};}
# 630
;_pop_dynregion(d);};}
# 643
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple0*name){
# 644
int ans=0;
# 645
struct Cyc_List_List*_tmp182=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
# 646
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp182))->hd)->name)!= 0){
# 647
++ ans;
# 648
_tmp182=_tmp182->tl;}
# 650
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 656
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
# 657
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
# 658
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
# 659
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){
# 661
struct _dyneither_ptr*_tmp184;struct Cyc_Absyn_Tqual _tmp185;void*_tmp186;struct _tuple8 _tmp183=*a;_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;_tmp186=_tmp183.f3;{
# 662
struct _tuple8*_tmpB12;return(_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12->f1=_tmp184,((_tmpB12->f2=_tmp185,((_tmpB12->f3=Cyc_Toc_typ_to_c(_tmp186),_tmpB12)))))));};}
# 664
static struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){
# 665
struct Cyc_Absyn_Tqual _tmp189;void*_tmp18A;struct _tuple10 _tmp188=*x;_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{
# 666
struct _tuple10*_tmpB13;return(_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->f1=_tmp189,((_tmpB13->f2=Cyc_Toc_typ_to_c(_tmp18A),_tmpB13)))));};}
# 683 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
# 684
void*_tmp18C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp18E;void*_tmp18F;struct Cyc_Absyn_Tqual _tmp190;struct Cyc_Absyn_Exp*_tmp191;union Cyc_Absyn_Constraint*_tmp192;unsigned int _tmp193;void*_tmp195;_LL41: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18C;if(_tmp18D->tag != 8)goto _LL43;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18E.elt_type;_tmp190=_tmp18E.tq;_tmp191=_tmp18E.num_elts;_tmp192=_tmp18E.zero_term;_tmp193=_tmp18E.zt_loc;}}_LL42:
# 686
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp18F),_tmp190,_tmp191,Cyc_Absyn_false_conref,_tmp193);_LL43:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18C;if(_tmp194->tag != 1)goto _LL45;else{_tmp195=(void*)_tmp194->f2;}}if(!(_tmp195 != 0))goto _LL45;_LL44:
# 687
 return Cyc_Toc_typ_to_c_array((void*)_tmp195);_LL45:;_LL46:
# 688
 return Cyc_Toc_typ_to_c(t);_LL40:;}
# 692
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 694
struct Cyc_Absyn_Aggrfield*_tmpB14;return(_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->name=f->name,((_tmpB14->tq=Cyc_Toc_mt_tq,((_tmpB14->type=
# 696
Cyc_Toc_typ_to_c(f->type),((_tmpB14->width=f->width,((_tmpB14->attributes=f->attributes,_tmpB14)))))))))));}
# 700
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 702
return;}
# 705
static void*Cyc_Toc_char_star_typ(){
# 706
static void**cs=0;
# 707
if(cs == 0){
# 708
void**_tmpB15;cs=((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB15))));}
# 710
return*cs;}
# 712
static void*Cyc_Toc_rgn_typ(){
# 713
static void**r=0;
# 714
if(r == 0){
# 715
void**_tmpB16;r=((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpB16))));}
# 717
return*r;}
# 719
static void*Cyc_Toc_dyn_rgn_typ(){
# 720
static void**r=0;
# 721
if(r == 0){
# 722
void**_tmpB17;r=((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB17))));}
# 724
return*r;}
# 726
static int Cyc_Toc_is_boxed_tvar(void*t){
# 727
void*_tmp19A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp19C;_LL48: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp19A;if(_tmp19B->tag != 2)goto _LL4A;else{_tmp19C=_tmp19B->f1;}}_LL49:
# 729
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL4A:;_LL4B:
# 730
 return 0;_LL47:;}
# 733
static int Cyc_Toc_is_abstract_type(void*t){
# 734
struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind _tmp19E;enum Cyc_Absyn_KindQual _tmp19F;_LL4D: _tmp19E=*_tmp19D;_tmp19F=_tmp19E.kind;if(_tmp19F != Cyc_Absyn_AnyKind)goto _LL4F;_LL4E:
# 735
 return 1;_LL4F:;_LL50:
# 736
 return 0;_LL4C:;}
# 740
static void*Cyc_Toc_typ_to_c(void*t){
# 741
void*_tmp1A0=t;void*_tmp1A3;void**_tmp1A4;struct Cyc_Absyn_Tvar*_tmp1A6;struct Cyc_Absyn_DatatypeFieldInfo _tmp1A9;union Cyc_Absyn_DatatypeFieldInfoU _tmp1AA;struct _tuple1 _tmp1AB;struct Cyc_Absyn_Datatypedecl*_tmp1AC;struct Cyc_Absyn_Datatypefield*_tmp1AD;struct Cyc_Absyn_PtrInfo _tmp1B0;void*_tmp1B1;struct Cyc_Absyn_Tqual _tmp1B2;struct Cyc_Absyn_PtrAtts _tmp1B3;union Cyc_Absyn_Constraint*_tmp1B4;struct Cyc_Absyn_ArrayInfo _tmp1B8;void*_tmp1B9;struct Cyc_Absyn_Tqual _tmp1BA;struct Cyc_Absyn_Exp*_tmp1BB;unsigned int _tmp1BC;struct Cyc_Absyn_FnInfo _tmp1BE;struct Cyc_Absyn_Tqual _tmp1BF;void*_tmp1C0;struct Cyc_List_List*_tmp1C1;int _tmp1C2;struct Cyc_Absyn_VarargInfo*_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_List_List*_tmp1C6;enum Cyc_Absyn_AggrKind _tmp1C8;struct Cyc_List_List*_tmp1C9;struct Cyc_Absyn_AggrInfo _tmp1CB;union Cyc_Absyn_AggrInfoU _tmp1CC;struct Cyc_List_List*_tmp1CD;struct _tuple0*_tmp1CF;struct Cyc_List_List*_tmp1D1;struct _tuple0*_tmp1D3;struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Typedefdecl*_tmp1D5;void*_tmp1D6;void*_tmp1D9;struct Cyc_Absyn_TypeDecl*_tmp1E3;struct Cyc_Absyn_TypeDecl _tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Aggrdecl*_tmp1E7;struct Cyc_Absyn_TypeDecl*_tmp1E9;struct Cyc_Absyn_TypeDecl _tmp1EA;void*_tmp1EB;struct Cyc_Absyn_Enumdecl*_tmp1ED;struct Cyc_Absyn_TypeDecl*_tmp1EF;struct Cyc_Absyn_TypeDecl _tmp1F0;void*_tmp1F1;struct Cyc_Absyn_Datatypedecl*_tmp1F3;void**_tmp1F4;_LL52: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1A1=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A1->tag != 0)goto _LL54;}_LL53:
# 742
 return t;_LL54: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1A2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0;if(_tmp1A2->tag != 1)goto _LL56;else{_tmp1A3=(void**)& _tmp1A2->f2;_tmp1A4=(void**)((void**)& _tmp1A2->f2);}}_LL55:
# 744
 if(*_tmp1A4 == 0){
# 745
*_tmp1A4=(void*)Cyc_Absyn_sint_typ;
# 746
return Cyc_Absyn_sint_typ;}
# 748
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1A4));_LL56: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A5->tag != 2)goto _LL58;else{_tmp1A6=_tmp1A5->f1;}}_LL57:
# 750
 if((Cyc_Tcutil_tvar_kind(_tmp1A6,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 753
return(void*)& Cyc_Absyn_VoidType_val;else{
# 755
return Cyc_Absyn_void_star_typ();}_LL58: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1A7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A7->tag != 3)goto _LL5A;}_LL59:
# 757
 return(void*)& Cyc_Absyn_VoidType_val;_LL5A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A0;if(_tmp1A8->tag != 4)goto _LL5C;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A9.field_info;if((_tmp1AA.KnownDatatypefield).tag != 2)goto _LL5C;_tmp1AB=(struct _tuple1)(_tmp1AA.KnownDatatypefield).val;_tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f2;}}_LL5B:
# 759
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1AD->name,_tmp1AC->name));_LL5C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1AE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A0;if(_tmp1AE->tag != 4)goto _LL5E;}_LL5D: {
# 760
const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1A="unresolved DatatypeFieldType",_tag_dyneither(_tmpB1A,sizeof(char),29))),_tag_dyneither(_tmpB19,sizeof(void*),0)));}_LL5E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A0;if(_tmp1AF->tag != 5)goto _LL60;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1B0.elt_typ;_tmp1B2=_tmp1B0.elt_tq;_tmp1B3=_tmp1B0.ptr_atts;_tmp1B4=_tmp1B3.bounds;}}_LL5F:
# 764
 _tmp1B1=Cyc_Toc_typ_to_c_array(_tmp1B1);{
# 765
void*_tmp1F7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1B4);_LL8F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1F8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F7;if(_tmp1F8->tag != 0)goto _LL91;}_LL90:
# 766
 return Cyc_Toc_dyneither_ptr_typ;_LL91:;_LL92:
# 767
 return Cyc_Absyn_star_typ(_tmp1B1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1B2,Cyc_Absyn_false_conref);_LL8E:;};_LL60: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B5->tag != 6)goto _LL62;}_LL61:
# 769
 goto _LL63;_LL62: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B6->tag != 7)goto _LL64;}_LL63:
# 770
 return t;_LL64: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A0;if(_tmp1B7->tag != 8)goto _LL66;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B8.elt_type;_tmp1BA=_tmp1B8.tq;_tmp1BB=_tmp1B8.num_elts;_tmp1BC=_tmp1B8.zt_loc;}}_LL65:
# 772
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp1B9),_tmp1BA,_tmp1BB,Cyc_Absyn_false_conref,_tmp1BC);_LL66: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1A0;if(_tmp1BD->tag != 9)goto _LL68;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.ret_tqual;_tmp1C0=_tmp1BE.ret_typ;_tmp1C1=_tmp1BE.args;_tmp1C2=_tmp1BE.c_varargs;_tmp1C3=_tmp1BE.cyc_varargs;_tmp1C4=_tmp1BE.attributes;}}_LL67: {
# 778
struct Cyc_List_List*_tmp1F9=0;
# 779
for(0;_tmp1C4 != 0;_tmp1C4=_tmp1C4->tl){
# 780
void*_tmp1FA=(void*)_tmp1C4->hd;_LL94: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1FB=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FB->tag != 4)goto _LL96;}_LL95:
# 781
 goto _LL97;_LL96: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1FC=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FC->tag != 5)goto _LL98;}_LL97:
# 782
 goto _LL99;_LL98: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1FD=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FD->tag != 19)goto _LL9A;}_LL99:
# 783
 continue;_LL9A: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1FE=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FE->tag != 22)goto _LL9C;}_LL9B:
# 784
 continue;_LL9C: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1FF=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp1FF->tag != 21)goto _LL9E;}_LL9D:
# 785
 continue;_LL9E: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp200=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1FA;if(_tmp200->tag != 20)goto _LLA0;}_LL9F:
# 786
 continue;_LLA0:;_LLA1:
# 787
{struct Cyc_List_List*_tmpB1B;_tmp1F9=((_tmpB1B=_cycalloc(sizeof(*_tmpB1B)),((_tmpB1B->hd=(void*)_tmp1C4->hd,((_tmpB1B->tl=_tmp1F9,_tmpB1B))))));}goto _LL93;_LL93:;}{
# 789
struct Cyc_List_List*_tmp202=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1C1);
# 790
if(_tmp1C3 != 0){
# 792
void*_tmp203=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1C3->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
# 793
struct _tuple8*_tmpB1C;struct _tuple8*_tmp204=(_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C->f1=_tmp1C3->name,((_tmpB1C->f2=_tmp1C3->tq,((_tmpB1C->f3=_tmp203,_tmpB1C)))))));
# 794
struct Cyc_List_List*_tmpB1D;_tmp202=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp202,((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->hd=_tmp204,((_tmpB1D->tl=0,_tmpB1D)))))));}{
# 796
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB23;struct Cyc_Absyn_FnInfo _tmpB22;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB21;return(void*)((_tmpB21=_cycalloc(sizeof(*_tmpB21)),((_tmpB21[0]=((_tmpB23.tag=9,((_tmpB23.f1=((_tmpB22.tvars=0,((_tmpB22.effect=0,((_tmpB22.ret_tqual=_tmp1BF,((_tmpB22.ret_typ=Cyc_Toc_typ_to_c(_tmp1C0),((_tmpB22.args=_tmp202,((_tmpB22.c_varargs=_tmp1C2,((_tmpB22.cyc_varargs=0,((_tmpB22.rgn_po=0,((_tmpB22.attributes=_tmp1F9,_tmpB22)))))))))))))))))),_tmpB23)))),_tmpB21))));};};}_LL68: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1C5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1A0;if(_tmp1C5->tag != 10)goto _LL6A;else{_tmp1C6=_tmp1C5->f1;}}_LL69:
# 801
 _tmp1C6=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1C6);
# 802
return Cyc_Toc_add_tuple_type(_tmp1C6);_LL6A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1C7=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1A0;if(_tmp1C7->tag != 12)goto _LL6C;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL6B: {
# 806
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB26;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB25;return(void*)((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=12,((_tmpB26.f1=_tmp1C8,((_tmpB26.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1C9),_tmpB26)))))),_tmpB25))));}_LL6C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A0;if(_tmp1CA->tag != 11)goto _LL6E;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CB.aggr_info;_tmp1CD=_tmp1CB.targs;}}_LL6D:
# 810
{union Cyc_Absyn_AggrInfoU _tmp20C=_tmp1CC;struct _tuple2 _tmp20D;_LLA3: if((_tmp20C.UnknownAggr).tag != 1)goto _LLA5;_tmp20D=(struct _tuple2)(_tmp20C.UnknownAggr).val;_LLA4:
# 811
 return t;_LLA5:;_LLA6:
# 812
 goto _LLA2;_LLA2:;}{
# 814
struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(_tmp1CC);
# 815
if(_tmp20E->kind == Cyc_Absyn_UnionA)
# 816
return Cyc_Toc_aggrdecl_type(_tmp20E->name,Cyc_Absyn_unionq_typ);{
# 817
struct Cyc_List_List*_tmp20F=_tmp20E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->fields;
# 818
if(_tmp20F == 0)return Cyc_Toc_aggrdecl_type(_tmp20E->name,Cyc_Absyn_strctq);
# 819
for(0;_tmp20F->tl != 0;_tmp20F=_tmp20F->tl){;}{
# 820
void*_tmp210=((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type;
# 821
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp210)))
# 831 "toc.cyc"
return Cyc_Toc_add_struct_type(_tmp20E->name,_tmp20E->tvs,
# 832
((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c,_tmp1CD),((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20E->impl))->fields);
# 834
return Cyc_Toc_aggrdecl_type(_tmp20E->name,Cyc_Absyn_strctq);};};};_LL6E: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1CE=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1A0;if(_tmp1CE->tag != 13)goto _LL70;else{_tmp1CF=_tmp1CE->f1;}}_LL6F:
# 835
 return t;_LL70: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D0->tag != 14)goto _LL72;else{_tmp1D1=_tmp1D0->f1;}}_LL71:
# 836
 Cyc_Toc_enumfields_to_c(_tmp1D1);return t;_LL72: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1D2=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D2->tag != 17)goto _LL74;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D2->f2;_tmp1D5=_tmp1D2->f3;_tmp1D6=(void*)_tmp1D2->f4;}}_LL73:
# 838
 if(_tmp1D6 == 0  || Cyc_noexpand_r){
# 839
if(_tmp1D4 != 0){
# 840
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB29;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB28;return(void*)((_tmpB28=_cycalloc(sizeof(*_tmpB28)),((_tmpB28[0]=((_tmpB29.tag=17,((_tmpB29.f1=_tmp1D3,((_tmpB29.f2=0,((_tmpB29.f3=_tmp1D5,((_tmpB29.f4=(void*)0,_tmpB29)))))))))),_tmpB28))));}else{
# 841
return t;}}else{
# 843
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB2C;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB2B;return(void*)((_tmpB2B=_cycalloc(sizeof(*_tmpB2B)),((_tmpB2B[0]=((_tmpB2C.tag=17,((_tmpB2C.f1=_tmp1D3,((_tmpB2C.f2=0,((_tmpB2C.f3=_tmp1D5,((_tmpB2C.f4=(void*)((void*)Cyc_Toc_typ_to_c_array((void*)_tmp1D6)),_tmpB2C)))))))))),_tmpB2B))));}_LL74: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1D7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D7->tag != 19)goto _LL76;}_LL75:
# 844
 return Cyc_Absyn_uint_typ;_LL76: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1D8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A0;if(_tmp1D8->tag != 15)goto _LL78;else{_tmp1D9=(void*)_tmp1D8->f1;}}_LL77:
# 845
 return Cyc_Toc_rgn_typ();_LL78: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1A0;if(_tmp1DA->tag != 16)goto _LL7A;}_LL79:
# 846
 return Cyc_Toc_dyn_rgn_typ();_LL7A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1DB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DB->tag != 20)goto _LL7C;}_LL7B:
# 849
 goto _LL7D;_LL7C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DC->tag != 21)goto _LL7E;}_LL7D:
# 850
 goto _LL7F;_LL7E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1DD=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1A0;if(_tmp1DD->tag != 22)goto _LL80;}_LL7F:
# 851
 goto _LL81;_LL80: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1DE->tag != 23)goto _LL82;}_LL81:
# 852
 goto _LL83;_LL82: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1DF=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1DF->tag != 24)goto _LL84;}_LL83:
# 853
 goto _LL85;_LL84: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1A0;if(_tmp1E0->tag != 25)goto _LL86;}_LL85:
# 854
 goto _LL87;_LL86: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E1->tag != 18)goto _LL88;}_LL87:
# 855
 return Cyc_Absyn_void_star_typ();_LL88: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E2->tag != 26)goto _LL8A;else{_tmp1E3=_tmp1E2->f1;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1E6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1E5;if(_tmp1E6->tag != 0)goto _LL8A;else{_tmp1E7=_tmp1E6->f1;}};}}_LL89:
# 857
 Cyc_Toc_aggrdecl_to_c(_tmp1E7,1);
# 858
if(_tmp1E7->kind == Cyc_Absyn_UnionA)
# 859
return Cyc_Toc_aggrdecl_type(_tmp1E7->name,Cyc_Absyn_unionq_typ);else{
# 860
return Cyc_Toc_aggrdecl_type(_tmp1E7->name,Cyc_Absyn_strctq);}_LL8A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1E8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1E8->tag != 26)goto _LL8C;else{_tmp1E9=_tmp1E8->f1;_tmp1EA=*_tmp1E9;_tmp1EB=_tmp1EA.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1EC=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1EB;if(_tmp1EC->tag != 1)goto _LL8C;else{_tmp1ED=_tmp1EC->f1;}};}}_LL8B:
# 862
 Cyc_Toc_enumdecl_to_c(_tmp1ED);
# 863
return t;_LL8C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A0;if(_tmp1EE->tag != 26)goto _LL51;else{_tmp1EF=_tmp1EE->f1;_tmp1F0=*_tmp1EF;_tmp1F1=_tmp1F0.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1F2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1F1;if(_tmp1F2->tag != 2)goto _LL51;else{_tmp1F3=_tmp1F2->f1;}};_tmp1F4=_tmp1EE->f2;}}_LL8D:
# 865
 Cyc_Toc_datatypedecl_to_c(_tmp1F3);
# 866
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1F4)));_LL51:;}
# 870
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
# 871
void*_tmp215=t;struct Cyc_Absyn_ArrayInfo _tmp217;void*_tmp218;struct Cyc_Absyn_Tqual _tmp219;_LLA8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp215;if(_tmp216->tag != 8)goto _LLAA;else{_tmp217=_tmp216->f1;_tmp218=_tmp217.elt_type;_tmp219=_tmp217.tq;}}_LLA9:
# 873
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp218,(void*)& Cyc_Absyn_HeapRgn_val,_tmp219,Cyc_Absyn_false_conref),e);_LLAA:;_LLAB:
# 874
 return Cyc_Toc_cast_it(t,e);_LLA7:;}
# 880
static int Cyc_Toc_atomic_typ(void*t){
# 881
void*_tmp21A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp224;void*_tmp225;struct Cyc_Absyn_AggrInfo _tmp227;union Cyc_Absyn_AggrInfoU _tmp228;struct Cyc_List_List*_tmp22A;struct Cyc_Absyn_DatatypeFieldInfo _tmp22C;union Cyc_Absyn_DatatypeFieldInfoU _tmp22D;struct _tuple1 _tmp22E;struct Cyc_Absyn_Datatypedecl*_tmp22F;struct Cyc_Absyn_Datatypefield*_tmp230;struct Cyc_List_List*_tmp232;_LLAD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp21A;if(_tmp21B->tag != 0)goto _LLAF;}_LLAE:
# 882
 return 1;_LLAF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp21A;if(_tmp21C->tag != 2)goto _LLB1;}_LLB0:
# 883
 return 0;_LLB1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21A;if(_tmp21D->tag != 6)goto _LLB3;}_LLB2:
# 884
 goto _LLB4;_LLB3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp21A;if(_tmp21E->tag != 13)goto _LLB5;}_LLB4:
# 885
 goto _LLB6;_LLB5: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp21A;if(_tmp21F->tag != 14)goto _LLB7;}_LLB6:
# 886
 goto _LLB8;_LLB7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21A;if(_tmp220->tag != 7)goto _LLB9;}_LLB8:
# 887
 goto _LLBA;_LLB9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp21A;if(_tmp221->tag != 9)goto _LLBB;}_LLBA:
# 888
 goto _LLBC;_LLBB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp21A;if(_tmp222->tag != 19)goto _LLBD;}_LLBC:
# 889
 return 1;_LLBD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp21A;if(_tmp223->tag != 8)goto _LLBF;else{_tmp224=_tmp223->f1;_tmp225=_tmp224.elt_type;}}_LLBE:
# 890
 return Cyc_Toc_atomic_typ(_tmp225);_LLBF: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp21A;if(_tmp226->tag != 11)goto _LLC1;else{_tmp227=_tmp226->f1;_tmp228=_tmp227.aggr_info;}}_LLC0:
# 897
{union Cyc_Absyn_AggrInfoU _tmp237=_tmp228;struct _tuple2 _tmp238;_LLD2: if((_tmp237.UnknownAggr).tag != 1)goto _LLD4;_tmp238=(struct _tuple2)(_tmp237.UnknownAggr).val;_LLD3:
# 898
 return 0;_LLD4:;_LLD5:
# 899
 goto _LLD1;_LLD1:;}{
# 901
struct Cyc_Absyn_Aggrdecl*_tmp239=Cyc_Absyn_get_known_aggrdecl(_tmp228);
# 902
if(_tmp239->impl == 0)
# 903
return 0;
# 904
{struct Cyc_List_List*_tmp23A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp239->impl))->fields;for(0;_tmp23A != 0;_tmp23A=_tmp23A->tl){
# 905
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp23A->hd)->type))return 0;}}
# 906
return 1;};_LLC1: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp21A;if(_tmp229->tag != 12)goto _LLC3;else{_tmp22A=_tmp229->f2;}}_LLC2:
# 908
 for(0;_tmp22A != 0;_tmp22A=_tmp22A->tl){
# 909
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp22A->hd)->type))return 0;}
# 910
return 1;_LLC3: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp21A;if(_tmp22B->tag != 4)goto _LLC5;else{_tmp22C=_tmp22B->f1;_tmp22D=_tmp22C.field_info;if((_tmp22D.KnownDatatypefield).tag != 2)goto _LLC5;_tmp22E=(struct _tuple1)(_tmp22D.KnownDatatypefield).val;_tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;}}_LLC4:
# 912
 _tmp232=_tmp230->typs;goto _LLC6;_LLC5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp21A;if(_tmp231->tag != 10)goto _LLC7;else{_tmp232=_tmp231->f1;}}_LLC6:
# 914
 for(0;_tmp232 != 0;_tmp232=_tmp232->tl){
# 915
if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp232->hd)).f2))return 0;}
# 916
return 1;_LLC7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp21A;if(_tmp233->tag != 3)goto _LLC9;}_LLC8:
# 919
 goto _LLCA;_LLC9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp234=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21A;if(_tmp234->tag != 5)goto _LLCB;}_LLCA:
# 920
 goto _LLCC;_LLCB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp21A;if(_tmp235->tag != 16)goto _LLCD;}_LLCC:
# 921
 goto _LLCE;_LLCD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp21A;if(_tmp236->tag != 15)goto _LLCF;}_LLCE:
# 922
 return 0;_LLCF:;_LLD0: {
# 923
const char*_tmpB30;void*_tmpB2F[1];struct Cyc_String_pa_PrintArg_struct _tmpB2E;(_tmpB2E.tag=0,((_tmpB2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB2F[0]=& _tmpB2E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB30="atomic_typ:  bad type %s",_tag_dyneither(_tmpB30,sizeof(char),25))),_tag_dyneither(_tmpB2F,sizeof(void*),1)))))));}_LLAC:;}
# 927
static int Cyc_Toc_is_void_star(void*t){
# 928
void*_tmp23E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp240;void*_tmp241;_LLD7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23E;if(_tmp23F->tag != 5)goto _LLD9;else{_tmp240=_tmp23F->f1;_tmp241=_tmp240.elt_typ;}}_LLD8: {
# 930
void*_tmp242=Cyc_Tcutil_compress(_tmp241);_LLDC: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp242;if(_tmp243->tag != 0)goto _LLDE;}_LLDD:
# 931
 return 1;_LLDE:;_LLDF:
# 932
 return 0;_LLDB:;}_LLD9:;_LLDA:
# 934
 return 0;_LLD6:;}
# 938
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
# 939
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 943
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
# 944
void*_tmp244=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp246;union Cyc_Absyn_AggrInfoU _tmp247;struct Cyc_List_List*_tmp249;_LLE1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp245=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp244;if(_tmp245->tag != 11)goto _LLE3;else{_tmp246=_tmp245->f1;_tmp247=_tmp246.aggr_info;}}_LLE2: {
# 946
struct Cyc_Absyn_Aggrdecl*_tmp24A=Cyc_Absyn_get_known_aggrdecl(_tmp247);
# 947
if(_tmp24A->impl == 0){
# 948
const char*_tmpB33;void*_tmpB32;(_tmpB32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB33="is_poly_field: type missing fields",_tag_dyneither(_tmpB33,sizeof(char),35))),_tag_dyneither(_tmpB32,sizeof(void*),0)));}
# 949
_tmp249=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24A->impl))->fields;goto _LLE4;}_LLE3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244;if(_tmp248->tag != 12)goto _LLE5;else{_tmp249=_tmp248->f2;}}_LLE4: {
# 951
struct Cyc_Absyn_Aggrfield*_tmp24D=Cyc_Absyn_lookup_field(_tmp249,f);
# 952
if(_tmp24D == 0){
# 953
const char*_tmpB37;void*_tmpB36[1];struct Cyc_String_pa_PrintArg_struct _tmpB35;(_tmpB35.tag=0,((_tmpB35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB36[0]=& _tmpB35,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB37="is_poly_field: bad field %s",_tag_dyneither(_tmpB37,sizeof(char),28))),_tag_dyneither(_tmpB36,sizeof(void*),1)))))));}
# 954
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp24D->type);}_LLE5:;_LLE6: {
# 955
const char*_tmpB3B;void*_tmpB3A[1];struct Cyc_String_pa_PrintArg_struct _tmpB39;(_tmpB39.tag=0,((_tmpB39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB3A[0]=& _tmpB39,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3B="is_poly_field: bad type %s",_tag_dyneither(_tmpB3B,sizeof(char),27))),_tag_dyneither(_tmpB3A,sizeof(void*),1)))))));}_LLE0:;}
# 962
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
# 963
void*_tmp254=e->r;struct Cyc_Absyn_Exp*_tmp256;struct _dyneither_ptr*_tmp257;struct Cyc_Absyn_Exp*_tmp259;struct _dyneither_ptr*_tmp25A;_LLE8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp254;if(_tmp255->tag != 20)goto _LLEA;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}_LLE9:
# 964
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp256->topt),_tmp257);_LLEA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp254;if(_tmp258->tag != 21)goto _LLEC;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;}}_LLEB: {
# 966
void*_tmp25B=Cyc_Tcutil_compress((void*)_check_null(_tmp259->topt));struct Cyc_Absyn_PtrInfo _tmp25D;void*_tmp25E;_LLEF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25B;if(_tmp25C->tag != 5)goto _LLF1;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25D.elt_typ;}}_LLF0:
# 967
 return Cyc_Toc_is_poly_field(_tmp25E,_tmp25A);_LLF1:;_LLF2: {
# 968
const char*_tmpB3F;void*_tmpB3E[1];struct Cyc_String_pa_PrintArg_struct _tmpB3D;(_tmpB3D.tag=0,((_tmpB3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp259->topt))),((_tmpB3E[0]=& _tmpB3D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="is_poly_project: bad type %s",_tag_dyneither(_tmpB3F,sizeof(char),29))),_tag_dyneither(_tmpB3E,sizeof(void*),1)))))));}_LLEE:;}_LLEC:;_LLED:
# 970
 return 0;_LLE7:;}
# 975
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
# 976
struct Cyc_List_List*_tmpB40;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40->hd=s,((_tmpB40->tl=0,_tmpB40)))))),0);}
# 979
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
# 980
struct Cyc_List_List*_tmpB41;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41->hd=s,((_tmpB41->tl=0,_tmpB41)))))),0);}
# 983
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
# 984
if(Cyc_Toc_atomic_typ(t))
# 985
return Cyc_Toc_malloc_atomic(s);
# 986
return Cyc_Toc_malloc_ptr(s);}
# 989
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
# 990
struct Cyc_Absyn_Exp*_tmpB42[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB42[1]=s,((_tmpB42[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB42,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 993
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
# 994
if(Cyc_Toc_atomic_typ(elt_type)){
# 995
struct Cyc_Absyn_Exp*_tmpB43[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB43[1]=n,((_tmpB43[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB43,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 997
struct Cyc_Absyn_Exp*_tmpB44[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB44[1]=n,((_tmpB44[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB44,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1000
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
# 1001
struct Cyc_Absyn_Exp*_tmpB45[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB45[2]=n,((_tmpB45[1]=s,((_tmpB45[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB45,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1005
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
# 1006
struct Cyc_List_List*_tmpB46;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB46=_cycalloc(sizeof(*_tmpB46)),((_tmpB46->hd=e,((_tmpB46->tl=0,_tmpB46)))))),0);}
# 1009
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
# 1010
struct Cyc_Absyn_Exp*_tmpB47[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB47,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1015
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1023
int is_string=0;
# 1024
{void*_tmp26A=e->r;union Cyc_Absyn_Cnst _tmp26C;struct _dyneither_ptr _tmp26D;union Cyc_Absyn_Cnst _tmp26F;struct _dyneither_ptr _tmp270;_LLF4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp26B->tag != 0)goto _LLF6;else{_tmp26C=_tmp26B->f1;if((_tmp26C.String_c).tag != 8)goto _LLF6;_tmp26D=(struct _dyneither_ptr)(_tmp26C.String_c).val;}}_LLF5:
# 1025
 is_string=1;goto _LLF3;_LLF6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26A;if(_tmp26E->tag != 0)goto _LLF8;else{_tmp26F=_tmp26E->f1;if((_tmp26F.Wstring_c).tag != 9)goto _LLF8;_tmp270=(struct _dyneither_ptr)(_tmp26F.Wstring_c).val;}}_LLF7:
# 1026
 is_string=1;goto _LLF3;_LLF8:;_LLF9:
# 1027
 goto _LLF3;_LLF3:;}{
# 1029
struct Cyc_Absyn_Exp*xexp;
# 1030
struct Cyc_Absyn_Exp*xplussz;
# 1031
if(is_string){
# 1032
struct _tuple0*x=Cyc_Toc_temp_var();
# 1033
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,0);
# 1034
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)e);
# 1035
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB4D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB4C;struct Cyc_List_List*_tmpB4B;Cyc_Toc_result_decls=((_tmpB4B=_cycalloc(sizeof(*_tmpB4B)),((_tmpB4B->hd=Cyc_Absyn_new_decl((void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4C.tag=0,((_tmpB4C.f1=vd,_tmpB4C)))),_tmpB4D)))),0),((_tmpB4B->tl=Cyc_Toc_result_decls,_tmpB4B))))));}
# 1036
xexp=Cyc_Absyn_var_exp(x,0);
# 1037
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1039
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1041
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1043
struct Cyc_Absyn_Exp*urm_exp;
# 1044
{struct _tuple16*_tmpB54;struct _tuple16*_tmpB53;struct _tuple16*_tmpB52;struct _tuple16*_tmpB51[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB51[2]=(
# 1046
(_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->f1=0,((_tmpB52->f2=xplussz,_tmpB52)))))),((_tmpB51[1]=(
# 1045
(_tmpB53=_cycalloc(sizeof(*_tmpB53)),((_tmpB53->f1=0,((_tmpB53->f2=xexp,_tmpB53)))))),((_tmpB51[0]=(
# 1044
(_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->f1=0,((_tmpB54->f2=xexp,_tmpB54)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB51,sizeof(struct _tuple16*),3)))))))),0);}
# 1047
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct _RegionHandle*rgn;};
# 1083 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1085
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
# 1086
struct Cyc_Toc_Env _tmp279;int _tmp27A;struct Cyc_Toc_Env*_tmp278=nv;_tmp279=*_tmp278;_tmp27A=_tmp279.toplevel;
# 1087
return _tmp27A;}
# 1089
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
# 1090
struct Cyc_Toc_Env _tmp27C;int*_tmp27D;struct Cyc_Toc_Env*_tmp27B=nv;_tmp27C=*_tmp27B;_tmp27D=_tmp27C.in_lhs;
# 1091
return*_tmp27D;}
# 1094
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple0*x){
# 1095
struct Cyc_Toc_Env _tmp27F;struct Cyc_Dict_Dict _tmp280;struct Cyc_Toc_Env*_tmp27E=nv;_tmp27F=*_tmp27E;_tmp280=_tmp27F.varmap;
# 1096
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp280,x);}
# 1100
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
# 1101
int*_tmpB57;struct Cyc_Toc_Env*_tmpB56;return(_tmpB56=_region_malloc(r,sizeof(*_tmpB56)),((_tmpB56->break_lab=(struct _dyneither_ptr**)0,((_tmpB56->continue_lab=(struct _dyneither_ptr**)0,((_tmpB56->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB56->varmap=(struct Cyc_Dict_Dict)
# 1104
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB56->toplevel=(int)1,((_tmpB56->in_lhs=(int*)(
# 1106
(_tmpB57=_region_malloc(r,sizeof(*_tmpB57)),((_tmpB57[0]=0,_tmpB57)))),((_tmpB56->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB56)))))))))))))));}
# 1110
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1111
struct Cyc_Toc_Env _tmp284;struct _dyneither_ptr**_tmp285;struct _dyneither_ptr**_tmp286;struct Cyc_Toc_FallthruInfo*_tmp287;struct Cyc_Dict_Dict _tmp288;int _tmp289;int*_tmp28A;struct Cyc_Toc_Env*_tmp283=e;_tmp284=*_tmp283;_tmp285=_tmp284.break_lab;_tmp286=_tmp284.continue_lab;_tmp287=_tmp284.fallthru_info;_tmp288=_tmp284.varmap;_tmp289=_tmp284.toplevel;_tmp28A=_tmp284.in_lhs;{
# 1112
struct Cyc_Toc_Env*_tmpB58;return(_tmpB58=_region_malloc(r,sizeof(*_tmpB58)),((_tmpB58->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp285),((_tmpB58->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp286),((_tmpB58->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp287,((_tmpB58->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp288),((_tmpB58->toplevel=(int)_tmp289,((_tmpB58->in_lhs=(int*)_tmp28A,((_tmpB58->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB58)))))))))))))));};}
# 1115
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1116
struct Cyc_Toc_Env _tmp28D;struct _dyneither_ptr**_tmp28E;struct _dyneither_ptr**_tmp28F;struct Cyc_Toc_FallthruInfo*_tmp290;struct Cyc_Dict_Dict _tmp291;int _tmp292;int*_tmp293;struct Cyc_Toc_Env*_tmp28C=e;_tmp28D=*_tmp28C;_tmp28E=_tmp28D.break_lab;_tmp28F=_tmp28D.continue_lab;_tmp290=_tmp28D.fallthru_info;_tmp291=_tmp28D.varmap;_tmp292=_tmp28D.toplevel;_tmp293=_tmp28D.in_lhs;{
# 1117
struct Cyc_Toc_Env*_tmpB59;return(_tmpB59=_region_malloc(r,sizeof(*_tmpB59)),((_tmpB59->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp28E),((_tmpB59->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp28F),((_tmpB59->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp290,((_tmpB59->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp291),((_tmpB59->toplevel=(int)0,((_tmpB59->in_lhs=(int*)_tmp293,((_tmpB59->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB59)))))))))))))));};}
# 1119
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1120
struct Cyc_Toc_Env _tmp296;struct _dyneither_ptr**_tmp297;struct _dyneither_ptr**_tmp298;struct Cyc_Toc_FallthruInfo*_tmp299;struct Cyc_Dict_Dict _tmp29A;int _tmp29B;int*_tmp29C;struct Cyc_Toc_Env*_tmp295=e;_tmp296=*_tmp295;_tmp297=_tmp296.break_lab;_tmp298=_tmp296.continue_lab;_tmp299=_tmp296.fallthru_info;_tmp29A=_tmp296.varmap;_tmp29B=_tmp296.toplevel;_tmp29C=_tmp296.in_lhs;{
# 1121
struct Cyc_Toc_Env*_tmpB5A;return(_tmpB5A=_region_malloc(r,sizeof(*_tmpB5A)),((_tmpB5A->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp297),((_tmpB5A->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp298),((_tmpB5A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp299,((_tmpB5A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29A),((_tmpB5A->toplevel=(int)1,((_tmpB5A->in_lhs=(int*)_tmp29C,((_tmpB5A->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB5A)))))))))))))));};}
# 1123
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
# 1124
struct Cyc_Toc_Env _tmp29F;int*_tmp2A0;struct Cyc_Toc_Env*_tmp29E=e;_tmp29F=*_tmp29E;_tmp2A0=_tmp29F.in_lhs;
# 1125
*_tmp2A0=b;}
# 1130
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){
# 1131
{union Cyc_Absyn_Nmspace _tmp2A1=(*x).f1;struct Cyc_List_List*_tmp2A2;_LLFB: if((_tmp2A1.Rel_n).tag != 1)goto _LLFD;_tmp2A2=(struct Cyc_List_List*)(_tmp2A1.Rel_n).val;_LLFC: {
# 1133
const char*_tmpB5E;void*_tmpB5D[1];struct Cyc_String_pa_PrintArg_struct _tmpB5C;(_tmpB5C.tag=0,((_tmpB5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB5D[0]=& _tmpB5C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5E="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB5E,sizeof(char),30))),_tag_dyneither(_tmpB5D,sizeof(void*),1)))))));}_LLFD:;_LLFE:
# 1134
 goto _LLFA;_LLFA:;}{
# 1136
struct Cyc_Toc_Env _tmp2A7;struct _dyneither_ptr**_tmp2A8;struct _dyneither_ptr**_tmp2A9;struct Cyc_Toc_FallthruInfo*_tmp2AA;struct Cyc_Dict_Dict _tmp2AB;int _tmp2AC;int*_tmp2AD;struct Cyc_Toc_Env*_tmp2A6=e;_tmp2A7=*_tmp2A6;_tmp2A8=_tmp2A7.break_lab;_tmp2A9=_tmp2A7.continue_lab;_tmp2AA=_tmp2A7.fallthru_info;_tmp2AB=_tmp2A7.varmap;_tmp2AC=_tmp2A7.toplevel;_tmp2AD=_tmp2A7.in_lhs;{
# 1137
struct Cyc_Dict_Dict _tmp2AE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AB),x,y);
# 1138
struct Cyc_Toc_Env*_tmpB5F;return(_tmpB5F=_region_malloc(r,sizeof(*_tmpB5F)),((_tmpB5F->break_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2A8),((_tmpB5F->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2A9),((_tmpB5F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AA,((_tmpB5F->varmap=(struct Cyc_Dict_Dict)_tmp2AE,((_tmpB5F->toplevel=(int)_tmp2AC,((_tmpB5F->in_lhs=(int*)_tmp2AD,((_tmpB5F->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB5F)))))))))))))));};};}
# 1143
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
# 1144
struct Cyc_Toc_Env _tmp2B1;struct _dyneither_ptr**_tmp2B2;struct _dyneither_ptr**_tmp2B3;struct Cyc_Toc_FallthruInfo*_tmp2B4;struct Cyc_Dict_Dict _tmp2B5;int _tmp2B6;int*_tmp2B7;struct Cyc_Toc_Env*_tmp2B0=e;_tmp2B1=*_tmp2B0;_tmp2B2=_tmp2B1.break_lab;_tmp2B3=_tmp2B1.continue_lab;_tmp2B4=_tmp2B1.fallthru_info;_tmp2B5=_tmp2B1.varmap;_tmp2B6=_tmp2B1.toplevel;_tmp2B7=_tmp2B1.in_lhs;{
# 1145
struct Cyc_Toc_Env*_tmpB60;return(_tmpB60=_region_malloc(r,sizeof(*_tmpB60)),((_tmpB60->break_lab=(struct _dyneither_ptr**)0,((_tmpB60->continue_lab=(struct _dyneither_ptr**)0,((_tmpB60->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B4,((_tmpB60->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B5),((_tmpB60->toplevel=(int)_tmp2B6,((_tmpB60->in_lhs=(int*)_tmp2B7,((_tmpB60->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB60)))))))))))))));};}
# 1149
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1154
struct Cyc_List_List*fallthru_vars=0;
# 1155
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
# 1156
struct Cyc_List_List*_tmpB61;fallthru_vars=((_tmpB61=_region_malloc(r,sizeof(*_tmpB61)),((_tmpB61->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB61->tl=fallthru_vars,_tmpB61))))));}
# 1157
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
# 1158
struct Cyc_Toc_Env _tmp2BB;struct _dyneither_ptr**_tmp2BC;struct _dyneither_ptr**_tmp2BD;struct Cyc_Toc_FallthruInfo*_tmp2BE;struct Cyc_Dict_Dict _tmp2BF;int _tmp2C0;int*_tmp2C1;struct Cyc_Toc_Env*_tmp2BA=e;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.break_lab;_tmp2BD=_tmp2BB.continue_lab;_tmp2BE=_tmp2BB.fallthru_info;_tmp2BF=_tmp2BB.varmap;_tmp2C0=_tmp2BB.toplevel;_tmp2C1=_tmp2BB.in_lhs;{
# 1159
struct Cyc_Toc_Env _tmp2C3;struct Cyc_Dict_Dict _tmp2C4;struct Cyc_Toc_Env*_tmp2C2=next_case_env;_tmp2C3=*_tmp2C2;_tmp2C4=_tmp2C3.varmap;{
# 1160
struct Cyc_Toc_FallthruInfo*_tmpB62;struct Cyc_Toc_FallthruInfo*fi=
# 1161
(_tmpB62=_region_malloc(r,sizeof(*_tmpB62)),((_tmpB62->label=fallthru_l,((_tmpB62->binders=fallthru_vars,((_tmpB62->next_case_env=
# 1162
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C4),_tmpB62)))))));
# 1163
struct _dyneither_ptr**_tmpB65;struct Cyc_Toc_Env*_tmpB64;return(_tmpB64=_region_malloc(r,sizeof(*_tmpB64)),((_tmpB64->break_lab=(struct _dyneither_ptr**)((_tmpB65=_region_malloc(r,sizeof(*_tmpB65)),((_tmpB65[0]=break_l,_tmpB65)))),((_tmpB64->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2BD),((_tmpB64->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB64->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BF),((_tmpB64->toplevel=(int)_tmp2C0,((_tmpB64->in_lhs=(int*)((int*)_tmp2C1),((_tmpB64->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB64)))))))))))))));};};};}
# 1166
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1169
struct Cyc_Toc_Env _tmp2C9;struct _dyneither_ptr**_tmp2CA;struct _dyneither_ptr**_tmp2CB;struct Cyc_Toc_FallthruInfo*_tmp2CC;struct Cyc_Dict_Dict _tmp2CD;int _tmp2CE;int*_tmp2CF;struct Cyc_Toc_Env*_tmp2C8=e;_tmp2C9=*_tmp2C8;_tmp2CA=_tmp2C9.break_lab;_tmp2CB=_tmp2C9.continue_lab;_tmp2CC=_tmp2C9.fallthru_info;_tmp2CD=_tmp2C9.varmap;_tmp2CE=_tmp2C9.toplevel;_tmp2CF=_tmp2C9.in_lhs;{
# 1170
struct _dyneither_ptr**_tmpB68;struct Cyc_Toc_Env*_tmpB67;return(_tmpB67=_region_malloc(r,sizeof(*_tmpB67)),((_tmpB67->break_lab=(struct _dyneither_ptr**)((_tmpB68=_region_malloc(r,sizeof(*_tmpB68)),((_tmpB68[0]=break_l,_tmpB68)))),((_tmpB67->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2CB),((_tmpB67->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB67->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CD),((_tmpB67->toplevel=(int)_tmp2CE,((_tmpB67->in_lhs=(int*)_tmp2CF,((_tmpB67->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB67)))))))))))))));};}
# 1176
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1179
struct Cyc_Toc_Env _tmp2D3;struct _dyneither_ptr**_tmp2D4;struct _dyneither_ptr**_tmp2D5;struct Cyc_Toc_FallthruInfo*_tmp2D6;struct Cyc_Dict_Dict _tmp2D7;int _tmp2D8;int*_tmp2D9;struct Cyc_Toc_Env*_tmp2D2=e;_tmp2D3=*_tmp2D2;_tmp2D4=_tmp2D3.break_lab;_tmp2D5=_tmp2D3.continue_lab;_tmp2D6=_tmp2D3.fallthru_info;_tmp2D7=_tmp2D3.varmap;_tmp2D8=_tmp2D3.toplevel;_tmp2D9=_tmp2D3.in_lhs;{
# 1180
struct Cyc_Toc_FallthruInfo*_tmpB6B;struct Cyc_Toc_Env*_tmpB6A;return(_tmpB6A=_region_malloc(r,sizeof(*_tmpB6A)),((_tmpB6A->break_lab=(struct _dyneither_ptr**)0,((_tmpB6A->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp2D5),((_tmpB6A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB6B=_region_malloc(r,sizeof(*_tmpB6B)),((_tmpB6B->label=next_l,((_tmpB6B->binders=0,((_tmpB6B->next_case_env=
# 1181
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB6B)))))))),((_tmpB6A->varmap=(struct Cyc_Dict_Dict)
# 1182
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D7),((_tmpB6A->toplevel=(int)_tmp2D8,((_tmpB6A->in_lhs=(int*)((int*)_tmp2D9),((_tmpB6A->rgn=(struct _RegionHandle*)r,(struct Cyc_Toc_Env*)_tmpB6A)))))))))))))));};}
# 1194 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
# 1195
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1197
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1198
void*_tmp2DC=e->annot;_LL100: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2DD=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2DC;if(_tmp2DD->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL102;}_LL101:
# 1199
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL102: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2DE=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2DC;if(_tmp2DE->tag != Cyc_CfFlowInfo_NotZero)goto _LL104;}_LL103:
# 1200
 return 0;_LL104: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2DF=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2DC;if(_tmp2DF->tag != Cyc_CfFlowInfo_IsZero)goto _LL106;}_LL105:
# 1202
{const char*_tmpB6E;void*_tmpB6D;(_tmpB6D=0,Cyc_Tcutil_terr(e->loc,((_tmpB6E="dereference of NULL pointer",_tag_dyneither(_tmpB6E,sizeof(char),28))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));}
# 1203
return 0;_LL106: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2E0=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2DC;if(_tmp2E0->tag != Cyc_Absyn_EmptyAnnot)goto _LL108;}_LL107:
# 1207
 return 0;_LL108:;_LL109: {
# 1208
const char*_tmpB71;void*_tmpB70;(_tmpB70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB71="need_null_check",_tag_dyneither(_tmpB71,sizeof(char),16))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}_LLFF:;}
# 1212
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
# 1213
void*_tmp2E5=e->annot;struct Cyc_List_List*_tmp2E7;struct Cyc_List_List*_tmp2E9;_LL10B: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2E6=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2E5;if(_tmp2E6->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL10D;else{_tmp2E7=_tmp2E6->f1;}}_LL10C:
# 1214
 return _tmp2E7;_LL10D: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2E8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2E5;if(_tmp2E8->tag != Cyc_CfFlowInfo_NotZero)goto _LL10F;else{_tmp2E9=_tmp2E8->f1;}}_LL10E:
# 1215
 return _tmp2E9;_LL10F: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2EA=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2E5;if(_tmp2EA->tag != Cyc_CfFlowInfo_IsZero)goto _LL111;}_LL110:
# 1217
{const char*_tmpB74;void*_tmpB73;(_tmpB73=0,Cyc_Tcutil_terr(e->loc,((_tmpB74="dereference of NULL pointer",_tag_dyneither(_tmpB74,sizeof(char),28))),_tag_dyneither(_tmpB73,sizeof(void*),0)));}
# 1218
return 0;_LL111: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2EB=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2E5;if(_tmp2EB->tag != Cyc_Absyn_EmptyAnnot)goto _LL113;}_LL112:
# 1219
 return 0;_LL113:;_LL114: {
# 1220
const char*_tmpB77;void*_tmpB76;(_tmpB76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB77="get_relns",_tag_dyneither(_tmpB77,sizeof(char),10))),_tag_dyneither(_tmpB76,sizeof(void*),0)));}_LL10A:;}
# 1225
static int Cyc_Toc_check_const_array(unsigned int i,void*t){
# 1226
void*_tmp2F0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2F2;struct Cyc_Absyn_PtrAtts _tmp2F3;union Cyc_Absyn_Constraint*_tmp2F4;union Cyc_Absyn_Constraint*_tmp2F5;struct Cyc_Absyn_ArrayInfo _tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;_LL116: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F1->tag != 5)goto _LL118;else{_tmp2F2=_tmp2F1->f1;_tmp2F3=_tmp2F2.ptr_atts;_tmp2F4=_tmp2F3.bounds;_tmp2F5=_tmp2F3.zero_term;}}_LL117: {
# 1228
void*_tmp2F9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2F4);struct Cyc_Absyn_Exp*_tmp2FC;_LL11D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2FA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2F9;if(_tmp2FA->tag != 0)goto _LL11F;}_LL11E:
# 1229
 return 0;_LL11F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2FB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2F9;if(_tmp2FB->tag != 1)goto _LL11C;else{_tmp2FC=_tmp2FB->f1;}}_LL120: {
# 1231
unsigned int _tmp2FE;int _tmp2FF;struct _tuple11 _tmp2FD=Cyc_Evexp_eval_const_uint_exp(_tmp2FC);_tmp2FE=_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;
# 1232
return _tmp2FF  && i <= _tmp2FE;}_LL11C:;}_LL118: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2F6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F0;if(_tmp2F6->tag != 8)goto _LL11A;else{_tmp2F7=_tmp2F6->f1;_tmp2F8=_tmp2F7.num_elts;}}_LL119:
# 1235
 if(_tmp2F8 == 0)return 0;{
# 1236
unsigned int _tmp301;int _tmp302;struct _tuple11 _tmp300=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2F8);_tmp301=_tmp300.f1;_tmp302=_tmp300.f2;
# 1237
return _tmp302  && i <= _tmp301;};_LL11A:;_LL11B:
# 1238
 return 0;_LL115:;}
# 1243
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
# 1245
for(0;relns != 0;relns=relns->tl){
# 1246
struct Cyc_CfFlowInfo_Reln*_tmp303=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
# 1247
if(_tmp303->vd != y)continue;{
# 1248
union Cyc_CfFlowInfo_RelnOp _tmp304=_tmp303->rop;struct Cyc_Absyn_Vardecl*_tmp305;struct Cyc_Absyn_Vardecl*_tmp306;_LL122: if((_tmp304.LessNumelts).tag != 3)goto _LL124;_tmp305=(struct Cyc_Absyn_Vardecl*)(_tmp304.LessNumelts).val;_LL123:
# 1249
 _tmp306=_tmp305;goto _LL125;_LL124: if((_tmp304.LessEqNumelts).tag != 5)goto _LL126;_tmp306=(struct Cyc_Absyn_Vardecl*)(_tmp304.LessEqNumelts).val;_LL125:
# 1250
 if(_tmp306 == v)return 1;else{goto _LL121;}_LL126:;_LL127:
# 1251
 continue;_LL121:;};}
# 1254
return 0;}
# 1258
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e,void*vtype){
# 1260
{void*_tmp307=e->r;void*_tmp309;struct Cyc_Absyn_Vardecl*_tmp30B;void*_tmp30D;struct Cyc_Absyn_Vardecl*_tmp30F;void*_tmp311;struct Cyc_Absyn_Vardecl*_tmp313;void*_tmp315;struct Cyc_Absyn_Vardecl*_tmp317;union Cyc_Absyn_Cnst _tmp319;struct _tuple5 _tmp31A;enum Cyc_Absyn_Sign _tmp31B;int _tmp31C;union Cyc_Absyn_Cnst _tmp31E;struct _tuple5 _tmp31F;enum Cyc_Absyn_Sign _tmp320;int _tmp321;union Cyc_Absyn_Cnst _tmp323;struct _tuple5 _tmp324;enum Cyc_Absyn_Sign _tmp325;int _tmp326;enum Cyc_Absyn_Primop _tmp328;struct Cyc_List_List*_tmp329;struct Cyc_List_List _tmp32A;struct Cyc_Absyn_Exp*_tmp32B;_LL129: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp308=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp308->tag != 1)goto _LL12B;else{_tmp309=(void*)_tmp308->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp30A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp309;if(_tmp30A->tag != 5)goto _LL12B;else{_tmp30B=_tmp30A->f1;}};}}_LL12A:
# 1261
 _tmp30F=_tmp30B;goto _LL12C;_LL12B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp30C->tag != 1)goto _LL12D;else{_tmp30D=(void*)_tmp30C->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp30D;if(_tmp30E->tag != 4)goto _LL12D;else{_tmp30F=_tmp30E->f1;}};}}_LL12C:
# 1262
 _tmp313=_tmp30F;goto _LL12E;_LL12D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp310=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp310->tag != 1)goto _LL12F;else{_tmp311=(void*)_tmp310->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp312=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp311;if(_tmp312->tag != 1)goto _LL12F;else{_tmp313=_tmp312->f1;}};}}_LL12E:
# 1263
 _tmp317=_tmp313;goto _LL130;_LL12F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp314=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp314->tag != 1)goto _LL131;else{_tmp315=(void*)_tmp314->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp316=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp315;if(_tmp316->tag != 3)goto _LL131;else{_tmp317=_tmp316->f1;}};}}_LL130:
# 1265
 if(_tmp317->escapes)return 0;
# 1267
if(Cyc_Toc_check_leq_size_var(relns,v,_tmp317))return 1;
# 1268
goto _LL128;_LL131: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp318=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp318->tag != 0)goto _LL133;else{_tmp319=_tmp318->f1;if((_tmp319.Int_c).tag != 5)goto _LL133;_tmp31A=(struct _tuple5)(_tmp319.Int_c).val;_tmp31B=_tmp31A.f1;if(_tmp31B != Cyc_Absyn_None)goto _LL133;_tmp31C=_tmp31A.f2;}}_LL132:
# 1269
 _tmp321=_tmp31C;goto _LL134;_LL133: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp31D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp31D->tag != 0)goto _LL135;else{_tmp31E=_tmp31D->f1;if((_tmp31E.Int_c).tag != 5)goto _LL135;_tmp31F=(struct _tuple5)(_tmp31E.Int_c).val;_tmp320=_tmp31F.f1;if(_tmp320 != Cyc_Absyn_Signed)goto _LL135;_tmp321=_tmp31F.f2;}}_LL134:
# 1271
 return _tmp321 >= 0  && Cyc_Toc_check_const_array((unsigned int)_tmp321,vtype);_LL135: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp322=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp322->tag != 0)goto _LL137;else{_tmp323=_tmp322->f1;if((_tmp323.Int_c).tag != 5)goto _LL137;_tmp324=(struct _tuple5)(_tmp323.Int_c).val;_tmp325=_tmp324.f1;if(_tmp325 != Cyc_Absyn_Unsigned)goto _LL137;_tmp326=_tmp324.f2;}}_LL136:
# 1273
 return Cyc_Toc_check_const_array((unsigned int)_tmp326,vtype);_LL137: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp327=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp307;if(_tmp327->tag != 2)goto _LL139;else{_tmp328=_tmp327->f1;if(_tmp328 != Cyc_Absyn_Numelts)goto _LL139;_tmp329=_tmp327->f2;if(_tmp329 == 0)goto _LL139;_tmp32A=*_tmp329;_tmp32B=(struct Cyc_Absyn_Exp*)_tmp32A.hd;}}_LL138:
# 1276
{void*_tmp32C=_tmp32B->r;void*_tmp32E;struct Cyc_Absyn_Vardecl*_tmp330;void*_tmp332;struct Cyc_Absyn_Vardecl*_tmp334;void*_tmp336;struct Cyc_Absyn_Vardecl*_tmp338;void*_tmp33A;struct Cyc_Absyn_Vardecl*_tmp33C;_LL13C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp32D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp32C;if(_tmp32D->tag != 1)goto _LL13E;else{_tmp32E=(void*)_tmp32D->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp32F=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp32E;if(_tmp32F->tag != 5)goto _LL13E;else{_tmp330=_tmp32F->f1;}};}}_LL13D:
# 1277
 _tmp334=_tmp330;goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp331=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp32C;if(_tmp331->tag != 1)goto _LL140;else{_tmp332=(void*)_tmp331->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp333=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp332;if(_tmp333->tag != 4)goto _LL140;else{_tmp334=_tmp333->f1;}};}}_LL13F:
# 1278
 _tmp338=_tmp334;goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp335=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp32C;if(_tmp335->tag != 1)goto _LL142;else{_tmp336=(void*)_tmp335->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp337=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp336;if(_tmp337->tag != 1)goto _LL142;else{_tmp338=_tmp337->f1;}};}}_LL141:
# 1279
 _tmp33C=_tmp338;goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp339=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp32C;if(_tmp339->tag != 1)goto _LL144;else{_tmp33A=(void*)_tmp339->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp33B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp33A;if(_tmp33B->tag != 3)goto _LL144;else{_tmp33C=_tmp33B->f1;}};}}_LL143:
# 1281
 return _tmp33C == v;_LL144:;_LL145:
# 1282
 goto _LL13B;_LL13B:;}
# 1284
goto _LL128;_LL139:;_LL13A:
# 1285
 goto _LL128;_LL128:;}
# 1287
return 0;}static char _tmp34E[8]="*bogus*";struct _tuple19{void*f1;void*f2;};
# 1293
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1303 "toc.cyc"
struct Cyc_Absyn_Vardecl*x;
# 1305
{void*_tmp33D=a->r;void*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;void*_tmp343;struct Cyc_Absyn_Vardecl*_tmp345;void*_tmp347;struct Cyc_Absyn_Vardecl*_tmp349;void*_tmp34B;struct Cyc_Absyn_Vardecl*_tmp34D;_LL147: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp33E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp33D;if(_tmp33E->tag != 1)goto _LL149;else{_tmp33F=(void*)_tmp33E->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp340=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp33F;if(_tmp340->tag != 5)goto _LL149;else{_tmp341=_tmp340->f1;}};}}_LL148:
# 1306
 _tmp345=_tmp341;goto _LL14A;_LL149: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp342=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp33D;if(_tmp342->tag != 1)goto _LL14B;else{_tmp343=(void*)_tmp342->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp344=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp343;if(_tmp344->tag != 4)goto _LL14B;else{_tmp345=_tmp344->f1;}};}}_LL14A:
# 1307
 _tmp349=_tmp345;goto _LL14C;_LL14B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp346=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp33D;if(_tmp346->tag != 1)goto _LL14D;else{_tmp347=(void*)_tmp346->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp348=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp347;if(_tmp348->tag != 1)goto _LL14D;else{_tmp349=_tmp348->f1;}};}}_LL14C:
# 1308
 _tmp34D=_tmp349;goto _LL14E;_LL14D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp34A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp33D;if(_tmp34A->tag != 1)goto _LL14F;else{_tmp34B=(void*)_tmp34A->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp34C=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp34B;if(_tmp34C->tag != 3)goto _LL14F;else{_tmp34D=_tmp34C->f1;}};}}_LL14E:
# 1310
 if(_tmp34D->escapes)goto _LL150;
# 1311
x=_tmp34D;
# 1312
goto _LL146;_LL14F:;_LL150: {
# 1315
static struct _dyneither_ptr bogus_string={_tmp34E,_tmp34E,_tmp34E + 8};
# 1316
static struct _tuple0 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1318
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1324
x=& bogus_vardecl;
# 1325
x->type=a_typ;}_LL146:;}{
# 1327
void*_tmp34F=a_typ;
# 1329
inner_loop: {
# 1330
void*_tmp350=i->r;void*_tmp352;struct Cyc_Absyn_Exp*_tmp353;union Cyc_Absyn_Cnst _tmp355;struct _tuple5 _tmp356;enum Cyc_Absyn_Sign _tmp357;int _tmp358;union Cyc_Absyn_Cnst _tmp35A;struct _tuple5 _tmp35B;enum Cyc_Absyn_Sign _tmp35C;int _tmp35D;union Cyc_Absyn_Cnst _tmp35F;struct _tuple5 _tmp360;enum Cyc_Absyn_Sign _tmp361;int _tmp362;enum Cyc_Absyn_Primop _tmp364;struct Cyc_List_List*_tmp365;struct Cyc_List_List _tmp366;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_List_List*_tmp368;struct Cyc_List_List _tmp369;struct Cyc_Absyn_Exp*_tmp36A;void*_tmp36C;struct Cyc_Absyn_Vardecl*_tmp36E;void*_tmp370;struct Cyc_Absyn_Vardecl*_tmp372;void*_tmp374;struct Cyc_Absyn_Vardecl*_tmp376;void*_tmp378;struct Cyc_Absyn_Vardecl*_tmp37A;_LL152: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp351->tag != 13)goto _LL154;else{_tmp352=(void*)_tmp351->f1;_tmp353=_tmp351->f2;}}_LL153:
# 1331
 i=_tmp353;goto inner_loop;_LL154: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp354=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp354->tag != 0)goto _LL156;else{_tmp355=_tmp354->f1;if((_tmp355.Int_c).tag != 5)goto _LL156;_tmp356=(struct _tuple5)(_tmp355.Int_c).val;_tmp357=_tmp356.f1;if(_tmp357 != Cyc_Absyn_None)goto _LL156;_tmp358=_tmp356.f2;}}_LL155:
# 1332
 _tmp35D=_tmp358;goto _LL157;_LL156: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp359=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp359->tag != 0)goto _LL158;else{_tmp35A=_tmp359->f1;if((_tmp35A.Int_c).tag != 5)goto _LL158;_tmp35B=(struct _tuple5)(_tmp35A.Int_c).val;_tmp35C=_tmp35B.f1;if(_tmp35C != Cyc_Absyn_Signed)goto _LL158;_tmp35D=_tmp35B.f2;}}_LL157:
# 1334
 return _tmp35D >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp35D + 1),_tmp34F);_LL158: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp35E->tag != 0)goto _LL15A;else{_tmp35F=_tmp35E->f1;if((_tmp35F.Int_c).tag != 5)goto _LL15A;_tmp360=(struct _tuple5)(_tmp35F.Int_c).val;_tmp361=_tmp360.f1;if(_tmp361 != Cyc_Absyn_Unsigned)goto _LL15A;_tmp362=_tmp360.f2;}}_LL159:
# 1336
 return Cyc_Toc_check_const_array((unsigned int)(_tmp362 + 1),_tmp34F);_LL15A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp363->tag != 2)goto _LL15C;else{_tmp364=_tmp363->f1;if(_tmp364 != Cyc_Absyn_Mod)goto _LL15C;_tmp365=_tmp363->f2;if(_tmp365 == 0)goto _LL15C;_tmp366=*_tmp365;_tmp367=(struct Cyc_Absyn_Exp*)_tmp366.hd;_tmp368=_tmp366.tl;if(_tmp368 == 0)goto _LL15C;_tmp369=*_tmp368;_tmp36A=(struct Cyc_Absyn_Exp*)_tmp369.hd;}}_LL15B:
# 1340
 return Cyc_Toc_check_leq_size(relns,x,_tmp36A,_tmp34F);_LL15C: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp36B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp36B->tag != 1)goto _LL15E;else{_tmp36C=(void*)_tmp36B->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp36D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp36C;if(_tmp36D->tag != 5)goto _LL15E;else{_tmp36E=_tmp36D->f1;}};}}_LL15D:
# 1341
 _tmp372=_tmp36E;goto _LL15F;_LL15E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp36F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp36F->tag != 1)goto _LL160;else{_tmp370=(void*)_tmp36F->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp371=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp370;if(_tmp371->tag != 4)goto _LL160;else{_tmp372=_tmp371->f1;}};}}_LL15F:
# 1342
 _tmp376=_tmp372;goto _LL161;_LL160: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp373=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp373->tag != 1)goto _LL162;else{_tmp374=(void*)_tmp373->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp375=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp374;if(_tmp375->tag != 1)goto _LL162;else{_tmp376=_tmp375->f1;}};}}_LL161:
# 1343
 _tmp37A=_tmp376;goto _LL163;_LL162: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp377=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp350;if(_tmp377->tag != 1)goto _LL164;else{_tmp378=(void*)_tmp377->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp379=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp378;if(_tmp379->tag != 3)goto _LL164;else{_tmp37A=_tmp379->f1;}};}}_LL163:
# 1345
 if(_tmp37A->escapes)return 0;
# 1348
{struct Cyc_List_List*_tmp37B=relns;for(0;_tmp37B != 0;_tmp37B=_tmp37B->tl){
# 1349
struct Cyc_CfFlowInfo_Reln*_tmp37C=(struct Cyc_CfFlowInfo_Reln*)_tmp37B->hd;
# 1350
if(_tmp37C->vd == _tmp37A){
# 1351
union Cyc_CfFlowInfo_RelnOp _tmp37D=_tmp37C->rop;struct Cyc_Absyn_Vardecl*_tmp37E;struct _tuple12 _tmp37F;struct Cyc_Absyn_Vardecl*_tmp380;void*_tmp381;unsigned int _tmp382;unsigned int _tmp383;_LL167: if((_tmp37D.LessNumelts).tag != 3)goto _LL169;_tmp37E=(struct Cyc_Absyn_Vardecl*)(_tmp37D.LessNumelts).val;_LL168:
# 1353
 if(x == _tmp37E)return 1;else{goto _LL166;}_LL169: if((_tmp37D.LessVar).tag != 2)goto _LL16B;_tmp37F=(struct _tuple12)(_tmp37D.LessVar).val;_tmp380=_tmp37F.f1;_tmp381=_tmp37F.f2;_LL16A:
# 1367 "toc.cyc"
{struct _tuple19 _tmpB78;struct _tuple19 _tmp385=(_tmpB78.f1=Cyc_Tcutil_compress(_tmp381),((_tmpB78.f2=Cyc_Tcutil_compress((void*)_check_null(a->topt)),_tmpB78)));void*_tmp386;void*_tmp388;void*_tmp389;struct Cyc_Absyn_PtrInfo _tmp38B;struct Cyc_Absyn_PtrAtts _tmp38C;union Cyc_Absyn_Constraint*_tmp38D;_LL172: _tmp386=_tmp385.f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp386;if(_tmp387->tag != 19)goto _LL174;else{_tmp388=(void*)_tmp387->f1;}};_tmp389=_tmp385.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp389;if(_tmp38A->tag != 5)goto _LL174;else{_tmp38B=_tmp38A->f1;_tmp38C=_tmp38B.ptr_atts;_tmp38D=_tmp38C.bounds;}};_LL173:
# 1369
{void*_tmp38E=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp38D);struct Cyc_Absyn_Exp*_tmp390;_LL177: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp38F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp38E;if(_tmp38F->tag != 1)goto _LL179;else{_tmp390=_tmp38F->f1;}}_LL178: {
# 1371
struct Cyc_Absyn_Exp*_tmp391=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp388,0),0,Cyc_Absyn_No_coercion,0);
# 1377
if(Cyc_Evexp_lte_const_exp(_tmp391,_tmp390))
# 1378
return 1;
# 1379
goto _LL176;}_LL179:;_LL17A:
# 1380
 goto _LL176;_LL176:;}
# 1382
goto _LL171;_LL174:;_LL175:
# 1383
 goto _LL171;_LL171:;}
# 1386
{struct Cyc_List_List*_tmp392=relns;for(0;_tmp392 != 0;_tmp392=_tmp392->tl){
# 1387
struct Cyc_CfFlowInfo_Reln*_tmp393=(struct Cyc_CfFlowInfo_Reln*)_tmp392->hd;
# 1388
if(_tmp393->vd == _tmp380){
# 1389
union Cyc_CfFlowInfo_RelnOp _tmp394=_tmp393->rop;struct Cyc_Absyn_Vardecl*_tmp395;struct Cyc_Absyn_Vardecl*_tmp396;unsigned int _tmp397;unsigned int _tmp398;struct _tuple12 _tmp399;struct Cyc_Absyn_Vardecl*_tmp39A;_LL17C: if((_tmp394.LessEqNumelts).tag != 5)goto _LL17E;_tmp395=(struct Cyc_Absyn_Vardecl*)(_tmp394.LessEqNumelts).val;_LL17D:
# 1390
 _tmp396=_tmp395;goto _LL17F;_LL17E: if((_tmp394.LessNumelts).tag != 3)goto _LL180;_tmp396=(struct Cyc_Absyn_Vardecl*)(_tmp394.LessNumelts).val;_LL17F:
# 1392
 if(x == _tmp396)return 1;
# 1393
goto _LL17B;_LL180: if((_tmp394.EqualConst).tag != 1)goto _LL182;_tmp397=(unsigned int)(_tmp394.EqualConst).val;_LL181:
# 1395
 return Cyc_Toc_check_const_array(_tmp397,_tmp34F);_LL182: if((_tmp394.LessEqConst).tag != 6)goto _LL184;_tmp398=(unsigned int)(_tmp394.LessEqConst).val;if(!(_tmp398 > 0))goto _LL184;_LL183:
# 1397
 return Cyc_Toc_check_const_array(_tmp398,_tmp34F);_LL184: if((_tmp394.LessVar).tag != 2)goto _LL186;_tmp399=(struct _tuple12)(_tmp394.LessVar).val;_tmp39A=_tmp399.f1;_LL185:
# 1399
 if(Cyc_Toc_check_leq_size_var(relns,x,_tmp39A))return 1;
# 1400
goto _LL17B;_LL186:;_LL187:
# 1401
 goto _LL17B;_LL17B:;}}}
# 1404
goto _LL166;_LL16B: if((_tmp37D.LessConst).tag != 4)goto _LL16D;_tmp382=(unsigned int)(_tmp37D.LessConst).val;_LL16C:
# 1407
 return Cyc_Toc_check_const_array(_tmp382,_tmp34F);_LL16D: if((_tmp37D.LessEqConst).tag != 6)goto _LL16F;_tmp383=(unsigned int)(_tmp37D.LessEqConst).val;_LL16E:
# 1410
 return Cyc_Toc_check_const_array(_tmp383 + 1,_tmp34F);_LL16F:;_LL170:
# 1411
 goto _LL166;_LL166:;}}}
# 1415
goto _LL151;_LL164:;_LL165:
# 1416
 goto _LL151;_LL151:;}
# 1418
return 0;};}
# 1421
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
# 1422
if(e->topt == 0){const char*_tmpB7B;void*_tmpB7A;(_tmpB7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7B="Missing type in primop ",_tag_dyneither(_tmpB7B,sizeof(char),24))),_tag_dyneither(_tmpB7A,sizeof(void*),0)));}
# 1423
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1425
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
# 1426
if(e->topt == 0){const char*_tmpB7E;void*_tmpB7D;(_tmpB7D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7E="Missing type in primop ",_tag_dyneither(_tmpB7E,sizeof(char),24))),_tag_dyneither(_tmpB7D,sizeof(void*),0)));}
# 1427
return(void*)_check_null(e->topt);}
# 1429
static struct _tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
# 1430
struct _tuple10*_tmpB7F;return(_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->f1=Cyc_Toc_mt_tq,((_tmpB7F->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB7F)))));}
# 1432
static struct _tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
# 1433
Cyc_Toc_exp_to_c(nv,e);{
# 1434
struct _tuple16*_tmpB80;return(_tmpB80=_cycalloc(sizeof(*_tmpB80)),((_tmpB80->f1=0,((_tmpB80->f2=e,_tmpB80)))));};}
# 1437
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){
# 1439
struct Cyc_Absyn_Exp*eo;
# 1440
void*t;
# 1441
if(pointer){
# 1442
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{
# 1443
struct Cyc_Absyn_Exp*_tmp3A1=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
# 1444
if(rgnopt == 0  || Cyc_Absyn_no_regions)
# 1445
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp3A1): Cyc_Toc_malloc_ptr(_tmp3A1));else{
# 1447
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;
# 1448
Cyc_Toc_exp_to_c(nv,r);
# 1449
eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp3A1);}};}else{
# 1452
t=struct_typ;
# 1453
eo=0;}
# 1455
return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
# 1458
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1462
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1467
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1470
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
# 1471
{struct Cyc_List_List*_tmp3A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp3A2 != 0;_tmp3A2=_tmp3A2->tl){
# 1473
struct _tuple16 _tmp3A4;struct Cyc_List_List*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A6;struct _tuple16*_tmp3A3=(struct _tuple16*)_tmp3A2->hd;_tmp3A4=*_tmp3A3;_tmp3A5=_tmp3A4.f1;_tmp3A6=_tmp3A4.f2;{
# 1477
struct Cyc_Absyn_Exp*e_index;
# 1478
if(_tmp3A5 == 0)
# 1479
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1481
if(_tmp3A5->tl != 0){const char*_tmpB83;void*_tmpB82;(_tmpB82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB83="multiple designators in array",_tag_dyneither(_tmpB83,sizeof(char),30))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}{
# 1482
void*_tmp3A9=(void*)_tmp3A5->hd;
# 1483
void*_tmp3AA=_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AC;_LL189: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp3AB=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp3AA;if(_tmp3AB->tag != 0)goto _LL18B;else{_tmp3AC=_tmp3AB->f1;}}_LL18A:
# 1485
 Cyc_Toc_exp_to_c(nv,_tmp3AC);
# 1486
e_index=_tmp3AC;
# 1487
goto _LL188;_LL18B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3AD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3AA;if(_tmp3AD->tag != 1)goto _LL188;}_LL18C: {
# 1488
const char*_tmpB86;void*_tmpB85;(_tmpB85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB86="field name designators in array",_tag_dyneither(_tmpB86,sizeof(char),32))),_tag_dyneither(_tmpB85,sizeof(void*),0)));}_LL188:;};}{
# 1491
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
# 1492
void*_tmp3B0=_tmp3A6->r;struct Cyc_List_List*_tmp3B2;struct Cyc_Absyn_Vardecl*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;int _tmp3B7;void*_tmp3B9;struct Cyc_List_List*_tmp3BA;_LL18E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3B1=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3B0;if(_tmp3B1->tag != 25)goto _LL190;else{_tmp3B2=_tmp3B1->f1;}}_LL18F:
# 1494
 s=Cyc_Toc_init_array(nv,lval,_tmp3B2,s);
# 1495
goto _LL18D;_LL190: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3B3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3B0;if(_tmp3B3->tag != 26)goto _LL192;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=_tmp3B3->f2;_tmp3B6=_tmp3B3->f3;_tmp3B7=_tmp3B3->f4;}}_LL191:
# 1497
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp3B4,_tmp3B5,_tmp3B6,_tmp3B7,s,0);
# 1498
goto _LL18D;_LL192: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3B8=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3B0;if(_tmp3B8->tag != 28)goto _LL194;else{_tmp3B9=(void*)_tmp3B8->f1;_tmp3BA=_tmp3B8->f2;}}_LL193:
# 1500
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3B9,_tmp3BA,s);
# 1501
goto _LL18D;_LL194:;_LL195:
# 1503
 Cyc_Toc_exp_to_c(nv,_tmp3A6);
# 1504
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp3A6,0),s,0);
# 1505
goto _LL18D;_LL18D:;};};}}
# 1508
return s;}
# 1513
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1516
struct _tuple0*_tmp3BB=vd->name;
# 1517
void*_tmp3BC=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
# 1518
if(!e1_already_translated)
# 1519
Cyc_Toc_exp_to_c(nv,e1);{
# 1520
struct _RegionHandle _tmp3BD=_new_region("r2");struct _RegionHandle*r2=& _tmp3BD;_push_region(r2);
# 1521
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB89;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB88;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp3BB,Cyc_Absyn_varb_exp(_tmp3BB,(void*)((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88[0]=((_tmpB89.tag=4,((_tmpB89.f1=vd,_tmpB89)))),_tmpB88)))),0));
# 1522
struct _tuple0*max=Cyc_Toc_temp_var();
# 1523
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp3BB,0),Cyc_Absyn_signed_int_exp(0,0),0);
# 1524
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp3BB,0),Cyc_Absyn_var_exp(max,0),0);
# 1525
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp3BB,0),0);
# 1528
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp3BB,0),0);
# 1529
struct Cyc_Absyn_Stmt*body;
# 1532
{void*_tmp3BE=e2->r;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;int _tmp3C5;void*_tmp3C7;struct Cyc_List_List*_tmp3C8;_LL197: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3BF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3BE;if(_tmp3BF->tag != 25)goto _LL199;else{_tmp3C0=_tmp3BF->f1;}}_LL198:
# 1534
 body=Cyc_Toc_init_array(nv2,lval,_tmp3C0,Cyc_Toc_skip_stmt_dl());
# 1535
goto _LL196;_LL199: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3C1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3BE;if(_tmp3C1->tag != 26)goto _LL19B;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;_tmp3C4=_tmp3C1->f3;_tmp3C5=_tmp3C1->f4;}}_LL19A:
# 1537
 body=Cyc_Toc_init_comprehension(nv2,lval,_tmp3C2,_tmp3C3,_tmp3C4,_tmp3C5,Cyc_Toc_skip_stmt_dl(),0);
# 1538
goto _LL196;_LL19B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3C6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3BE;if(_tmp3C6->tag != 28)goto _LL19D;else{_tmp3C7=(void*)_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}}_LL19C:
# 1540
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp3C7,_tmp3C8,Cyc_Toc_skip_stmt_dl());
# 1541
goto _LL196;_LL19D:;_LL19E:
# 1543
 Cyc_Toc_exp_to_c(nv2,e2);
# 1544
body=Cyc_Absyn_assign_stmt(lval,e2,0);
# 1545
goto _LL196;_LL196:;}{
# 1547
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1549
if(zero_term){
# 1554
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
# 1555
Cyc_Toc_cast_it(_tmp3BC,Cyc_Absyn_uint_exp(0,0)),0);
# 1556
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1559
struct Cyc_Absyn_Stmt*_tmp3C9=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,
# 1560
Cyc_Absyn_declare_stmt(_tmp3BB,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp3C9;};};}
# 1521
;_pop_region(r2);};}
# 1565
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1568
{struct Cyc_List_List*_tmp3CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp3CC != 0;_tmp3CC=_tmp3CC->tl){
# 1569
struct _tuple16 _tmp3CE;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Exp*_tmp3D0;struct _tuple16*_tmp3CD=(struct _tuple16*)_tmp3CC->hd;_tmp3CE=*_tmp3CD;_tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;
# 1570
if(_tmp3CF == 0){
# 1571
const char*_tmpB8C;void*_tmpB8B;(_tmpB8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8C="empty designator list",_tag_dyneither(_tmpB8C,sizeof(char),22))),_tag_dyneither(_tmpB8B,sizeof(void*),0)));}
# 1572
if(_tmp3CF->tl != 0){
# 1573
const char*_tmpB8F;void*_tmpB8E;(_tmpB8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8F="too many designators in anonymous struct",_tag_dyneither(_tmpB8F,sizeof(char),41))),_tag_dyneither(_tmpB8E,sizeof(void*),0)));}{
# 1574
void*_tmp3D5=(void*)_tmp3CF->hd;struct _dyneither_ptr*_tmp3D7;_LL1A0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3D6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3D5;if(_tmp3D6->tag != 1)goto _LL1A2;else{_tmp3D7=_tmp3D6->f1;}}_LL1A1: {
# 1576
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp3D7,0);
# 1577
{void*_tmp3D8=_tmp3D0->r;struct Cyc_List_List*_tmp3DA;struct Cyc_Absyn_Vardecl*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;int _tmp3DF;void*_tmp3E1;struct Cyc_List_List*_tmp3E2;_LL1A5: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3D9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3D9->tag != 25)goto _LL1A7;else{_tmp3DA=_tmp3D9->f1;}}_LL1A6:
# 1579
 s=Cyc_Toc_init_array(nv,lval,_tmp3DA,s);goto _LL1A4;_LL1A7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3DB=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3DB->tag != 26)goto _LL1A9;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DB->f2;_tmp3DE=_tmp3DB->f3;_tmp3DF=_tmp3DB->f4;}}_LL1A8:
# 1581
 s=Cyc_Toc_init_comprehension(nv,lval,_tmp3DC,_tmp3DD,_tmp3DE,_tmp3DF,s,0);goto _LL1A4;_LL1A9: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3E0=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3D8;if(_tmp3E0->tag != 28)goto _LL1AB;else{_tmp3E1=(void*)_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;}}_LL1AA:
# 1583
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3E1,_tmp3E2,s);goto _LL1A4;_LL1AB:;_LL1AC:
# 1585
 Cyc_Toc_exp_to_c(nv,_tmp3D0);
# 1587
if(Cyc_Toc_is_poly_field(struct_type,_tmp3D7))
# 1588
_tmp3D0=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3D0);
# 1590
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3D0,0),0),s,0);
# 1591
goto _LL1A4;_LL1A4:;}
# 1593
goto _LL19F;}_LL1A2:;_LL1A3: {
# 1594
const char*_tmpB92;void*_tmpB91;(_tmpB91=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB92="array designator in struct",_tag_dyneither(_tmpB92,sizeof(char),27))),_tag_dyneither(_tmpB91,sizeof(void*),0)));}_LL19F:;};}}
# 1597
return s;}
# 1602
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1606
struct _RegionHandle _tmp3E5=_new_region("r");struct _RegionHandle*r=& _tmp3E5;_push_region(r);
# 1607
{struct Cyc_List_List*_tmp3E6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
# 1608
void*_tmp3E7=Cyc_Toc_add_tuple_type(_tmp3E6);
# 1610
struct _tuple0*_tmp3E8=Cyc_Toc_temp_var();
# 1611
struct Cyc_Absyn_Exp*_tmp3E9=Cyc_Absyn_var_exp(_tmp3E8,0);
# 1612
struct Cyc_Absyn_Stmt*_tmp3EA=Cyc_Absyn_exp_stmt(_tmp3E9,0);
# 1614
struct Cyc_Absyn_Exp*(*_tmp3EB)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
# 1616
int is_atomic=1;
# 1617
struct Cyc_List_List*_tmp3EC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
# 1618
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3EC);for(0;_tmp3EC != 0;(_tmp3EC=_tmp3EC->tl,-- i)){
# 1619
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3EC->hd;
# 1620
struct Cyc_Absyn_Exp*lval=_tmp3EB(_tmp3E9,Cyc_Absyn_fieldname(i),0);
# 1621
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
# 1622
void*_tmp3ED=e->r;struct Cyc_List_List*_tmp3EF;struct Cyc_Absyn_Vardecl*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;int _tmp3F4;_LL1AE: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3EE=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3ED;if(_tmp3EE->tag != 25)goto _LL1B0;else{_tmp3EF=_tmp3EE->f1;}}_LL1AF:
# 1624
 _tmp3EA=Cyc_Toc_init_array(nv,lval,_tmp3EF,_tmp3EA);
# 1625
goto _LL1AD;_LL1B0: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3F0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3ED;if(_tmp3F0->tag != 26)goto _LL1B2;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F0->f2;_tmp3F3=_tmp3F0->f3;_tmp3F4=_tmp3F0->f4;}}_LL1B1:
# 1627
 _tmp3EA=Cyc_Toc_init_comprehension(nv,lval,_tmp3F1,_tmp3F2,_tmp3F3,_tmp3F4,_tmp3EA,0);
# 1628
goto _LL1AD;_LL1B2:;_LL1B3:
# 1631
 Cyc_Toc_exp_to_c(nv,e);
# 1632
_tmp3EA=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3EB(_tmp3E9,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp3EA,0);
# 1634
goto _LL1AD;_LL1AD:;};}}{
# 1637
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Toc_make_struct(nv,_tmp3E8,_tmp3E7,_tmp3EA,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp3F5;};}
# 1607
;_pop_region(r);}
# 1641
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
# 1642
int i=1;
# 1643
{struct Cyc_List_List*_tmp3F6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp3F6 != 0;_tmp3F6=_tmp3F6->tl){
# 1644
struct Cyc_Absyn_Aggrfield*_tmp3F7=(struct Cyc_Absyn_Aggrfield*)_tmp3F6->hd;
# 1645
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp3F7->name,(struct _dyneither_ptr)*f)== 0)return i;
# 1646
++ i;}}{
# 1648
struct Cyc_String_pa_PrintArg_struct _tmpB9A;void*_tmpB99[1];const char*_tmpB98;void*_tmpB97;(_tmpB97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB9A.tag=0,((_tmpB9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB99[0]=& _tmpB9A,Cyc_aprintf(((_tmpB98="get_member_offset %s failed",_tag_dyneither(_tmpB98,sizeof(char),28))),_tag_dyneither(_tmpB99,sizeof(void*),1)))))))),_tag_dyneither(_tmpB97,sizeof(void*),0)));};}
# 1651
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple0*tdn){
# 1656
struct _tuple0*_tmp3FC=Cyc_Toc_temp_var();
# 1657
struct Cyc_Absyn_Exp*_tmp3FD=Cyc_Absyn_var_exp(_tmp3FC,0);
# 1658
struct Cyc_Absyn_Stmt*_tmp3FE=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp3FD),0);
# 1660
struct Cyc_Absyn_Exp*(*_tmp3FF)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
# 1661
void*_tmp400=Cyc_Toc_typ_to_c(struct_type);
# 1663
int is_atomic=1;
# 1664
struct Cyc_List_List*forall_types;
# 1665
struct Cyc_Absyn_Aggrdecl*ad;
# 1666
struct Cyc_List_List*aggrfields;
# 1667
{void*_tmp401=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo _tmp403;union Cyc_Absyn_AggrInfoU _tmp404;struct Cyc_List_List*_tmp405;_LL1B5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp401;if(_tmp402->tag != 11)goto _LL1B7;else{_tmp403=_tmp402->f1;_tmp404=_tmp403.aggr_info;_tmp405=_tmp403.targs;}}_LL1B6:
# 1669
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp404);
# 1670
aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
# 1671
forall_types=_tmp405;
# 1672
goto _LL1B4;_LL1B7:;_LL1B8: {
# 1673
const char*_tmpB9D;void*_tmpB9C;(_tmpB9C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB9D="init_struct: bad struct type",_tag_dyneither(_tmpB9D,sizeof(char),29))),_tag_dyneither(_tmpB9C,sizeof(void*),0)));}_LL1B4:;}{
# 1675
void*orig_typ=Cyc_Toc_typ_to_c(struct_type);
# 1676
void*aggr_typ=orig_typ;
# 1677
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
# 1681
struct Cyc_List_List*_tmp408=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
# 1682
for(0;_tmp408->tl != 0;_tmp408=_tmp408->tl){;}{
# 1683
struct Cyc_Absyn_Aggrfield*_tmp409=(struct Cyc_Absyn_Aggrfield*)_tmp408->hd;
# 1684
struct _RegionHandle _tmp40A=_new_region("temp");struct _RegionHandle*temp=& _tmp40A;_push_region(temp);
# 1685
{void*cast_type;
# 1686
if(pointer)
# 1687
cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);else{
# 1689
cast_type=orig_typ;}
# 1690
_tmp3FE=Cyc_Absyn_exp_stmt(Cyc_Toc_cast_it(cast_type,Cyc_Absyn_copy_exp(_tmp3FD)),0);{
# 1691
struct Cyc_List_List*_tmp40B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1692
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp40B,_tmp409->type))){
# 1694
struct Cyc_List_List*_tmp40C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
# 1695
struct Cyc_List_List*_tmp40D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp40B,_tmp40C);
# 1697
struct Cyc_List_List*new_fields=0;
# 1698
for(_tmp408=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp408 != 0;_tmp408=_tmp408->tl){
# 1699
struct Cyc_Absyn_Aggrfield*_tmp40E=(struct Cyc_Absyn_Aggrfield*)_tmp408->hd;
# 1700
struct Cyc_Absyn_Aggrfield*_tmpB9E;struct Cyc_Absyn_Aggrfield*_tmp40F=(_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->name=_tmp40E->name,((_tmpB9E->tq=Cyc_Toc_mt_tq,((_tmpB9E->type=
# 1702
Cyc_Tcutil_rsubstitute(temp,_tmp40D,_tmp40E->type),((_tmpB9E->width=_tmp40E->width,((_tmpB9E->attributes=_tmp40E->attributes,_tmpB9E)))))))))));
# 1706
struct Cyc_List_List*_tmpB9F;new_fields=((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->hd=_tmp40F,((_tmpB9F->tl=new_fields,_tmpB9F))))));}
# 1708
exist_types=0;
# 1709
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
# 1710
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpBA2;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpBA1;struct_type=(void*)((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1[0]=((_tmpBA2.tag=12,((_tmpBA2.f1=ad->kind,((_tmpBA2.f2=aggrfields,_tmpBA2)))))),_tmpBA1))));}
# 1711
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}};}
# 1685
;_pop_region(temp);};}{
# 1715
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
# 1716
struct _RegionHandle _tmp414=_new_region("r");struct _RegionHandle*r=& _tmp414;_push_region(r);
# 1717
{struct Cyc_List_List*_tmp415=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp415 != 0;_tmp415=_tmp415->tl){
# 1718
struct _tuple16 _tmp417;struct Cyc_List_List*_tmp418;struct Cyc_Absyn_Exp*_tmp419;struct _tuple16*_tmp416=(struct _tuple16*)_tmp415->hd;_tmp417=*_tmp416;_tmp418=_tmp417.f1;_tmp419=_tmp417.f2;
# 1719
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp419->topt));
# 1720
if(_tmp418 == 0){
# 1721
const char*_tmpBA5;void*_tmpBA4;(_tmpBA4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA5="empty designator list",_tag_dyneither(_tmpBA5,sizeof(char),22))),_tag_dyneither(_tmpBA4,sizeof(void*),0)));}
# 1722
if(_tmp418->tl != 0){
# 1725
struct _tuple0*_tmp41C=Cyc_Toc_temp_var();
# 1726
struct Cyc_Absyn_Exp*_tmp41D=Cyc_Absyn_var_exp(_tmp41C,0);
# 1727
for(0;_tmp418 != 0;_tmp418=_tmp418->tl){
# 1728
void*_tmp41E=(void*)_tmp418->hd;struct _dyneither_ptr*_tmp420;_LL1BA: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp41F=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp41E;if(_tmp41F->tag != 1)goto _LL1BC;else{_tmp420=_tmp41F->f1;}}_LL1BB:
# 1731
 if(Cyc_Toc_is_poly_field(struct_type,_tmp420))
# 1732
_tmp41D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp41D);
# 1733
_tmp3FE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3FF(_tmp3FD,_tmp420,0),_tmp41D,0),0),_tmp3FE,0);
# 1735
goto _LL1B9;_LL1BC:;_LL1BD: {
# 1736
const char*_tmpBA8;void*_tmpBA7;(_tmpBA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA8="array designator in struct",_tag_dyneither(_tmpBA8,sizeof(char),27))),_tag_dyneither(_tmpBA7,sizeof(void*),0)));}_LL1B9:;}
# 1739
Cyc_Toc_exp_to_c(nv,_tmp419);
# 1740
_tmp3FE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp41D,_tmp419,0),0),_tmp3FE,0);}else{
# 1742
void*_tmp423=(void*)_tmp418->hd;struct _dyneither_ptr*_tmp425;_LL1BF: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp424=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp423;if(_tmp424->tag != 1)goto _LL1C1;else{_tmp425=_tmp424->f1;}}_LL1C0: {
# 1744
struct Cyc_Absyn_Exp*lval=_tmp3FF(_tmp3FD,_tmp425,0);
# 1745
if(is_tagged_union){
# 1746
int i=Cyc_Toc_get_member_offset(ad,_tmp425);
# 1747
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
# 1748
struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);
# 1749
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
# 1750
_tmp3FE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3FE,0);
# 1751
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}
# 1753
{void*_tmp426=_tmp419->r;struct Cyc_List_List*_tmp428;struct Cyc_Absyn_Vardecl*_tmp42A;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Exp*_tmp42C;int _tmp42D;void*_tmp42F;struct Cyc_List_List*_tmp430;_LL1C4: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp427=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp426;if(_tmp427->tag != 25)goto _LL1C6;else{_tmp428=_tmp427->f1;}}_LL1C5:
# 1755
 _tmp3FE=Cyc_Toc_init_array(nv,lval,_tmp428,_tmp3FE);goto _LL1C3;_LL1C6: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp429=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp426;if(_tmp429->tag != 26)goto _LL1C8;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;_tmp42C=_tmp429->f3;_tmp42D=_tmp429->f4;}}_LL1C7:
# 1757
 _tmp3FE=Cyc_Toc_init_comprehension(nv,lval,_tmp42A,_tmp42B,_tmp42C,_tmp42D,_tmp3FE,0);goto _LL1C3;_LL1C8: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp42E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp426;if(_tmp42E->tag != 28)goto _LL1CA;else{_tmp42F=(void*)_tmp42E->f1;_tmp430=_tmp42E->f2;}}_LL1C9:
# 1759
 _tmp3FE=Cyc_Toc_init_anon_struct(nv,lval,_tmp42F,_tmp430,_tmp3FE);goto _LL1C3;_LL1CA:;_LL1CB: {
# 1761
int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)_check_null(_tmp419->topt));
# 1762
Cyc_Toc_exp_to_c(nv,_tmp419);{
# 1763
struct Cyc_Absyn_Aggrfield*_tmp431=Cyc_Absyn_lookup_field(aggrfields,_tmp425);
# 1765
if(Cyc_Toc_is_poly_field(struct_type,_tmp425) && !was_ptr_type)
# 1766
_tmp419=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp419);
# 1768
if(exist_types != 0)
# 1769
_tmp419=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp431))->type),_tmp419);
# 1771
_tmp3FE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp419,0),0),_tmp3FE,0);
# 1772
goto _LL1C3;};}_LL1C3:;}
# 1774
goto _LL1BE;}_LL1C1:;_LL1C2: {
# 1775
const char*_tmpBAB;void*_tmpBAA;(_tmpBAA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAB="array designator in struct",_tag_dyneither(_tmpBAB,sizeof(char),27))),_tag_dyneither(_tmpBAA,sizeof(void*),0)));}_LL1BE:;}}}{
# 1778
struct Cyc_Absyn_Exp*_tmp434=Cyc_Toc_make_struct(nv,_tmp3FC,aggr_typ,_tmp3FE,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp434;};
# 1717
;_pop_region(r);};};}struct _tuple20{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1783
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple20*pr){
# 1784
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 1786
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
# 1787
return Cyc_Absyn_address_exp(e1,0);}
# 1789
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
# 1790
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBAE;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBAD;return Cyc_Absyn_new_exp((void*)((_tmpBAD=_cycalloc(sizeof(*_tmpBAD)),((_tmpBAD[0]=((_tmpBAE.tag=4,((_tmpBAE.f1=e1,((_tmpBAE.f2=incr,_tmpBAE)))))),_tmpBAD)))),0);}
# 1793
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1801
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1803
void*_tmp437=e1->r;struct Cyc_Absyn_Stmt*_tmp439;void*_tmp43B;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43E;struct _dyneither_ptr*_tmp43F;int _tmp440;int _tmp441;_LL1CD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp438=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp437;if(_tmp438->tag != 35)goto _LL1CF;else{_tmp439=_tmp438->f1;}}_LL1CE:
# 1804
 Cyc_Toc_lvalue_assign_stmt(_tmp439,fs,f,f_env);goto _LL1CC;_LL1CF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp43A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp437;if(_tmp43A->tag != 13)goto _LL1D1;else{_tmp43B=(void*)_tmp43A->f1;_tmp43C=_tmp43A->f2;}}_LL1D0:
# 1805
 Cyc_Toc_lvalue_assign(_tmp43C,fs,f,f_env);goto _LL1CC;_LL1D1: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp43D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp437;if(_tmp43D->tag != 20)goto _LL1D3;else{_tmp43E=_tmp43D->f1;_tmp43F=_tmp43D->f2;_tmp440=_tmp43D->f3;_tmp441=_tmp43D->f4;}}_LL1D2:
# 1808
 e1->r=_tmp43E->r;
# 1809
{struct Cyc_List_List*_tmpBAF;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF->hd=_tmp43F,((_tmpBAF->tl=fs,_tmpBAF)))))),f,f_env);}
# 1810
goto _LL1CC;_LL1D3:;_LL1D4: {
# 1816
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1818
for(0;fs != 0;fs=fs->tl){
# 1819
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
# 1820
e1->r=(f(e1_copy,f_env))->r;
# 1821
goto _LL1CC;}_LL1CC:;}
# 1824
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1826
void*_tmp443=s->r;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Decl*_tmp447;struct Cyc_Absyn_Stmt*_tmp448;struct Cyc_Absyn_Stmt*_tmp44A;_LL1D6: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp444=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp443;if(_tmp444->tag != 1)goto _LL1D8;else{_tmp445=_tmp444->f1;}}_LL1D7:
# 1827
 Cyc_Toc_lvalue_assign(_tmp445,fs,f,f_env);goto _LL1D5;_LL1D8: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp446=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp443;if(_tmp446->tag != 12)goto _LL1DA;else{_tmp447=_tmp446->f1;_tmp448=_tmp446->f2;}}_LL1D9:
# 1829
 Cyc_Toc_lvalue_assign_stmt(_tmp448,fs,f,f_env);goto _LL1D5;_LL1DA: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp449=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp443;if(_tmp449->tag != 2)goto _LL1DC;else{_tmp44A=_tmp449->f2;}}_LL1DB:
# 1830
 Cyc_Toc_lvalue_assign_stmt(_tmp44A,fs,f,f_env);goto _LL1D5;_LL1DC:;_LL1DD: {
# 1831
const char*_tmpBB3;void*_tmpBB2[1];struct Cyc_String_pa_PrintArg_struct _tmpBB1;(_tmpBB1.tag=0,((_tmpBB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBB2[0]=& _tmpBB1,Cyc_Toc_toc_impos(((_tmpBB3="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBB3,sizeof(char),23))),_tag_dyneither(_tmpBB2,sizeof(void*),1)))))));}_LL1D5:;}
# 1835
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1839
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
# 1840
void*_tmp44E=e->r;void*_tmp450;void**_tmp451;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Exp**_tmp453;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Stmt*_tmp457;_LL1DF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp44F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp44E;if(_tmp44F->tag != 13)goto _LL1E1;else{_tmp450=(void**)& _tmp44F->f1;_tmp451=(void**)((void**)& _tmp44F->f1);_tmp452=_tmp44F->f2;_tmp453=(struct Cyc_Absyn_Exp**)& _tmp44F->f2;}}_LL1E0:
# 1842
*_tmp453=Cyc_Toc_push_address_exp(*_tmp453);
# 1843
*_tmp451=Cyc_Absyn_cstar_typ(*_tmp451,Cyc_Toc_mt_tq);
# 1844
return e;_LL1E1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp454=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp44E;if(_tmp454->tag != 19)goto _LL1E3;else{_tmp455=_tmp454->f1;}}_LL1E2:
# 1846
 return _tmp455;_LL1E3: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp456=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp44E;if(_tmp456->tag != 35)goto _LL1E5;else{_tmp457=_tmp456->f1;}}_LL1E4:
# 1850
 Cyc_Toc_push_address_stmt(_tmp457);
# 1851
return e;_LL1E5:;_LL1E6:
# 1853
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
# 1854
const char*_tmpBB7;void*_tmpBB6[1];struct Cyc_String_pa_PrintArg_struct _tmpBB5;(_tmpBB5.tag=0,((_tmpBB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBB6[0]=& _tmpBB5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB7="can't take & of exp %s",_tag_dyneither(_tmpBB7,sizeof(char),23))),_tag_dyneither(_tmpBB6,sizeof(void*),1)))))));};_LL1DE:;}
# 1858
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
# 1859
void*_tmp45B=s->r;struct Cyc_Absyn_Stmt*_tmp45D;struct Cyc_Absyn_Stmt*_tmp45F;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_Absyn_Exp**_tmp462;_LL1E8: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp45C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45B;if(_tmp45C->tag != 2)goto _LL1EA;else{_tmp45D=_tmp45C->f2;}}_LL1E9:
# 1860
 _tmp45F=_tmp45D;goto _LL1EB;_LL1EA: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp45E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45B;if(_tmp45E->tag != 12)goto _LL1EC;else{_tmp45F=_tmp45E->f2;}}_LL1EB:
# 1861
 Cyc_Toc_push_address_stmt(_tmp45F);goto _LL1E7;_LL1EC: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp460=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp45B;if(_tmp460->tag != 1)goto _LL1EE;else{_tmp461=_tmp460->f1;_tmp462=(struct Cyc_Absyn_Exp**)& _tmp460->f1;}}_LL1ED:
# 1862
*_tmp462=Cyc_Toc_push_address_exp(*_tmp462);goto _LL1E7;_LL1EE:;_LL1EF: {
# 1864
const char*_tmpBBB;void*_tmpBBA[1];struct Cyc_String_pa_PrintArg_struct _tmpBB9;(_tmpBB9.tag=0,((_tmpBB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBBA[0]=& _tmpBB9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBB="can't take & of stmt %s",_tag_dyneither(_tmpBBB,sizeof(char),24))),_tag_dyneither(_tmpBBA,sizeof(void*),1)))))));}_LL1E7:;}
# 1869
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1871
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 1873
if(x == 0)return 0;
# 1874
{struct Cyc_List_List*_tmpBBC;result=((_tmpBBC=_region_malloc(r2,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)f((void*)x->hd,env),((_tmpBBC->tl=0,_tmpBBC))))));}
# 1875
prev=result;
# 1876
for(x=x->tl;x != 0;x=x->tl){
# 1877
{struct Cyc_List_List*_tmpBBD;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBBD=_region_malloc(r2,sizeof(*_tmpBBD)),((_tmpBBD->hd=(void*)f((void*)x->hd,env),((_tmpBBD->tl=0,_tmpBBD))))));}
# 1878
prev=prev->tl;}
# 1880
return result;}
# 1882
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 1883
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 1886
static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
# 1887
struct _tuple16*_tmpBBE;return(_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE->f1=0,((_tmpBBE->f2=e,_tmpBBE)))));}
# 1890
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
# 1891
void*_tmp469=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp46B;_LL1F1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469;if(_tmp46A->tag != 5)goto _LL1F3;else{_tmp46B=_tmp46A->f1;}}_LL1F2:
# 1892
 return _tmp46B;_LL1F3:;_LL1F4: {
# 1893
const char*_tmpBC1;void*_tmpBC0;(_tmpBC0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC1="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBC1,sizeof(char),28))),_tag_dyneither(_tmpBC0,sizeof(void*),0)));}_LL1F0:;}
# 1900
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
# 1901
struct Cyc_Absyn_Exp*res;
# 1902
{void*_tmp46E=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp471;enum Cyc_Absyn_Size_of _tmp472;enum Cyc_Absyn_Sign _tmp474;enum Cyc_Absyn_Size_of _tmp475;enum Cyc_Absyn_Sign _tmp479;enum Cyc_Absyn_Size_of _tmp47A;enum Cyc_Absyn_Sign _tmp47C;enum Cyc_Absyn_Size_of _tmp47D;enum Cyc_Absyn_Sign _tmp47F;enum Cyc_Absyn_Size_of _tmp480;int _tmp482;int _tmp484;int _tmp486;_LL1F6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46E;if(_tmp46F->tag != 5)goto _LL1F8;}_LL1F7:
# 1903
 res=Cyc_Absyn_null_exp(0);goto _LL1F5;_LL1F8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp470=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46E;if(_tmp470->tag != 6)goto _LL1FA;else{_tmp471=_tmp470->f1;_tmp472=_tmp470->f2;if(_tmp472 != Cyc_Absyn_Char_sz)goto _LL1FA;}}_LL1F9:
# 1904
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp471,'\000'),0);goto _LL1F5;_LL1FA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp473=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46E;if(_tmp473->tag != 6)goto _LL1FC;else{_tmp474=_tmp473->f1;_tmp475=_tmp473->f2;if(_tmp475 != Cyc_Absyn_Short_sz)goto _LL1FC;}}_LL1FB:
# 1905
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp474,0),0);goto _LL1F5;_LL1FC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp476=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp46E;if(_tmp476->tag != 13)goto _LL1FE;}_LL1FD:
# 1906
 goto _LL1FF;_LL1FE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp46E;if(_tmp477->tag != 14)goto _LL200;}_LL1FF:
# 1907
 _tmp479=Cyc_Absyn_Unsigned;goto _LL201;_LL200: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp478=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46E;if(_tmp478->tag != 6)goto _LL202;else{_tmp479=_tmp478->f1;_tmp47A=_tmp478->f2;if(_tmp47A != Cyc_Absyn_Int_sz)goto _LL202;}}_LL201:
# 1908
 _tmp47C=_tmp479;goto _LL203;_LL202: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46E;if(_tmp47B->tag != 6)goto _LL204;else{_tmp47C=_tmp47B->f1;_tmp47D=_tmp47B->f2;if(_tmp47D != Cyc_Absyn_Long_sz)goto _LL204;}}_LL203:
# 1910
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp47C,0),0);goto _LL1F5;_LL204: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46E;if(_tmp47E->tag != 6)goto _LL206;else{_tmp47F=_tmp47E->f1;_tmp480=_tmp47E->f2;if(_tmp480 != Cyc_Absyn_LongLong_sz)goto _LL206;}}_LL205:
# 1912
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp47F,(long long)0),0);goto _LL1F5;_LL206: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46E;if(_tmp481->tag != 7)goto _LL208;else{_tmp482=_tmp481->f1;if(_tmp482 != 0)goto _LL208;}}_LL207:
# 1913
{const char*_tmpBC2;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC2="0.0F",_tag_dyneither(_tmpBC2,sizeof(char),5))),0),0);}goto _LL1F5;_LL208: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46E;if(_tmp483->tag != 7)goto _LL20A;else{_tmp484=_tmp483->f1;if(_tmp484 != 1)goto _LL20A;}}_LL209:
# 1914
{const char*_tmpBC3;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC3="0.0",_tag_dyneither(_tmpBC3,sizeof(char),4))),1),0);}goto _LL1F5;_LL20A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp46E;if(_tmp485->tag != 7)goto _LL20C;else{_tmp486=_tmp485->f1;}}_LL20B:
# 1915
{const char*_tmpBC4;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBC4="0.0L",_tag_dyneither(_tmpBC4,sizeof(char),5))),_tmp486),0);}goto _LL1F5;_LL20C:;_LL20D: {
# 1917
const char*_tmpBC8;void*_tmpBC7[1];struct Cyc_String_pa_PrintArg_struct _tmpBC6;(_tmpBC6.tag=0,((_tmpBC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBC7[0]=& _tmpBC6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC8="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBC8,sizeof(char),40))),_tag_dyneither(_tmpBC7,sizeof(void*),1)))))));}_LL1F5:;}
# 1919
res->topt=(void*)t;
# 1920
return res;}
# 1926
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1939 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
# 1940
void*_tmp48D=Cyc_Toc_typ_to_c(elt_type);
# 1941
void*_tmp48E=Cyc_Toc_typ_to_c(fat_ptr_type);
# 1942
void*_tmp48F=Cyc_Absyn_cstar_typ(_tmp48D,Cyc_Toc_mt_tq);
# 1943
struct Cyc_Core_Opt*_tmpBC9;struct Cyc_Core_Opt*_tmp490=(_tmpBC9=_cycalloc(sizeof(*_tmpBC9)),((_tmpBC9->v=_tmp48F,_tmpBC9)));
# 1944
struct Cyc_Absyn_Exp*xinit;
# 1945
{void*_tmp491=e1->r;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_Absyn_Exp*_tmp496;_LL20F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp492=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp491;if(_tmp492->tag != 19)goto _LL211;else{_tmp493=_tmp492->f1;}}_LL210:
# 1947
 if(!is_dyneither){
# 1948
_tmp493=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp493,0,Cyc_Absyn_Other_coercion,0);
# 1949
_tmp493->topt=(void*)fat_ptr_type;}
# 1951
Cyc_Toc_exp_to_c(nv,_tmp493);xinit=_tmp493;goto _LL20E;_LL211: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp494=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp491;if(_tmp494->tag != 22)goto _LL213;else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;}}_LL212:
# 1953
 if(!is_dyneither){
# 1954
_tmp495=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp495,0,Cyc_Absyn_Other_coercion,0);
# 1955
_tmp495->topt=(void*)fat_ptr_type;}
# 1957
Cyc_Toc_exp_to_c(nv,_tmp495);Cyc_Toc_exp_to_c(nv,_tmp496);
# 1958
{struct Cyc_Absyn_Exp*_tmpBCA[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
# 1959
(_tmpBCA[2]=_tmp496,((_tmpBCA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBCA[0]=_tmp495,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBCA,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1960
goto _LL20E;_LL213:;_LL214: {
# 1961
const char*_tmpBCD;void*_tmpBCC;(_tmpBCC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCD="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpBCD,sizeof(char),53))),_tag_dyneither(_tmpBCC,sizeof(void*),0)));}_LL20E:;}{
# 1963
struct _tuple0*_tmp49A=Cyc_Toc_temp_var();
# 1964
struct _RegionHandle _tmp49B=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp49B;_push_region(rgn2);
# 1965
{struct Cyc_Toc_Env*_tmp49C=Cyc_Toc_add_varmap(rgn2,nv,_tmp49A,Cyc_Absyn_var_exp(_tmp49A,0));
# 1966
struct Cyc_Absyn_Vardecl*_tmpBCE;struct Cyc_Absyn_Vardecl*_tmp49D=(_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->sc=Cyc_Absyn_Public,((_tmpBCE->name=_tmp49A,((_tmpBCE->tq=Cyc_Toc_mt_tq,((_tmpBCE->type=_tmp48E,((_tmpBCE->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpBCE->rgn=0,((_tmpBCE->attributes=0,((_tmpBCE->escapes=0,_tmpBCE)))))))))))))))));
# 1967
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD1;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBD0;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp49E=(_tmpBD0=_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD1.tag=4,((_tmpBD1.f1=_tmp49D,_tmpBD1)))),_tmpBD0)));
# 1968
struct Cyc_Absyn_Exp*_tmp49F=Cyc_Absyn_varb_exp(_tmp49A,(void*)_tmp49E,0);
# 1969
_tmp49F->topt=(void*)fat_ptr_type;{
# 1970
struct Cyc_Absyn_Exp*_tmp4A0=Cyc_Absyn_deref_exp(_tmp49F,0);
# 1971
_tmp4A0->topt=(void*)elt_type;
# 1972
Cyc_Toc_exp_to_c(_tmp49C,_tmp4A0);{
# 1973
struct _tuple0*_tmp4A1=Cyc_Toc_temp_var();
# 1974
_tmp49C=Cyc_Toc_add_varmap(rgn2,_tmp49C,_tmp4A1,Cyc_Absyn_var_exp(_tmp4A1,0));{
# 1975
struct Cyc_Absyn_Vardecl*_tmpBD2;struct Cyc_Absyn_Vardecl*_tmp4A2=(_tmpBD2=_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2->sc=Cyc_Absyn_Public,((_tmpBD2->name=_tmp4A1,((_tmpBD2->tq=Cyc_Toc_mt_tq,((_tmpBD2->type=_tmp48D,((_tmpBD2->initializer=(struct Cyc_Absyn_Exp*)_tmp4A0,((_tmpBD2->rgn=0,((_tmpBD2->attributes=0,((_tmpBD2->escapes=0,_tmpBD2)))))))))))))))));
# 1976
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD5;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBD4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4A3=(_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4[0]=((_tmpBD5.tag=4,((_tmpBD5.f1=_tmp4A2,_tmpBD5)))),_tmpBD4)));
# 1977
struct Cyc_Absyn_Exp*z_init=e2;
# 1978
if(popt != 0){
# 1979
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_Absyn_varb_exp(_tmp4A1,(void*)_tmp4A3,0);
# 1980
_tmp4A4->topt=_tmp4A0->topt;
# 1981
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp4A4,e2,0);
# 1982
z_init->topt=_tmp4A4->topt;}
# 1984
Cyc_Toc_exp_to_c(_tmp49C,z_init);{
# 1985
struct _tuple0*_tmp4A5=Cyc_Toc_temp_var();
# 1986
struct Cyc_Absyn_Vardecl*_tmpBD6;struct Cyc_Absyn_Vardecl*_tmp4A6=(_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6->sc=Cyc_Absyn_Public,((_tmpBD6->name=_tmp4A5,((_tmpBD6->tq=Cyc_Toc_mt_tq,((_tmpBD6->type=_tmp48D,((_tmpBD6->initializer=(struct Cyc_Absyn_Exp*)z_init,((_tmpBD6->rgn=0,((_tmpBD6->attributes=0,((_tmpBD6->escapes=0,_tmpBD6)))))))))))))))));
# 1987
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBD9;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBD8;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4A7=(_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8[0]=((_tmpBD9.tag=4,((_tmpBD9.f1=_tmp4A6,_tmpBD9)))),_tmpBD8)));
# 1988
_tmp49C=Cyc_Toc_add_varmap(rgn2,_tmp49C,_tmp4A5,Cyc_Absyn_var_exp(_tmp4A5,0));{
# 1991
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Absyn_varb_exp(_tmp4A1,(void*)_tmp4A3,0);_tmp4A8->topt=_tmp4A0->topt;{
# 1992
struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Toc_generate_zero(elt_type);
# 1993
struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp4A8,_tmp4A9,0);
# 1994
_tmp4AA->topt=(void*)Cyc_Absyn_sint_typ;
# 1995
Cyc_Toc_exp_to_c(_tmp49C,_tmp4AA);{
# 1997
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_varb_exp(_tmp4A5,(void*)_tmp4A7,0);_tmp4AB->topt=_tmp4A0->topt;{
# 1998
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Toc_generate_zero(elt_type);
# 1999
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp4AB,_tmp4AC,0);
# 2000
_tmp4AD->topt=(void*)Cyc_Absyn_sint_typ;
# 2001
Cyc_Toc_exp_to_c(_tmp49C,_tmp4AD);{
# 2003
struct Cyc_Absyn_Exp*_tmpBDA[2];struct Cyc_List_List*_tmp4AE=(_tmpBDA[1]=
# 2004
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBDA[0]=
# 2003
Cyc_Absyn_varb_exp(_tmp49A,(void*)_tmp49E,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDA,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2005
struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_uint_exp(1,0);
# 2006
struct Cyc_Absyn_Exp*xsize;
# 2007
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp4AE,0),_tmp4AF,0);{
# 2010
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp4AA,_tmp4AD,0),0);
# 2011
struct Cyc_Absyn_Stmt*_tmp4B1=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
# 2012
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_varb_exp(_tmp49A,(void*)_tmp49E,0),Cyc_Toc_curr_sp,0);
# 2013
_tmp4B2=Cyc_Toc_cast_it(_tmp48F,_tmp4B2);{
# 2014
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Absyn_deref_exp(_tmp4B2,0);
# 2015
struct Cyc_Absyn_Exp*_tmp4B4=Cyc_Absyn_assign_exp(_tmp4B3,Cyc_Absyn_var_exp(_tmp4A5,0),0);
# 2016
struct Cyc_Absyn_Stmt*_tmp4B5=Cyc_Absyn_exp_stmt(_tmp4B4,0);
# 2017
_tmp4B5=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp4B0,_tmp4B1,Cyc_Absyn_skip_stmt(0),0),_tmp4B5,0);
# 2018
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBE0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBDF;struct Cyc_Absyn_Decl*_tmpBDE;_tmp4B5=Cyc_Absyn_decl_stmt(((_tmpBDE=_cycalloc(sizeof(*_tmpBDE)),((_tmpBDE->r=(void*)((_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBDF.tag=0,((_tmpBDF.f1=_tmp4A6,_tmpBDF)))),_tmpBE0)))),((_tmpBDE->loc=0,_tmpBDE)))))),_tmp4B5,0);}
# 2019
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBE6;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBE5;struct Cyc_Absyn_Decl*_tmpBE4;_tmp4B5=Cyc_Absyn_decl_stmt(((_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4->r=(void*)((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE5.tag=0,((_tmpBE5.f1=_tmp4A2,_tmpBE5)))),_tmpBE6)))),((_tmpBE4->loc=0,_tmpBE4)))))),_tmp4B5,0);}
# 2020
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBEC;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBEB;struct Cyc_Absyn_Decl*_tmpBEA;_tmp4B5=Cyc_Absyn_decl_stmt(((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->r=(void*)((_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC[0]=((_tmpBEB.tag=0,((_tmpBEB.f1=_tmp49D,_tmpBEB)))),_tmpBEC)))),((_tmpBEA->loc=0,_tmpBEA)))))),_tmp4B5,0);}
# 2021
e->r=Cyc_Toc_stmt_exp_r(_tmp4B5);};};};};};};};};};};};}
# 1965
;_pop_region(rgn2);};}
# 2036 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2040
struct Cyc_Absyn_Aggrdecl*ad;
# 2041
{void*_tmp4CA=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp4CC;union Cyc_Absyn_AggrInfoU _tmp4CD;_LL216: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CA;if(_tmp4CB->tag != 11)goto _LL218;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CC.aggr_info;}}_LL217:
# 2043
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4CD);goto _LL215;_LL218:;_LL219: {
# 2044
struct Cyc_String_pa_PrintArg_struct _tmpBF4;void*_tmpBF3[1];const char*_tmpBF2;void*_tmpBF1;(_tmpBF1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpBF4.tag=0,((_tmpBF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpBF3[0]=& _tmpBF4,Cyc_aprintf(((_tmpBF2="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpBF2,sizeof(char),51))),_tag_dyneither(_tmpBF3,sizeof(void*),1)))))))),_tag_dyneither(_tmpBF1,sizeof(void*),0)));}_LL215:;}{
# 2047
struct _tuple0*_tmp4D2=Cyc_Toc_temp_var();
# 2048
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_Absyn_var_exp(_tmp4D2,0);
# 2049
struct Cyc_Absyn_Exp*_tmp4D4=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
# 2050
if(in_lhs){
# 2051
struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Absyn_aggrarrow_exp(_tmp4D3,Cyc_Toc_tag_sp,0);
# 2052
struct Cyc_Absyn_Exp*_tmp4D6=Cyc_Absyn_neq_exp(_tmp4D5,_tmp4D4,0);
# 2053
struct Cyc_Absyn_Exp*_tmp4D7=Cyc_Absyn_aggrarrow_exp(_tmp4D3,Cyc_Toc_val_sp,0);
# 2054
struct Cyc_Absyn_Stmt*_tmp4D8=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4D7,0),0);
# 2055
struct Cyc_Absyn_Stmt*_tmp4D9=Cyc_Absyn_ifthenelse_stmt(_tmp4D6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
# 2056
void*_tmp4DA=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
# 2057
struct Cyc_Absyn_Exp*_tmp4DB=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
# 2058
struct Cyc_Absyn_Stmt*_tmp4DC=Cyc_Absyn_declare_stmt(_tmp4D2,_tmp4DA,(struct Cyc_Absyn_Exp*)_tmp4DB,Cyc_Absyn_seq_stmt(_tmp4D9,_tmp4D8,0),0);
# 2059
return Cyc_Toc_stmt_exp_r(_tmp4DC);}else{
# 2061
struct Cyc_Absyn_Exp*_tmp4DD=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D3,f,0),Cyc_Toc_tag_sp,0);
# 2062
struct Cyc_Absyn_Exp*_tmp4DE=Cyc_Absyn_neq_exp(_tmp4DD,_tmp4D4,0);
# 2063
struct Cyc_Absyn_Exp*_tmp4DF=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp4D3,f,0),Cyc_Toc_val_sp,0);
# 2064
struct Cyc_Absyn_Stmt*_tmp4E0=Cyc_Absyn_exp_stmt(_tmp4DF,0);
# 2065
struct Cyc_Absyn_Stmt*_tmp4E1=Cyc_Absyn_ifthenelse_stmt(_tmp4DE,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
# 2066
struct Cyc_Absyn_Stmt*_tmp4E2=Cyc_Absyn_declare_stmt(_tmp4D2,e1_c_type,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp4E1,_tmp4E0,0),0);
# 2067
return Cyc_Toc_stmt_exp_r(_tmp4E2);}};}
# 2074
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2077
void*_tmp4E3=e->r;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E7;struct _dyneither_ptr*_tmp4E8;int _tmp4E9;int*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EC;struct _dyneither_ptr*_tmp4ED;int _tmp4EE;int*_tmp4EF;_LL21B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4E3;if(_tmp4E4->tag != 13)goto _LL21D;else{_tmp4E5=_tmp4E4->f2;}}_LL21C: {
# 2078
const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF7="cast on lhs!",_tag_dyneither(_tmpBF7,sizeof(char),13))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}_LL21D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4E6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4E3;if(_tmp4E6->tag != 20)goto _LL21F;else{_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;_tmp4E9=_tmp4E6->f4;_tmp4EA=(int*)& _tmp4E6->f4;}}_LL21E: {
# 2080
void*_tmp4F2=Cyc_Tcutil_compress((void*)_check_null(_tmp4E7->topt));struct Cyc_Absyn_AggrInfo _tmp4F4;union Cyc_Absyn_AggrInfoU _tmp4F5;_LL224: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F2;if(_tmp4F3->tag != 11)goto _LL226;else{_tmp4F4=_tmp4F3->f1;_tmp4F5=_tmp4F4.aggr_info;}}_LL225: {
# 2082
struct Cyc_Absyn_Aggrdecl*_tmp4F6=Cyc_Absyn_get_known_aggrdecl(_tmp4F5);
# 2083
*f_tag=Cyc_Toc_get_member_offset(_tmp4F6,_tmp4E8);{
# 2085
const char*_tmpBFC;void*_tmpBFB[2];struct Cyc_String_pa_PrintArg_struct _tmpBFA;struct Cyc_String_pa_PrintArg_struct _tmpBF9;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpBF9.tag=0,((_tmpBF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4E8),((_tmpBFA.tag=0,((_tmpBFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4F6->name).f2),((_tmpBFB[0]=& _tmpBFA,((_tmpBFB[1]=& _tmpBF9,Cyc_aprintf(((_tmpBFC="_union_%s_%s",_tag_dyneither(_tmpBFC,sizeof(char),13))),_tag_dyneither(_tmpBFB,sizeof(void*),2))))))))))))));
# 2086
{struct _dyneither_ptr*_tmpBFD;*tagged_member_type=Cyc_Absyn_strct(((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD[0]=str,_tmpBFD)))));}
# 2087
if(clear_read)*_tmp4EA=0;
# 2088
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4F6->impl))->tagged;};}_LL226:;_LL227:
# 2089
 return 0;_LL223:;}_LL21F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4EB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4E3;if(_tmp4EB->tag != 21)goto _LL221;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;_tmp4EE=_tmp4EB->f4;_tmp4EF=(int*)& _tmp4EB->f4;}}_LL220: {
# 2092
void*_tmp4FC=Cyc_Tcutil_compress((void*)_check_null(_tmp4EC->topt));struct Cyc_Absyn_PtrInfo _tmp4FE;void*_tmp4FF;_LL229: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FC;if(_tmp4FD->tag != 5)goto _LL22B;else{_tmp4FE=_tmp4FD->f1;_tmp4FF=_tmp4FE.elt_typ;}}_LL22A: {
# 2094
void*_tmp500=Cyc_Tcutil_compress(_tmp4FF);struct Cyc_Absyn_AggrInfo _tmp502;union Cyc_Absyn_AggrInfoU _tmp503;_LL22E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp500;if(_tmp501->tag != 11)goto _LL230;else{_tmp502=_tmp501->f1;_tmp503=_tmp502.aggr_info;}}_LL22F: {
# 2096
struct Cyc_Absyn_Aggrdecl*_tmp504=Cyc_Absyn_get_known_aggrdecl(_tmp503);
# 2097
*f_tag=Cyc_Toc_get_member_offset(_tmp504,_tmp4ED);{
# 2099
const char*_tmpC02;void*_tmpC01[2];struct Cyc_String_pa_PrintArg_struct _tmpC00;struct Cyc_String_pa_PrintArg_struct _tmpBFF;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpBFF.tag=0,((_tmpBFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4ED),((_tmpC00.tag=0,((_tmpC00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp504->name).f2),((_tmpC01[0]=& _tmpC00,((_tmpC01[1]=& _tmpBFF,Cyc_aprintf(((_tmpC02="_union_%s_%s",_tag_dyneither(_tmpC02,sizeof(char),13))),_tag_dyneither(_tmpC01,sizeof(void*),2))))))))))))));
# 2100
{struct _dyneither_ptr*_tmpC03;*tagged_member_type=Cyc_Absyn_strct(((_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=str,_tmpC03)))));}
# 2101
if(clear_read)*_tmp4EF=0;
# 2102
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp504->impl))->tagged;};}_LL230:;_LL231:
# 2103
 return 0;_LL22D:;}_LL22B:;_LL22C:
# 2105
 return 0;_LL228:;}_LL221:;_LL222:
# 2107
 return 0;_LL21A:;}
# 2119 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
# 2120
struct _tuple0*_tmp50A=Cyc_Toc_temp_var();
# 2121
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp50A,0);
# 2122
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
# 2123
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
# 2124
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
# 2125
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
# 2126
struct Cyc_Absyn_Exp*_tmp50B=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
# 2127
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp50B,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
# 2128
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp50A,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)
# 2129
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
# 2130
Cyc_Absyn_seq_stmt(s2,s3,0),0);
# 2131
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2143 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2147
struct _tuple0*_tmp50C=Cyc_Toc_temp_var();
# 2148
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp50C,0);
# 2149
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
# 2150
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
# 2151
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
# 2152
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
# 2153
struct Cyc_Absyn_Stmt*s2;
# 2154
if(popt == 0)
# 2155
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2157
struct Cyc_Absyn_Exp*_tmp50D=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
# 2158
s2=Cyc_Absyn_ifthenelse_stmt(_tmp50D,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2160
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp50C,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)
# 2161
Cyc_Toc_push_address_exp(e1),
# 2162
Cyc_Absyn_seq_stmt(s2,s3,0),0);
# 2163
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple21{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC52(unsigned int*_tmpC51,unsigned int*_tmpC50,struct _tuple0***_tmpC4E){for(*_tmpC51=0;*_tmpC51 < *_tmpC50;(*_tmpC51)++){(*_tmpC4E)[*_tmpC51]=
# 2529 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2166 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
# 2167
void*_tmp50E=e->r;
# 2168
if(e->topt == 0){
# 2169
const char*_tmpC07;void*_tmpC06[1];struct Cyc_String_pa_PrintArg_struct _tmpC05;(_tmpC05.tag=0,((_tmpC05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC06[0]=& _tmpC05,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC07="exp_to_c: no type for %s",_tag_dyneither(_tmpC07,sizeof(char),25))),_tag_dyneither(_tmpC06,sizeof(void*),1)))))));}{
# 2172
void*old_typ=(void*)_check_null(e->topt);
# 2173
void*_tmp512=_tmp50E;union Cyc_Absyn_Cnst _tmp514;int _tmp515;struct _tuple0*_tmp518;void*_tmp519;enum Cyc_Absyn_Primop _tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;enum Cyc_Absyn_Incrementor _tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Core_Opt*_tmp522;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_List_List*_tmp533;struct Cyc_Absyn_VarargCallInfo*_tmp534;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_VarargCallInfo*_tmp538;struct Cyc_Absyn_VarargCallInfo _tmp539;int _tmp53A;struct Cyc_List_List*_tmp53B;struct Cyc_Absyn_VarargInfo*_tmp53C;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp540;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_List_List*_tmp543;void*_tmp545;void**_tmp546;struct Cyc_Absyn_Exp*_tmp547;int _tmp548;enum Cyc_Absyn_Coercion _tmp549;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp550;void*_tmp552;void*_tmp554;void*_tmp555;struct _dyneither_ptr*_tmp557;void*_tmp559;void*_tmp55A;unsigned int _tmp55C;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp560;struct _dyneither_ptr*_tmp561;int _tmp562;int _tmp563;struct Cyc_Absyn_Exp*_tmp565;struct _dyneither_ptr*_tmp566;int _tmp567;int _tmp568;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_List_List*_tmp56D;struct Cyc_List_List*_tmp56F;struct Cyc_Absyn_Vardecl*_tmp571;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp573;int _tmp574;struct _tuple0*_tmp576;struct Cyc_List_List*_tmp577;struct Cyc_List_List*_tmp578;struct Cyc_Absyn_Aggrdecl*_tmp579;void*_tmp57B;struct Cyc_List_List*_tmp57C;struct Cyc_List_List*_tmp57E;struct Cyc_Absyn_Datatypedecl*_tmp57F;struct Cyc_Absyn_Datatypefield*_tmp580;struct Cyc_Absyn_MallocInfo _tmp584;int _tmp585;struct Cyc_Absyn_Exp*_tmp586;void**_tmp587;struct Cyc_Absyn_Exp*_tmp588;int _tmp589;struct Cyc_Absyn_Exp*_tmp58B;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Exp*_tmp58E;struct _dyneither_ptr*_tmp58F;struct Cyc_Absyn_Stmt*_tmp591;_LL233: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp513=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp513->tag != 0)goto _LL235;else{_tmp514=_tmp513->f1;if((_tmp514.Null_c).tag != 1)goto _LL235;_tmp515=(int)(_tmp514.Null_c).val;}}_LL234: {
# 2179
struct Cyc_Absyn_Exp*_tmp596=Cyc_Absyn_uint_exp(0,0);
# 2180
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
# 2181
if(Cyc_Toc_is_toplevel(nv))
# 2182
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp596,_tmp596))->r;else{
# 2184
struct Cyc_Absyn_Exp*_tmpC08[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC08[2]=_tmp596,((_tmpC08[1]=_tmp596,((_tmpC08[0]=_tmp596,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC08,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2186
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2188
goto _LL232;}_LL235: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp516=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp516->tag != 0)goto _LL237;}_LL236:
# 2189
 goto _LL232;_LL237: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp517->tag != 1)goto _LL239;else{_tmp518=_tmp517->f1;_tmp519=(void*)_tmp517->f2;}}_LL238:
# 2191
{struct _handler_cons _tmp598;_push_handler(& _tmp598);{int _tmp59A=0;if(setjmp(_tmp598.handler))_tmp59A=1;if(!_tmp59A){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp518))->r;;_pop_handler();}else{void*_tmp599=(void*)_exn_thrown;void*_tmp59C=_tmp599;_LL288: {struct Cyc_Dict_Absent_exn_struct*_tmp59D=(struct Cyc_Dict_Absent_exn_struct*)_tmp59C;if(_tmp59D->tag != Cyc_Dict_Absent)goto _LL28A;}_LL289: {
# 2193
const char*_tmpC0C;void*_tmpC0B[1];struct Cyc_String_pa_PrintArg_struct _tmpC0A;(_tmpC0A.tag=0,((_tmpC0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp518)),((_tmpC0B[0]=& _tmpC0A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0C="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC0C,sizeof(char),32))),_tag_dyneither(_tmpC0B,sizeof(void*),1)))))));}_LL28A:;_LL28B:(void)_throw(_tmp59C);_LL287:;}};}
# 2195
goto _LL232;_LL239: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp51A->tag != 2)goto _LL23B;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL23A: {
# 2198
struct Cyc_List_List*_tmp5A1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp51C);
# 2200
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp51C);
# 2201
switch(_tmp51B){case Cyc_Absyn_Numelts: _LL28C: {
# 2203
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
# 2204
{void*_tmp5A2=Cyc_Tcutil_compress((void*)_check_null(arg->topt));struct Cyc_Absyn_ArrayInfo _tmp5A4;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_PtrInfo _tmp5A7;void*_tmp5A8;struct Cyc_Absyn_PtrAtts _tmp5A9;union Cyc_Absyn_Constraint*_tmp5AA;union Cyc_Absyn_Constraint*_tmp5AB;union Cyc_Absyn_Constraint*_tmp5AC;_LL28F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A2;if(_tmp5A3->tag != 8)goto _LL291;else{_tmp5A4=_tmp5A3->f1;_tmp5A5=_tmp5A4.num_elts;}}_LL290:
# 2207
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp5A5))){
# 2208
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,Cyc_Tcutil_terr(e->loc,((_tmpC0F="can't calculate numelts",_tag_dyneither(_tmpC0F,sizeof(char),24))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}
# 2209
e->r=_tmp5A5->r;goto _LL28E;_LL291: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2;if(_tmp5A6->tag != 5)goto _LL293;else{_tmp5A7=_tmp5A6->f1;_tmp5A8=_tmp5A7.elt_typ;_tmp5A9=_tmp5A7.ptr_atts;_tmp5AA=_tmp5A9.nullable;_tmp5AB=_tmp5A9.bounds;_tmp5AC=_tmp5A9.zero_term;}}_LL292:
# 2211
{void*_tmp5AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5AB);struct Cyc_Absyn_Exp*_tmp5B2;_LL296: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5B0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5AF;if(_tmp5B0->tag != 0)goto _LL298;}_LL297:
# 2213
{struct Cyc_Absyn_Exp*_tmpC10[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
# 2214
(_tmpC10[1]=
# 2215
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A8),0),((_tmpC10[0]=(struct Cyc_Absyn_Exp*)_tmp51C->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC10,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 2216
goto _LL295;_LL298: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5B1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AF;if(_tmp5B1->tag != 1)goto _LL295;else{_tmp5B2=_tmp5B1->f1;}}_LL299:
# 2219
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AC)){
# 2220
struct Cyc_Absyn_Exp*function_e=
# 2221
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp51C->hd);
# 2223
struct Cyc_Absyn_Exp*_tmpC11[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC11[1]=_tmp5B2,((_tmpC11[0]=(struct Cyc_Absyn_Exp*)_tmp51C->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC11,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2224
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AA)){
# 2225
if(!Cyc_Evexp_c_can_eval(_tmp5B2)){
# 2226
const char*_tmpC14;void*_tmpC13;(_tmpC13=0,Cyc_Tcutil_terr(e->loc,((_tmpC14="can't calculate numelts",_tag_dyneither(_tmpC14,sizeof(char),24))),_tag_dyneither(_tmpC13,sizeof(void*),0)));}
# 2228
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp5B2,Cyc_Absyn_uint_exp(0,0));}else{
# 2230
e->r=_tmp5B2->r;goto _LL295;}}
# 2232
goto _LL295;_LL295:;}
# 2234
goto _LL28E;_LL293:;_LL294: {
# 2236
const char*_tmpC19;void*_tmpC18[2];struct Cyc_String_pa_PrintArg_struct _tmpC17;struct Cyc_String_pa_PrintArg_struct _tmpC16;(_tmpC16.tag=0,((_tmpC16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2237
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC17.tag=0,((_tmpC17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC18[0]=& _tmpC17,((_tmpC18[1]=& _tmpC16,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC19="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC19,sizeof(char),41))),_tag_dyneither(_tmpC18,sizeof(void*),2)))))))))))));}_LL28E:;}
# 2239
break;}case Cyc_Absyn_Plus: _LL28D:
# 2244
{void*_tmp5BB=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp5A1))->hd);struct Cyc_Absyn_PtrInfo _tmp5BD;void*_tmp5BE;struct Cyc_Absyn_PtrAtts _tmp5BF;union Cyc_Absyn_Constraint*_tmp5C0;union Cyc_Absyn_Constraint*_tmp5C1;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5BC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BB;if(_tmp5BC->tag != 5)goto _LL29E;else{_tmp5BD=_tmp5BC->f1;_tmp5BE=_tmp5BD.elt_typ;_tmp5BF=_tmp5BD.ptr_atts;_tmp5C0=_tmp5BF.bounds;_tmp5C1=_tmp5BF.zero_term;}}_LL29D:
# 2246
{void*_tmp5C2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5C0);struct Cyc_Absyn_Exp*_tmp5C5;_LL2A1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5C3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5C2;if(_tmp5C3->tag != 0)goto _LL2A3;}_LL2A2: {
# 2248
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
# 2249
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
# 2250
{struct Cyc_Absyn_Exp*_tmpC1A[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
# 2251
(_tmpC1A[2]=e2,((_tmpC1A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5BE),0),((_tmpC1A[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
# 2252
goto _LL2A0;}_LL2A3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5C4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5C2;if(_tmp5C4->tag != 1)goto _LL2A0;else{_tmp5C5=_tmp5C4->f1;}}_LL2A4:
# 2254
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5C1)){
# 2255
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
# 2256
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
# 2257
struct Cyc_Absyn_Exp*_tmpC1B[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC1B[2]=e2,((_tmpC1B[1]=_tmp5C5,((_tmpC1B[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2259
goto _LL2A0;_LL2A0:;}
# 2261
goto _LL29B;_LL29E:;_LL29F:
# 2263
 goto _LL29B;_LL29B:;}
# 2265
break;case Cyc_Absyn_Minus: _LL29A: {
# 2270
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2271
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp5A1))->hd,& elt_typ)){
# 2272
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
# 2273
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
# 2274
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp5A1->tl))->hd)){
# 2275
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
# 2276
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
# 2277
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
# 2278
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2280
struct Cyc_Absyn_Exp*_tmpC1C[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
# 2281
(_tmpC1C[2]=
# 2282
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC1C[1]=
# 2281
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC1C[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2285
break;}case Cyc_Absyn_Eq: _LL2A5:
# 2286
 goto _LL2A6;case Cyc_Absyn_Neq: _LL2A6:
# 2287
 goto _LL2A7;case Cyc_Absyn_Gt: _LL2A7:
# 2288
 goto _LL2A8;case Cyc_Absyn_Gte: _LL2A8:
# 2289
 goto _LL2A9;case Cyc_Absyn_Lt: _LL2A9:
# 2290
 goto _LL2AA;case Cyc_Absyn_Lte: _LL2AA: {
# 2293
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;
# 2294
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51C->tl))->hd;
# 2295
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp5A1))->hd;
# 2296
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp5A1->tl))->hd;
# 2297
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2298
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
# 2299
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
# 2300
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
# 2301
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Absyn_aggrmember_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
# 2302
break;}default: _LL2AB:
# 2303
 break;}
# 2305
goto _LL232;}_LL23B: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp51D->tag != 4)goto _LL23D;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL23C: {
# 2307
void*e2_cyc_typ=(void*)_check_null(_tmp51E->topt);
# 2316 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
# 2317
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
# 2318
int is_dyneither=0;
# 2319
const char*_tmpC1D;struct _dyneither_ptr incr_str=(_tmpC1D="increment",_tag_dyneither(_tmpC1D,sizeof(char),10));
# 2320
if(_tmp51F == Cyc_Absyn_PreDec  || _tmp51F == Cyc_Absyn_PostDec){const char*_tmpC1E;incr_str=((_tmpC1E="decrement",_tag_dyneither(_tmpC1E,sizeof(char),10)));}
# 2321
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp51E,& ptr_type,& is_dyneither,& elt_type)){
# 2322
{const char*_tmpC22;void*_tmpC21[1];struct Cyc_String_pa_PrintArg_struct _tmpC20;(_tmpC20.tag=0,((_tmpC20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC21[0]=& _tmpC20,Cyc_Tcutil_terr(e->loc,((_tmpC22="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC22,sizeof(char),74))),_tag_dyneither(_tmpC21,sizeof(void*),1)))))));}{
# 2323
const char*_tmpC25;void*_tmpC24;(_tmpC24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC25="in-place inc/dec on zero-term",_tag_dyneither(_tmpC25,sizeof(char),30))),_tag_dyneither(_tmpC24,sizeof(void*),0)));};}{
# 2325
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
# 2326
int f_tag=0;
# 2327
if(Cyc_Toc_is_tagged_union_project(_tmp51E,& f_tag,& tunion_member_type,1)){
# 2328
struct Cyc_Absyn_Exp*_tmp5CF=Cyc_Absyn_signed_int_exp(1,0);
# 2329
_tmp5CF->topt=(void*)Cyc_Absyn_sint_typ;
# 2330
switch(_tmp51F){case Cyc_Absyn_PreInc: _LL2AD:
# 2332
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC2B;struct Cyc_Core_Opt*_tmpC2A;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC29;e->r=(void*)((_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2B.tag=3,((_tmpC2B.f1=_tmp51E,((_tmpC2B.f2=((_tmpC2A=_cycalloc_atomic(sizeof(*_tmpC2A)),((_tmpC2A->v=(void*)Cyc_Absyn_Plus,_tmpC2A)))),((_tmpC2B.f3=_tmp5CF,_tmpC2B)))))))),_tmpC29))));}
# 2333
Cyc_Toc_exp_to_c(nv,e);
# 2334
return;case Cyc_Absyn_PreDec: _LL2AE:
# 2336
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC31;struct Cyc_Core_Opt*_tmpC30;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC2F;e->r=(void*)((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F[0]=((_tmpC31.tag=3,((_tmpC31.f1=_tmp51E,((_tmpC31.f2=((_tmpC30=_cycalloc_atomic(sizeof(*_tmpC30)),((_tmpC30->v=(void*)Cyc_Absyn_Minus,_tmpC30)))),((_tmpC31.f3=_tmp5CF,_tmpC31)))))))),_tmpC2F))));}
# 2337
Cyc_Toc_exp_to_c(nv,e);
# 2338
return;default: _LL2AF:
# 2340
{const char*_tmpC35;void*_tmpC34[1];struct Cyc_String_pa_PrintArg_struct _tmpC33;(_tmpC33.tag=0,((_tmpC33.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC34[0]=& _tmpC33,Cyc_Tcutil_terr(e->loc,((_tmpC35="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC35,sizeof(char),59))),_tag_dyneither(_tmpC34,sizeof(void*),1)))))));}{
# 2342
const char*_tmpC38;void*_tmpC37;(_tmpC37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC38="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC38,sizeof(char),34))),_tag_dyneither(_tmpC37,sizeof(void*),0)));};}}
# 2345
Cyc_Toc_set_lhs(nv,1);
# 2346
Cyc_Toc_exp_to_c(nv,_tmp51E);
# 2347
Cyc_Toc_set_lhs(nv,0);{
# 2350
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2351
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
# 2352
struct Cyc_Absyn_Exp*fn_e;
# 2353
int change=1;
# 2354
fn_e=(_tmp51F == Cyc_Absyn_PostInc  || _tmp51F == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2356
if(_tmp51F == Cyc_Absyn_PreDec  || _tmp51F == Cyc_Absyn_PostDec)
# 2357
change=- 1;{
# 2358
struct Cyc_Absyn_Exp*_tmpC39[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC39[2]=
# 2360
Cyc_Absyn_signed_int_exp(change,0),((_tmpC39[1]=
# 2359
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC39[0]=
# 2358
Cyc_Toc_push_address_exp(_tmp51E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC39,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2361
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2365
struct Cyc_Toc_functionSet*_tmp5DC=_tmp51F != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2367
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5DC,_tmp51E);
# 2368
struct Cyc_Absyn_Exp*_tmpC3A[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC3A[1]=
# 2369
Cyc_Absyn_signed_int_exp(1,0),((_tmpC3A[0]=
# 2368
Cyc_Toc_push_address_exp(_tmp51E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2370
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp51E)){
# 2371
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp51E,0,Cyc_Toc_incr_lvalue,_tmp51F);
# 2372
e->r=_tmp51E->r;}}}
# 2374
goto _LL232;};};}_LL23D: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp520->tag != 3)goto _LL23F;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;_tmp523=_tmp520->f3;}}_LL23E: {
# 2393 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp521->topt);
# 2394
void*e2_old_typ=(void*)_check_null(_tmp523->topt);
# 2395
int f_tag=0;
# 2396
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
# 2397
if(Cyc_Toc_is_tagged_union_project(_tmp521,& f_tag,& tagged_member_struct_type,1)){
# 2398
Cyc_Toc_set_lhs(nv,1);
# 2399
Cyc_Toc_exp_to_c(nv,_tmp521);
# 2400
Cyc_Toc_set_lhs(nv,0);
# 2401
Cyc_Toc_exp_to_c(nv,_tmp523);
# 2402
e->r=Cyc_Toc_tagged_union_assignop(_tmp521,e1_old_typ,_tmp522,_tmp523,e2_old_typ,f_tag,tagged_member_struct_type);
# 2404
return;}{
# 2406
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
# 2407
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
# 2408
int is_dyneither=0;
# 2409
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp521,& ptr_type,& is_dyneither,& elt_type)){
# 2410
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp521,_tmp522,_tmp523,ptr_type,is_dyneither,elt_type);
# 2412
return;}{
# 2416
int e1_poly=Cyc_Toc_is_poly_project(_tmp521);
# 2417
Cyc_Toc_set_lhs(nv,1);
# 2418
Cyc_Toc_exp_to_c(nv,_tmp521);
# 2419
Cyc_Toc_set_lhs(nv,0);
# 2420
Cyc_Toc_exp_to_c(nv,_tmp523);{
# 2422
int done=0;
# 2423
if(_tmp522 != 0){
# 2424
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
# 2425
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
# 2426
struct Cyc_Absyn_Exp*change;
# 2427
switch((enum Cyc_Absyn_Primop)_tmp522->v){case Cyc_Absyn_Plus: _LL2B1:
# 2428
 change=_tmp523;break;case Cyc_Absyn_Minus: _LL2B2:
# 2430
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp523,0);break;default: _LL2B3: {
# 2431
const char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3D="bad t ? pointer arithmetic",_tag_dyneither(_tmpC3D,sizeof(char),27))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}}
# 2433
done=1;{
# 2435
struct Cyc_Absyn_Exp*_tmp5E1=Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2436
struct Cyc_Absyn_Exp*_tmpC3E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5E1,((_tmpC3E[2]=change,((_tmpC3E[1]=
# 2437
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC3E[0]=
# 2436
Cyc_Toc_push_address_exp(_tmp521),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2439
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))
# 2442
switch((enum Cyc_Absyn_Primop)_tmp522->v){case Cyc_Absyn_Plus: _LL2B5:
# 2444
 done=1;
# 2445
{struct Cyc_Absyn_Exp*_tmpC3F[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp521),((_tmpC3F[1]=_tmp523,((_tmpC3F[0]=_tmp521,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 2446
break;default: _LL2B6: {
# 2447
const char*_tmpC42;void*_tmpC41;(_tmpC41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC42="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC42,sizeof(char),39))),_tag_dyneither(_tmpC41,sizeof(void*),0)));}}}}
# 2451
if(!done){
# 2453
if(e1_poly)
# 2454
_tmp523->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp523->r,0));
# 2459
if(!Cyc_Absyn_is_lvalue(_tmp521)){
# 2460
{struct _tuple20 _tmpC45;struct _tuple20*_tmpC44;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple20*),struct _tuple20*f_env))Cyc_Toc_lvalue_assign)(_tmp521,0,Cyc_Toc_assignop_lvalue,((_tmpC44=_cycalloc(sizeof(struct _tuple20)* 1),((_tmpC44[0]=((_tmpC45.f1=_tmp522,((_tmpC45.f2=_tmp523,_tmpC45)))),_tmpC44)))));}
# 2461
e->r=_tmp521->r;}}
# 2464
goto _LL232;};};};}_LL23F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp524=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp524->tag != 5)goto _LL241;else{_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;}}_LL240:
# 2466
 Cyc_Toc_exp_to_c(nv,_tmp525);
# 2467
Cyc_Toc_exp_to_c(nv,_tmp526);
# 2468
Cyc_Toc_exp_to_c(nv,_tmp527);
# 2469
goto _LL232;_LL241: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp528->tag != 6)goto _LL243;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}}_LL242:
# 2471
 Cyc_Toc_exp_to_c(nv,_tmp529);
# 2472
Cyc_Toc_exp_to_c(nv,_tmp52A);
# 2473
goto _LL232;_LL243: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp52B->tag != 7)goto _LL245;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52B->f2;}}_LL244:
# 2475
 Cyc_Toc_exp_to_c(nv,_tmp52C);
# 2476
Cyc_Toc_exp_to_c(nv,_tmp52D);
# 2477
goto _LL232;_LL245: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp52E->tag != 8)goto _LL247;else{_tmp52F=_tmp52E->f1;_tmp530=_tmp52E->f2;}}_LL246:
# 2479
 Cyc_Toc_exp_to_c(nv,_tmp52F);
# 2480
Cyc_Toc_exp_to_c(nv,_tmp530);
# 2481
goto _LL232;_LL247: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp531=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp531->tag != 9)goto _LL249;else{_tmp532=_tmp531->f1;_tmp533=_tmp531->f2;_tmp534=_tmp531->f3;if(_tmp534 != 0)goto _LL249;}}_LL248:
# 2483
 Cyc_Toc_exp_to_c(nv,_tmp532);
# 2484
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp533);
# 2485
goto _LL232;_LL249: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp535->tag != 9)goto _LL24B;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;_tmp538=_tmp535->f3;if(_tmp538 == 0)goto _LL24B;_tmp539=*_tmp538;_tmp53A=_tmp539.num_varargs;_tmp53B=_tmp539.injectors;_tmp53C=_tmp539.vai;}}_LL24A: {
# 2495 "toc.cyc"
struct _RegionHandle _tmp5E8=_new_region("r");struct _RegionHandle*r=& _tmp5E8;_push_region(r);{
# 2496
struct _tuple0*argv=Cyc_Toc_temp_var();
# 2497
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
# 2498
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp53A,0);
# 2499
void*cva_type=Cyc_Toc_typ_to_c(_tmp53C->type);
# 2500
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2503
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp537);
# 2504
int num_normargs=num_args - _tmp53A;
# 2507
struct Cyc_List_List*new_args=0;
# 2508
{int i=0;for(0;i < num_normargs;(++ i,_tmp537=_tmp537->tl)){
# 2509
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp537))->hd);{
# 2510
struct Cyc_List_List*_tmpC46;new_args=((_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->hd=(struct Cyc_Absyn_Exp*)_tmp537->hd,((_tmpC46->tl=new_args,_tmpC46))))));};}}
# 2512
{struct Cyc_Absyn_Exp*_tmpC49[3];struct Cyc_List_List*_tmpC48;new_args=((_tmpC48=_cycalloc(sizeof(*_tmpC48)),((_tmpC48->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
# 2513
(_tmpC49[2]=num_varargs_exp,((_tmpC49[1]=
# 2514
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC49[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC49,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC48->tl=new_args,_tmpC48))))));}
# 2517
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2519
Cyc_Toc_exp_to_c(nv,_tmp536);{
# 2520
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp536,new_args,0),0);
# 2523
if(_tmp53C->inject){
# 2524
struct Cyc_Absyn_Datatypedecl*tud;
# 2525
{void*_tmp5EC=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp53C->type));struct Cyc_Absyn_DatatypeInfo _tmp5EE;union Cyc_Absyn_DatatypeInfoU _tmp5EF;struct Cyc_Absyn_Datatypedecl**_tmp5F0;struct Cyc_Absyn_Datatypedecl*_tmp5F1;_LL2B9: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5EC;if(_tmp5ED->tag != 3)goto _LL2BB;else{_tmp5EE=_tmp5ED->f1;_tmp5EF=_tmp5EE.datatype_info;if((_tmp5EF.KnownDatatype).tag != 2)goto _LL2BB;_tmp5F0=(struct Cyc_Absyn_Datatypedecl**)(_tmp5EF.KnownDatatype).val;_tmp5F1=*_tmp5F0;}}_LL2BA:
# 2526
 tud=_tmp5F1;goto _LL2B8;_LL2BB:;_LL2BC: {
# 2527
const char*_tmpC4C;void*_tmpC4B;(_tmpC4B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4C="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC4C,sizeof(char),44))),_tag_dyneither(_tmpC4B,sizeof(void*),0)));}_LL2B8:;}{
# 2529
unsigned int _tmpC51;unsigned int _tmpC50;struct _dyneither_ptr _tmpC4F;struct _tuple0**_tmpC4E;unsigned int _tmpC4D;struct _dyneither_ptr vs=(_tmpC4D=(unsigned int)_tmp53A,((_tmpC4E=(struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmpC4D)),((_tmpC4F=_tag_dyneither(_tmpC4E,sizeof(struct _tuple0*),_tmpC4D),((((_tmpC50=_tmpC4D,_tmpC52(& _tmpC51,& _tmpC50,& _tmpC4E))),_tmpC4F)))))));
# 2531
if(_tmp53A != 0){
# 2533
struct Cyc_List_List*_tmp5F4=0;
# 2534
{int i=_tmp53A - 1;for(0;i >= 0;-- i){
# 2535
struct Cyc_List_List*_tmpC53;_tmp5F4=((_tmpC53=_cycalloc(sizeof(*_tmpC53)),((_tmpC53->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i)),0),0)),((_tmpC53->tl=_tmp5F4,_tmpC53))))));}}
# 2537
s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp5F4,0),s,0);{
# 2540
int i=0;for(0;_tmp537 != 0;(((_tmp537=_tmp537->tl,_tmp53B=_tmp53B->tl)),++ i)){
# 2541
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp537->hd;
# 2542
void*arg_type=(void*)_check_null(arg->topt);
# 2543
struct _tuple0*var=*((struct _tuple0**)_check_dyneither_subscript(vs,sizeof(struct _tuple0*),i));
# 2544
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
# 2545
struct Cyc_Absyn_Datatypefield _tmp5F7;struct _tuple0*_tmp5F8;struct Cyc_List_List*_tmp5F9;struct Cyc_Absyn_Datatypefield*_tmp5F6=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp53B))->hd;_tmp5F7=*_tmp5F6;_tmp5F8=_tmp5F7.name;_tmp5F9=_tmp5F7.typs;{
# 2546
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp5F9))->hd)).f2);
# 2547
Cyc_Toc_exp_to_c(nv,arg);
# 2548
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
# 2549
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2555
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2558
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),
# 2559
Cyc_Toc_datatype_tag(tud,_tmp5F8),0),s,0);
# 2561
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5F8,tud->name)),0,s,0);};}};}else{
# 2568
struct _tuple16*_tmpC54[3];struct Cyc_List_List*_tmp5FA=(_tmpC54[2]=
# 2569
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC54[1]=
# 2568
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC54[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC54,sizeof(struct _tuple16*),3)))))));
# 2570
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2580
{int i=0;for(0;_tmp537 != 0;(_tmp537=_tmp537->tl,++ i)){
# 2581
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp537->hd);
# 2582
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp537->hd,0),s,0);}}
# 2586
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2599 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2601
_npop_handler(0);goto _LL232;
# 2495 "toc.cyc"
;_pop_region(r);}_LL24B: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp53D=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp53D->tag != 10)goto _LL24D;else{_tmp53E=_tmp53D->f1;}}_LL24C:
# 2604 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp53E);
# 2605
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp53E),0))->r;
# 2606
goto _LL232;_LL24D: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp53F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp53F->tag != 11)goto _LL24F;else{_tmp540=_tmp53F->f1;}}_LL24E:
# 2607
 Cyc_Toc_exp_to_c(nv,_tmp540);goto _LL232;_LL24F: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp541->tag != 12)goto _LL251;else{_tmp542=_tmp541->f1;_tmp543=_tmp541->f2;}}_LL250:
# 2609
 Cyc_Toc_exp_to_c(nv,_tmp542);
# 2618 "toc.cyc"
for(0;_tmp543 != 0;_tmp543=_tmp543->tl){
# 2619
enum Cyc_Absyn_KindQual _tmp600=(Cyc_Tcutil_typ_kind((void*)_tmp543->hd))->kind;
# 2620
if(_tmp600 != Cyc_Absyn_EffKind  && _tmp600 != Cyc_Absyn_RgnKind){
# 2621
{void*_tmp601=Cyc_Tcutil_compress((void*)_tmp543->hd);_LL2BE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp601;if(_tmp602->tag != 2)goto _LL2C0;}_LL2BF:
# 2622
 goto _LL2C1;_LL2C0: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp601;if(_tmp603->tag != 3)goto _LL2C2;}_LL2C1:
# 2623
 continue;_LL2C2:;_LL2C3:
# 2625
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp542,0))->r;
# 2626
goto _LL2BD;_LL2BD:;}
# 2628
break;}}
# 2631
goto _LL232;_LL251: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp544=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp544->tag != 13)goto _LL253;else{_tmp545=(void**)& _tmp544->f1;_tmp546=(void**)((void**)& _tmp544->f1);_tmp547=_tmp544->f2;_tmp548=_tmp544->f3;_tmp549=_tmp544->f4;}}_LL252: {
# 2633
void*old_t2=(void*)_check_null(_tmp547->topt);
# 2634
void*new_typ=*_tmp546;
# 2635
*_tmp546=Cyc_Toc_typ_to_c(new_typ);
# 2636
Cyc_Toc_exp_to_c(nv,_tmp547);
# 2638
{struct _tuple19 _tmpC55;struct _tuple19 _tmp605=(_tmpC55.f1=Cyc_Tcutil_compress(old_t2),((_tmpC55.f2=Cyc_Tcutil_compress(new_typ),_tmpC55)));void*_tmp606;struct Cyc_Absyn_PtrInfo _tmp608;void*_tmp609;struct Cyc_Absyn_PtrInfo _tmp60B;void*_tmp60C;struct Cyc_Absyn_PtrInfo _tmp60E;void*_tmp60F;_LL2C5: _tmp606=_tmp605.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 5)goto _LL2C7;else{_tmp608=_tmp607->f1;}};_tmp609=_tmp605.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp609;if(_tmp60A->tag != 5)goto _LL2C7;else{_tmp60B=_tmp60A->f1;}};_LL2C6: {
# 2640
int _tmp611=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp608.ptr_atts).nullable);
# 2641
int _tmp612=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp60B.ptr_atts).nullable);
# 2642
void*_tmp613=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp608.ptr_atts).bounds);
# 2643
void*_tmp614=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp60B.ptr_atts).bounds);
# 2644
int _tmp615=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp608.ptr_atts).zero_term);
# 2645
int _tmp616=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp60B.ptr_atts).zero_term);
# 2646
{struct _tuple19 _tmpC56;struct _tuple19 _tmp618=(_tmpC56.f1=_tmp613,((_tmpC56.f2=_tmp614,_tmpC56)));void*_tmp619;struct Cyc_Absyn_Exp*_tmp61B;void*_tmp61C;struct Cyc_Absyn_Exp*_tmp61E;void*_tmp61F;struct Cyc_Absyn_Exp*_tmp621;void*_tmp622;void*_tmp624;void*_tmp626;struct Cyc_Absyn_Exp*_tmp628;void*_tmp629;void*_tmp62B;_LL2CC: _tmp619=_tmp618.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp619;if(_tmp61A->tag != 1)goto _LL2CE;else{_tmp61B=_tmp61A->f1;}};_tmp61C=_tmp618.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61C;if(_tmp61D->tag != 1)goto _LL2CE;else{_tmp61E=_tmp61D->f1;}};_LL2CD:
# 2648
 if((!Cyc_Evexp_c_can_eval(_tmp61B) || !Cyc_Evexp_c_can_eval(_tmp61E)) && !
# 2649
Cyc_Evexp_same_const_exp(_tmp61B,_tmp61E)){
# 2650
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,Cyc_Tcutil_terr(e->loc,((_tmpC59="can't validate cast due to potential size differences",_tag_dyneither(_tmpC59,sizeof(char),54))),_tag_dyneither(_tmpC58,sizeof(void*),0)));}
# 2651
if(_tmp611  && !_tmp612){
# 2652
if(Cyc_Toc_is_toplevel(nv)){
# 2653
const char*_tmpC5C;void*_tmpC5B;(_tmpC5B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC5C="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC5C,sizeof(char),44))),_tag_dyneither(_tmpC5B,sizeof(void*),0)));}
# 2657
if(_tmp549 != Cyc_Absyn_NonNull_to_Null){
# 2658
const char*_tmpC60;void*_tmpC5F[1];struct Cyc_String_pa_PrintArg_struct _tmpC5E;(_tmpC5E.tag=0,((_tmpC5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC5F[0]=& _tmpC5E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC60="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC60,sizeof(char),41))),_tag_dyneither(_tmpC5F,sizeof(void*),1)))))));}{
# 2659
int do_null_check=Cyc_Toc_need_null_check(_tmp547);
# 2660
if(do_null_check){
# 2661
if(!_tmp548){
# 2662
const char*_tmpC63;void*_tmpC62;(_tmpC62=0,Cyc_Tcutil_warn(e->loc,((_tmpC63="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC63,sizeof(char),58))),_tag_dyneither(_tmpC62,sizeof(void*),0)));}{
# 2664
struct Cyc_List_List*_tmpC64;e->r=Cyc_Toc_cast_it_r(*_tmp546,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
# 2665
(_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->hd=_tmp547,((_tmpC64->tl=0,_tmpC64)))))),0));};}};}
# 2672
goto _LL2CB;_LL2CE: _tmp61F=_tmp618.f1;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp620=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61F;if(_tmp620->tag != 1)goto _LL2D0;else{_tmp621=_tmp620->f1;}};_tmp622=_tmp618.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp623=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp622;if(_tmp623->tag != 0)goto _LL2D0;};_LL2CF:
# 2674
 if(!Cyc_Evexp_c_can_eval(_tmp621)){
# 2675
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,Cyc_Tcutil_terr(e->loc,((_tmpC67="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC67,sizeof(char),71))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}
# 2677
if(_tmp549 == Cyc_Absyn_NonNull_to_Null){
# 2678
const char*_tmpC6B;void*_tmpC6A[1];struct Cyc_String_pa_PrintArg_struct _tmpC69;(_tmpC69.tag=0,((_tmpC69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC6A[0]=& _tmpC69,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC6B="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC6B,sizeof(char),44))),_tag_dyneither(_tmpC6A,sizeof(void*),1)))))));}
# 2679
if(Cyc_Toc_is_toplevel(nv)){
# 2681
if((_tmp615  && !(_tmp60B.elt_tq).real_const) && !_tmp616)
# 2684
_tmp621=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp621,Cyc_Absyn_uint_exp(1,0),0);
# 2686
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp621,_tmp547))->r;}else{
# 2688
struct Cyc_Absyn_Exp*_tmp63C=Cyc_Toc__tag_dyneither_e;
# 2690
if(_tmp615){
# 2695
struct _tuple0*_tmp63D=Cyc_Toc_temp_var();
# 2696
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Absyn_var_exp(_tmp63D,0);
# 2697
struct Cyc_Absyn_Exp*arg3;
# 2700
{void*_tmp63F=_tmp547->r;union Cyc_Absyn_Cnst _tmp641;struct _dyneither_ptr _tmp642;union Cyc_Absyn_Cnst _tmp644;struct _dyneither_ptr _tmp645;_LL2D5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp640->tag != 0)goto _LL2D7;else{_tmp641=_tmp640->f1;if((_tmp641.String_c).tag != 8)goto _LL2D7;_tmp642=(struct _dyneither_ptr)(_tmp641.String_c).val;}}_LL2D6:
# 2702
 arg3=_tmp621;goto _LL2D4;_LL2D7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp643=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp643->tag != 0)goto _LL2D9;else{_tmp644=_tmp643->f1;if((_tmp644.Wstring_c).tag != 9)goto _LL2D9;_tmp645=(struct _dyneither_ptr)(_tmp644.Wstring_c).val;}}_LL2D8:
# 2704
 arg3=_tmp621;goto _LL2D4;_LL2D9:;_LL2DA:
# 2706
{struct Cyc_Absyn_Exp*_tmpC6C[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp547),(
# 2708
(_tmpC6C[1]=_tmp621,((_tmpC6C[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp63E),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 2709
goto _LL2D4;_LL2D4:;}
# 2711
if(!_tmp616  && !(_tmp60B.elt_tq).real_const)
# 2714
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 2716
struct Cyc_Absyn_Exp*_tmp647=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60B.elt_typ),0);
# 2717
struct Cyc_Absyn_Exp*_tmpC6D[3];struct Cyc_Absyn_Exp*_tmp648=Cyc_Absyn_fncall_exp(_tmp63C,((_tmpC6D[2]=arg3,((_tmpC6D[1]=_tmp647,((_tmpC6D[0]=_tmp63E,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2718
struct Cyc_Absyn_Stmt*_tmp649=Cyc_Absyn_exp_stmt(_tmp648,0);
# 2719
_tmp649=Cyc_Absyn_declare_stmt(_tmp63D,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)_tmp547,_tmp649,0);
# 2720
e->r=Cyc_Toc_stmt_exp_r(_tmp649);};}else{
# 2723
struct Cyc_Absyn_Exp*_tmpC6E[3];e->r=Cyc_Toc_fncall_exp_r(_tmp63C,(
# 2724
(_tmpC6E[2]=_tmp621,((_tmpC6E[1]=
# 2725
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp60B.elt_typ),0),((_tmpC6E[0]=_tmp547,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2729
goto _LL2CB;_LL2D0: _tmp624=_tmp618.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp625=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp624;if(_tmp625->tag != 0)goto _LL2D2;};_tmp626=_tmp618.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp627=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp626;if(_tmp627->tag != 1)goto _LL2D2;else{_tmp628=_tmp627->f1;}};_LL2D1:
# 2731
 if(!Cyc_Evexp_c_can_eval(_tmp628)){
# 2732
const char*_tmpC71;void*_tmpC70;(_tmpC70=0,Cyc_Tcutil_terr(e->loc,((_tmpC71="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC71,sizeof(char),71))),_tag_dyneither(_tmpC70,sizeof(void*),0)));}
# 2733
if(Cyc_Toc_is_toplevel(nv)){
# 2734
const char*_tmpC74;void*_tmpC73;(_tmpC73=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC74="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC74,sizeof(char),45))),_tag_dyneither(_tmpC73,sizeof(void*),0)));}{
# 2743 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp650=_tmp628;
# 2744
if(_tmp615  && !_tmp616)
# 2745
_tmp650=Cyc_Absyn_add_exp(_tmp628,Cyc_Absyn_uint_exp(1,0),0);{
# 2750
struct Cyc_Absyn_Exp*_tmp651=Cyc_Toc__untag_dyneither_ptr_e;
# 2751
struct Cyc_Absyn_Exp*_tmpC75[3];struct Cyc_Absyn_Exp*_tmp652=Cyc_Absyn_fncall_exp(_tmp651,(
# 2752
(_tmpC75[2]=_tmp650,((_tmpC75[1]=
# 2753
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp608.elt_typ),0),((_tmpC75[0]=_tmp547,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC75,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2755
if(_tmp612){
# 2756
struct Cyc_List_List*_tmpC76;_tmp652->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
# 2757
(_tmpC76=_cycalloc(sizeof(*_tmpC76)),((_tmpC76->hd=Cyc_Absyn_copy_exp(_tmp652),((_tmpC76->tl=0,_tmpC76)))))));}
# 2758
e->r=Cyc_Toc_cast_it_r(*_tmp546,_tmp652);
# 2759
goto _LL2CB;};};_LL2D2: _tmp629=_tmp618.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp62A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp629;if(_tmp62A->tag != 0)goto _LL2CB;};_tmp62B=_tmp618.f2;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp62C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp62B;if(_tmp62C->tag != 0)goto _LL2CB;};_LL2D3:
# 2763
 DynCast:
# 2764
 if((_tmp615  && !_tmp616) && !(_tmp60B.elt_tq).real_const){
# 2765
if(Cyc_Toc_is_toplevel(nv)){
# 2766
const char*_tmpC79;void*_tmpC78;(_tmpC78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC79="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC79,sizeof(char),70))),_tag_dyneither(_tmpC78,sizeof(void*),0)));}{
# 2767
struct Cyc_Absyn_Exp*_tmp657=Cyc_Toc__dyneither_ptr_decrease_size_e;
# 2768
struct Cyc_Absyn_Exp*_tmpC7A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp657,(
# 2769
(_tmpC7A[2]=
# 2770
Cyc_Absyn_uint_exp(1,0),((_tmpC7A[1]=
# 2769
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp608.elt_typ),0),((_tmpC7A[0]=_tmp547,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 2772
goto _LL2CB;_LL2CB:;}
# 2774
goto _LL2C4;}_LL2C7: _tmp60C=_tmp605.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60C;if(_tmp60D->tag != 5)goto _LL2C9;else{_tmp60E=_tmp60D->f1;}};_tmp60F=_tmp605.f2;{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp610=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60F;if(_tmp610->tag != 6)goto _LL2C9;};_LL2C8:
# 2776
{void*_tmp659=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp60E.ptr_atts).bounds);_LL2DC: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp65A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp659;if(_tmp65A->tag != 0)goto _LL2DE;}_LL2DD:
# 2778
 _tmp547->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp547->r,_tmp547->loc),Cyc_Toc_curr_sp);goto _LL2DB;_LL2DE:;_LL2DF:
# 2779
 goto _LL2DB;_LL2DB:;}
# 2781
goto _LL2C4;_LL2C9:;_LL2CA:
# 2782
 goto _LL2C4;_LL2C4:;}
# 2784
goto _LL232;}_LL253: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp54A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp54A->tag != 14)goto _LL255;else{_tmp54B=_tmp54A->f1;}}_LL254:
# 2788
{void*_tmp65B=_tmp54B->r;struct _tuple0*_tmp65D;struct Cyc_List_List*_tmp65E;struct Cyc_List_List*_tmp65F;struct Cyc_List_List*_tmp661;_LL2E1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp65C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp65B;if(_tmp65C->tag != 27)goto _LL2E3;else{_tmp65D=_tmp65C->f1;_tmp65E=_tmp65C->f2;_tmp65F=_tmp65C->f3;}}_LL2E2:
# 2790
 if(Cyc_Toc_is_toplevel(nv)){
# 2791
const char*_tmpC7E;void*_tmpC7D[1];struct Cyc_String_pa_PrintArg_struct _tmpC7C;(_tmpC7C.tag=0,((_tmpC7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2792
Cyc_Position_string_of_segment(_tmp54B->loc)),((_tmpC7D[0]=& _tmpC7C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC7E="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC7E,sizeof(char),42))),_tag_dyneither(_tmpC7D,sizeof(void*),1)))))));}
# 2793
e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp54B->topt),_tmp65E,1,0,_tmp65F,_tmp65D))->r;
# 2794
goto _LL2E0;_LL2E3: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp660=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp65B;if(_tmp660->tag != 23)goto _LL2E5;else{_tmp661=_tmp660->f1;}}_LL2E4:
# 2796
 if(Cyc_Toc_is_toplevel(nv)){
# 2797
const char*_tmpC82;void*_tmpC81[1];struct Cyc_String_pa_PrintArg_struct _tmpC80;(_tmpC80.tag=0,((_tmpC80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2798
Cyc_Position_string_of_segment(_tmp54B->loc)),((_tmpC81[0]=& _tmpC80,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC82="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC82,sizeof(char),42))),_tag_dyneither(_tmpC81,sizeof(void*),1)))))));}
# 2799
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp661))->r;
# 2800
goto _LL2E0;_LL2E5:;_LL2E6:
# 2802
 Cyc_Toc_set_lhs(nv,1);
# 2803
Cyc_Toc_exp_to_c(nv,_tmp54B);
# 2804
Cyc_Toc_set_lhs(nv,0);
# 2805
if(!Cyc_Absyn_is_lvalue(_tmp54B)){
# 2806
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp54B,0,Cyc_Toc_address_lvalue,1);
# 2808
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp54B);}
# 2810
goto _LL2E0;_LL2E0:;}
# 2812
goto _LL232;_LL255: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp54C=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp54C->tag != 15)goto _LL257;else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54C->f2;}}_LL256:
# 2815
 if(Cyc_Toc_is_toplevel(nv)){
# 2816
const char*_tmpC86;void*_tmpC85[1];struct Cyc_String_pa_PrintArg_struct _tmpC84;(_tmpC84.tag=0,((_tmpC84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp54E->loc)),((_tmpC85[0]=& _tmpC84,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC86="%s: new at top-level",_tag_dyneither(_tmpC86,sizeof(char),21))),_tag_dyneither(_tmpC85,sizeof(void*),1)))))));}
# 2817
{void*_tmp66B=_tmp54E->r;struct Cyc_List_List*_tmp66D;struct Cyc_Absyn_Vardecl*_tmp66F;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp671;int _tmp672;struct _tuple0*_tmp674;struct Cyc_List_List*_tmp675;struct Cyc_List_List*_tmp676;struct Cyc_Absyn_Aggrdecl*_tmp677;struct Cyc_List_List*_tmp679;_LL2E8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp66C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp66B;if(_tmp66C->tag != 25)goto _LL2EA;else{_tmp66D=_tmp66C->f1;}}_LL2E9: {
# 2822
struct _tuple0*_tmp67A=Cyc_Toc_temp_var();
# 2823
struct Cyc_Absyn_Exp*_tmp67B=Cyc_Absyn_var_exp(_tmp67A,0);
# 2824
struct Cyc_Absyn_Stmt*_tmp67C=Cyc_Toc_init_array(nv,_tmp67B,_tmp66D,Cyc_Absyn_exp_stmt(_tmp67B,0));
# 2825
void*old_elt_typ;
# 2826
{void*_tmp67D=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp67F;void*_tmp680;struct Cyc_Absyn_Tqual _tmp681;struct Cyc_Absyn_PtrAtts _tmp682;union Cyc_Absyn_Constraint*_tmp683;_LL2F3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp67D;if(_tmp67E->tag != 5)goto _LL2F5;else{_tmp67F=_tmp67E->f1;_tmp680=_tmp67F.elt_typ;_tmp681=_tmp67F.elt_tq;_tmp682=_tmp67F.ptr_atts;_tmp683=_tmp682.zero_term;}}_LL2F4:
# 2828
 old_elt_typ=_tmp680;goto _LL2F2;_LL2F5:;_LL2F6: {
# 2830
const char*_tmpC89;void*_tmpC88;old_elt_typ=(
# 2831
(_tmpC88=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC89="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpC89,sizeof(char),52))),_tag_dyneither(_tmpC88,sizeof(void*),0))));}_LL2F2:;}{
# 2833
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2834
void*_tmp686=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
# 2835
struct Cyc_Absyn_Exp*_tmp687=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
# 2836
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp66D),0),0);
# 2837
struct Cyc_Absyn_Exp*e1;
# 2838
if(_tmp54D == 0  || Cyc_Absyn_no_regions)
# 2839
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp687);else{
# 2841
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp54D;
# 2842
Cyc_Toc_exp_to_c(nv,r);
# 2843
e1=Cyc_Toc_rmalloc_exp(r,_tmp687);}
# 2846
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp67A,_tmp686,(struct Cyc_Absyn_Exp*)e1,_tmp67C,0));
# 2847
goto _LL2E7;};}_LL2EA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp66E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp66B;if(_tmp66E->tag != 26)goto _LL2EC;else{_tmp66F=_tmp66E->f1;_tmp670=_tmp66E->f2;_tmp671=_tmp66E->f3;_tmp672=_tmp66E->f4;}}_LL2EB: {
# 2859 "toc.cyc"
int is_dyneither_ptr=0;
# 2860
{void*_tmp688=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp68A;void*_tmp68B;struct Cyc_Absyn_Tqual _tmp68C;struct Cyc_Absyn_PtrAtts _tmp68D;union Cyc_Absyn_Constraint*_tmp68E;union Cyc_Absyn_Constraint*_tmp68F;_LL2F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp689=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp688;if(_tmp689->tag != 5)goto _LL2FA;else{_tmp68A=_tmp689->f1;_tmp68B=_tmp68A.elt_typ;_tmp68C=_tmp68A.elt_tq;_tmp68D=_tmp68A.ptr_atts;_tmp68E=_tmp68D.bounds;_tmp68F=_tmp68D.zero_term;}}_LL2F9:
# 2862
 is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp68E)== (void*)& Cyc_Absyn_DynEither_b_val;
# 2863
goto _LL2F7;_LL2FA:;_LL2FB: {
# 2864
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8C="exp_to_c: comprehension not an array type",_tag_dyneither(_tmpC8C,sizeof(char),42))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}_LL2F7:;}{
# 2866
struct _tuple0*max=Cyc_Toc_temp_var();
# 2867
struct _tuple0*a=Cyc_Toc_temp_var();
# 2868
void*old_elt_typ=(void*)_check_null(_tmp671->topt);
# 2869
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2870
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
# 2871
Cyc_Toc_exp_to_c(nv,_tmp670);{
# 2872
struct Cyc_Absyn_Exp*_tmp692=Cyc_Absyn_var_exp(max,0);
# 2874
if(_tmp672)
# 2875
_tmp692=Cyc_Absyn_add_exp(_tmp692,Cyc_Absyn_uint_exp(1,0),0);{
# 2876
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),_tmp66F,Cyc_Absyn_var_exp(max,0),_tmp671,_tmp672,
# 2877
Cyc_Toc_skip_stmt_dl(),1);
# 2879
struct _RegionHandle _tmp693=_new_region("r");struct _RegionHandle*r=& _tmp693;_push_region(r);{
# 2880
struct _tuple21*_tmpC8F;struct Cyc_List_List*_tmpC8E;struct Cyc_List_List*decls=
# 2881
(_tmpC8E=_region_malloc(r,sizeof(*_tmpC8E)),((_tmpC8E->hd=((_tmpC8F=_region_malloc(r,sizeof(*_tmpC8F)),((_tmpC8F->f1=max,((_tmpC8F->f2=Cyc_Absyn_uint_typ,((_tmpC8F->f3=(struct Cyc_Absyn_Exp*)_tmp670,_tmpC8F)))))))),((_tmpC8E->tl=0,_tmpC8E)))));
# 2882
struct Cyc_Absyn_Exp*ai;
# 2883
if(_tmp54D == 0  || Cyc_Absyn_no_regions){
# 2884
struct Cyc_Absyn_Exp*_tmpC90[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
# 2885
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
# 2886
(_tmpC90[1]=_tmp692,((_tmpC90[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC90,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2889
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp54D;
# 2890
Cyc_Toc_exp_to_c(nv,r);{
# 2891
struct Cyc_Absyn_Exp*_tmpC91[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
# 2892
(_tmpC91[1]=_tmp692,((_tmpC91[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC91,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2896
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
# 2897
{struct _tuple21*_tmpC94;struct Cyc_List_List*_tmpC93;decls=((_tmpC93=_region_malloc(r,sizeof(*_tmpC93)),((_tmpC93->hd=((_tmpC94=_region_malloc(r,sizeof(*_tmpC94)),((_tmpC94->f1=a,((_tmpC94->f2=ptr_typ,((_tmpC94->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC94)))))))),((_tmpC93->tl=decls,_tmpC93))))));}
# 2898
if(is_dyneither_ptr){
# 2899
struct _tuple0*_tmp698=Cyc_Toc_temp_var();
# 2900
void*_tmp699=Cyc_Toc_typ_to_c(old_typ);
# 2901
struct Cyc_Absyn_Exp*_tmp69A=Cyc_Toc__tag_dyneither_e;
# 2902
struct Cyc_Absyn_Exp*_tmpC95[3];struct Cyc_Absyn_Exp*_tmp69B=
# 2903
Cyc_Absyn_fncall_exp(_tmp69A,(
# 2904
(_tmpC95[2]=_tmp692,((_tmpC95[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC95[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC95,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2906
{struct _tuple21*_tmpC98;struct Cyc_List_List*_tmpC97;decls=((_tmpC97=_region_malloc(r,sizeof(*_tmpC97)),((_tmpC97->hd=((_tmpC98=_region_malloc(r,sizeof(*_tmpC98)),((_tmpC98->f1=_tmp698,((_tmpC98->f2=_tmp699,((_tmpC98->f3=(struct Cyc_Absyn_Exp*)_tmp69B,_tmpC98)))))))),((_tmpC97->tl=decls,_tmpC97))))));}
# 2907
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp698,0),0),0);}else{
# 2909
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
# 2910
{struct Cyc_List_List*_tmp69F=decls;for(0;_tmp69F != 0;_tmp69F=_tmp69F->tl){
# 2911
struct _tuple0*_tmp6A1;void*_tmp6A2;struct Cyc_Absyn_Exp*_tmp6A3;struct _tuple21 _tmp6A0=*((struct _tuple21*)_tmp69F->hd);_tmp6A1=_tmp6A0.f1;_tmp6A2=_tmp6A0.f2;_tmp6A3=_tmp6A0.f3;
# 2912
s=Cyc_Absyn_declare_stmt(_tmp6A1,_tmp6A2,_tmp6A3,s,0);}}
# 2914
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2916
_npop_handler(0);goto _LL2E7;
# 2879
;_pop_region(r);};};};}_LL2EC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp66B;if(_tmp673->tag != 27)goto _LL2EE;else{_tmp674=_tmp673->f1;_tmp675=_tmp673->f2;_tmp676=_tmp673->f3;_tmp677=_tmp673->f4;}}_LL2ED:
# 2919
 e->r=(Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp54E->topt),_tmp675,1,_tmp54D,_tmp676,_tmp674))->r;
# 2920
goto _LL2E7;_LL2EE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp678=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp66B;if(_tmp678->tag != 23)goto _LL2F0;else{_tmp679=_tmp678->f1;}}_LL2EF:
# 2923
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp54D,_tmp679))->r;
# 2924
goto _LL2E7;_LL2F0:;_LL2F1: {
# 2930
void*old_elt_typ=(void*)_check_null(_tmp54E->topt);
# 2931
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 2933
struct _tuple0*_tmp6A6=Cyc_Toc_temp_var();
# 2934
struct Cyc_Absyn_Exp*_tmp6A7=Cyc_Absyn_var_exp(_tmp6A6,0);
# 2935
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp6A7,0),0);
# 2936
struct Cyc_Absyn_Exp*inner_mexp=mexp;
# 2937
if(_tmp54D == 0  || Cyc_Absyn_no_regions)
# 2938
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 2940
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp54D;
# 2941
Cyc_Toc_exp_to_c(nv,r);
# 2942
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 2948
int done=0;
# 2949
{void*_tmp6A8=_tmp54E->r;void*_tmp6AA;struct Cyc_Absyn_Exp*_tmp6AB;_LL2FD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6A9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6A8;if(_tmp6A9->tag != 13)goto _LL2FF;else{_tmp6AA=(void*)_tmp6A9->f1;_tmp6AB=_tmp6A9->f2;}}_LL2FE:
# 2951
{struct _tuple19 _tmpC99;struct _tuple19 _tmp6AD=(_tmpC99.f1=Cyc_Tcutil_compress(_tmp6AA),((_tmpC99.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp6AB->topt)),_tmpC99)));void*_tmp6AE;struct Cyc_Absyn_PtrInfo _tmp6B0;void*_tmp6B1;struct Cyc_Absyn_PtrAtts _tmp6B2;union Cyc_Absyn_Constraint*_tmp6B3;void*_tmp6B4;struct Cyc_Absyn_PtrInfo _tmp6B6;struct Cyc_Absyn_PtrAtts _tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;_LL302: _tmp6AE=_tmp6AD.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6AF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6AE;if(_tmp6AF->tag != 5)goto _LL304;else{_tmp6B0=_tmp6AF->f1;_tmp6B1=_tmp6B0.elt_typ;_tmp6B2=_tmp6B0.ptr_atts;_tmp6B3=_tmp6B2.bounds;}};_tmp6B4=_tmp6AD.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6B4;if(_tmp6B5->tag != 5)goto _LL304;else{_tmp6B6=_tmp6B5->f1;_tmp6B7=_tmp6B6.ptr_atts;_tmp6B8=_tmp6B7.bounds;}};_LL303:
# 2954
{struct _tuple19 _tmpC9A;struct _tuple19 _tmp6BA=(_tmpC9A.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6B3),((_tmpC9A.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6B8),_tmpC9A)));void*_tmp6BB;void*_tmp6BD;struct Cyc_Absyn_Exp*_tmp6BF;_LL307: _tmp6BB=_tmp6BA.f1;{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6BC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6BB;if(_tmp6BC->tag != 0)goto _LL309;};_tmp6BD=_tmp6BA.f2;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6BE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6BD;if(_tmp6BE->tag != 1)goto _LL309;else{_tmp6BF=_tmp6BE->f1;}};_LL308:
# 2956
 Cyc_Toc_exp_to_c(nv,_tmp6AB);
# 2957
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
# 2958
done=1;{
# 2959
struct Cyc_Absyn_Exp*_tmp6C0=Cyc_Toc__init_dyneither_ptr_e;
# 2960
{struct Cyc_Absyn_Exp*_tmpC9B[4];e->r=Cyc_Toc_fncall_exp_r(_tmp6C0,(
# 2961
(_tmpC9B[3]=_tmp6BF,((_tmpC9B[2]=
# 2962
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp6B1),0),((_tmpC9B[1]=_tmp6AB,((_tmpC9B[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9B,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 2964
goto _LL306;};_LL309:;_LL30A:
# 2965
 goto _LL306;_LL306:;}
# 2967
goto _LL301;_LL304:;_LL305:
# 2968
 goto _LL301;_LL301:;}
# 2970
goto _LL2FC;_LL2FF:;_LL300:
# 2971
 goto _LL2FC;_LL2FC:;}
# 2973
if(!done){
# 2974
struct Cyc_Absyn_Stmt*_tmp6C2=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp6A7),0);
# 2975
struct Cyc_Absyn_Exp*_tmp6C3=Cyc_Absyn_signed_int_exp(0,0);
# 2976
Cyc_Toc_exp_to_c(nv,_tmp54E);
# 2977
_tmp6C2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp6A7,_tmp6C3,0),_tmp54E,0),_tmp6C2,0);{
# 2979
void*_tmp6C4=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
# 2980
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp6A6,_tmp6C4,(struct Cyc_Absyn_Exp*)mexp,_tmp6C2,0));};}
# 2982
goto _LL2E7;};}_LL2E7:;}
# 2984
goto _LL232;_LL257: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp54F->tag != 17)goto _LL259;else{_tmp550=_tmp54F->f1;}}_LL258:
# 2985
 Cyc_Toc_exp_to_c(nv,_tmp550);goto _LL232;_LL259: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp551=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp551->tag != 16)goto _LL25B;else{_tmp552=(void*)_tmp551->f1;}}_LL25A:
# 2986
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpC9E;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC9D;e->r=(void*)((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D[0]=((_tmpC9E.tag=16,((_tmpC9E.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp552),_tmpC9E)))),_tmpC9D))));}goto _LL232;_LL25B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp553=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp553->tag != 18)goto _LL25D;else{_tmp554=(void*)_tmp553->f1;_tmp555=(void*)_tmp553->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp556=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp555;if(_tmp556->tag != 0)goto _LL25D;else{_tmp557=_tmp556->f1;}};}}_LL25C:
# 2988
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCA8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCA7;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCA6;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCA5;e->r=(void*)((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA8.tag=18,((_tmpCA8.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp554),((_tmpCA8.f2=(void*)((void*)((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6[0]=((_tmpCA7.tag=0,((_tmpCA7.f1=_tmp557,_tmpCA7)))),_tmpCA6))))),_tmpCA8)))))),_tmpCA5))));}goto _LL232;_LL25D: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp558=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp558->tag != 18)goto _LL25F;else{_tmp559=(void*)_tmp558->f1;_tmp55A=(void*)_tmp558->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp55B=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp55A;if(_tmp55B->tag != 1)goto _LL25F;else{_tmp55C=_tmp55B->f1;}};}}_LL25E:
# 2990
{void*_tmp6CB=Cyc_Tcutil_compress(_tmp559);struct Cyc_Absyn_AggrInfo _tmp6CD;union Cyc_Absyn_AggrInfoU _tmp6CE;struct Cyc_List_List*_tmp6D0;_LL30C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6CC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6CB;if(_tmp6CC->tag != 11)goto _LL30E;else{_tmp6CD=_tmp6CC->f1;_tmp6CE=_tmp6CD.aggr_info;}}_LL30D: {
# 2992
struct Cyc_Absyn_Aggrdecl*_tmp6D3=Cyc_Absyn_get_known_aggrdecl(_tmp6CE);
# 2993
if(_tmp6D3->impl == 0){
# 2994
const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAB="struct fields must be known",_tag_dyneither(_tmpCAB,sizeof(char),28))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}
# 2995
_tmp6D0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D3->impl))->fields;goto _LL30F;}_LL30E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6CF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6CB;if(_tmp6CF->tag != 12)goto _LL310;else{_tmp6D0=_tmp6CF->f2;}}_LL30F: {
# 2997
struct Cyc_Absyn_Aggrfield*_tmp6D6=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp6D0,(int)_tmp55C);
# 2998
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCB5;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCB4;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCB3;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCB2;e->r=(void*)((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB5.tag=18,((_tmpCB5.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp559),((_tmpCB5.f2=(void*)((void*)((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3[0]=((_tmpCB4.tag=0,((_tmpCB4.f1=_tmp6D6->name,_tmpCB4)))),_tmpCB3))))),_tmpCB5)))))),_tmpCB2))));}
# 2999
goto _LL30B;}_LL310: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6D1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6CB;if(_tmp6D1->tag != 10)goto _LL312;}_LL311:
# 3001
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCBF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCBE;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCBD;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCBC;e->r=(void*)((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBF.tag=18,((_tmpCBF.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp559),((_tmpCBF.f2=(void*)((void*)((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD[0]=((_tmpCBE.tag=0,((_tmpCBE.f1=Cyc_Absyn_fieldname((int)(_tmp55C + 1)),_tmpCBE)))),_tmpCBD))))),_tmpCBF)))))),_tmpCBC))));}
# 3002
goto _LL30B;_LL312: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6CB;if(_tmp6D2->tag != 4)goto _LL314;}_LL313:
# 3004
 if(_tmp55C == 0){
# 3005
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCC9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCC7;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCC6;e->r=(void*)((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6[0]=((_tmpCC9.tag=18,((_tmpCC9.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp559),((_tmpCC9.f2=(void*)((void*)((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7[0]=((_tmpCC8.tag=0,((_tmpCC8.f1=Cyc_Toc_tag_sp,_tmpCC8)))),_tmpCC7))))),_tmpCC9)))))),_tmpCC6))));}else{
# 3007
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCD3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCD2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCD1;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCD0;e->r=(void*)((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCD3.tag=18,((_tmpCD3.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp559),((_tmpCD3.f2=(void*)((void*)((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD2.tag=0,((_tmpCD2.f1=Cyc_Absyn_fieldname((int)_tmp55C),_tmpCD2)))),_tmpCD1))))),_tmpCD3)))))),_tmpCD0))));}
# 3008
goto _LL30B;_LL314:;_LL315: {
# 3009
const char*_tmpCD6;void*_tmpCD5;(_tmpCD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD6="impossible type for offsetof tuple index",_tag_dyneither(_tmpCD6,sizeof(char),41))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}_LL30B:;}
# 3011
goto _LL232;_LL25F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp55D->tag != 19)goto _LL261;else{_tmp55E=_tmp55D->f1;}}_LL260: {
# 3013
int _tmp6E9=Cyc_Toc_in_lhs(nv);
# 3014
Cyc_Toc_set_lhs(nv,0);{
# 3015
void*_tmp6EA=Cyc_Tcutil_compress((void*)_check_null(_tmp55E->topt));
# 3016
{void*_tmp6EB=_tmp6EA;struct Cyc_Absyn_PtrInfo _tmp6ED;void*_tmp6EE;struct Cyc_Absyn_Tqual _tmp6EF;struct Cyc_Absyn_PtrAtts _tmp6F0;void*_tmp6F1;union Cyc_Absyn_Constraint*_tmp6F2;union Cyc_Absyn_Constraint*_tmp6F3;union Cyc_Absyn_Constraint*_tmp6F4;_LL317: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6EB;if(_tmp6EC->tag != 5)goto _LL319;else{_tmp6ED=_tmp6EC->f1;_tmp6EE=_tmp6ED.elt_typ;_tmp6EF=_tmp6ED.elt_tq;_tmp6F0=_tmp6ED.ptr_atts;_tmp6F1=_tmp6F0.rgn;_tmp6F2=_tmp6F0.nullable;_tmp6F3=_tmp6F0.bounds;_tmp6F4=_tmp6F0.zero_term;}}_LL318:
# 3018
{void*_tmp6F5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6F3);struct Cyc_Absyn_Exp*_tmp6F7;_LL31C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6F6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6F5;if(_tmp6F6->tag != 1)goto _LL31E;else{_tmp6F7=_tmp6F6->f1;}}_LL31D: {
# 3020
int do_null_check=Cyc_Toc_need_null_check(_tmp55E);
# 3021
Cyc_Toc_exp_to_c(nv,_tmp55E);
# 3022
if(do_null_check){
# 3023
if(Cyc_Toc_warn_all_null_deref){
# 3024
const char*_tmpCD9;void*_tmpCD8;(_tmpCD8=0,Cyc_Tcutil_warn(e->loc,((_tmpCD9="inserted null check due to dereference",_tag_dyneither(_tmpCD9,sizeof(char),39))),_tag_dyneither(_tmpCD8,sizeof(void*),0)));}{
# 3026
struct Cyc_List_List*_tmpCDA;_tmp55E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6EA),
# 3027
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
# 3028
(_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->hd=Cyc_Absyn_copy_exp(_tmp55E),((_tmpCDA->tl=0,_tmpCDA)))))),0));};}
# 3034
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6F4)){
# 3035
unsigned int _tmp6FD;int _tmp6FE;struct _tuple11 _tmp6FC=Cyc_Evexp_eval_const_uint_exp(_tmp6F7);_tmp6FD=_tmp6FC.f1;_tmp6FE=_tmp6FC.f2;
# 3040
if(!_tmp6FE  || _tmp6FD <= 0){
# 3041
const char*_tmpCDD;void*_tmpCDC;(_tmpCDC=0,Cyc_Tcutil_terr(e->loc,((_tmpCDD="cannot determine dereference is in bounds",_tag_dyneither(_tmpCDD,sizeof(char),42))),_tag_dyneither(_tmpCDC,sizeof(void*),0)));}}
# 3043
goto _LL31B;}_LL31E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6F8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6F5;if(_tmp6F8->tag != 0)goto _LL31B;}_LL31F: {
# 3046
struct Cyc_Absyn_Exp*_tmp701=Cyc_Absyn_uint_exp(0,0);
# 3047
_tmp701->topt=(void*)Cyc_Absyn_uint_typ;
# 3048
e->r=Cyc_Toc_subscript_exp_r(_tmp55E,_tmp701);
# 3049
Cyc_Toc_exp_to_c(nv,e);
# 3050
goto _LL31B;}_LL31B:;}
# 3052
goto _LL316;_LL319:;_LL31A: {
# 3053
const char*_tmpCE0;void*_tmpCDF;(_tmpCDF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE0="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCE0,sizeof(char),29))),_tag_dyneither(_tmpCDF,sizeof(void*),0)));}_LL316:;}
# 3055
Cyc_Toc_set_lhs(nv,_tmp6E9);
# 3056
goto _LL232;};}_LL261: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp55F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp55F->tag != 20)goto _LL263;else{_tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;_tmp562=_tmp55F->f3;_tmp563=_tmp55F->f4;}}_LL262: {
# 3058
int is_poly=Cyc_Toc_is_poly_project(e);
# 3059
void*e1_cyc_type=(void*)_check_null(_tmp560->topt);
# 3060
Cyc_Toc_exp_to_c(nv,_tmp560);
# 3061
if(_tmp562  && _tmp563)
# 3062
e->r=Cyc_Toc_check_tagged_union(_tmp560,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp561,
# 3063
Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrmember_exp);
# 3066
if(is_poly)
# 3067
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
# 3068
goto _LL232;}_LL263: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp564=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp564->tag != 21)goto _LL265;else{_tmp565=_tmp564->f1;_tmp566=_tmp564->f2;_tmp567=_tmp564->f3;_tmp568=_tmp564->f4;}}_LL264: {
# 3070
int _tmp704=Cyc_Toc_in_lhs(nv);
# 3071
Cyc_Toc_set_lhs(nv,0);{
# 3072
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp565->topt));
# 3073
int do_null_check=Cyc_Toc_need_null_check(_tmp565);
# 3074
Cyc_Toc_exp_to_c(nv,_tmp565);{
# 3075
int is_poly=Cyc_Toc_is_poly_project(e);
# 3076
void*_tmp706;struct Cyc_Absyn_Tqual _tmp707;struct Cyc_Absyn_PtrAtts _tmp708;void*_tmp709;union Cyc_Absyn_Constraint*_tmp70A;union Cyc_Absyn_Constraint*_tmp70B;union Cyc_Absyn_Constraint*_tmp70C;struct Cyc_Absyn_PtrInfo _tmp705=Cyc_Toc_get_ptr_type(e1typ);_tmp706=_tmp705.elt_typ;_tmp707=_tmp705.elt_tq;_tmp708=_tmp705.ptr_atts;_tmp709=_tmp708.rgn;_tmp70A=_tmp708.nullable;_tmp70B=_tmp708.bounds;_tmp70C=_tmp708.zero_term;
# 3077
{void*_tmp70D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp70B);struct Cyc_Absyn_Exp*_tmp70F;_LL321: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp70E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp70D;if(_tmp70E->tag != 1)goto _LL323;else{_tmp70F=_tmp70E->f1;}}_LL322: {
# 3081
unsigned int _tmp712;int _tmp713;struct _tuple11 _tmp711=Cyc_Evexp_eval_const_uint_exp(_tmp70F);_tmp712=_tmp711.f1;_tmp713=_tmp711.f2;
# 3082
if(_tmp713){
# 3083
if(_tmp712 < 1){
# 3084
const char*_tmpCE3;void*_tmpCE2;(_tmpCE2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE3="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCE3,sizeof(char),44))),_tag_dyneither(_tmpCE2,sizeof(void*),0)));}
# 3085
if(do_null_check){
# 3086
if(Cyc_Toc_warn_all_null_deref){
# 3087
const char*_tmpCE6;void*_tmpCE5;(_tmpCE5=0,Cyc_Tcutil_warn(e->loc,((_tmpCE6="inserted null check due to dereference",_tag_dyneither(_tmpCE6,sizeof(char),39))),_tag_dyneither(_tmpCE5,sizeof(void*),0)));}{
# 3089
struct Cyc_Absyn_Exp*_tmpCE7[1];_tmp565->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
# 3090
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
# 3091
(_tmpCE7[0]=Cyc_Absyn_new_exp(_tmp565->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE7,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3094
if(!Cyc_Evexp_c_can_eval(_tmp70F)){
# 3095
const char*_tmpCEA;void*_tmpCE9;(_tmpCE9=0,Cyc_Tcutil_terr(e->loc,((_tmpCEA="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCEA,sizeof(char),47))),_tag_dyneither(_tmpCE9,sizeof(void*),0)));}{
# 3098
struct Cyc_Absyn_Exp*_tmpCEB[4];_tmp565->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
# 3099
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
# 3100
(_tmpCEB[3]=
# 3102
Cyc_Absyn_uint_exp(0,0),((_tmpCEB[2]=
# 3101
Cyc_Absyn_sizeoftyp_exp(_tmp706,0),((_tmpCEB[1]=_tmp70F,((_tmpCEB[0]=
# 3100
Cyc_Absyn_new_exp(_tmp565->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEB,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3104
goto _LL320;}_LL323: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp710=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp70D;if(_tmp710->tag != 0)goto _LL320;}_LL324: {
# 3107
void*ta1=Cyc_Toc_typ_to_c_array(_tmp706);
# 3108
{struct Cyc_Absyn_Exp*_tmpCEC[3];_tmp565->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp707),
# 3109
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
# 3110
(_tmpCEC[2]=
# 3112
Cyc_Absyn_uint_exp(0,0),((_tmpCEC[1]=
# 3111
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCEC[0]=
# 3110
Cyc_Absyn_new_exp(_tmp565->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3115
goto _LL320;}_LL320:;}
# 3117
if(_tmp567  && _tmp568)
# 3118
e->r=Cyc_Toc_check_tagged_union(_tmp565,Cyc_Toc_typ_to_c(e1typ),_tmp706,_tmp566,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3120
if(is_poly  && _tmp568)
# 3121
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
# 3122
Cyc_Toc_set_lhs(nv,_tmp704);
# 3123
goto _LL232;};};}_LL265: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp569=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp569->tag != 22)goto _LL267;else{_tmp56A=_tmp569->f1;_tmp56B=_tmp569->f2;}}_LL266: {
# 3125
int _tmp71D=Cyc_Toc_in_lhs(nv);
# 3126
Cyc_Toc_set_lhs(nv,0);{
# 3127
void*_tmp71E=Cyc_Tcutil_compress((void*)_check_null(_tmp56A->topt));
# 3129
{void*_tmp71F=_tmp71E;struct Cyc_List_List*_tmp721;struct Cyc_Absyn_PtrInfo _tmp723;void*_tmp724;struct Cyc_Absyn_Tqual _tmp725;struct Cyc_Absyn_PtrAtts _tmp726;void*_tmp727;union Cyc_Absyn_Constraint*_tmp728;union Cyc_Absyn_Constraint*_tmp729;union Cyc_Absyn_Constraint*_tmp72A;_LL326: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp71F;if(_tmp720->tag != 10)goto _LL328;else{_tmp721=_tmp720->f1;}}_LL327:
# 3132
 Cyc_Toc_exp_to_c(nv,_tmp56A);
# 3133
Cyc_Toc_exp_to_c(nv,_tmp56B);{
# 3134
unsigned int _tmp72C;int _tmp72D;struct _tuple11 _tmp72B=Cyc_Evexp_eval_const_uint_exp(_tmp56B);_tmp72C=_tmp72B.f1;_tmp72D=_tmp72B.f2;
# 3135
if(!_tmp72D){
# 3136
const char*_tmpCEF;void*_tmpCEE;(_tmpCEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCEF="unknown tuple subscript in translation to C",_tag_dyneither(_tmpCEF,sizeof(char),44))),_tag_dyneither(_tmpCEE,sizeof(void*),0)));}
# 3137
e->r=Cyc_Toc_aggrmember_exp_r(_tmp56A,Cyc_Absyn_fieldname((int)(_tmp72C + 1)));
# 3138
goto _LL325;};_LL328: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp722=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71F;if(_tmp722->tag != 5)goto _LL32A;else{_tmp723=_tmp722->f1;_tmp724=_tmp723.elt_typ;_tmp725=_tmp723.elt_tq;_tmp726=_tmp723.ptr_atts;_tmp727=_tmp726.rgn;_tmp728=_tmp726.nullable;_tmp729=_tmp726.bounds;_tmp72A=_tmp726.zero_term;}}_LL329: {
# 3140
struct Cyc_List_List*_tmp730=Cyc_Toc_get_relns(_tmp56A);
# 3146
int in_bnds=0;
# 3147
{void*_tmp731=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp729);_LL32D:;_LL32E:
# 3149
 in_bnds=Cyc_Toc_check_bounds(_tmp71E,_tmp730,_tmp56A,_tmp56B);
# 3150
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
# 3151
const char*_tmpCF3;void*_tmpCF2[1];struct Cyc_String_pa_PrintArg_struct _tmpCF1;(_tmpCF1.tag=0,((_tmpCF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCF2[0]=& _tmpCF1,Cyc_Tcutil_warn(e->loc,((_tmpCF3="bounds check necessary for %s",_tag_dyneither(_tmpCF3,sizeof(char),30))),_tag_dyneither(_tmpCF2,sizeof(void*),1)))))));}_LL32C:;}
# 3156
Cyc_Toc_exp_to_c(nv,_tmp56A);
# 3157
Cyc_Toc_exp_to_c(nv,_tmp56B);
# 3158
++ Cyc_Toc_total_bounds_checks;
# 3159
{void*_tmp735=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp729);struct Cyc_Absyn_Exp*_tmp737;_LL330: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp736=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp735;if(_tmp736->tag != 1)goto _LL332;else{_tmp737=_tmp736->f1;}}_LL331: {
# 3161
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp728);
# 3162
void*ta1=Cyc_Toc_typ_to_c(_tmp724);
# 3163
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp725);
# 3164
if(in_bnds)
# 3165
++ Cyc_Toc_bounds_checks_eliminated;else{
# 3167
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp72A)){
# 3169
if(!Cyc_Evexp_c_can_eval(_tmp737)){
# 3170
const char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,Cyc_Tcutil_terr(e->loc,((_tmpCF6="cannot determine subscript is in bounds",_tag_dyneither(_tmpCF6,sizeof(char),40))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}{
# 3171
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp56A);
# 3172
struct Cyc_Absyn_Exp*_tmpCF7[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
# 3173
Cyc_Absyn_fncall_exp(function_e,(
# 3174
(_tmpCF7[2]=_tmp56B,((_tmpCF7[1]=_tmp737,((_tmpCF7[0]=_tmp56A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
# 3175
if(possibly_null){
# 3176
if(!Cyc_Evexp_c_can_eval(_tmp737)){
# 3177
const char*_tmpCFA;void*_tmpCF9;(_tmpCF9=0,Cyc_Tcutil_terr(e->loc,((_tmpCFA="cannot determine subscript is in bounds",_tag_dyneither(_tmpCFA,sizeof(char),40))),_tag_dyneither(_tmpCF9,sizeof(void*),0)));}
# 3178
if(Cyc_Toc_warn_all_null_deref){
# 3179
const char*_tmpCFD;void*_tmpCFC;(_tmpCFC=0,Cyc_Tcutil_warn(e->loc,((_tmpCFD="inserted null check due to dereference",_tag_dyneither(_tmpCFD,sizeof(char),39))),_tag_dyneither(_tmpCFC,sizeof(void*),0)));}{
# 3181
struct Cyc_Absyn_Exp*_tmpCFE[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
# 3182
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
# 3183
(_tmpCFE[3]=_tmp56B,((_tmpCFE[2]=
# 3184
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCFE[1]=_tmp737,((_tmpCFE[0]=_tmp56A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFE,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3187
if(!Cyc_Evexp_c_can_eval(_tmp737)){
# 3188
const char*_tmpD01;void*_tmpD00;(_tmpD00=0,Cyc_Tcutil_terr(e->loc,((_tmpD01="cannot determine subscript is in bounds",_tag_dyneither(_tmpD01,sizeof(char),40))),_tag_dyneither(_tmpD00,sizeof(void*),0)));}{
# 3190
struct Cyc_Absyn_Exp*_tmpD02[2];_tmp56B->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
# 3191
(_tmpD02[1]=Cyc_Absyn_copy_exp(_tmp56B),((_tmpD02[0]=_tmp737,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD02,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}
# 3193
goto _LL32F;}_LL332: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp738=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp735;if(_tmp738->tag != 0)goto _LL32F;}_LL333: {
# 3195
void*ta1=Cyc_Toc_typ_to_c_array(_tmp724);
# 3196
if(in_bnds){
# 3199
++ Cyc_Toc_bounds_checks_eliminated;
# 3200
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp725),
# 3201
Cyc_Absyn_aggrmember_exp(_tmp56A,Cyc_Toc_curr_sp,0)),_tmp56B);}else{
# 3204
struct Cyc_Absyn_Exp*_tmp744=Cyc_Toc__check_dyneither_subscript_e;
# 3205
struct Cyc_Absyn_Exp*_tmpD03[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp725),
# 3206
Cyc_Absyn_fncall_exp(_tmp744,(
# 3207
(_tmpD03[2]=_tmp56B,((_tmpD03[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD03[0]=_tmp56A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD03,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3210
goto _LL32F;}_LL32F:;}
# 3212
goto _LL325;}_LL32A:;_LL32B: {
# 3213
const char*_tmpD06;void*_tmpD05;(_tmpD05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD06="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD06,sizeof(char),49))),_tag_dyneither(_tmpD05,sizeof(void*),0)));}_LL325:;}
# 3215
Cyc_Toc_set_lhs(nv,_tmp71D);
# 3216
goto _LL232;};}_LL267: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp56C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp56C->tag != 23)goto _LL269;else{_tmp56D=_tmp56C->f1;}}_LL268:
# 3218
 if(!Cyc_Toc_is_toplevel(nv))
# 3219
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp56D))->r;else{
# 3223
struct Cyc_List_List*_tmp748=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp56D);
# 3224
void*_tmp749=Cyc_Toc_add_tuple_type(_tmp748);
# 3225
struct Cyc_List_List*dles=0;
# 3226
{int i=1;for(0;_tmp56D != 0;(_tmp56D=_tmp56D->tl,i ++)){
# 3227
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp56D->hd);{
# 3228
struct _tuple16*_tmpD09;struct Cyc_List_List*_tmpD08;dles=((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=((_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->f1=0,((_tmpD09->f2=(struct Cyc_Absyn_Exp*)_tmp56D->hd,_tmpD09)))))),((_tmpD08->tl=dles,_tmpD08))))));};}}
# 3230
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
# 3231
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3233
goto _LL232;_LL269: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp56E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp56E->tag != 25)goto _LL26B;else{_tmp56F=_tmp56E->f1;}}_LL26A:
# 3237
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp56F);
# 3238
{struct Cyc_List_List*_tmp74C=_tmp56F;for(0;_tmp74C != 0;_tmp74C=_tmp74C->tl){
# 3239
struct _tuple16 _tmp74E;struct Cyc_Absyn_Exp*_tmp74F;struct _tuple16*_tmp74D=(struct _tuple16*)_tmp74C->hd;_tmp74E=*_tmp74D;_tmp74F=_tmp74E.f2;
# 3240
Cyc_Toc_exp_to_c(nv,_tmp74F);}}
# 3242
goto _LL232;_LL26B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp570=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp570->tag != 26)goto _LL26D;else{_tmp571=_tmp570->f1;_tmp572=_tmp570->f2;_tmp573=_tmp570->f3;_tmp574=_tmp570->f4;}}_LL26C: {
# 3246
unsigned int _tmp751;int _tmp752;struct _tuple11 _tmp750=Cyc_Evexp_eval_const_uint_exp(_tmp572);_tmp751=_tmp750.f1;_tmp752=_tmp750.f2;{
# 3247
void*_tmp753=Cyc_Toc_typ_to_c((void*)_check_null(_tmp573->topt));
# 3248
Cyc_Toc_exp_to_c(nv,_tmp573);{
# 3249
struct Cyc_List_List*es=0;
# 3251
if(!Cyc_Toc_is_zero(_tmp573)){
# 3252
if(!_tmp752){
# 3253
const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,Cyc_Tcutil_terr(_tmp572->loc,((_tmpD0C="cannot determine value of constant",_tag_dyneither(_tmpD0C,sizeof(char),35))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}
# 3254
{unsigned int i=0;for(0;i < _tmp751;++ i){
# 3255
struct _tuple16*_tmpD0F;struct Cyc_List_List*_tmpD0E;es=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->f1=0,((_tmpD0F->f2=_tmp573,_tmpD0F)))))),((_tmpD0E->tl=es,_tmpD0E))))));}}
# 3257
if(_tmp574){
# 3258
struct Cyc_Absyn_Exp*_tmp758=Cyc_Toc_cast_it(_tmp753,Cyc_Absyn_uint_exp(0,0));
# 3259
struct _tuple16*_tmpD12;struct Cyc_List_List*_tmpD11;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11->hd=((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->f1=0,((_tmpD12->f2=_tmp758,_tmpD12)))))),((_tmpD11->tl=0,_tmpD11)))))));}}
# 3262
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
# 3263
goto _LL232;};};}_LL26D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp575=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp575->tag != 27)goto _LL26F;else{_tmp576=_tmp575->f1;_tmp577=_tmp575->f2;_tmp578=_tmp575->f3;_tmp579=_tmp575->f4;}}_LL26E:
# 3267
 if(!Cyc_Toc_is_toplevel(nv))
# 3268
e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp577,0,0,_tmp578,_tmp576))->r;else{
# 3275
if(_tmp579 == 0){
# 3276
const char*_tmpD15;void*_tmpD14;(_tmpD14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD15="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD15,sizeof(char),38))),_tag_dyneither(_tmpD14,sizeof(void*),0)));}{
# 3277
struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp579;
# 3279
struct _RegionHandle _tmp75D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp75D;_push_region(rgn);
# 3280
{struct Cyc_List_List*_tmp75E=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp578,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3283
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp579->impl))->tagged){
# 3285
struct _tuple22 _tmp760;struct Cyc_Absyn_Aggrfield*_tmp761;struct Cyc_Absyn_Exp*_tmp762;struct _tuple22*_tmp75F=(struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp75E))->hd;_tmp760=*_tmp75F;_tmp761=_tmp760.f1;_tmp762=_tmp760.f2;{
# 3286
void*_tmp763=_tmp761->type;
# 3287
Cyc_Toc_exp_to_c(nv,_tmp762);
# 3288
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp763))
# 3289
_tmp762->r=
# 3290
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp762->r,0));{
# 3292
int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)_tmp579,_tmp761->name);
# 3293
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
# 3294
struct _tuple16*_tmpD1A;struct _tuple16*_tmpD19;struct _tuple16*_tmpD18[2];struct Cyc_List_List*_tmp764=(_tmpD18[1]=((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->f1=0,((_tmpD19->f2=_tmp762,_tmpD19)))))),((_tmpD18[0]=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->f1=0,((_tmpD1A->f2=field_tag_exp,_tmpD1A)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD18,sizeof(struct _tuple16*),2)))));
# 3295
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp764,0);
# 3296
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD20;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD1F;void*_tmpD1E[1];struct Cyc_List_List*ds=(_tmpD1E[0]=(void*)((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD1F.tag=1,((_tmpD1F.f1=_tmp761->name,_tmpD1F)))),_tmpD20)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1E,sizeof(void*),1)));
# 3297
struct _tuple16*_tmpD23;struct _tuple16*_tmpD22[1];struct Cyc_List_List*dles=(_tmpD22[0]=((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->f1=ds,((_tmpD23->f2=umem,_tmpD23)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD22,sizeof(struct _tuple16*),1)));
# 3298
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3301
struct Cyc_List_List*_tmp76D=0;
# 3302
struct Cyc_List_List*_tmp76E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
# 3303
for(0;_tmp76E != 0;_tmp76E=_tmp76E->tl){
# 3304
struct Cyc_List_List*_tmp76F=_tmp75E;for(0;_tmp76F != 0;_tmp76F=_tmp76F->tl){
# 3305
if((*((struct _tuple22*)_tmp76F->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp76E->hd){
# 3306
struct _tuple22 _tmp771;struct Cyc_Absyn_Aggrfield*_tmp772;struct Cyc_Absyn_Exp*_tmp773;struct _tuple22*_tmp770=(struct _tuple22*)_tmp76F->hd;_tmp771=*_tmp770;_tmp772=_tmp771.f1;_tmp773=_tmp771.f2;{
# 3307
void*_tmp774=_tmp772->type;
# 3308
Cyc_Toc_exp_to_c(nv,_tmp773);
# 3309
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp774))
# 3310
_tmp773->r=
# 3311
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp773->r,0));
# 3312
{struct _tuple16*_tmpD26;struct Cyc_List_List*_tmpD25;_tmp76D=((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->hd=((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26->f1=0,((_tmpD26->f2=_tmp773,_tmpD26)))))),((_tmpD25->tl=_tmp76D,_tmpD25))))));}
# 3313
break;};}}}
# 3316
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp76D));}}
# 3280
;_pop_region(rgn);};}
# 3320
goto _LL232;_LL26F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp57A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp57A->tag != 28)goto _LL271;else{_tmp57B=(void*)_tmp57A->f1;_tmp57C=_tmp57A->f2;}}_LL270: {
# 3322
struct Cyc_List_List*fs;
# 3323
{void*_tmp777=Cyc_Tcutil_compress(_tmp57B);struct Cyc_List_List*_tmp779;_LL335: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp778=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp777;if(_tmp778->tag != 12)goto _LL337;else{_tmp779=_tmp778->f2;}}_LL336:
# 3324
 fs=_tmp779;goto _LL334;_LL337:;_LL338: {
# 3325
const char*_tmpD2A;void*_tmpD29[1];struct Cyc_String_pa_PrintArg_struct _tmpD28;(_tmpD28.tag=0,((_tmpD28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp57B)),((_tmpD29[0]=& _tmpD28,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2A="anon struct has type %s",_tag_dyneither(_tmpD2A,sizeof(char),24))),_tag_dyneither(_tmpD29,sizeof(void*),1)))))));}_LL334:;}{
# 3327
struct _RegionHandle _tmp77D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp77D;_push_region(rgn);{
# 3328
struct Cyc_List_List*_tmp77E=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp57C,Cyc_Absyn_StructA,fs);
# 3329
for(0;_tmp77E != 0;_tmp77E=_tmp77E->tl){
# 3330
struct _tuple22 _tmp780;struct Cyc_Absyn_Aggrfield*_tmp781;struct Cyc_Absyn_Exp*_tmp782;struct _tuple22*_tmp77F=(struct _tuple22*)_tmp77E->hd;_tmp780=*_tmp77F;_tmp781=_tmp780.f1;_tmp782=_tmp780.f2;{
# 3331
void*_tmp783=_tmp781->type;
# 3332
Cyc_Toc_exp_to_c(nv,_tmp782);
# 3333
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp783))
# 3334
_tmp782->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp782->r,0));};}
# 3339
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp57C);}
# 3341
_npop_handler(0);goto _LL232;
# 3327
;_pop_region(rgn);};}_LL271: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp57D=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp57D->tag != 29)goto _LL273;else{_tmp57E=_tmp57D->f1;_tmp57F=_tmp57D->f2;_tmp580=_tmp57D->f3;}}_LL272: {
# 3344
void*datatype_ctype;
# 3345
struct Cyc_Absyn_Exp*tag_exp;
# 3346
struct _tuple0*_tmp784=Cyc_Toc_temp_var();
# 3347
struct Cyc_Absyn_Exp*_tmp785=Cyc_Absyn_var_exp(_tmp784,0);
# 3348
struct Cyc_Absyn_Exp*member_exp;
# 3349
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp580->name,_tmp57F->name));
# 3350
tag_exp=_tmp57F->is_extensible?Cyc_Absyn_var_exp(_tmp580->name,0):
# 3351
 Cyc_Toc_datatype_tag(_tmp57F,_tmp580->name);
# 3352
member_exp=_tmp785;{
# 3353
struct Cyc_List_List*_tmp786=_tmp580->typs;
# 3355
if(Cyc_Toc_is_toplevel(nv)){
# 3357
struct Cyc_List_List*dles=0;
# 3358
for(0;_tmp57E != 0;(_tmp57E=_tmp57E->tl,_tmp786=_tmp786->tl)){
# 3359
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp57E->hd;
# 3360
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp786))->hd)).f2);
# 3361
Cyc_Toc_exp_to_c(nv,cur_e);
# 3362
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
# 3363
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
# 3364
struct _tuple16*_tmpD2D;struct Cyc_List_List*_tmpD2C;dles=((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->hd=((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D->f1=0,((_tmpD2D->f2=cur_e,_tmpD2D)))))),((_tmpD2C->tl=dles,_tmpD2C))))));};}
# 3366
{struct _tuple16*_tmpD30;struct Cyc_List_List*_tmpD2F;dles=((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->hd=((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->f1=0,((_tmpD30->f2=tag_exp,_tmpD30)))))),((_tmpD2F->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD2F))))));}
# 3367
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3372
struct Cyc_List_List*_tmpD31;struct Cyc_List_List*_tmp78B=
# 3373
(_tmpD31=_cycalloc(sizeof(*_tmpD31)),((_tmpD31->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD31->tl=0,_tmpD31)))));
# 3375
{int i=1;for(0;_tmp57E != 0;(((_tmp57E=_tmp57E->tl,i ++)),_tmp786=_tmp786->tl)){
# 3376
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp57E->hd;
# 3377
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp786))->hd)).f2);
# 3378
Cyc_Toc_exp_to_c(nv,cur_e);
# 3379
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
# 3380
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
# 3381
struct Cyc_Absyn_Stmt*_tmp78C=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3383
struct Cyc_List_List*_tmpD32;_tmp78B=((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32->hd=_tmp78C,((_tmpD32->tl=_tmp78B,_tmpD32))))));};}}{
# 3385
struct Cyc_Absyn_Stmt*_tmp78E=Cyc_Absyn_exp_stmt(_tmp785,0);
# 3386
struct Cyc_List_List*_tmpD33;struct Cyc_Absyn_Stmt*_tmp78F=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->hd=_tmp78E,((_tmpD33->tl=_tmp78B,_tmpD33))))))),0);
# 3387
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp784,datatype_ctype,0,_tmp78F,0));};}
# 3389
goto _LL232;};}_LL273: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp581=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp581->tag != 30)goto _LL275;}_LL274:
# 3391
 goto _LL276;_LL275: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp582=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp582->tag != 31)goto _LL277;}_LL276:
# 3392
 goto _LL232;_LL277: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp583=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp583->tag != 32)goto _LL279;else{_tmp584=_tmp583->f1;_tmp585=_tmp584.is_calloc;_tmp586=_tmp584.rgn;_tmp587=_tmp584.elt_type;_tmp588=_tmp584.num_elts;_tmp589=_tmp584.fat_result;}}_LL278: {
# 3395
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp587)));
# 3396
Cyc_Toc_exp_to_c(nv,_tmp588);
# 3399
if(_tmp589){
# 3400
struct _tuple0*_tmp792=Cyc_Toc_temp_var();
# 3401
struct _tuple0*_tmp793=Cyc_Toc_temp_var();
# 3402
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
# 3403
if(_tmp585){
# 3404
xexp=_tmp588;
# 3405
if(_tmp586 != 0  && !Cyc_Absyn_no_regions){
# 3406
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp586;
# 3407
Cyc_Toc_exp_to_c(nv,rgn);
# 3408
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp792,0));}else{
# 3410
pexp=Cyc_Toc_calloc_exp(*_tmp587,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp792,0));}{
# 3412
struct Cyc_Absyn_Exp*_tmpD34[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
# 3413
(_tmpD34[2]=
# 3414
Cyc_Absyn_var_exp(_tmp792,0),((_tmpD34[1]=
# 3413
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD34[0]=Cyc_Absyn_var_exp(_tmp793,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD34,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3416
if(_tmp586 != 0  && !Cyc_Absyn_no_regions){
# 3417
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp586;
# 3418
Cyc_Toc_exp_to_c(nv,rgn);
# 3419
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp792,0));}else{
# 3421
pexp=Cyc_Toc_malloc_exp(*_tmp587,Cyc_Absyn_var_exp(_tmp792,0));}{
# 3423
struct Cyc_Absyn_Exp*_tmpD35[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD35[2]=
# 3424
Cyc_Absyn_var_exp(_tmp792,0),((_tmpD35[1]=
# 3423
Cyc_Absyn_uint_exp(1,0),((_tmpD35[0]=Cyc_Absyn_var_exp(_tmp793,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD35,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3426
struct Cyc_Absyn_Stmt*_tmp796=Cyc_Absyn_declare_stmt(_tmp792,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)_tmp588,
# 3427
Cyc_Absyn_declare_stmt(_tmp793,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,
# 3428
Cyc_Absyn_exp_stmt(rexp,0),0),0);
# 3429
e->r=Cyc_Toc_stmt_exp_r(_tmp796);};}else{
# 3431
if(_tmp585){
# 3432
if(_tmp586 != 0  && !Cyc_Absyn_no_regions){
# 3433
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp586;
# 3434
Cyc_Toc_exp_to_c(nv,rgn);
# 3435
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp588))->r;}else{
# 3437
e->r=(Cyc_Toc_calloc_exp(*_tmp587,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp588))->r;}}else{
# 3440
if(_tmp586 != 0  && !Cyc_Absyn_no_regions){
# 3441
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp586;
# 3442
Cyc_Toc_exp_to_c(nv,rgn);
# 3443
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp588))->r;}else{
# 3445
e->r=(Cyc_Toc_malloc_exp(*_tmp587,_tmp588))->r;}}}
# 3449
goto _LL232;}_LL279: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp58A=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp58A->tag != 33)goto _LL27B;else{_tmp58B=_tmp58A->f1;_tmp58C=_tmp58A->f2;}}_LL27A: {
# 3458
int is_dyneither_ptr=0;
# 3459
void*e1_old_typ=(void*)_check_null(_tmp58B->topt);
# 3460
void*e2_old_typ=(void*)_check_null(_tmp58C->topt);
# 3461
if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,& is_dyneither_ptr)){
# 3462
const char*_tmpD38;void*_tmpD37;(_tmpD37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD38="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD38,sizeof(char),57))),_tag_dyneither(_tmpD37,sizeof(void*),0)));}{
# 3465
struct Cyc_Absyn_Exp*swap_fn;
# 3466
if(is_dyneither_ptr)
# 3467
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3469
swap_fn=Cyc_Toc__swap_word_e;}{
# 3473
int f1_tag=0;
# 3474
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
# 3475
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp58B,& f1_tag,& tagged_mem_type1,1);
# 3476
int f2_tag=0;
# 3477
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
# 3478
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp58C,& f2_tag,& tagged_mem_type2,1);
# 3480
Cyc_Toc_set_lhs(nv,1);
# 3481
Cyc_Toc_exp_to_c(nv,_tmp58B);
# 3482
Cyc_Toc_exp_to_c(nv,_tmp58C);
# 3483
Cyc_Toc_set_lhs(nv,0);
# 3486
if(e1_tmem)
# 3487
Cyc_Toc_add_tagged_union_check_for_swap(_tmp58B,f1_tag,tagged_mem_type1);else{
# 3490
_tmp58B=Cyc_Toc_push_address_exp(_tmp58B);}
# 3492
if(e2_tmem)
# 3493
Cyc_Toc_add_tagged_union_check_for_swap(_tmp58C,f2_tag,tagged_mem_type2);else{
# 3496
_tmp58C=Cyc_Toc_push_address_exp(_tmp58C);}
# 3498
{struct Cyc_Absyn_Exp*_tmpD39[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD39[1]=_tmp58C,((_tmpD39[0]=_tmp58B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD39,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3500
goto _LL232;};};}_LL27B: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp58D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp58D->tag != 36)goto _LL27D;else{_tmp58E=_tmp58D->f1;_tmp58F=_tmp58D->f2;}}_LL27C: {
# 3503
void*_tmp79A=Cyc_Tcutil_compress((void*)_check_null(_tmp58E->topt));
# 3504
Cyc_Toc_exp_to_c(nv,_tmp58E);
# 3505
{void*_tmp79B=_tmp79A;struct Cyc_Absyn_AggrInfo _tmp79D;union Cyc_Absyn_AggrInfoU _tmp79E;struct Cyc_Absyn_Aggrdecl**_tmp79F;struct Cyc_Absyn_Aggrdecl*_tmp7A0;_LL33A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp79C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp79B;if(_tmp79C->tag != 11)goto _LL33C;else{_tmp79D=_tmp79C->f1;_tmp79E=_tmp79D.aggr_info;if((_tmp79E.KnownAggr).tag != 2)goto _LL33C;_tmp79F=(struct Cyc_Absyn_Aggrdecl**)(_tmp79E.KnownAggr).val;_tmp7A0=*_tmp79F;}}_LL33B: {
# 3507
struct Cyc_Absyn_Exp*_tmp7A1=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7A0,_tmp58F),0);
# 3508
struct Cyc_Absyn_Exp*_tmp7A2=Cyc_Absyn_aggrmember_exp(_tmp58E,_tmp58F,0);
# 3509
struct Cyc_Absyn_Exp*_tmp7A3=Cyc_Absyn_aggrmember_exp(_tmp7A2,Cyc_Toc_tag_sp,0);
# 3510
e->r=(Cyc_Absyn_eq_exp(_tmp7A3,_tmp7A1,0))->r;
# 3511
goto _LL339;}_LL33C:;_LL33D: {
# 3512
const char*_tmpD3C;void*_tmpD3B;(_tmpD3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3C="non-aggregate type in tagcheck",_tag_dyneither(_tmpD3C,sizeof(char),31))),_tag_dyneither(_tmpD3B,sizeof(void*),0)));}_LL339:;}
# 3514
goto _LL232;}_LL27D: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp590=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp590->tag != 35)goto _LL27F;else{_tmp591=_tmp590->f1;}}_LL27E:
# 3515
 Cyc_Toc_stmt_to_c(nv,_tmp591);goto _LL232;_LL27F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp592=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp592->tag != 34)goto _LL281;}_LL280: {
# 3516
const char*_tmpD3F;void*_tmpD3E;(_tmpD3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3F="UnresolvedMem",_tag_dyneither(_tmpD3F,sizeof(char),14))),_tag_dyneither(_tmpD3E,sizeof(void*),0)));}_LL281: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp593=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp593->tag != 24)goto _LL283;}_LL282: {
# 3517
const char*_tmpD42;void*_tmpD41;(_tmpD41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD42="compoundlit",_tag_dyneither(_tmpD42,sizeof(char),12))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}_LL283: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp594=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp594->tag != 37)goto _LL285;}_LL284: {
# 3518
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD45="valueof(-)",_tag_dyneither(_tmpD45,sizeof(char),11))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}_LL285: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp595=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp512;if(_tmp595->tag != 38)goto _LL232;}_LL286: {
# 3519
const char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD48="__asm__",_tag_dyneither(_tmpD48,sizeof(char),8))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}_LL232:;};}
# 3551 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
# 3552
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3554
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple23{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple24{struct _tuple0*f1;void*f2;};struct _tuple25{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3557
static struct _tuple23 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3562
struct Cyc_Absyn_Stmt*s;
# 3563
{void*_tmp7AE=p->r;struct Cyc_Absyn_Vardecl*_tmp7B0;struct Cyc_Absyn_Vardecl _tmp7B1;struct _tuple0*_tmp7B2;struct Cyc_Absyn_Pat*_tmp7B3;struct Cyc_Absyn_Vardecl*_tmp7B5;struct Cyc_Absyn_Vardecl _tmp7B6;struct _tuple0*_tmp7B7;struct Cyc_Absyn_Vardecl*_tmp7BA;struct Cyc_Absyn_Pat*_tmp7BB;enum Cyc_Absyn_Sign _tmp7BE;int _tmp7BF;char _tmp7C1;struct _dyneither_ptr _tmp7C3;int _tmp7C4;struct Cyc_Absyn_Enumdecl*_tmp7C6;struct Cyc_Absyn_Enumfield*_tmp7C7;void*_tmp7C9;struct Cyc_Absyn_Enumfield*_tmp7CA;struct Cyc_Absyn_Pat*_tmp7CC;struct Cyc_Absyn_Pat _tmp7CD;void*_tmp7CE;struct Cyc_Absyn_Datatypedecl*_tmp7D0;struct Cyc_Absyn_Datatypefield*_tmp7D1;struct Cyc_List_List*_tmp7D2;struct Cyc_List_List*_tmp7D4;struct Cyc_List_List*_tmp7D6;struct Cyc_Absyn_AggrInfo*_tmp7D8;struct Cyc_Absyn_AggrInfo*_tmp7DA;struct Cyc_Absyn_AggrInfo _tmp7DB;union Cyc_Absyn_AggrInfoU _tmp7DC;struct Cyc_List_List*_tmp7DD;struct Cyc_Absyn_Pat*_tmp7DF;_LL33F: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7AF=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7AF->tag != 1)goto _LL341;else{_tmp7B0=_tmp7AF->f1;_tmp7B1=*_tmp7B0;_tmp7B2=_tmp7B1.name;_tmp7B3=_tmp7AF->f2;}}_LL340:
# 3566
 return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp7B2,r),rgn,t,r,path,_tmp7B3,fail_stmt,decls);_LL341: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp7B4=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7B4->tag != 3)goto _LL343;else{_tmp7B5=_tmp7B4->f2;_tmp7B6=*_tmp7B5;_tmp7B7=_tmp7B6.name;}}_LL342:
# 3570
 nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7B7,r);
# 3571
goto _LL344;_LL343: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp7B8=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7B8->tag != 0)goto _LL345;}_LL344:
# 3572
 s=Cyc_Toc_skip_stmt_dl();goto _LL33E;_LL345: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp7B9=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7B9->tag != 2)goto _LL347;else{_tmp7BA=_tmp7B9->f1;_tmp7BB=_tmp7B9->f2;}}_LL346: {
# 3575
struct _tuple0*_tmp7E3=Cyc_Toc_temp_var();
# 3576
{struct _tuple24*_tmpD4B;struct Cyc_List_List*_tmpD4A;decls=((_tmpD4A=_region_malloc(rgn,sizeof(*_tmpD4A)),((_tmpD4A->hd=((_tmpD4B=_region_malloc(rgn,sizeof(*_tmpD4B)),((_tmpD4B->f1=_tmp7E3,((_tmpD4B->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpD4B)))))),((_tmpD4A->tl=decls,_tmpD4A))))));}
# 3577
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp7BA->name,Cyc_Absyn_var_exp(_tmp7E3,0));
# 3579
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7E3,0),
# 3580
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
# 3581
Cyc_Toc_push_address_exp(path)),0);{
# 3582
struct _tuple23 _tmp7E6=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp7BB,fail_stmt,decls);
# 3583
_tmp7E6.f3=Cyc_Absyn_seq_stmt(s,_tmp7E6.f3,0);
# 3584
return _tmp7E6;};}_LL347: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp7BC=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7BC->tag != 8)goto _LL349;}_LL348:
# 3585
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL33E;_LL349: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp7BD=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7BD->tag != 9)goto _LL34B;else{_tmp7BE=_tmp7BD->f1;_tmp7BF=_tmp7BD->f2;}}_LL34A:
# 3586
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp7BE,_tmp7BF,0),fail_stmt);goto _LL33E;_LL34B: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp7C0=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7C0->tag != 10)goto _LL34D;else{_tmp7C1=_tmp7C0->f1;}}_LL34C:
# 3587
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp7C1,0),fail_stmt);goto _LL33E;_LL34D: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp7C2=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7C2->tag != 11)goto _LL34F;else{_tmp7C3=_tmp7C2->f1;_tmp7C4=_tmp7C2->f2;}}_LL34E:
# 3588
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp7C3,_tmp7C4,0),fail_stmt);goto _LL33E;_LL34F: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp7C5=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7C5->tag != 12)goto _LL351;else{_tmp7C6=_tmp7C5->f1;_tmp7C7=_tmp7C5->f2;}}_LL350:
# 3591
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD4E;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD4D;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D[0]=((_tmpD4E.tag=30,((_tmpD4E.f1=_tmp7C7->name,((_tmpD4E.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7C6,((_tmpD4E.f3=(struct Cyc_Absyn_Enumfield*)_tmp7C7,_tmpD4E)))))))),_tmpD4D)))),0),fail_stmt);}
# 3592
goto _LL33E;_LL351: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7C8=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7C8->tag != 13)goto _LL353;else{_tmp7C9=(void*)_tmp7C8->f1;_tmp7CA=_tmp7C8->f2;}}_LL352:
# 3594
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD51;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD50;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD51.tag=31,((_tmpD51.f1=_tmp7CA->name,((_tmpD51.f2=(void*)_tmp7C9,((_tmpD51.f3=(struct Cyc_Absyn_Enumfield*)_tmp7CA,_tmpD51)))))))),_tmpD50)))),0),fail_stmt);}
# 3595
goto _LL33E;_LL353: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7CB=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7CB->tag != 5)goto _LL355;else{_tmp7CC=_tmp7CB->f1;_tmp7CD=*_tmp7CC;_tmp7CE=_tmp7CD.r;{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7CF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp7CE;if(_tmp7CF->tag != 7)goto _LL355;else{_tmp7D0=_tmp7CF->f1;_tmp7D1=_tmp7CF->f2;_tmp7D2=_tmp7CF->f3;}};}}_LL354:
# 3604
 s=Cyc_Toc_skip_stmt_dl();{
# 3605
struct _tuple0*_tmp7EB=Cyc_Toc_temp_var();
# 3606
struct _tuple0*tufstrct=Cyc_Toc_collapse_qvars(_tmp7D1->name,_tmp7D0->name);
# 3607
void*_tmp7EC=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
# 3608
int cnt=1;
# 3609
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7EC,r);
# 3610
struct Cyc_List_List*_tmp7ED=_tmp7D1->typs;
# 3611
for(0;_tmp7D2 != 0;(((_tmp7D2=_tmp7D2->tl,_tmp7ED=((struct Cyc_List_List*)_check_null(_tmp7ED))->tl)),++ cnt)){
# 3612
struct Cyc_Absyn_Pat*_tmp7EE=(struct Cyc_Absyn_Pat*)_tmp7D2->hd;
# 3613
if(_tmp7EE->r == (void*)& Cyc_Absyn_Wild_p_val)
# 3614
continue;{
# 3615
void*_tmp7EF=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp7ED))->hd)).f2;
# 3616
struct _tuple0*_tmp7F0=Cyc_Toc_temp_var();
# 3617
void*_tmp7F1=(void*)_check_null(_tmp7EE->topt);
# 3618
void*_tmp7F2=Cyc_Toc_typ_to_c(_tmp7F1);
# 3619
struct Cyc_Absyn_Exp*_tmp7F3=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7EB,0),Cyc_Absyn_fieldname(cnt),0);
# 3620
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7EF)))
# 3621
_tmp7F3=Cyc_Toc_cast_it(_tmp7F2,_tmp7F3);
# 3622
{struct _tuple24*_tmpD54;struct Cyc_List_List*_tmpD53;decls=((_tmpD53=_region_malloc(rgn,sizeof(*_tmpD53)),((_tmpD53->hd=((_tmpD54=_region_malloc(rgn,sizeof(*_tmpD54)),((_tmpD54->f1=_tmp7F0,((_tmpD54->f2=_tmp7F2,_tmpD54)))))),((_tmpD53->tl=decls,_tmpD53))))));}{
# 3623
struct _tuple23 _tmp7F6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F1,Cyc_Absyn_var_exp(_tmp7F0,0),_tmp7F3,_tmp7EE,fail_stmt,decls);
# 3625
nv=_tmp7F6.f1;
# 3626
decls=_tmp7F6.f2;{
# 3627
struct Cyc_Absyn_Stmt*_tmp7F7=_tmp7F6.f3;
# 3628
struct Cyc_Absyn_Stmt*_tmp7F8=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7F0,0),_tmp7F3,0);
# 3629
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7F8,_tmp7F7,0),0);};};};}{
# 3631
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7EB,0),Cyc_Toc_tag_sp,0);
# 3632
struct Cyc_Absyn_Exp*tag_exp=
# 3633
_tmp7D0->is_extensible?Cyc_Absyn_var_exp(_tmp7D1->name,0): Cyc_Toc_datatype_tag(_tmp7D0,_tmp7D1->name);
# 3634
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
# 3635
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
# 3636
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3639
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7EB,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3642
s=Cyc_Absyn_declare_stmt(_tmp7EB,_tmp7EC,(struct Cyc_Absyn_Exp*)rcast,s,0);
# 3643
goto _LL33E;};};_LL355: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7D3=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7D3->tag != 7)goto _LL357;else{_tmp7D4=_tmp7D3->f3;}}_LL356:
# 3645
 _tmp7D6=_tmp7D4;goto _LL358;_LL357: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7D5=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7D5->tag != 4)goto _LL359;else{_tmp7D6=_tmp7D5->f1;}}_LL358:
# 3653
 s=Cyc_Toc_skip_stmt_dl();{
# 3654
int cnt=1;
# 3655
for(0;_tmp7D6 != 0;(_tmp7D6=_tmp7D6->tl,++ cnt)){
# 3656
struct Cyc_Absyn_Pat*_tmp7F9=(struct Cyc_Absyn_Pat*)_tmp7D6->hd;
# 3657
if(_tmp7F9->r == (void*)& Cyc_Absyn_Wild_p_val)
# 3658
continue;{
# 3659
struct _tuple0*_tmp7FA=Cyc_Toc_temp_var();
# 3660
void*_tmp7FB=(void*)_check_null(_tmp7F9->topt);
# 3661
{struct _tuple24*_tmpD57;struct Cyc_List_List*_tmpD56;decls=((_tmpD56=_region_malloc(rgn,sizeof(*_tmpD56)),((_tmpD56->hd=((_tmpD57=_region_malloc(rgn,sizeof(*_tmpD57)),((_tmpD57->f1=_tmp7FA,((_tmpD57->f2=Cyc_Toc_typ_to_c(_tmp7FB),_tmpD57)))))),((_tmpD56->tl=decls,_tmpD56))))));}{
# 3662
struct _tuple23 _tmp7FE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FB,Cyc_Absyn_var_exp(_tmp7FA,0),
# 3663
Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp7F9,fail_stmt,decls);
# 3665
nv=_tmp7FE.f1;
# 3666
decls=_tmp7FE.f2;{
# 3667
struct Cyc_Absyn_Stmt*_tmp7FF=_tmp7FE.f3;
# 3668
struct Cyc_Absyn_Stmt*_tmp800=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7FA,0),
# 3669
Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);
# 3670
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp800,_tmp7FF,0),0);};};};}
# 3672
goto _LL33E;};_LL359: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7D7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7D7->tag != 6)goto _LL35B;else{_tmp7D8=_tmp7D7->f1;if(_tmp7D8 != 0)goto _LL35B;}}_LL35A: {
# 3674
const char*_tmpD5A;void*_tmpD59;(_tmpD59=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5A="unresolved aggregate pattern!",_tag_dyneither(_tmpD5A,sizeof(char),30))),_tag_dyneither(_tmpD59,sizeof(void*),0)));}_LL35B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7D9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7D9->tag != 6)goto _LL35D;else{_tmp7DA=_tmp7D9->f1;if(_tmp7DA == 0)goto _LL35D;_tmp7DB=*_tmp7DA;_tmp7DC=_tmp7DB.aggr_info;_tmp7DD=_tmp7D9->f3;}}_LL35C: {
# 3676
struct Cyc_Absyn_Aggrdecl*_tmp803=Cyc_Absyn_get_known_aggrdecl(_tmp7DC);
# 3677
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp803->impl))->tagged){
# 3680
struct _tuple25 _tmp805;struct Cyc_List_List*_tmp806;struct Cyc_Absyn_Pat*_tmp807;struct _tuple25*_tmp804=(struct _tuple25*)((struct Cyc_List_List*)_check_null(_tmp7DD))->hd;_tmp805=*_tmp804;_tmp806=_tmp805.f1;_tmp807=_tmp805.f2;{
# 3681
struct _dyneither_ptr*f;
# 3682
{void*_tmp808=(void*)((struct Cyc_List_List*)_check_null(_tmp806))->hd;struct _dyneither_ptr*_tmp80A;_LL366: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp809=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp808;if(_tmp809->tag != 1)goto _LL368;else{_tmp80A=_tmp809->f1;}}_LL367:
# 3683
 f=_tmp80A;goto _LL365;_LL368:;_LL369: {
# 3684
const char*_tmpD5D;void*_tmpD5C;(_tmpD5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5D="no field name in tagged union pattern",_tag_dyneither(_tmpD5D,sizeof(char),38))),_tag_dyneither(_tmpD5C,sizeof(void*),0)));}_LL365:;}{
# 3686
struct _tuple0*_tmp80D=Cyc_Toc_temp_var();
# 3687
void*_tmp80E=(void*)_check_null(_tmp807->topt);
# 3688
void*_tmp80F=Cyc_Toc_typ_to_c(_tmp80E);
# 3689
{struct _tuple24*_tmpD60;struct Cyc_List_List*_tmpD5F;decls=((_tmpD5F=_region_malloc(rgn,sizeof(*_tmpD5F)),((_tmpD5F->hd=((_tmpD60=_region_malloc(rgn,sizeof(*_tmpD60)),((_tmpD60->f1=_tmp80D,((_tmpD60->f2=_tmp80F,_tmpD60)))))),((_tmpD5F->tl=decls,_tmpD5F))))));}{
# 3690
struct Cyc_Absyn_Exp*_tmp812=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);
# 3691
struct Cyc_Absyn_Exp*_tmp813=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
# 3692
_tmp813=Cyc_Toc_cast_it(_tmp80F,_tmp813);
# 3693
_tmp812=Cyc_Toc_cast_it(_tmp80F,_tmp812);{
# 3694
struct _tuple23 _tmp814=Cyc_Toc_xlate_pat(nv,rgn,_tmp80E,Cyc_Absyn_var_exp(_tmp80D,0),_tmp812,_tmp807,fail_stmt,decls);
# 3695
nv=_tmp814.f1;
# 3696
decls=_tmp814.f2;{
# 3697
struct Cyc_Absyn_Stmt*_tmp815=_tmp814.f3;
# 3698
struct Cyc_Absyn_Stmt*_tmp816=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),
# 3699
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp803,f),0),fail_stmt);
# 3701
struct Cyc_Absyn_Stmt*_tmp817=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp80D,0),_tmp813,0);
# 3702
s=Cyc_Absyn_seq_stmt(_tmp816,Cyc_Absyn_seq_stmt(_tmp817,_tmp815,0),0);};};};};};}else{
# 3705
s=Cyc_Toc_skip_stmt_dl();
# 3706
for(0;_tmp7DD != 0;_tmp7DD=_tmp7DD->tl){
# 3707
struct _tuple25*_tmp818=(struct _tuple25*)_tmp7DD->hd;
# 3708
struct Cyc_Absyn_Pat*_tmp819=(*_tmp818).f2;
# 3709
if(_tmp819->r == (void*)& Cyc_Absyn_Wild_p_val)
# 3710
continue;{
# 3711
struct _dyneither_ptr*f;
# 3712
{void*_tmp81A=(void*)((struct Cyc_List_List*)_check_null((*_tmp818).f1))->hd;struct _dyneither_ptr*_tmp81C;_LL36B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp81B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp81A;if(_tmp81B->tag != 1)goto _LL36D;else{_tmp81C=_tmp81B->f1;}}_LL36C:
# 3713
 f=_tmp81C;goto _LL36A;_LL36D:;_LL36E: {
# 3714
struct Cyc_Toc_Match_error_exn_struct _tmpD63;struct Cyc_Toc_Match_error_exn_struct*_tmpD62;(int)_throw((void*)((_tmpD62=_cycalloc_atomic(sizeof(*_tmpD62)),((_tmpD62[0]=((_tmpD63.tag=Cyc_Toc_Match_error,_tmpD63)),_tmpD62)))));}_LL36A:;}{
# 3716
struct _tuple0*_tmp81F=Cyc_Toc_temp_var();
# 3717
void*_tmp820=(void*)_check_null(_tmp819->topt);
# 3718
void*_tmp821=Cyc_Toc_typ_to_c(_tmp820);
# 3719
{struct _tuple24*_tmpD66;struct Cyc_List_List*_tmpD65;decls=((_tmpD65=_region_malloc(rgn,sizeof(*_tmpD65)),((_tmpD65->hd=((_tmpD66=_region_malloc(rgn,sizeof(*_tmpD66)),((_tmpD66->f1=_tmp81F,((_tmpD66->f2=_tmp821,_tmpD66)))))),((_tmpD65->tl=decls,_tmpD65))))));}{
# 3720
struct _tuple23 _tmp824=Cyc_Toc_xlate_pat(nv,rgn,_tmp820,Cyc_Absyn_var_exp(_tmp81F,0),
# 3721
Cyc_Absyn_aggrmember_exp(path,f,0),_tmp819,fail_stmt,decls);
# 3722
nv=_tmp824.f1;
# 3723
decls=_tmp824.f2;{
# 3724
struct Cyc_Absyn_Exp*_tmp825=Cyc_Absyn_aggrmember_exp(r,f,0);
# 3725
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp803->impl))->fields,f)))->type))
# 3726
_tmp825=Cyc_Toc_cast_it(_tmp821,_tmp825);{
# 3727
struct Cyc_Absyn_Stmt*_tmp826=_tmp824.f3;
# 3728
struct Cyc_Absyn_Stmt*_tmp827=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp81F,0),_tmp825,0);
# 3729
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp827,_tmp826,0),0);};};};};};}}
# 3732
goto _LL33E;}_LL35D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7DE=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7DE->tag != 5)goto _LL35F;else{_tmp7DF=_tmp7DE->f1;}}_LL35E: {
# 3736
struct _tuple0*_tmp828=Cyc_Toc_temp_var();
# 3737
void*_tmp829=(void*)_check_null(_tmp7DF->topt);
# 3738
{struct _tuple24*_tmpD69;struct Cyc_List_List*_tmpD68;decls=((_tmpD68=_region_malloc(rgn,sizeof(*_tmpD68)),((_tmpD68->hd=((_tmpD69=_region_malloc(rgn,sizeof(*_tmpD69)),((_tmpD69->f1=_tmp828,((_tmpD69->f2=Cyc_Toc_typ_to_c(_tmp829),_tmpD69)))))),((_tmpD68->tl=decls,_tmpD68))))));}{
# 3739
struct _tuple23 _tmp82C=Cyc_Toc_xlate_pat(nv,rgn,_tmp829,Cyc_Absyn_var_exp(_tmp828,0),Cyc_Absyn_deref_exp(path,0),_tmp7DF,fail_stmt,decls);
# 3741
nv=_tmp82C.f1;
# 3742
decls=_tmp82C.f2;{
# 3743
struct Cyc_Absyn_Stmt*_tmp82D=_tmp82C.f3;
# 3744
struct Cyc_Absyn_Stmt*_tmp82E=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp828,0),Cyc_Absyn_deref_exp(r,0),0),_tmp82D,0);
# 3745
if(Cyc_Toc_is_nullable(t))
# 3746
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 3748
Cyc_Toc_skip_stmt_dl(),0),_tmp82E,0);else{
# 3751
s=_tmp82E;}
# 3752
goto _LL33E;};};}_LL35F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7E0=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7E0->tag != 14)goto _LL361;}_LL360: {
# 3753
const char*_tmpD6C;void*_tmpD6B;(_tmpD6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6C="unknownid pat",_tag_dyneither(_tmpD6C,sizeof(char),14))),_tag_dyneither(_tmpD6B,sizeof(void*),0)));}_LL361: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7E1=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7E1->tag != 15)goto _LL363;}_LL362: {
# 3754
const char*_tmpD6F;void*_tmpD6E;(_tmpD6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6F="unknowncall pat",_tag_dyneither(_tmpD6F,sizeof(char),16))),_tag_dyneither(_tmpD6E,sizeof(void*),0)));}_LL363: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7E2=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp7AE;if(_tmp7E2->tag != 16)goto _LL33E;}_LL364: {
# 3755
const char*_tmpD72;void*_tmpD71;(_tmpD71=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD72="exp pat",_tag_dyneither(_tmpD72,sizeof(char),8))),_tag_dyneither(_tmpD71,sizeof(void*),0)));}_LL33E:;}{
# 3757
struct _tuple23 _tmpD73;return(_tmpD73.f1=nv,((_tmpD73.f2=decls,((_tmpD73.f3=s,_tmpD73)))));};}struct _tuple26{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 3794 "toc.cyc"
static struct _tuple26*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3796
struct _tuple26*_tmpD74;return(_tmpD74=_region_malloc(r,sizeof(*_tmpD74)),((_tmpD74->f1=Cyc_Toc_fresh_label(),((_tmpD74->f2=Cyc_Toc_fresh_label(),((_tmpD74->f3=sc,_tmpD74)))))));}
# 3799
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 3801
Cyc_Toc_exp_to_c(nv,e);{
# 3802
void*_tmp837=(void*)_check_null(e->topt);
# 3806
int leave_as_switch;
# 3807
{void*_tmp838=Cyc_Tcutil_compress(_tmp837);_LL370: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp839=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp838;if(_tmp839->tag != 6)goto _LL372;}_LL371:
# 3808
 goto _LL373;_LL372: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp83A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp838;if(_tmp83A->tag != 13)goto _LL374;}_LL373:
# 3809
 leave_as_switch=1;goto _LL36F;_LL374:;_LL375:
# 3810
 leave_as_switch=0;goto _LL36F;_LL36F:;}
# 3812
{struct Cyc_List_List*_tmp83B=scs;for(0;_tmp83B != 0;_tmp83B=_tmp83B->tl){
# 3813
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp83B->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp83B->hd)->where_clause != 0){
# 3814
leave_as_switch=0;
# 3815
break;}}}
# 3817
if(leave_as_switch){
# 3819
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
# 3820
{struct Cyc_List_List*_tmp83C=scs;for(0;_tmp83C != 0;_tmp83C=_tmp83C->tl){
# 3821
struct Cyc_Absyn_Stmt*_tmp83D=((struct Cyc_Absyn_Switch_clause*)_tmp83C->hd)->body;
# 3822
((struct Cyc_Absyn_Switch_clause*)_tmp83C->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp83D,0);
# 3823
next_l=Cyc_Toc_fresh_label();{
# 3824
struct _RegionHandle _tmp83E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp83E;_push_region(rgn);
# 3825
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp83D);;_pop_region(rgn);};}}
# 3828
return;}{
# 3831
struct _tuple0*v=Cyc_Toc_temp_var();
# 3832
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
# 3833
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
# 3834
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 3836
struct _RegionHandle _tmp83F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp83F;_push_region(rgn);
# 3837
{struct Cyc_Toc_Env*_tmp840=Cyc_Toc_share_env(rgn,nv);
# 3838
struct Cyc_List_List*lscs=
# 3839
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
# 3840
struct Cyc_List_List*test_stmts=0;
# 3841
struct Cyc_List_List*nvs=0;
# 3842
struct Cyc_List_List*decls=0;
# 3847
{struct Cyc_List_List*_tmp841=lscs;for(0;_tmp841 != 0;_tmp841=_tmp841->tl){
# 3848
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple26*)_tmp841->hd)).f3;
# 3849
struct _dyneither_ptr*fail_lab=_tmp841->tl == 0?end_l:(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp841->tl))->hd)).f1;
# 3850
struct Cyc_Toc_Env*_tmp843;struct Cyc_List_List*_tmp844;struct Cyc_Absyn_Stmt*_tmp845;struct _tuple23 _tmp842=Cyc_Toc_xlate_pat(_tmp840,rgn,_tmp837,r,path,sc->pattern,
# 3851
Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp843=_tmp842.f1;_tmp844=_tmp842.f2;_tmp845=_tmp842.f3;
# 3853
if(sc->where_clause != 0){
# 3854
struct Cyc_Absyn_Exp*_tmp846=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
# 3855
Cyc_Toc_exp_to_c(_tmp843,_tmp846);
# 3856
_tmp845=Cyc_Absyn_seq_stmt(_tmp845,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp846,0),
# 3857
Cyc_Absyn_goto_stmt(fail_lab,0),
# 3858
Cyc_Toc_skip_stmt_dl(),0),0);}
# 3860
decls=_tmp844;
# 3861
{struct Cyc_List_List*_tmpD75;nvs=((_tmpD75=_region_malloc(rgn,sizeof(*_tmpD75)),((_tmpD75->hd=_tmp843,((_tmpD75->tl=nvs,_tmpD75))))));}{
# 3862
struct Cyc_List_List*_tmpD76;test_stmts=((_tmpD76=_region_malloc(rgn,sizeof(*_tmpD76)),((_tmpD76->hd=_tmp845,((_tmpD76->tl=test_stmts,_tmpD76))))));};}}
# 3864
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
# 3865
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
# 3866
struct Cyc_List_List*stmts=0;
# 3869
for(0;lscs != 0;
# 3870
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
# 3871
struct _tuple26 _tmp84A;struct _dyneither_ptr*_tmp84B;struct _dyneither_ptr*_tmp84C;struct Cyc_Absyn_Switch_clause*_tmp84D;struct _tuple26*_tmp849=(struct _tuple26*)lscs->hd;_tmp84A=*_tmp849;_tmp84B=_tmp84A.f1;_tmp84C=_tmp84A.f2;_tmp84D=_tmp84A.f3;{
# 3872
struct Cyc_Toc_Env*_tmp84E=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
# 3873
struct Cyc_Absyn_Stmt*s=_tmp84D->body;
# 3874
struct _RegionHandle _tmp84F=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp84F;_push_region(rgn2);
# 3875
if(lscs->tl != 0){
# 3876
struct _tuple26 _tmp851;struct _dyneither_ptr*_tmp852;struct Cyc_Absyn_Switch_clause*_tmp853;struct _tuple26*_tmp850=(struct _tuple26*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp851=*_tmp850;_tmp852=_tmp851.f2;_tmp853=_tmp851.f3;
# 3877
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp84E,end_l,_tmp852,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp853->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);}else{
# 3882
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp84E,end_l),s);}
# 3884
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp84B,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
# 3885
Cyc_Absyn_label_stmt(_tmp84C,s,0),0);{
# 3886
struct Cyc_List_List*_tmpD77;stmts=((_tmpD77=_region_malloc(rgn,sizeof(*_tmpD77)),((_tmpD77->hd=s,((_tmpD77->tl=stmts,_tmpD77))))));};
# 3874
;_pop_region(rgn2);};}{
# 3888
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
# 3889
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 3891
for(decls;decls != 0;decls=decls->tl){
# 3892
struct _tuple24 _tmp856;struct _tuple0*_tmp857;void*_tmp858;struct _tuple24*_tmp855=(struct _tuple24*)decls->hd;_tmp856=*_tmp855;_tmp857=_tmp856.f1;_tmp858=_tmp856.f2;
# 3893
res=Cyc_Absyn_declare_stmt(_tmp857,_tmp858,0,res,0);}
# 3896
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),(struct Cyc_Absyn_Exp*)e,res,0))->r;};};}
# 3837
;_pop_region(rgn);};};}
# 3901
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3902
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3907
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3909
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
# 3910
struct Cyc_List_List*_tmpD78;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
# 3911
(_tmpD78=_cycalloc(sizeof(*_tmpD78)),((_tmpD78->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD78->tl=0,_tmpD78)))))),0),0);}
# 3913
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
# 3914
if(n > 0)
# 3915
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}
# 3918
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3920
while(1){
# 3921
void*_tmp85A=s->r;struct Cyc_Absyn_Exp*_tmp85D;struct Cyc_Absyn_Stmt*_tmp85F;struct Cyc_Absyn_Stmt*_tmp860;struct Cyc_Absyn_Exp*_tmp862;struct Cyc_Absyn_Exp*_tmp864;struct Cyc_Absyn_Stmt*_tmp865;struct Cyc_Absyn_Stmt*_tmp866;struct _tuple9 _tmp868;struct Cyc_Absyn_Exp*_tmp869;struct Cyc_Absyn_Stmt*_tmp86A;struct Cyc_Absyn_Stmt*_tmp86C;struct Cyc_Absyn_Stmt*_tmp86E;struct Cyc_Absyn_Stmt*_tmp870;struct Cyc_Absyn_Exp*_tmp872;struct _tuple9 _tmp873;struct Cyc_Absyn_Exp*_tmp874;struct _tuple9 _tmp875;struct Cyc_Absyn_Exp*_tmp876;struct Cyc_Absyn_Stmt*_tmp877;struct Cyc_Absyn_Exp*_tmp879;struct Cyc_List_List*_tmp87A;struct Cyc_List_List*_tmp87C;struct Cyc_Absyn_Switch_clause**_tmp87D;struct Cyc_Absyn_Decl*_tmp87F;struct Cyc_Absyn_Stmt*_tmp880;struct _dyneither_ptr*_tmp882;struct Cyc_Absyn_Stmt*_tmp883;struct Cyc_Absyn_Stmt*_tmp885;struct _tuple9 _tmp886;struct Cyc_Absyn_Exp*_tmp887;struct Cyc_Absyn_Stmt*_tmp889;struct Cyc_List_List*_tmp88A;struct Cyc_Absyn_Exp*_tmp88C;_LL377: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp85B=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp85B->tag != 0)goto _LL379;}_LL378:
# 3923
 return;_LL379: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp85C=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp85C->tag != 1)goto _LL37B;else{_tmp85D=_tmp85C->f1;}}_LL37A:
# 3925
 Cyc_Toc_exp_to_c(nv,_tmp85D);
# 3926
return;_LL37B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp85E=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp85E->tag != 2)goto _LL37D;else{_tmp85F=_tmp85E->f1;_tmp860=_tmp85E->f2;}}_LL37C:
# 3928
 Cyc_Toc_stmt_to_c(nv,_tmp85F);
# 3929
s=_tmp860;
# 3930
continue;_LL37D: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp861=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp861->tag != 3)goto _LL37F;else{_tmp862=_tmp861->f1;}}_LL37E: {
# 3932
void*topt=0;
# 3933
if(_tmp862 != 0){
# 3934
topt=(void*)Cyc_Toc_typ_to_c((void*)_check_null(_tmp862->topt));
# 3935
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp862);}
# 3938
if(s->try_depth > 0){
# 3939
if(topt != 0){
# 3940
struct _tuple0*_tmp88D=Cyc_Toc_temp_var();
# 3941
struct Cyc_Absyn_Stmt*_tmp88E=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp88D,0),0);
# 3942
s->r=(Cyc_Absyn_declare_stmt(_tmp88D,(void*)topt,_tmp862,
# 3943
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp88E,0),0))->r;}else{
# 3946
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 3948
return;}_LL37F: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp863=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp863->tag != 4)goto _LL381;else{_tmp864=_tmp863->f1;_tmp865=_tmp863->f2;_tmp866=_tmp863->f3;}}_LL380:
# 3950
 Cyc_Toc_exp_to_c(nv,_tmp864);
# 3951
Cyc_Toc_stmt_to_c(nv,_tmp865);
# 3952
s=_tmp866;
# 3953
continue;_LL381: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp867=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp867->tag != 5)goto _LL383;else{_tmp868=_tmp867->f1;_tmp869=_tmp868.f1;_tmp86A=_tmp867->f2;}}_LL382:
# 3955
 Cyc_Toc_exp_to_c(nv,_tmp869);{
# 3956
struct _RegionHandle _tmp88F=_new_region("temp");struct _RegionHandle*temp=& _tmp88F;_push_region(temp);
# 3957
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp86A);
# 3959
_npop_handler(0);return;
# 3956
;_pop_region(temp);};_LL383: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp86B=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp86B->tag != 6)goto _LL385;else{_tmp86C=_tmp86B->f1;}}_LL384: {
# 3961
struct Cyc_Toc_Env _tmp891;struct _dyneither_ptr**_tmp892;struct Cyc_Toc_Env*_tmp890=nv;_tmp891=*_tmp890;_tmp892=_tmp891.break_lab;
# 3962
if(_tmp892 != 0)
# 3963
s->r=Cyc_Toc_goto_stmt_r(*_tmp892,0);{
# 3965
int dest_depth=_tmp86C == 0?0: _tmp86C->try_depth;
# 3966
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
# 3967
return;};}_LL385: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp86D=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp86D->tag != 7)goto _LL387;else{_tmp86E=_tmp86D->f1;}}_LL386: {
# 3969
struct Cyc_Toc_Env _tmp894;struct _dyneither_ptr**_tmp895;struct Cyc_Toc_Env*_tmp893=nv;_tmp894=*_tmp893;_tmp895=_tmp894.continue_lab;
# 3970
if(_tmp895 != 0)
# 3971
s->r=Cyc_Toc_goto_stmt_r(*_tmp895,0);
# 3972
_tmp870=_tmp86E;goto _LL388;}_LL387: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp86F=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp86F->tag != 8)goto _LL389;else{_tmp870=_tmp86F->f2;}}_LL388:
# 3975
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp870))->try_depth,s);
# 3976
return;_LL389: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp871=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp871->tag != 9)goto _LL38B;else{_tmp872=_tmp871->f1;_tmp873=_tmp871->f2;_tmp874=_tmp873.f1;_tmp875=_tmp871->f3;_tmp876=_tmp875.f1;_tmp877=_tmp871->f4;}}_LL38A:
# 3979
 Cyc_Toc_exp_to_c(nv,_tmp872);Cyc_Toc_exp_to_c(nv,_tmp874);Cyc_Toc_exp_to_c(nv,_tmp876);{
# 3980
struct _RegionHandle _tmp896=_new_region("temp");struct _RegionHandle*temp=& _tmp896;_push_region(temp);
# 3981
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp877);
# 3983
_npop_handler(0);return;
# 3980
;_pop_region(temp);};_LL38B: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp878=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp878->tag != 10)goto _LL38D;else{_tmp879=_tmp878->f1;_tmp87A=_tmp878->f2;}}_LL38C:
# 3985
 Cyc_Toc_xlate_switch(nv,s,_tmp879,_tmp87A);
# 3986
return;_LL38D: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp87B=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp87B->tag != 11)goto _LL38F;else{_tmp87C=_tmp87B->f1;_tmp87D=_tmp87B->f2;}}_LL38E: {
# 3988
struct Cyc_Toc_Env _tmp898;struct Cyc_Toc_FallthruInfo*_tmp899;struct Cyc_Toc_Env*_tmp897=nv;_tmp898=*_tmp897;_tmp899=_tmp898.fallthru_info;
# 3989
if(_tmp899 == 0){
# 3990
const char*_tmpD7B;void*_tmpD7A;(_tmpD7A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7B="fallthru in unexpected place",_tag_dyneither(_tmpD7B,sizeof(char),29))),_tag_dyneither(_tmpD7A,sizeof(void*),0)));}{
# 3991
struct _dyneither_ptr*_tmp89D;struct Cyc_List_List*_tmp89E;struct Cyc_Dict_Dict _tmp89F;struct Cyc_Toc_FallthruInfo _tmp89C=*_tmp899;_tmp89D=_tmp89C.label;_tmp89E=_tmp89C.binders;_tmp89F=_tmp89C.next_case_env;{
# 3992
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp89D,0);
# 3994
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp87D)))->body)->try_depth,s2);{
# 3995
struct Cyc_List_List*_tmp8A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp89E);
# 3996
struct Cyc_List_List*_tmp8A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp87C);
# 3997
for(0;_tmp8A0 != 0;(_tmp8A0=_tmp8A0->tl,_tmp8A1=_tmp8A1->tl)){
# 3998
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8A1))->hd);
# 3999
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp89F,(struct _tuple0*)_tmp8A0->hd),(struct Cyc_Absyn_Exp*)_tmp8A1->hd,0),s2,0);}
# 4002
s->r=s2->r;
# 4003
return;};};};}_LL38F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp87E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp87E->tag != 12)goto _LL391;else{_tmp87F=_tmp87E->f1;_tmp880=_tmp87E->f2;}}_LL390:
# 4008
{void*_tmp8A2=_tmp87F->r;struct Cyc_Absyn_Vardecl*_tmp8A4;struct Cyc_Absyn_Pat*_tmp8A6;struct Cyc_Absyn_Exp*_tmp8A7;struct Cyc_List_List*_tmp8A9;struct Cyc_Absyn_Fndecl*_tmp8AB;struct Cyc_Absyn_Tvar*_tmp8AD;struct Cyc_Absyn_Vardecl*_tmp8AE;int _tmp8AF;struct Cyc_Absyn_Exp*_tmp8B0;struct Cyc_Absyn_Exp*_tmp8B2;struct Cyc_Absyn_Tvar*_tmp8B3;struct Cyc_Absyn_Vardecl*_tmp8B4;_LL39A: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8A3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A2;if(_tmp8A3->tag != 0)goto _LL39C;else{_tmp8A4=_tmp8A3->f1;}}_LL39B: {
# 4010
struct _RegionHandle _tmp8B5=_new_region("temp");struct _RegionHandle*temp=& _tmp8B5;_push_region(temp);{
# 4011
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD7E;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD7D;struct Cyc_Toc_Env*_tmp8B6=Cyc_Toc_add_varmap(temp,nv,_tmp8A4->name,
# 4012
Cyc_Absyn_varb_exp(_tmp8A4->name,(void*)((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7E.tag=4,((_tmpD7E.f1=_tmp8A4,_tmpD7E)))),_tmpD7D)))),0));
# 4013
Cyc_Toc_local_decl_to_c(_tmp8B6,_tmp8B6,_tmp8A4,_tmp880);}
# 4015
_npop_handler(0);goto _LL399;
# 4010
;_pop_region(temp);}_LL39C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp8A5=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp8A2;if(_tmp8A5->tag != 2)goto _LL39E;else{_tmp8A6=_tmp8A5->f1;_tmp8A7=_tmp8A5->f3;}}_LL39D:
# 4019
{void*_tmp8B9=_tmp8A6->r;struct Cyc_Absyn_Vardecl*_tmp8BB;struct Cyc_Absyn_Pat*_tmp8BC;struct Cyc_Absyn_Pat _tmp8BD;void*_tmp8BE;_LL3A9: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp8BA=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp8B9;if(_tmp8BA->tag != 1)goto _LL3AB;else{_tmp8BB=_tmp8BA->f1;_tmp8BC=_tmp8BA->f2;_tmp8BD=*_tmp8BC;_tmp8BE=_tmp8BD.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp8BF=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp8BE;if(_tmp8BF->tag != 0)goto _LL3AB;};}}_LL3AA: {
# 4021
struct _tuple0*old_name=_tmp8BB->name;
# 4022
struct _tuple0*new_name=Cyc_Toc_temp_var();
# 4023
_tmp8BB->name=new_name;
# 4024
_tmp8BB->initializer=(struct Cyc_Absyn_Exp*)_tmp8A7;
# 4025
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD81;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD80;_tmp87F->r=(void*)((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD81.tag=0,((_tmpD81.f1=_tmp8BB,_tmpD81)))),_tmpD80))));}{
# 4026
struct _RegionHandle _tmp8C2=_new_region("temp");struct _RegionHandle*temp=& _tmp8C2;_push_region(temp);{
# 4027
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD84;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD83;struct Cyc_Toc_Env*_tmp8C3=
# 4028
Cyc_Toc_add_varmap(temp,nv,old_name,
# 4029
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=4,((_tmpD84.f1=_tmp8BB,_tmpD84)))),_tmpD83)))),0));
# 4030
Cyc_Toc_local_decl_to_c(_tmp8C3,nv,_tmp8BB,_tmp880);}
# 4032
_npop_handler(0);goto _LL3A8;
# 4026
;_pop_region(temp);};}_LL3AB:;_LL3AC:
# 4037
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp8A6,(void*)_check_null(_tmp8A7->topt),_tmp8A7,_tmp880))->r;
# 4038
goto _LL3A8;_LL3A8:;}
# 4040
goto _LL399;_LL39E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp8A8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp8A2;if(_tmp8A8->tag != 3)goto _LL3A0;else{_tmp8A9=_tmp8A8->f1;}}_LL39F: {
# 4053 "toc.cyc"
struct Cyc_List_List*_tmp8C6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp8A9);
# 4054
if(_tmp8C6 == 0){
# 4055
const char*_tmpD87;void*_tmpD86;(_tmpD86=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD87="empty Letv_d",_tag_dyneither(_tmpD87,sizeof(char),13))),_tag_dyneither(_tmpD86,sizeof(void*),0)));}
# 4056
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD8A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD89;_tmp87F->r=(void*)((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89[0]=((_tmpD8A.tag=0,((_tmpD8A.f1=(struct Cyc_Absyn_Vardecl*)_tmp8C6->hd,_tmpD8A)))),_tmpD89))));}
# 4057
_tmp8C6=_tmp8C6->tl;
# 4058
for(0;_tmp8C6 != 0;_tmp8C6=_tmp8C6->tl){
# 4059
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD8D;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD8C;struct Cyc_Absyn_Decl*_tmp8CB=Cyc_Absyn_new_decl((void*)((_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C[0]=((_tmpD8D.tag=0,((_tmpD8D.f1=(struct Cyc_Absyn_Vardecl*)_tmp8C6->hd,_tmpD8D)))),_tmpD8C)))),0);
# 4060
s->r=(Cyc_Absyn_decl_stmt(_tmp8CB,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4062
Cyc_Toc_stmt_to_c(nv,s);
# 4063
goto _LL399;}_LL3A0: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp8AA=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp8A2;if(_tmp8AA->tag != 1)goto _LL3A2;else{_tmp8AB=_tmp8AA->f1;}}_LL3A1: {
# 4065
struct _tuple0*_tmp8CE=_tmp8AB->name;
# 4066
struct _RegionHandle _tmp8CF=_new_region("temp");struct _RegionHandle*temp=& _tmp8CF;_push_region(temp);{
# 4067
struct Cyc_Toc_Env*_tmp8D0=Cyc_Toc_add_varmap(temp,nv,_tmp8AB->name,Cyc_Absyn_var_exp(_tmp8CE,0));
# 4068
Cyc_Toc_fndecl_to_c(_tmp8D0,_tmp8AB,0);
# 4069
Cyc_Toc_stmt_to_c(_tmp8D0,_tmp880);}
# 4071
_npop_handler(0);goto _LL399;
# 4066
;_pop_region(temp);}_LL3A2: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp8AC=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp8A2;if(_tmp8AC->tag != 4)goto _LL3A4;else{_tmp8AD=_tmp8AC->f1;_tmp8AE=_tmp8AC->f2;_tmp8AF=_tmp8AC->f3;_tmp8B0=_tmp8AC->f4;}}_LL3A3: {
# 4073
struct Cyc_Absyn_Stmt*_tmp8D1=_tmp880;
# 4089 "toc.cyc"
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
# 4090
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
# 4091
struct _tuple0*rh_var=Cyc_Toc_temp_var();
# 4092
struct _tuple0*x_var=_tmp8AE->name;
# 4093
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
# 4094
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4096
struct _RegionHandle _tmp8D2=_new_region("temp");struct _RegionHandle*temp=& _tmp8D2;_push_region(temp);
# 4097
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp8D1);
# 4099
if(Cyc_Absyn_no_regions)
# 4100
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
# 4101
Cyc_Absyn_uint_exp(0,0),_tmp8D1,0))->r;else{
# 4102
if(_tmp8B0 == 0){
# 4103
struct Cyc_Absyn_Exp*_tmpD90[1];struct Cyc_Absyn_Exp*_tmpD8F[1];struct Cyc_List_List*_tmpD8E;s->r=(
# 4104
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
# 4105
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
# 4106
(_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD8E->tl=0,_tmpD8E)))))),0),
# 4108
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
# 4109
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD8F[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD8F,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
# 4110
Cyc_Absyn_seq_stmt(_tmp8D1,
# 4111
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD90[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD90,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}else{
# 4114
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp8B0);{
# 4115
struct Cyc_Absyn_Exp*_tmpD92[1];struct Cyc_Absyn_Exp*_tmpD91[2];s->r=(
# 4116
Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,
# 4117
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
# 4118
Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,(
# 4119
(_tmpD91[1]=(struct Cyc_Absyn_Exp*)_tmp8B0,((_tmpD91[0]=Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD91,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
# 4122
Cyc_Absyn_seq_stmt(_tmp8D1,
# 4123
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,(
# 4124
(_tmpD92[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD92,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
# 4127
_npop_handler(0);return;
# 4096
;_pop_region(temp);}_LL3A4: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp8B1=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp8A2;if(_tmp8B1->tag != 5)goto _LL3A6;else{_tmp8B2=_tmp8B1->f1;_tmp8B3=_tmp8B1->f2;_tmp8B4=_tmp8B1->f3;}}_LL3A5: {
# 4130
struct _tuple0*old_name=_tmp8B4->name;
# 4131
struct _tuple0*new_name=Cyc_Toc_temp_var();
# 4132
_tmp8B4->name=new_name;
# 4133
_tmp8B4->initializer=(struct Cyc_Absyn_Exp*)_tmp8B2;
# 4134
{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmpDA1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDA0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD9F;struct Cyc_Absyn_Decl*_tmpD9E;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD9D;s->r=(void*)((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((_tmpDA1.tag=12,((_tmpDA1.f1=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->r=(void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpD9F.tag=0,((_tmpD9F.f1=_tmp8B4,_tmpD9F)))),_tmpDA0)))),((_tmpD9E->loc=0,_tmpD9E)))))),((_tmpDA1.f2=_tmp880,_tmpDA1)))))),_tmpD9D))));}{
# 4135
struct _RegionHandle _tmp8DD=_new_region("temp");struct _RegionHandle*temp=& _tmp8DD;_push_region(temp);{
# 4136
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpDA4;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDA3;struct Cyc_Toc_Env*_tmp8DE=
# 4137
Cyc_Toc_add_varmap(temp,nv,old_name,
# 4138
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3[0]=((_tmpDA4.tag=4,((_tmpDA4.f1=_tmp8B4,_tmpDA4)))),_tmpDA3)))),0));
# 4139
Cyc_Toc_local_decl_to_c(_tmp8DE,nv,_tmp8B4,_tmp880);}
# 4141
_npop_handler(0);return;
# 4135
;_pop_region(temp);};}_LL3A6:;_LL3A7: {
# 4143
const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA7="bad nested declaration within function",_tag_dyneither(_tmpDA7,sizeof(char),39))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}_LL399:;}
# 4145
return;_LL391: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp881=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp881->tag != 13)goto _LL393;else{_tmp882=_tmp881->f1;_tmp883=_tmp881->f2;}}_LL392:
# 4147
 s=_tmp883;continue;_LL393: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp884=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp884->tag != 14)goto _LL395;else{_tmp885=_tmp884->f1;_tmp886=_tmp884->f2;_tmp887=_tmp886.f1;}}_LL394: {
# 4149
struct _RegionHandle _tmp8E3=_new_region("temp");struct _RegionHandle*temp=& _tmp8E3;_push_region(temp);
# 4150
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp885);
# 4151
Cyc_Toc_exp_to_c(nv,_tmp887);
# 4153
_npop_handler(0);return;
# 4149
;_pop_region(temp);}_LL395: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp888=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp888->tag != 15)goto _LL397;else{_tmp889=_tmp888->f1;_tmp88A=_tmp888->f2;}}_LL396: {
# 4170 "toc.cyc"
struct _tuple0*h_var=Cyc_Toc_temp_var();
# 4171
struct _tuple0*e_var=Cyc_Toc_temp_var();
# 4172
struct _tuple0*was_thrown_var=Cyc_Toc_temp_var();
# 4173
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
# 4174
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
# 4175
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
# 4176
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
# 4177
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
# 4178
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4181
e_exp->topt=(void*)e_typ;{
# 4182
struct _RegionHandle _tmp8E4=_new_region("temp");struct _RegionHandle*temp=& _tmp8E4;_push_region(temp);{
# 4183
struct Cyc_Toc_Env*_tmp8E5=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4185
Cyc_Toc_stmt_to_c(_tmp8E5,_tmp889);{
# 4186
struct Cyc_Absyn_Stmt*_tmp8E6=Cyc_Absyn_seq_stmt(_tmp889,
# 4187
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4191
struct _tuple0*_tmp8E7=Cyc_Toc_temp_var();
# 4192
struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Absyn_var_exp(_tmp8E7,0);
# 4193
struct Cyc_Absyn_Vardecl*_tmp8E9=Cyc_Absyn_new_vardecl(_tmp8E7,Cyc_Absyn_exn_typ(),0);
# 4194
_tmp8E8->topt=(void*)Cyc_Absyn_exn_typ();{
# 4195
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDB1;struct Cyc_Absyn_Pat*_tmpDB0;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDAF;struct Cyc_Absyn_Pat*_tmpDAE;struct Cyc_Absyn_Pat*_tmp8EA=(_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->r=(void*)((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDAF.tag=1,((_tmpDAF.f1=_tmp8E9,((_tmpDAF.f2=((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDB0->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpDB0->loc=0,_tmpDB0)))))))),_tmpDAF)))))),_tmpDB1)))),((_tmpDAE->topt=(void*)Cyc_Absyn_exn_typ(),((_tmpDAE->loc=0,_tmpDAE)))))));
# 4196
struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Absyn_throw_exp(_tmp8E8,0);
# 4197
_tmp8EB->topt=(void*)& Cyc_Absyn_VoidType_val;{
# 4198
struct Cyc_Absyn_Stmt*_tmp8EC=Cyc_Absyn_exp_stmt(_tmp8EB,0);
# 4199
struct Cyc_Core_Opt*_tmpDB7;struct Cyc_List_List*_tmpDB6;struct Cyc_Absyn_Switch_clause*_tmpDB5;struct Cyc_Absyn_Switch_clause*_tmp8ED=
# 4200
(_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->pattern=_tmp8EA,((_tmpDB5->pat_vars=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->v=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=_tmp8E9,((_tmpDB6->tl=0,_tmpDB6)))))),_tmpDB7)))),((_tmpDB5->where_clause=0,((_tmpDB5->body=_tmp8EC,((_tmpDB5->loc=0,_tmpDB5)))))))))));
# 4202
struct Cyc_List_List*_tmpDB8;struct Cyc_Absyn_Stmt*_tmp8EE=Cyc_Absyn_switch_stmt(e_exp,
# 4203
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp88A,(
# 4204
(_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->hd=_tmp8ED,((_tmpDB8->tl=0,_tmpDB8))))))),0);
# 4206
Cyc_Toc_stmt_to_c(_tmp8E5,_tmp8EE);{
# 4209
struct Cyc_List_List*_tmpDB9;struct Cyc_Absyn_Exp*_tmp8EF=
# 4210
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
# 4211
(_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDB9->tl=0,_tmpDB9)))))),0);
# 4213
struct Cyc_List_List*_tmpDBA;struct Cyc_Absyn_Stmt*_tmp8F0=
# 4214
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
# 4215
(_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDBA->tl=0,_tmpDBA)))))),0),0);
# 4217
struct Cyc_Absyn_Exp*_tmp8F1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
# 4218
struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
# 4219
s->r=(
# 4220
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
# 4221
Cyc_Absyn_seq_stmt(_tmp8F0,
# 4222
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8F1,
# 4223
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp8EF,
# 4224
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8F2,0),
# 4225
Cyc_Toc_skip_stmt_dl(),0),
# 4226
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8E6,
# 4228
Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
# 4229
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8EE,0),0),0),0),0),0))->r;};};};};}
# 4232
_npop_handler(0);return;
# 4182
;_pop_region(temp);};}_LL397: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp88B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp85A;if(_tmp88B->tag != 16)goto _LL376;else{_tmp88C=_tmp88B->f1;}}_LL398:
# 4234
 if(Cyc_Absyn_no_regions)
# 4235
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4237
Cyc_Toc_exp_to_c(nv,_tmp88C);{
# 4238
struct Cyc_List_List*_tmpDBB;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->hd=_tmp88C,((_tmpDBB->tl=0,_tmpDBB)))))),0));};}
# 4240
return;_LL376:;}}
# 4249
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
# 4250
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
# 4251
f->tvs=0;
# 4252
f->effect=0;
# 4253
f->rgn_po=0;
# 4254
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
# 4255
struct _RegionHandle _tmp8FE=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8FE;_push_region(frgn);
# 4256
{struct Cyc_Toc_Env*_tmp8FF=Cyc_Toc_share_env(frgn,nv);
# 4257
{struct Cyc_List_List*_tmp900=f->args;for(0;_tmp900 != 0;_tmp900=_tmp900->tl){
# 4258
struct _tuple0*_tmpDBC;struct _tuple0*_tmp901=(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBC->f2=(*((struct _tuple8*)_tmp900->hd)).f1,_tmpDBC)))));
# 4259
(*((struct _tuple8*)_tmp900->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp900->hd)).f3);
# 4260
_tmp8FF=Cyc_Toc_add_varmap(frgn,_tmp8FF,_tmp901,Cyc_Absyn_var_exp(_tmp901,0));}}
# 4264
if(cinclude){
# 4265
Cyc_Toc_stmttypes_to_c(f->body);
# 4266
_npop_handler(0);return;}
# 4268
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
# 4269
struct _dyneither_ptr*_tmp904;struct Cyc_Absyn_Tqual _tmp905;void*_tmp906;int _tmp907;struct Cyc_Absyn_VarargInfo _tmp903=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));_tmp904=_tmp903.name;_tmp905=_tmp903.tq;_tmp906=_tmp903.type;_tmp907=_tmp903.inject;{
# 4270
void*_tmp908=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp906,(void*)& Cyc_Absyn_HeapRgn_val,_tmp905,Cyc_Absyn_false_conref));
# 4271
struct _tuple0*_tmpDBD;struct _tuple0*_tmp909=(_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpDBD->f2=(struct _dyneither_ptr*)_check_null(_tmp904),_tmpDBD)))));
# 4272
{struct _tuple8*_tmpDC0;struct Cyc_List_List*_tmpDBF;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->hd=((_tmpDC0=_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->f1=(struct _dyneither_ptr*)_tmp904,((_tmpDC0->f2=_tmp905,((_tmpDC0->f3=_tmp908,_tmpDC0)))))))),((_tmpDBF->tl=0,_tmpDBF)))))));}
# 4273
_tmp8FF=Cyc_Toc_add_varmap(frgn,_tmp8FF,_tmp909,Cyc_Absyn_var_exp(_tmp909,0));
# 4274
f->cyc_varargs=0;};}
# 4277
{struct Cyc_List_List*_tmp90D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp90D != 0;_tmp90D=_tmp90D->tl){
# 4278
((struct Cyc_Absyn_Vardecl*)_tmp90D->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp90D->hd)->type);}}
# 4280
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8FF),f->body);}
# 4256
;_pop_region(frgn);};}
# 4285
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
# 4286
switch(s){case Cyc_Absyn_Abstract: _LL3AD:
# 4287
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3AE:
# 4288
 return Cyc_Absyn_Extern;default: _LL3AF:
# 4289
 return s;}}
# 4301 "toc.cyc"
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
# 4302
struct _tuple0*_tmp90E=ad->name;
# 4303
struct _DynRegionHandle*_tmp910;struct Cyc_Dict_Dict*_tmp911;struct Cyc_Toc_TocState _tmp90F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp910=_tmp90F.dyn;_tmp911=_tmp90F.aggrs_so_far;{
# 4304
struct _DynRegionFrame _tmp912;struct _RegionHandle*d=_open_dynregion(& _tmp912,_tmp910);
# 4305
{int seen_defn_before;
# 4306
struct _tuple15**_tmp913=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp911,_tmp90E);
# 4307
if(_tmp913 == 0){
# 4308
seen_defn_before=0;{
# 4309
struct _tuple15*v;
# 4310
if(ad->kind == Cyc_Absyn_StructA){
# 4311
struct _tuple15*_tmpDC1;v=((_tmpDC1=_region_malloc(d,sizeof(*_tmpDC1)),((_tmpDC1->f1=ad,((_tmpDC1->f2=Cyc_Absyn_strctq(_tmp90E),_tmpDC1))))));}else{
# 4313
struct _tuple15*_tmpDC2;v=((_tmpDC2=_region_malloc(d,sizeof(*_tmpDC2)),((_tmpDC2->f1=ad,((_tmpDC2->f2=Cyc_Absyn_unionq_typ(_tmp90E),_tmpDC2))))));}
# 4314
*_tmp911=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp911,_tmp90E,v);};}else{
# 4316
struct _tuple15 _tmp917;struct Cyc_Absyn_Aggrdecl*_tmp918;void*_tmp919;struct _tuple15*_tmp916=*_tmp913;_tmp917=*_tmp916;_tmp918=_tmp917.f1;_tmp919=_tmp917.f2;
# 4317
if(_tmp918->impl == 0){
# 4318
{struct _tuple15*_tmpDC3;*_tmp911=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp911,_tmp90E,((_tmpDC3=_region_malloc(d,sizeof(*_tmpDC3)),((_tmpDC3->f1=ad,((_tmpDC3->f2=_tmp919,_tmpDC3)))))));}
# 4319
seen_defn_before=0;}else{
# 4321
seen_defn_before=1;}}{
# 4323
struct Cyc_Absyn_Aggrdecl*_tmpDC4;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->kind=ad->kind,((_tmpDC4->sc=Cyc_Absyn_Public,((_tmpDC4->name=ad->name,((_tmpDC4->tvs=0,((_tmpDC4->impl=0,((_tmpDC4->attributes=ad->attributes,_tmpDC4)))))))))))));
# 4329
if(ad->impl != 0  && !seen_defn_before){
# 4330
{struct Cyc_Absyn_AggrdeclImpl*_tmpDC5;new_ad->impl=((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->exist_vars=0,((_tmpDC5->rgn_po=0,((_tmpDC5->fields=0,((_tmpDC5->tagged=0,_tmpDC5))))))))));}{
# 4334
struct Cyc_List_List*new_fields=0;
# 4335
{struct Cyc_List_List*_tmp91C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp91C != 0;_tmp91C=_tmp91C->tl){
# 4338
struct Cyc_Absyn_Aggrfield*_tmp91D=(struct Cyc_Absyn_Aggrfield*)_tmp91C->hd;
# 4339
if(_tmp91C->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp91D->type)))
# 4341
continue;{
# 4342
struct Cyc_Absyn_Aggrfield*_tmpDC6;struct Cyc_Absyn_Aggrfield*_tmp91E=(_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->name=_tmp91D->name,((_tmpDC6->tq=Cyc_Toc_mt_tq,((_tmpDC6->type=
# 4344
Cyc_Toc_typ_to_c_array(_tmp91D->type),((_tmpDC6->width=_tmp91D->width,((_tmpDC6->attributes=_tmp91D->attributes,_tmpDC6)))))))))));
# 4350
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
# 4351
void*_tmp91F=_tmp91E->type;
# 4352
struct _dyneither_ptr*_tmp920=_tmp91E->name;
# 4353
const char*_tmpDCB;void*_tmpDCA[2];struct Cyc_String_pa_PrintArg_struct _tmpDC9;struct Cyc_String_pa_PrintArg_struct _tmpDC8;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDC8.tag=0,((_tmpDC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp920),((_tmpDC9.tag=0,((_tmpDC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpDCA[0]=& _tmpDC9,((_tmpDCA[1]=& _tmpDC8,Cyc_aprintf(((_tmpDCB="_union_%s_%s",_tag_dyneither(_tmpDCB,sizeof(char),13))),_tag_dyneither(_tmpDCA,sizeof(void*),2))))))))))))));
# 4354
struct _dyneither_ptr*_tmpDCC;struct _dyneither_ptr*str=(_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC[0]=s,_tmpDCC)));
# 4355
struct Cyc_Absyn_Aggrfield*_tmpDCD;struct Cyc_Absyn_Aggrfield*_tmp921=(_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->name=Cyc_Toc_val_sp,((_tmpDCD->tq=Cyc_Toc_mt_tq,((_tmpDCD->type=_tmp91F,((_tmpDCD->width=0,((_tmpDCD->attributes=0,_tmpDCD)))))))))));
# 4356
struct Cyc_Absyn_Aggrfield*_tmpDCE;struct Cyc_Absyn_Aggrfield*_tmp922=(_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->name=Cyc_Toc_tag_sp,((_tmpDCE->tq=Cyc_Toc_mt_tq,((_tmpDCE->type=Cyc_Absyn_sint_typ,((_tmpDCE->width=0,((_tmpDCE->attributes=0,_tmpDCE)))))))))));
# 4357
struct Cyc_Absyn_Aggrfield*_tmpDCF[2];struct Cyc_List_List*_tmp923=(_tmpDCF[1]=_tmp921,((_tmpDCF[0]=_tmp922,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDCF,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
# 4358
struct Cyc_Absyn_AggrdeclImpl*_tmpDD4;struct _tuple0*_tmpDD3;struct Cyc_Absyn_Aggrdecl*_tmpDD2;struct Cyc_Absyn_Aggrdecl*_tmp924=(_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->kind=Cyc_Absyn_StructA,((_tmpDD2->sc=Cyc_Absyn_Public,((_tmpDD2->name=(
# 4359
(_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3->f1=Cyc_Absyn_Loc_n,((_tmpDD3->f2=str,_tmpDD3)))))),((_tmpDD2->tvs=0,((_tmpDD2->impl=(
# 4360
(_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->exist_vars=0,((_tmpDD4->rgn_po=0,((_tmpDD4->fields=_tmp923,((_tmpDD4->tagged=0,_tmpDD4)))))))))),((_tmpDD2->attributes=0,_tmpDD2)))))))))))));
# 4362
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDDA;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDD9;struct Cyc_List_List*_tmpDD8;Cyc_Toc_result_decls=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->hd=Cyc_Absyn_new_decl((void*)((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA[0]=((_tmpDD9.tag=6,((_tmpDD9.f1=_tmp924,_tmpDD9)))),_tmpDDA)))),0),((_tmpDD8->tl=Cyc_Toc_result_decls,_tmpDD8))))));}
# 4363
_tmp91E->type=Cyc_Absyn_strct(str);}{
# 4365
struct Cyc_List_List*_tmpDDB;new_fields=((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->hd=_tmp91E,((_tmpDDB->tl=new_fields,_tmpDDB))))));};};}}
# 4367
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4369
if(add_to_decls){
# 4370
struct Cyc_Absyn_Decl*_tmpDE5;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDE4;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDE3;struct Cyc_List_List*_tmpDE2;Cyc_Toc_result_decls=((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->hd=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->r=(void*)((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3[0]=((_tmpDE4.tag=6,((_tmpDE4.f1=new_ad,_tmpDE4)))),_tmpDE3)))),((_tmpDE5->loc=0,_tmpDE5)))))),((_tmpDE2->tl=Cyc_Toc_result_decls,_tmpDE2))))));}};}
# 4305
;_pop_dynregion(d);};}
# 4398 "toc.cyc"
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
# 4399
struct _DynRegionHandle*_tmp93B;struct Cyc_Set_Set**_tmp93C;struct Cyc_Toc_TocState _tmp93A=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp93B=_tmp93A.dyn;_tmp93C=_tmp93A.datatypes_so_far;{
# 4400
struct _DynRegionFrame _tmp93D;struct _RegionHandle*d=_open_dynregion(& _tmp93D,_tmp93B);{
# 4401
struct _tuple0*_tmp93E=tud->name;
# 4402
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)(*_tmp93C,_tmp93E)){
# 4403
_npop_handler(0);return;}
# 4404
*_tmp93C=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_rinsert)(d,*_tmp93C,_tmp93E);}{
# 4406
struct Cyc_List_List*_tmp93F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp93F != 0;_tmp93F=_tmp93F->tl){
# 4407
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp93F->hd;
# 4409
struct Cyc_List_List*_tmp940=0;
# 4410
int i=1;
# 4411
{struct Cyc_List_List*_tmp941=f->typs;for(0;_tmp941 != 0;(_tmp941=_tmp941->tl,i ++)){
# 4412
struct _dyneither_ptr*_tmp942=Cyc_Absyn_fieldname(i);
# 4413
struct Cyc_Absyn_Aggrfield*_tmpDE6;struct Cyc_Absyn_Aggrfield*_tmp943=(_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->name=_tmp942,((_tmpDE6->tq=(*((struct _tuple10*)_tmp941->hd)).f1,((_tmpDE6->type=
# 4414
Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp941->hd)).f2),((_tmpDE6->width=0,((_tmpDE6->attributes=0,_tmpDE6)))))))))));
# 4415
struct Cyc_List_List*_tmpDE7;_tmp940=((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->hd=_tmp943,((_tmpDE7->tl=_tmp940,_tmpDE7))))));}}
# 4417
{struct Cyc_Absyn_Aggrfield*_tmpDEA;struct Cyc_List_List*_tmpDE9;_tmp940=((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->hd=((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->name=Cyc_Toc_tag_sp,((_tmpDEA->tq=Cyc_Toc_mt_tq,((_tmpDEA->type=Cyc_Absyn_sint_typ,((_tmpDEA->width=0,((_tmpDEA->attributes=0,_tmpDEA)))))))))))),((_tmpDE9->tl=
# 4418
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp940),_tmpDE9))))));}{
# 4419
struct Cyc_Absyn_AggrdeclImpl*_tmpDED;struct Cyc_Absyn_Aggrdecl*_tmpDEC;struct Cyc_Absyn_Aggrdecl*_tmp948=
# 4420
(_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->kind=Cyc_Absyn_StructA,((_tmpDEC->sc=Cyc_Absyn_Public,((_tmpDEC->name=
# 4421
Cyc_Toc_collapse_qvars(f->name,tud->name),((_tmpDEC->tvs=0,((_tmpDEC->impl=(
# 4423
(_tmpDED=_cycalloc(sizeof(*_tmpDED)),((_tmpDED->exist_vars=0,((_tmpDED->rgn_po=0,((_tmpDED->fields=_tmp940,((_tmpDED->tagged=0,_tmpDED)))))))))),((_tmpDEC->attributes=0,_tmpDEC)))))))))))));
# 4425
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF3;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF2;struct Cyc_List_List*_tmpDF1;Cyc_Toc_result_decls=((_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->hd=Cyc_Absyn_new_decl((void*)((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF2.tag=6,((_tmpDF2.f1=_tmp948,_tmpDF2)))),_tmpDF3)))),0),((_tmpDF1->tl=Cyc_Toc_result_decls,_tmpDF1))))));};}};
# 4400
;_pop_dynregion(d);};}
# 4446 "toc.cyc"
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
# 4447
if(xd->fields == 0)
# 4448
return;{
# 4449
struct _DynRegionHandle*_tmp94F;struct Cyc_Dict_Dict*_tmp950;struct Cyc_Toc_TocState _tmp94E=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp94F=_tmp94E.dyn;_tmp950=_tmp94E.xdatatypes_so_far;{
# 4450
struct _DynRegionFrame _tmp951;struct _RegionHandle*d=_open_dynregion(& _tmp951,_tmp94F);
# 4451
{struct _tuple0*_tmp952=xd->name;
# 4452
struct Cyc_List_List*_tmp953=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp953 != 0;_tmp953=_tmp953->tl){
# 4453
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp953->hd;
# 4454
struct _dyneither_ptr*fn=(*f->name).f2;
# 4455
struct Cyc_Absyn_Exp*_tmp954=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
# 4456
void*_tmp955=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp954,Cyc_Absyn_false_conref,0);
# 4458
int*_tmp956=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)(*_tmp950,f->name);int _tmp957;_LL3B2: if(_tmp956 != 0)goto _LL3B4;_LL3B3: {
# 4460
struct Cyc_Absyn_Exp*initopt=0;
# 4461
if(f->sc != Cyc_Absyn_Extern)
# 4462
initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*fn,0);{
# 4464
struct Cyc_Absyn_Vardecl*_tmp958=Cyc_Absyn_new_vardecl(f->name,_tmp955,initopt);
# 4465
_tmp958->sc=f->sc;
# 4466
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDF9;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDF8;struct Cyc_List_List*_tmpDF7;Cyc_Toc_result_decls=((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7->hd=Cyc_Absyn_new_decl((void*)((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDF8.tag=0,((_tmpDF8.f1=_tmp958,_tmpDF8)))),_tmpDF9)))),0),((_tmpDF7->tl=Cyc_Toc_result_decls,_tmpDF7))))));}
# 4467
*_tmp950=
# 4468
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp950,f->name,f->sc != Cyc_Absyn_Extern);{
# 4469
struct Cyc_List_List*fields=0;
# 4470
int i=1;
# 4471
{struct Cyc_List_List*_tmp95C=f->typs;for(0;_tmp95C != 0;(_tmp95C=_tmp95C->tl,i ++)){
# 4472
struct Cyc_Int_pa_PrintArg_struct _tmpE01;void*_tmpE00[1];const char*_tmpDFF;struct _dyneither_ptr*_tmpDFE;struct _dyneither_ptr*_tmp95D=(_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE[0]=(struct _dyneither_ptr)((_tmpE01.tag=1,((_tmpE01.f1=(unsigned long)i,((_tmpE00[0]=& _tmpE01,Cyc_aprintf(((_tmpDFF="f%d",_tag_dyneither(_tmpDFF,sizeof(char),4))),_tag_dyneither(_tmpE00,sizeof(void*),1)))))))),_tmpDFE)));
# 4473
struct Cyc_Absyn_Aggrfield*_tmpE02;struct Cyc_Absyn_Aggrfield*_tmp95E=(_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->name=_tmp95D,((_tmpE02->tq=(*((struct _tuple10*)_tmp95C->hd)).f1,((_tmpE02->type=
# 4474
Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp95C->hd)).f2),((_tmpE02->width=0,((_tmpE02->attributes=0,_tmpE02)))))))))));
# 4475
struct Cyc_List_List*_tmpE03;fields=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03->hd=_tmp95E,((_tmpE03->tl=fields,_tmpE03))))));}}
# 4477
{struct Cyc_Absyn_Aggrfield*_tmpE06;struct Cyc_List_List*_tmpE05;fields=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->hd=((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->name=Cyc_Toc_tag_sp,((_tmpE06->tq=Cyc_Toc_mt_tq,((_tmpE06->type=
# 4478
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE06->width=0,((_tmpE06->attributes=0,_tmpE06)))))))))))),((_tmpE05->tl=
# 4479
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE05))))));}{
# 4480
struct Cyc_Absyn_AggrdeclImpl*_tmpE09;struct Cyc_Absyn_Aggrdecl*_tmpE08;struct Cyc_Absyn_Aggrdecl*_tmp967=
# 4481
(_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->kind=Cyc_Absyn_StructA,((_tmpE08->sc=Cyc_Absyn_Public,((_tmpE08->name=
# 4482
Cyc_Toc_collapse_qvars(f->name,xd->name),((_tmpE08->tvs=0,((_tmpE08->impl=(
# 4484
(_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09->exist_vars=0,((_tmpE09->rgn_po=0,((_tmpE09->fields=fields,((_tmpE09->tagged=0,_tmpE09)))))))))),((_tmpE08->attributes=0,_tmpE08)))))))))))));
# 4486
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE0F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE0E;struct Cyc_List_List*_tmpE0D;Cyc_Toc_result_decls=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->hd=Cyc_Absyn_new_decl((void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE0E.tag=6,((_tmpE0E.f1=_tmp967,_tmpE0E)))),_tmpE0F)))),0),((_tmpE0D->tl=Cyc_Toc_result_decls,_tmpE0D))))));}
# 4488
goto _LL3B1;};};};}_LL3B4: if(_tmp956 == 0)goto _LL3B6;_tmp957=*_tmp956;if(_tmp957 != 0)goto _LL3B6;_LL3B5:
# 4490
 if(f->sc != Cyc_Absyn_Extern){
# 4491
struct Cyc_Absyn_Exp*_tmp96D=Cyc_Absyn_string_exp(*fn,0);
# 4492
struct Cyc_Absyn_Vardecl*_tmp96E=Cyc_Absyn_new_vardecl(f->name,_tmp955,(struct Cyc_Absyn_Exp*)_tmp96D);
# 4493
_tmp96E->sc=f->sc;
# 4494
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE15;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE14;struct Cyc_List_List*_tmpE13;Cyc_Toc_result_decls=((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13->hd=Cyc_Absyn_new_decl((void*)((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15[0]=((_tmpE14.tag=0,((_tmpE14.f1=_tmp96E,_tmpE14)))),_tmpE15)))),0),((_tmpE13->tl=Cyc_Toc_result_decls,_tmpE13))))));}
# 4495
*_tmp950=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,int v))Cyc_Dict_insert)(*_tmp950,f->name,1);}
# 4497
goto _LL3B1;_LL3B6:;_LL3B7:
# 4498
 goto _LL3B1;_LL3B1:;}}
# 4451
;_pop_dynregion(d);};};}
# 4504
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
# 4505
ed->sc=Cyc_Absyn_Public;
# 4506
if(ed->fields != 0)
# 4507
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4512
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
# 4513
void*old_typ=vd->type;
# 4514
vd->type=Cyc_Toc_typ_to_c_array(old_typ);
# 4516
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
# 4517
vd->sc=Cyc_Absyn_Public;
# 4518
Cyc_Toc_stmt_to_c(body_nv,s);
# 4519
if(vd->initializer != 0){
# 4520
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
# 4521
void*_tmp972=init->r;struct Cyc_Absyn_Vardecl*_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp976;int _tmp977;_LL3B9: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp973=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp972;if(_tmp973->tag != 26)goto _LL3BB;else{_tmp974=_tmp973->f1;_tmp975=_tmp973->f2;_tmp976=_tmp973->f3;_tmp977=_tmp973->f4;}}_LL3BA:
# 4523
 vd->initializer=0;
# 4524
s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp974,_tmp975,_tmp976,_tmp977,
# 4525
Cyc_Absyn_new_stmt(s->r,0),0))->r;
# 4526
goto _LL3B8;_LL3BB:;_LL3BC:
# 4528
 if(vd->sc == Cyc_Absyn_Static){
# 4532
struct _RegionHandle _tmp978=_new_region("temp");struct _RegionHandle*temp=& _tmp978;_push_region(temp);
# 4533
{struct Cyc_Toc_Env*_tmp979=Cyc_Toc_set_toplevel(temp,init_nv);
# 4534
Cyc_Toc_exp_to_c(_tmp979,init);}
# 4533
;_pop_region(temp);}else{
# 4537
Cyc_Toc_exp_to_c(init_nv,init);}
# 4538
goto _LL3B8;_LL3B8:;}else{
# 4542
void*_tmp97A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo _tmp97C;void*_tmp97D;struct Cyc_Absyn_Exp*_tmp97E;union Cyc_Absyn_Constraint*_tmp97F;_LL3BE:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp97B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97A;if(_tmp97B->tag != 8)goto _LL3C0;else{_tmp97C=_tmp97B->f1;_tmp97D=_tmp97C.elt_type;_tmp97E=_tmp97C.num_elts;_tmp97F=_tmp97C.zero_term;}}if(!
# 4543
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp97F))goto _LL3C0;_LL3BF:
# 4544
 if(_tmp97E == 0){
# 4545
const char*_tmpE18;void*_tmpE17;(_tmpE17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE18="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE18,sizeof(char),55))),_tag_dyneither(_tmpE17,sizeof(void*),0)));}{
# 4546
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp97E;
# 4547
struct Cyc_Absyn_Exp*_tmp982=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
# 4548
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4550
struct Cyc_Absyn_Exp*_tmp983=Cyc_Absyn_signed_int_exp(0,0);
# 4551
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp982,_tmp983,0),0),
# 4552
Cyc_Absyn_new_stmt(s->r,0));
# 4553
goto _LL3BD;};_LL3C0:;_LL3C1:
# 4554
 goto _LL3BD;_LL3BD:;}}
# 4564
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4565
Cyc_Toc_exp_to_c(nv,e);{
# 4566
struct _tuple0*x=Cyc_Toc_temp_var();
# 4567
struct _RegionHandle _tmp984=_new_region("prgn");struct _RegionHandle*prgn=& _tmp984;_push_region(prgn);{
# 4568
struct Cyc_Absyn_Stmt*_tmp985=Cyc_Toc_throw_match_stmt();
# 4569
struct Cyc_Toc_Env*_tmp986=Cyc_Toc_share_env(prgn,nv);
# 4570
struct Cyc_Toc_Env*_tmp988;struct Cyc_List_List*_tmp989;struct Cyc_Absyn_Stmt*_tmp98A;struct _tuple23 _tmp987=
# 4571
Cyc_Toc_xlate_pat(_tmp986,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
# 4572
Cyc_Toc_throw_match_stmt(),0);_tmp988=_tmp987.f1;_tmp989=_tmp987.f2;_tmp98A=_tmp987.f3;
# 4573
Cyc_Toc_stmt_to_c(_tmp988,s);
# 4574
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp98A,s,0),0);
# 4575
for(0;_tmp989 != 0;_tmp989=_tmp989->tl){
# 4576
struct _tuple24 _tmp98C;struct _tuple0*_tmp98D;void*_tmp98E;struct _tuple24*_tmp98B=(struct _tuple24*)_tmp989->hd;_tmp98C=*_tmp98B;_tmp98D=_tmp98C.f1;_tmp98E=_tmp98C.f2;
# 4577
s=Cyc_Absyn_declare_stmt(_tmp98D,_tmp98E,0,s,0);}}{
# 4580
struct Cyc_Absyn_Stmt*_tmp98F=s;_npop_handler(0);return _tmp98F;};
# 4567
;_pop_region(prgn);};}
# 4586
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
# 4587
void*_tmp990=e->r;struct Cyc_Absyn_Exp*_tmp992;struct Cyc_Absyn_Exp*_tmp994;struct Cyc_Absyn_Exp*_tmp996;struct Cyc_Absyn_Exp*_tmp998;struct Cyc_Absyn_Exp*_tmp99A;struct Cyc_Absyn_Exp*_tmp99C;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_Absyn_Exp*_tmp9A0;struct Cyc_List_List*_tmp9A2;struct Cyc_Absyn_Exp*_tmp9A4;struct Cyc_Absyn_Exp*_tmp9A5;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_Absyn_Exp*_tmp9A8;struct Cyc_Absyn_Exp*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9AB;struct Cyc_Absyn_Exp*_tmp9AD;struct Cyc_Absyn_Exp*_tmp9AE;struct Cyc_Absyn_Exp*_tmp9B0;struct Cyc_Absyn_Exp*_tmp9B1;struct Cyc_Absyn_Exp*_tmp9B3;struct Cyc_Absyn_Exp*_tmp9B4;struct Cyc_Absyn_Exp*_tmp9B6;struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Exp*_tmp9B8;struct Cyc_Absyn_Exp*_tmp9BA;struct Cyc_List_List*_tmp9BB;void*_tmp9BD;void**_tmp9BE;struct Cyc_Absyn_Exp*_tmp9BF;struct _tuple8*_tmp9C1;struct _tuple8 _tmp9C2;void*_tmp9C3;void**_tmp9C4;struct Cyc_List_List*_tmp9C5;struct Cyc_List_List*_tmp9C7;struct Cyc_List_List*_tmp9C9;void*_tmp9CB;void**_tmp9CC;void*_tmp9CE;void**_tmp9CF;struct Cyc_Absyn_Stmt*_tmp9D1;struct Cyc_Absyn_MallocInfo _tmp9D3;struct Cyc_Absyn_MallocInfo*_tmp9D4;_LL3C3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp991=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp991->tag != 19)goto _LL3C5;else{_tmp992=_tmp991->f1;}}_LL3C4:
# 4588
 _tmp994=_tmp992;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp993=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp993->tag != 20)goto _LL3C7;else{_tmp994=_tmp993->f1;}}_LL3C6:
# 4589
 _tmp996=_tmp994;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp995=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp995->tag != 21)goto _LL3C9;else{_tmp996=_tmp995->f1;}}_LL3C8:
# 4590
 _tmp998=_tmp996;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp997=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp997->tag != 14)goto _LL3CB;else{_tmp998=_tmp997->f1;}}_LL3CA:
# 4591
 _tmp99A=_tmp998;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp999=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp999->tag != 10)goto _LL3CD;else{_tmp99A=_tmp999->f1;}}_LL3CC:
# 4592
 _tmp99C=_tmp99A;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp99B->tag != 11)goto _LL3CF;else{_tmp99C=_tmp99B->f1;}}_LL3CE:
# 4593
 _tmp99E=_tmp99C;goto _LL3D0;_LL3CF: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp99D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp99D->tag != 17)goto _LL3D1;else{_tmp99E=_tmp99D->f1;}}_LL3D0:
# 4594
 _tmp9A0=_tmp99E;goto _LL3D2;_LL3D1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp99F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp99F->tag != 4)goto _LL3D3;else{_tmp9A0=_tmp99F->f1;}}_LL3D2:
# 4595
 Cyc_Toc_exptypes_to_c(_tmp9A0);goto _LL3C2;_LL3D3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9A1->tag != 2)goto _LL3D5;else{_tmp9A2=_tmp9A1->f2;}}_LL3D4:
# 4596
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9A2);goto _LL3C2;_LL3D5: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp9A3=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9A3->tag != 6)goto _LL3D7;else{_tmp9A4=_tmp9A3->f1;_tmp9A5=_tmp9A3->f2;}}_LL3D6:
# 4597
 _tmp9A7=_tmp9A4;_tmp9A8=_tmp9A5;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9A6=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9A6->tag != 7)goto _LL3D9;else{_tmp9A7=_tmp9A6->f1;_tmp9A8=_tmp9A6->f2;}}_LL3D8:
# 4598
 _tmp9AA=_tmp9A7;_tmp9AB=_tmp9A8;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp9A9=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9A9->tag != 8)goto _LL3DB;else{_tmp9AA=_tmp9A9->f1;_tmp9AB=_tmp9A9->f2;}}_LL3DA:
# 4599
 _tmp9AD=_tmp9AA;_tmp9AE=_tmp9AB;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9AC=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9AC->tag != 22)goto _LL3DD;else{_tmp9AD=_tmp9AC->f1;_tmp9AE=_tmp9AC->f2;}}_LL3DC:
# 4600
 _tmp9B0=_tmp9AD;_tmp9B1=_tmp9AE;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp9AF=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9AF->tag != 33)goto _LL3DF;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9AF->f2;}}_LL3DE:
# 4601
 _tmp9B3=_tmp9B0;_tmp9B4=_tmp9B1;goto _LL3E0;_LL3DF: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp9B2=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9B2->tag != 3)goto _LL3E1;else{_tmp9B3=_tmp9B2->f1;_tmp9B4=_tmp9B2->f3;}}_LL3E0:
# 4602
 Cyc_Toc_exptypes_to_c(_tmp9B3);Cyc_Toc_exptypes_to_c(_tmp9B4);goto _LL3C2;_LL3E1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9B5->tag != 5)goto _LL3E3;else{_tmp9B6=_tmp9B5->f1;_tmp9B7=_tmp9B5->f2;_tmp9B8=_tmp9B5->f3;}}_LL3E2:
# 4604
 Cyc_Toc_exptypes_to_c(_tmp9B6);Cyc_Toc_exptypes_to_c(_tmp9B7);Cyc_Toc_exptypes_to_c(_tmp9B8);goto _LL3C2;_LL3E3: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp9B9=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9B9->tag != 9)goto _LL3E5;else{_tmp9BA=_tmp9B9->f1;_tmp9BB=_tmp9B9->f2;}}_LL3E4:
# 4606
 Cyc_Toc_exptypes_to_c(_tmp9BA);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp9BB);goto _LL3C2;_LL3E5: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp9BC=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9BC->tag != 13)goto _LL3E7;else{_tmp9BD=(void**)& _tmp9BC->f1;_tmp9BE=(void**)((void**)& _tmp9BC->f1);_tmp9BF=_tmp9BC->f2;}}_LL3E6:
# 4607
*_tmp9BE=Cyc_Toc_typ_to_c(*_tmp9BE);Cyc_Toc_exptypes_to_c(_tmp9BF);goto _LL3C2;_LL3E7: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp9C0=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9C0->tag != 24)goto _LL3E9;else{_tmp9C1=_tmp9C0->f1;_tmp9C2=*_tmp9C1;_tmp9C3=_tmp9C2.f3;_tmp9C4=(void**)&(*_tmp9C0->f1).f3;_tmp9C5=_tmp9C0->f2;}}_LL3E8:
# 4609
*_tmp9C4=Cyc_Toc_typ_to_c(*_tmp9C4);
# 4610
_tmp9C7=_tmp9C5;goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp9C6=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9C6->tag != 34)goto _LL3EB;else{_tmp9C7=_tmp9C6->f2;}}_LL3EA:
# 4611
 _tmp9C9=_tmp9C7;goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9C8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9C8->tag != 25)goto _LL3ED;else{_tmp9C9=_tmp9C8->f1;}}_LL3EC:
# 4613
 for(0;_tmp9C9 != 0;_tmp9C9=_tmp9C9->tl){
# 4614
struct Cyc_Absyn_Exp*_tmp9E4;struct _tuple16 _tmp9E3=*((struct _tuple16*)_tmp9C9->hd);_tmp9E4=_tmp9E3.f2;
# 4615
Cyc_Toc_exptypes_to_c(_tmp9E4);}
# 4617
goto _LL3C2;_LL3ED: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9CA=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9CA->tag != 18)goto _LL3EF;else{_tmp9CB=(void**)& _tmp9CA->f1;_tmp9CC=(void**)((void**)& _tmp9CA->f1);}}_LL3EE:
# 4618
 _tmp9CF=_tmp9CC;goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9CD=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9CD->tag != 16)goto _LL3F1;else{_tmp9CE=(void**)& _tmp9CD->f1;_tmp9CF=(void**)((void**)& _tmp9CD->f1);}}_LL3F0:
# 4619
*_tmp9CF=Cyc_Toc_typ_to_c(*_tmp9CF);goto _LL3C2;_LL3F1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9D0=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D0->tag != 35)goto _LL3F3;else{_tmp9D1=_tmp9D0->f1;}}_LL3F2:
# 4620
 Cyc_Toc_stmttypes_to_c(_tmp9D1);goto _LL3C2;_LL3F3: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp9D2=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D2->tag != 32)goto _LL3F5;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=(struct Cyc_Absyn_MallocInfo*)& _tmp9D2->f1;}}_LL3F4:
# 4622
 if(_tmp9D4->elt_type != 0){
# 4623
void**_tmpE19;_tmp9D4->elt_type=((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9D4->elt_type))),_tmpE19))));}
# 4624
Cyc_Toc_exptypes_to_c(_tmp9D4->num_elts);
# 4625
goto _LL3C2;_LL3F5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9D5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D5->tag != 0)goto _LL3F7;}_LL3F6:
# 4626
 goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9D6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D6->tag != 1)goto _LL3F9;}_LL3F8:
# 4627
 goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9D7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D7->tag != 30)goto _LL3FB;}_LL3FA:
# 4628
 goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9D8=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D8->tag != 38)goto _LL3FD;}_LL3FC:
# 4629
 goto _LL3FE;_LL3FD: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9D9=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9D9->tag != 31)goto _LL3FF;}_LL3FE:
# 4630
 goto _LL3C2;_LL3FF: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9DA=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9DA->tag != 28)goto _LL401;}_LL400:
# 4632
 goto _LL402;_LL401: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9DB=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9DB->tag != 29)goto _LL403;}_LL402:
# 4633
 goto _LL404;_LL403: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9DC=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9DC->tag != 27)goto _LL405;}_LL404:
# 4634
 goto _LL406;_LL405: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9DD=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9DD->tag != 26)goto _LL407;}_LL406:
# 4635
 goto _LL408;_LL407: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9DE=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9DE->tag != 23)goto _LL409;}_LL408:
# 4636
 goto _LL40A;_LL409: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9DF=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9DF->tag != 12)goto _LL40B;}_LL40A:
# 4637
 goto _LL40C;_LL40B: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9E0=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9E0->tag != 15)goto _LL40D;}_LL40C:
# 4638
 goto _LL40E;_LL40D: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9E1=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9E1->tag != 37)goto _LL40F;}_LL40E:
# 4639
 goto _LL410;_LL40F: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9E2=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp990;if(_tmp9E2->tag != 36)goto _LL3C2;}_LL410:
# 4641
{const char*_tmpE1C;void*_tmpE1B;(_tmpE1B=0,Cyc_Tcutil_terr(e->loc,((_tmpE1C="Cyclone expression within C code",_tag_dyneither(_tmpE1C,sizeof(char),33))),_tag_dyneither(_tmpE1B,sizeof(void*),0)));}
# 4642
goto _LL3C2;_LL3C2:;}
# 4646
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
# 4647
void*_tmp9E8=d->r;struct Cyc_Absyn_Vardecl*_tmp9EA;struct Cyc_Absyn_Fndecl*_tmp9EC;struct Cyc_Absyn_Aggrdecl*_tmp9EE;struct Cyc_Absyn_Enumdecl*_tmp9F0;struct Cyc_Absyn_Typedefdecl*_tmp9F2;_LL412: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9E9=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9E9->tag != 0)goto _LL414;else{_tmp9EA=_tmp9E9->f1;}}_LL413:
# 4649
 _tmp9EA->type=Cyc_Toc_typ_to_c(_tmp9EA->type);
# 4650
if(_tmp9EA->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9EA->initializer));
# 4651
goto _LL411;_LL414: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9EB=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9EB->tag != 1)goto _LL416;else{_tmp9EC=_tmp9EB->f1;}}_LL415:
# 4653
 _tmp9EC->ret_type=Cyc_Toc_typ_to_c(_tmp9EC->ret_type);
# 4654
{struct Cyc_List_List*_tmp9FE=_tmp9EC->args;for(0;_tmp9FE != 0;_tmp9FE=_tmp9FE->tl){
# 4655
(*((struct _tuple8*)_tmp9FE->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple8*)_tmp9FE->hd)).f3);}}
# 4657
goto _LL411;_LL416: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9ED=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9ED->tag != 6)goto _LL418;else{_tmp9EE=_tmp9ED->f1;}}_LL417:
# 4658
 Cyc_Toc_aggrdecl_to_c(_tmp9EE,1);goto _LL411;_LL418: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9EF=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9EF->tag != 8)goto _LL41A;else{_tmp9F0=_tmp9EF->f1;}}_LL419:
# 4660
 if(_tmp9F0->fields != 0){
# 4661
struct Cyc_List_List*_tmp9FF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9F0->fields))->v;for(0;_tmp9FF != 0;_tmp9FF=_tmp9FF->tl){
# 4662
struct Cyc_Absyn_Enumfield*_tmpA00=(struct Cyc_Absyn_Enumfield*)_tmp9FF->hd;
# 4663
if(_tmpA00->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA00->tag));}}
# 4665
goto _LL411;_LL41A: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9F1=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F1->tag != 9)goto _LL41C;else{_tmp9F2=_tmp9F1->f1;}}_LL41B:
# 4666
 _tmp9F2->defn=(void*)Cyc_Toc_typ_to_c_array((void*)_check_null(_tmp9F2->defn));goto _LL411;_LL41C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9F3=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F3->tag != 2)goto _LL41E;}_LL41D:
# 4667
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9F4=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F4->tag != 3)goto _LL420;}_LL41F:
# 4668
 goto _LL421;_LL420: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9F5=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F5->tag != 7)goto _LL422;}_LL421:
# 4669
 goto _LL423;_LL422: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9F6=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F6->tag != 10)goto _LL424;}_LL423:
# 4670
 goto _LL425;_LL424: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9F7=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F7->tag != 11)goto _LL426;}_LL425:
# 4671
 goto _LL427;_LL426: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9F8=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F8->tag != 12)goto _LL428;}_LL427:
# 4672
 goto _LL429;_LL428: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9F9=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9F9->tag != 13)goto _LL42A;}_LL429:
# 4673
 goto _LL42B;_LL42A: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9FA=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9FA->tag != 4)goto _LL42C;}_LL42B:
# 4674
 goto _LL42D;_LL42C: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp9FB=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9FB->tag != 5)goto _LL42E;}_LL42D:
# 4676
{const char*_tmpE1F;void*_tmpE1E;(_tmpE1E=0,Cyc_Tcutil_terr(d->loc,((_tmpE1F="Cyclone declaration within C code",_tag_dyneither(_tmpE1F,sizeof(char),34))),_tag_dyneither(_tmpE1E,sizeof(void*),0)));}
# 4677
goto _LL411;_LL42E: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9FC=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9FC->tag != 14)goto _LL430;}_LL42F:
# 4678
 goto _LL431;_LL430: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9FD=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9E8;if(_tmp9FD->tag != 15)goto _LL411;}_LL431:
# 4680
 goto _LL411;_LL411:;}
# 4684
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
# 4685
void*_tmpA03=s->r;struct Cyc_Absyn_Exp*_tmpA05;struct Cyc_Absyn_Stmt*_tmpA07;struct Cyc_Absyn_Stmt*_tmpA08;struct Cyc_Absyn_Exp*_tmpA0A;struct Cyc_Absyn_Exp*_tmpA0C;struct Cyc_Absyn_Stmt*_tmpA0D;struct Cyc_Absyn_Stmt*_tmpA0E;struct _tuple9 _tmpA10;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Stmt*_tmpA12;struct Cyc_Absyn_Exp*_tmpA14;struct _tuple9 _tmpA15;struct Cyc_Absyn_Exp*_tmpA16;struct _tuple9 _tmpA17;struct Cyc_Absyn_Exp*_tmpA18;struct Cyc_Absyn_Stmt*_tmpA19;struct Cyc_Absyn_Exp*_tmpA1B;struct Cyc_List_List*_tmpA1C;struct Cyc_Absyn_Decl*_tmpA1E;struct Cyc_Absyn_Stmt*_tmpA1F;struct Cyc_Absyn_Stmt*_tmpA21;struct _tuple9 _tmpA22;struct Cyc_Absyn_Exp*_tmpA23;_LL433: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA04=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA04->tag != 1)goto _LL435;else{_tmpA05=_tmpA04->f1;}}_LL434:
# 4686
 Cyc_Toc_exptypes_to_c(_tmpA05);goto _LL432;_LL435: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA06=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA06->tag != 2)goto _LL437;else{_tmpA07=_tmpA06->f1;_tmpA08=_tmpA06->f2;}}_LL436:
# 4687
 Cyc_Toc_stmttypes_to_c(_tmpA07);Cyc_Toc_stmttypes_to_c(_tmpA08);goto _LL432;_LL437: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmpA09=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA09->tag != 3)goto _LL439;else{_tmpA0A=_tmpA09->f1;}}_LL438:
# 4688
 if(_tmpA0A != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmpA0A);goto _LL432;_LL439: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpA0B=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA0B->tag != 4)goto _LL43B;else{_tmpA0C=_tmpA0B->f1;_tmpA0D=_tmpA0B->f2;_tmpA0E=_tmpA0B->f3;}}_LL43A:
# 4690
 Cyc_Toc_exptypes_to_c(_tmpA0C);Cyc_Toc_stmttypes_to_c(_tmpA0D);Cyc_Toc_stmttypes_to_c(_tmpA0E);goto _LL432;_LL43B: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmpA0F=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA0F->tag != 5)goto _LL43D;else{_tmpA10=_tmpA0F->f1;_tmpA11=_tmpA10.f1;_tmpA12=_tmpA0F->f2;}}_LL43C:
# 4692
 Cyc_Toc_exptypes_to_c(_tmpA11);Cyc_Toc_stmttypes_to_c(_tmpA12);goto _LL432;_LL43D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmpA13=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA13->tag != 9)goto _LL43F;else{_tmpA14=_tmpA13->f1;_tmpA15=_tmpA13->f2;_tmpA16=_tmpA15.f1;_tmpA17=_tmpA13->f3;_tmpA18=_tmpA17.f1;_tmpA19=_tmpA13->f4;}}_LL43E:
# 4694
 Cyc_Toc_exptypes_to_c(_tmpA14);Cyc_Toc_exptypes_to_c(_tmpA16);Cyc_Toc_exptypes_to_c(_tmpA18);
# 4695
Cyc_Toc_stmttypes_to_c(_tmpA19);goto _LL432;_LL43F: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmpA1A=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA1A->tag != 10)goto _LL441;else{_tmpA1B=_tmpA1A->f1;_tmpA1C=_tmpA1A->f2;}}_LL440:
# 4697
 Cyc_Toc_exptypes_to_c(_tmpA1B);
# 4698
for(0;_tmpA1C != 0;_tmpA1C=_tmpA1C->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmpA1C->hd)->body);}
# 4699
goto _LL432;_LL441: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpA1D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA1D->tag != 12)goto _LL443;else{_tmpA1E=_tmpA1D->f1;_tmpA1F=_tmpA1D->f2;}}_LL442:
# 4700
 Cyc_Toc_decltypes_to_c(_tmpA1E);Cyc_Toc_stmttypes_to_c(_tmpA1F);goto _LL432;_LL443: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmpA20=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA20->tag != 14)goto _LL445;else{_tmpA21=_tmpA20->f1;_tmpA22=_tmpA20->f2;_tmpA23=_tmpA22.f1;}}_LL444:
# 4701
 Cyc_Toc_stmttypes_to_c(_tmpA21);Cyc_Toc_exptypes_to_c(_tmpA23);goto _LL432;_LL445: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpA24=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA24->tag != 0)goto _LL447;}_LL446:
# 4702
 goto _LL448;_LL447: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmpA25=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA25->tag != 6)goto _LL449;}_LL448:
# 4703
 goto _LL44A;_LL449: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmpA26=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA26->tag != 7)goto _LL44B;}_LL44A:
# 4704
 goto _LL44C;_LL44B: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA27=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA27->tag != 8)goto _LL44D;}_LL44C:
# 4705
 goto _LL432;_LL44D: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmpA28=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA28->tag != 11)goto _LL44F;}_LL44E:
# 4706
 goto _LL450;_LL44F: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpA29=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA29->tag != 13)goto _LL451;}_LL450:
# 4707
 goto _LL452;_LL451: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmpA2A=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA2A->tag != 15)goto _LL453;}_LL452:
# 4708
 goto _LL454;_LL453: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpA2B=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmpA03;if(_tmpA2B->tag != 16)goto _LL432;}_LL454:
# 4709
{const char*_tmpE22;void*_tmpE21;(_tmpE21=0,Cyc_Tcutil_terr(s->loc,((_tmpE22="Cyclone statement in C code",_tag_dyneither(_tmpE22,sizeof(char),28))),_tag_dyneither(_tmpE21,sizeof(void*),0)));}
# 4710
goto _LL432;_LL432:;}
# 4718
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
# 4719
for(0;ds != 0;ds=ds->tl){
# 4720
if(!Cyc_Toc_is_toplevel(nv)){
# 4721
const char*_tmpE25;void*_tmpE24;(_tmpE24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE25="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE25,sizeof(char),29))),_tag_dyneither(_tmpE24,sizeof(void*),0)));}{
# 4722
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
# 4723
void*_tmpA30=d->r;struct Cyc_Absyn_Vardecl*_tmpA32;struct Cyc_Absyn_Fndecl*_tmpA34;struct Cyc_Absyn_Aggrdecl*_tmpA3A;struct Cyc_Absyn_Datatypedecl*_tmpA3C;struct Cyc_Absyn_Enumdecl*_tmpA3E;struct Cyc_Absyn_Typedefdecl*_tmpA40;struct Cyc_List_List*_tmpA44;struct Cyc_List_List*_tmpA46;struct Cyc_List_List*_tmpA48;struct Cyc_List_List*_tmpA4A;_LL456: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA31=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA31->tag != 0)goto _LL458;else{_tmpA32=_tmpA31->f1;}}_LL457: {
# 4725
struct _tuple0*_tmpA4B=_tmpA32->name;
# 4727
if(_tmpA32->sc == Cyc_Absyn_ExternC){
# 4728
struct _tuple0*_tmpE26;_tmpA4B=((_tmpE26=_cycalloc(sizeof(*_tmpE26)),((_tmpE26->f1=Cyc_Absyn_Rel_n(0),((_tmpE26->f2=(*_tmpA4B).f2,_tmpE26))))));}
# 4729
if(_tmpA32->initializer != 0){
# 4730
if(cinclude)
# 4731
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA32->initializer));else{
# 4733
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA32->initializer));}}
# 4735
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE29;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE28;nv=Cyc_Toc_add_varmap(r,nv,_tmpA32->name,Cyc_Absyn_varb_exp(_tmpA4B,(void*)((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28[0]=((_tmpE29.tag=1,((_tmpE29.f1=_tmpA32,_tmpE29)))),_tmpE28)))),0));}
# 4736
_tmpA32->name=_tmpA4B;
# 4737
_tmpA32->sc=Cyc_Toc_scope_to_c(_tmpA32->sc);
# 4738
_tmpA32->type=Cyc_Toc_typ_to_c_array(_tmpA32->type);
# 4739
{struct Cyc_List_List*_tmpE2A;Cyc_Toc_result_decls=((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A->hd=d,((_tmpE2A->tl=Cyc_Toc_result_decls,_tmpE2A))))));}
# 4740
goto _LL455;}_LL458: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA33=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA33->tag != 1)goto _LL45A;else{_tmpA34=_tmpA33->f1;}}_LL459: {
# 4742
struct _tuple0*_tmpA50=_tmpA34->name;
# 4744
if(_tmpA34->sc == Cyc_Absyn_ExternC){
# 4745
{struct _tuple0*_tmpE2B;_tmpA50=((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE2B->f2=(*_tmpA50).f2,_tmpE2B))))));}
# 4746
_tmpA34->sc=Cyc_Absyn_Public;}
# 4748
nv=Cyc_Toc_add_varmap(r,nv,_tmpA34->name,Cyc_Absyn_var_exp(_tmpA50,0));
# 4749
_tmpA34->name=_tmpA50;
# 4750
Cyc_Toc_fndecl_to_c(nv,_tmpA34,cinclude);
# 4751
{struct Cyc_List_List*_tmpE2C;Cyc_Toc_result_decls=((_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->hd=d,((_tmpE2C->tl=Cyc_Toc_result_decls,_tmpE2C))))));}
# 4752
goto _LL455;}_LL45A: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA35=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA35->tag != 2)goto _LL45C;}_LL45B:
# 4753
 goto _LL45D;_LL45C: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA36=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA36->tag != 3)goto _LL45E;}_LL45D: {
# 4755
const char*_tmpE2F;void*_tmpE2E;(_tmpE2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE2F="letdecl at toplevel",_tag_dyneither(_tmpE2F,sizeof(char),20))),_tag_dyneither(_tmpE2E,sizeof(void*),0)));}_LL45E: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA37=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA37->tag != 4)goto _LL460;}_LL45F: {
# 4757
const char*_tmpE32;void*_tmpE31;(_tmpE31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE32="region decl at toplevel",_tag_dyneither(_tmpE32,sizeof(char),24))),_tag_dyneither(_tmpE31,sizeof(void*),0)));}_LL460: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmpA38=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA38->tag != 5)goto _LL462;}_LL461: {
# 4759
const char*_tmpE35;void*_tmpE34;(_tmpE34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE35="alias decl at toplevel",_tag_dyneither(_tmpE35,sizeof(char),23))),_tag_dyneither(_tmpE34,sizeof(void*),0)));}_LL462: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA39=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA39->tag != 6)goto _LL464;else{_tmpA3A=_tmpA39->f1;}}_LL463:
# 4761
 Cyc_Toc_aggrdecl_to_c(_tmpA3A,1);
# 4762
goto _LL455;_LL464: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA3B=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA3B->tag != 7)goto _LL466;else{_tmpA3C=_tmpA3B->f1;}}_LL465:
# 4764
 if(_tmpA3C->is_extensible)
# 4765
Cyc_Toc_xdatatypedecl_to_c(_tmpA3C);else{
# 4767
Cyc_Toc_datatypedecl_to_c(_tmpA3C);}
# 4768
goto _LL455;_LL466: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA3D=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA3D->tag != 8)goto _LL468;else{_tmpA3E=_tmpA3D->f1;}}_LL467:
# 4770
 Cyc_Toc_enumdecl_to_c(_tmpA3E);
# 4771
{struct Cyc_List_List*_tmpE36;Cyc_Toc_result_decls=((_tmpE36=_cycalloc(sizeof(*_tmpE36)),((_tmpE36->hd=d,((_tmpE36->tl=Cyc_Toc_result_decls,_tmpE36))))));}
# 4772
goto _LL455;_LL468: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA3F=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA3F->tag != 9)goto _LL46A;else{_tmpA40=_tmpA3F->f1;}}_LL469:
# 4774
 _tmpA40->name=_tmpA40->name;
# 4775
_tmpA40->tvs=0;
# 4776
if(_tmpA40->defn != 0)
# 4777
_tmpA40->defn=(void*)Cyc_Toc_typ_to_c_array((void*)_check_null(_tmpA40->defn));else{
# 4779
switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA40->kind))->v)->kind){case Cyc_Absyn_BoxKind: _LL476:
# 4780
 _tmpA40->defn=(void*)Cyc_Absyn_void_star_typ();break;default: _LL477:
# 4781
 _tmpA40->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 4784
{struct Cyc_List_List*_tmpE37;Cyc_Toc_result_decls=((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37->hd=d,((_tmpE37->tl=Cyc_Toc_result_decls,_tmpE37))))));}
# 4785
goto _LL455;_LL46A: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA41=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA41->tag != 14)goto _LL46C;}_LL46B:
# 4786
 goto _LL46D;_LL46C: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA42=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA42->tag != 15)goto _LL46E;}_LL46D:
# 4788
 goto _LL455;_LL46E: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA43=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA43->tag != 10)goto _LL470;else{_tmpA44=_tmpA43->f2;}}_LL46F:
# 4789
 _tmpA46=_tmpA44;goto _LL471;_LL470: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA45=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA45->tag != 11)goto _LL472;else{_tmpA46=_tmpA45->f2;}}_LL471:
# 4790
 _tmpA48=_tmpA46;goto _LL473;_LL472: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA47=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA47->tag != 12)goto _LL474;else{_tmpA48=_tmpA47->f1;}}_LL473:
# 4791
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA48,top,cinclude);goto _LL455;_LL474: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA49=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA30;if(_tmpA49->tag != 13)goto _LL455;else{_tmpA4A=_tmpA49->f1;}}_LL475:
# 4792
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA4A,top,1);goto _LL455;_LL455:;};}
# 4795
return nv;}
# 4799
static void Cyc_Toc_init(){
# 4800
struct _DynRegionHandle*_tmpA5C;struct Cyc_Core_NewRegion _tmpA5B=Cyc_Core__new_dynregion((const char*)"internal-error",0);_tmpA5C=_tmpA5B.dynregion;{
# 4801
struct _DynRegionFrame _tmpA5D;struct _RegionHandle*d=_open_dynregion(& _tmpA5D,_tmpA5C);{
# 4802
struct Cyc_Dict_Dict*_tmpE44;struct Cyc_Dict_Dict*_tmpE43;struct Cyc_Set_Set**_tmpE42;struct Cyc_List_List**_tmpE41;struct Cyc_Dict_Dict*_tmpE40;struct Cyc_List_List**_tmpE3F;struct Cyc_Toc_TocState*_tmpE3E;Cyc_Toc_toc_state=(
# 4803
(_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->dyn=(struct _DynRegionHandle*)_tmpA5C,((_tmpE3E->tuple_types=(struct Cyc_List_List**)(
# 4804
(_tmpE3F=_region_malloc(d,sizeof(*_tmpE3F)),((_tmpE3F[0]=0,_tmpE3F)))),((_tmpE3E->aggrs_so_far=(struct Cyc_Dict_Dict*)(
# 4805
(_tmpE40=_region_malloc(d,sizeof(*_tmpE40)),((_tmpE40[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE40)))),((_tmpE3E->abs_struct_types=(struct Cyc_List_List**)(
# 4806
(_tmpE41=_region_malloc(d,sizeof(*_tmpE41)),((_tmpE41[0]=0,_tmpE41)))),((_tmpE3E->datatypes_so_far=(struct Cyc_Set_Set**)(
# 4807
(_tmpE42=_region_malloc(d,sizeof(*_tmpE42)),((_tmpE42[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE42)))),((_tmpE3E->xdatatypes_so_far=(struct Cyc_Dict_Dict*)(
# 4808
(_tmpE43=_region_malloc(d,sizeof(*_tmpE43)),((_tmpE43[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpE43)))),((_tmpE3E->qvar_tags=(struct Cyc_Dict_Dict*)(
# 4809
(_tmpE44=_region_malloc(d,sizeof(*_tmpE44)),((_tmpE44[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpE44)))),((_tmpE3E->temp_labels=(struct Cyc_Xarray_Xarray*)
# 4810
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),(struct Cyc_Toc_TocState*)_tmpE3E))))))))))))))))));}
# 4813
Cyc_Toc_result_decls=0;
# 4814
Cyc_Toc_tuple_type_counter=0;
# 4815
Cyc_Toc_temp_var_counter=0;
# 4816
Cyc_Toc_fresh_label_counter=0;
# 4817
Cyc_Toc_total_bounds_checks=0;
# 4818
Cyc_Toc_bounds_checks_eliminated=0;{
# 4819
struct _dyneither_ptr**_tmpE45;Cyc_Toc_globals=_tag_dyneither(((_tmpE45=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE45[0]=& Cyc_Toc__throw_str,((_tmpE45[1]=& Cyc_Toc_setjmp_str,((_tmpE45[2]=& Cyc_Toc__push_handler_str,((_tmpE45[3]=& Cyc_Toc__pop_handler_str,((_tmpE45[4]=& Cyc_Toc__exn_thrown_str,((_tmpE45[5]=& Cyc_Toc__npop_handler_str,((_tmpE45[6]=& Cyc_Toc__check_null_str,((_tmpE45[7]=& Cyc_Toc__check_known_subscript_null_str,((_tmpE45[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE45[9]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpE45[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE45[11]=& Cyc_Toc__tag_dyneither_str,((_tmpE45[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE45[13]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE45[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE45[15]=& Cyc_Toc__get_zero_arr_size_str,((_tmpE45[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE45[17]=& Cyc_Toc__zero_arr_plus_str,((_tmpE45[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE45[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE45[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE45[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE45[22]=& Cyc_Toc__cycalloc_str,((_tmpE45[23]=& Cyc_Toc__cyccalloc_str,((_tmpE45[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE45[25]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpE45[26]=& Cyc_Toc__region_malloc_str,((_tmpE45[27]=& Cyc_Toc__region_calloc_str,((_tmpE45[28]=& Cyc_Toc__check_times_str,((_tmpE45[29]=& Cyc_Toc__new_region_str,((_tmpE45[30]=& Cyc_Toc__push_region_str,((_tmpE45[31]=& Cyc_Toc__pop_region_str,((_tmpE45[32]=& Cyc_Toc__open_dynregion_str,((_tmpE45[33]=& Cyc_Toc__push_dynregion_str,((_tmpE45[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE45[35]=& Cyc_Toc__reset_region_str,((_tmpE45[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE45[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE45[38]=& Cyc_Toc__throw_match_str,((_tmpE45[39]=& Cyc_Toc__swap_word_str,((_tmpE45[40]=& Cyc_Toc__swap_dyneither_str,_tmpE45)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};
# 4801
;_pop_dynregion(d);};}
# 4866
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
# 4867
Cyc_Toc_init();{
# 4868
struct _RegionHandle _tmpA66=_new_region("start");struct _RegionHandle*start=& _tmpA66;_push_region(start);
# 4869
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 4875
struct _DynRegionHandle*_tmpA68;struct Cyc_Toc_TocState _tmpA67=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA68=_tmpA67.dyn;
# 4876
Cyc_Core_free_dynregion(_tmpA68);};{
# 4878
struct Cyc_List_List*_tmpA69=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA69;};
# 4868
;_pop_region(start);};}
