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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244 "core.h"
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
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 282 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 907 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 953
extern struct _tuple0*Cyc_Absyn_exn_name;
# 984
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 69 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 65 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 170
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 35 "toc.h"
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 79 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};
# 92
typedef struct Cyc_Tovc_BoxingEnv Cyc_Tovc_boxingenv_t;struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};
# 101
typedef struct Cyc_Tovc_ToExpEnv Cyc_Tovc_toexpenv_t;struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 107
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;struct _dyneither_ptr*_tmp2;struct Cyc_Absyn_Exp*_tmp5;struct Cyc_Absyn_Exp*_tmp6;struct Cyc_Absyn_Exp*_tmp8;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_Absyn_Exp*_tmpC;struct _dyneither_ptr*_tmpD;int _tmpE;int _tmpF;struct Cyc_Absyn_Exp*_tmp11;struct _dyneither_ptr*_tmp12;int _tmp13;int _tmp14;struct Cyc_Absyn_Exp**_tmp17;struct Cyc_List_List*_tmp18;struct Cyc_List_List*_tmp1A;struct Cyc_Absyn_Exp**_tmp1C;struct Cyc_Absyn_Exp**_tmp1D;struct Cyc_Absyn_Exp**_tmp1E;struct Cyc_Absyn_Exp**_tmp20;struct Cyc_Absyn_Exp**_tmp21;struct Cyc_Absyn_Exp**_tmp23;struct Cyc_Absyn_Exp**_tmp24;struct Cyc_Absyn_Exp**_tmp26;struct Cyc_Absyn_Exp**_tmp27;struct Cyc_Absyn_Exp**_tmp29;struct Cyc_Absyn_Exp**_tmp2A;struct Cyc_Absyn_Exp**_tmp2C;struct Cyc_Absyn_Exp**_tmp2E;struct Cyc_Absyn_Exp**_tmp30;struct Cyc_Absyn_Exp**_tmp32;struct Cyc_Absyn_Exp**_tmp34;struct Cyc_List_List*_tmp39;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 1)goto _LL3;else{if((((_tmp1->f1)->f1).Loc_n).tag != 4)goto _LL3;_tmp2=(_tmp1->f1)->f2;}}_LL2:
# 113
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(env.varmap,_tmp2)){
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmp2))->r;
continue;}
# 118
{int i=0;for(0;i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){
if(Cyc_strptrcmp(_tmp2,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*),i)))== 0)
return 0;}}
# 122
for(0;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(_tmp2,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return 0;}
# 126
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp3A=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmp2,(*_tmp3A->name).f2)== 0){
*env.freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp3A);{
# 131
void*_tmp3B=_tmp3A->type;_LL34: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B;if(_tmp3C->tag != 8)goto _LL36;}_LL35:
 return 0;_LL36:;_LL37:
 return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp3A->name,0),0);_LL33:;};}}
# 140
for(0;env.all_locals != 0;env.all_locals=((struct Cyc_List_List*)_check_null(env.all_locals))->tl){
if(Cyc_strptrcmp(_tmp2,(*((struct _tuple0*)_check_null(((struct Cyc_Absyn_Vardecl*)_check_null((struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(env.all_locals))->hd))->name))).f2)== 0){
struct Cyc_Core_Impossible_exn_struct _tmp19A;const char*_tmp199;void*_tmp198[1];struct Cyc_String_pa_PrintArg_struct _tmp197;struct Cyc_Core_Impossible_exn_struct*_tmp196;(int)_throw((void*)((_tmp196=_cycalloc(sizeof(*_tmp196)),((_tmp196[0]=((_tmp19A.tag=Cyc_Core_Impossible,((_tmp19A.f1=(struct _dyneither_ptr)((_tmp197.tag=0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(_tmp2))),((_tmp198[0]=& _tmp197,Cyc_aprintf(((_tmp199="unbound variable %s found in box_free_vars_exp",_tag_dyneither(_tmp199,sizeof(char),47))),_tag_dyneither(_tmp198,sizeof(void*),1)))))))),_tmp19A)))),_tmp196)))));}}
# 147
return 0;_LL3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp3->tag != 1)goto _LL5;}_LL4:
# 150
 return 0;_LL5: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp4->tag != 22)goto _LL7;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;}}_LL6: {
# 155
struct Cyc_Absyn_Exp*_tmp42=Cyc_Tovc_box_free_vars_exp(env,_tmp5);
struct Cyc_Absyn_Exp*_tmp43=Cyc_Tovc_box_free_vars_exp(env,_tmp6);
if(_tmp42 != 0){
if(_tmp43 != 0){
struct Cyc_Absyn_Exp*_tmp44=Cyc_Absyn_subscript_exp(_tmp42,_tmp43,e->loc);
_tmp44->topt=e->topt;
_tmp44->annot=e->annot;
return _tmp44;}else{
# 165
struct Cyc_Absyn_Exp*_tmp45=Cyc_Absyn_subscript_exp(_tmp42,_tmp6,e->loc);
_tmp45->topt=e->topt;
_tmp45->annot=e->annot;
return _tmp45;}}else{
# 171
if(_tmp43 != 0){
struct Cyc_Absyn_Exp*_tmp46=Cyc_Absyn_subscript_exp(_tmp5,_tmp43,e->loc);
_tmp46->topt=e->topt;
_tmp46->annot=e->annot;
return _tmp46;}}
# 177
return 0;}_LL7: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp7=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp7->tag != 14)goto _LL9;else{_tmp8=_tmp7->f1;}}_LL8: {
# 180
struct Cyc_Absyn_Exp*_tmp47=Cyc_Tovc_box_free_vars_exp(env,_tmp8);
if(_tmp47 != 0){
struct Cyc_Absyn_Exp*_tmp48=Cyc_Absyn_address_exp(_tmp47,e->loc);
_tmp48->topt=e->topt;
_tmp48->annot=e->annot;
return _tmp48;}
# 187
return 0;}_LL9: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp9=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp9->tag != 19)goto _LLB;else{_tmpA=_tmp9->f1;}}_LLA: {
# 190
struct Cyc_Absyn_Exp*_tmp49=Cyc_Tovc_box_free_vars_exp(env,_tmpA);
if(_tmp49 != 0){
struct Cyc_Absyn_Exp*_tmp4A=Cyc_Absyn_deref_exp(_tmp49,e->loc);
_tmp4A->topt=e->topt;
_tmp4A->annot=e->annot;
return _tmp4A;}
# 197
return 0;}_LLB: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpB=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpB->tag != 20)goto _LLD;else{_tmpC=_tmpB->f1;_tmpD=_tmpB->f2;_tmpE=_tmpB->f3;_tmpF=_tmpB->f4;}}_LLC: {
# 200
struct Cyc_Absyn_Exp*_tmp4B=Cyc_Tovc_box_free_vars_exp(env,_tmpC);
if(_tmp4B != 0){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp19D;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp19C;struct Cyc_Absyn_Exp*_tmp4C=Cyc_Absyn_new_exp((void*)((_tmp19C=_cycalloc(sizeof(*_tmp19C)),((_tmp19C[0]=((_tmp19D.tag=20,((_tmp19D.f1=_tmp4B,((_tmp19D.f2=_tmpD,((_tmp19D.f3=_tmpE,((_tmp19D.f4=_tmpF,_tmp19D)))))))))),_tmp19C)))),e->loc);
_tmp4C->topt=e->topt;
_tmp4C->annot=e->annot;
return _tmp4C;}
# 207
return 0;}_LLD: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp10=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp10->tag != 21)goto _LLF;else{_tmp11=_tmp10->f1;_tmp12=_tmp10->f2;_tmp13=_tmp10->f3;_tmp14=_tmp10->f4;}}_LLE: {
# 210
struct Cyc_Absyn_Exp*_tmp4F=Cyc_Tovc_box_free_vars_exp(env,_tmp11);
if(_tmp4F != 0){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp1A0;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp19F;struct Cyc_Absyn_Exp*_tmp50=Cyc_Absyn_new_exp((void*)((_tmp19F=_cycalloc(sizeof(*_tmp19F)),((_tmp19F[0]=((_tmp1A0.tag=21,((_tmp1A0.f1=_tmp4F,((_tmp1A0.f2=_tmp12,((_tmp1A0.f3=_tmp13,((_tmp1A0.f4=_tmp14,_tmp1A0)))))))))),_tmp19F)))),e->loc);
_tmp50->topt=e->topt;
_tmp50->annot=e->annot;
return _tmp50;}
# 217
return 0;}_LLF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp15=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp15->tag != 0)goto _LL11;}_LL10:
# 220
 return 0;_LL11: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp16=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp16->tag != 9)goto _LL13;else{_tmp17=(struct Cyc_Absyn_Exp**)& _tmp16->f1;_tmp18=_tmp16->f2;}}_LL12:
# 222
{struct Cyc_Absyn_Exp*_tmp53=Cyc_Tovc_box_free_vars_exp(env,*_tmp17);if(_tmp53 != 0)*_tmp17=_tmp53;}
_tmp1A=_tmp18;goto _LL14;_LL13: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp19=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp19->tag != 2)goto _LL15;else{_tmp1A=_tmp19->f2;}}_LL14:
# 225
 for(0;_tmp1A != 0;_tmp1A=_tmp1A->tl){
struct Cyc_Absyn_Exp*_tmp54=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp1A->hd);if(_tmp54 != 0)_tmp1A->hd=(void*)_tmp54;}
# 228
return 0;_LL15: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1B->tag != 5)goto _LL17;else{_tmp1C=(struct Cyc_Absyn_Exp**)& _tmp1B->f1;_tmp1D=(struct Cyc_Absyn_Exp**)& _tmp1B->f2;_tmp1E=(struct Cyc_Absyn_Exp**)& _tmp1B->f3;}}_LL16:
# 231
{struct Cyc_Absyn_Exp*_tmp55=Cyc_Tovc_box_free_vars_exp(env,*_tmp1E);if(_tmp55 != 0)*_tmp1E=_tmp55;}
_tmp20=_tmp1C;_tmp21=_tmp1D;goto _LL18;_LL17: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1F->tag != 6)goto _LL19;else{_tmp20=(struct Cyc_Absyn_Exp**)& _tmp1F->f1;_tmp21=(struct Cyc_Absyn_Exp**)& _tmp1F->f2;}}_LL18:
 _tmp23=_tmp20;_tmp24=_tmp21;goto _LL1A;_LL19: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp22=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp22->tag != 7)goto _LL1B;else{_tmp23=(struct Cyc_Absyn_Exp**)& _tmp22->f1;_tmp24=(struct Cyc_Absyn_Exp**)& _tmp22->f2;}}_LL1A:
 _tmp26=_tmp23;_tmp27=_tmp24;goto _LL1C;_LL1B: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp25=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp25->tag != 8)goto _LL1D;else{_tmp26=(struct Cyc_Absyn_Exp**)& _tmp25->f1;_tmp27=(struct Cyc_Absyn_Exp**)& _tmp25->f2;}}_LL1C:
 _tmp29=_tmp26;_tmp2A=_tmp27;goto _LL1E;_LL1D: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp28=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp28->tag != 3)goto _LL1F;else{_tmp29=(struct Cyc_Absyn_Exp**)& _tmp28->f1;_tmp2A=(struct Cyc_Absyn_Exp**)& _tmp28->f3;}}_LL1E:
# 237
{struct Cyc_Absyn_Exp*_tmp56=Cyc_Tovc_box_free_vars_exp(env,*_tmp2A);if(_tmp56 != 0)*_tmp2A=_tmp56;}
_tmp2C=_tmp29;goto _LL20;_LL1F: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp2B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2B->tag != 11)goto _LL21;else{_tmp2C=(struct Cyc_Absyn_Exp**)& _tmp2B->f1;}}_LL20:
# 240
 _tmp2E=_tmp2C;goto _LL22;_LL21: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp2D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2D->tag != 12)goto _LL23;else{_tmp2E=(struct Cyc_Absyn_Exp**)& _tmp2D->f1;}}_LL22:
 _tmp30=_tmp2E;goto _LL24;_LL23: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2F->tag != 13)goto _LL25;else{_tmp30=(struct Cyc_Absyn_Exp**)& _tmp2F->f2;}}_LL24:
 _tmp32=_tmp30;goto _LL26;_LL25: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp31=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp31->tag != 17)goto _LL27;else{_tmp32=(struct Cyc_Absyn_Exp**)& _tmp31->f1;}}_LL26:
 _tmp34=_tmp32;goto _LL28;_LL27: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp33=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp33->tag != 4)goto _LL29;else{_tmp34=(struct Cyc_Absyn_Exp**)& _tmp33->f1;}}_LL28:
# 245
{struct Cyc_Absyn_Exp*_tmp57=Cyc_Tovc_box_free_vars_exp(env,*_tmp34);if(_tmp57 != 0)*_tmp34=_tmp57;}
return 0;_LL29: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp35->tag != 16)goto _LL2B;}_LL2A:
# 248
 goto _LL2C;_LL2B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp36=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp36->tag != 18)goto _LL2D;}_LL2C:
 goto _LL2E;_LL2D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp37=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp37->tag != 31)goto _LL2F;}_LL2E:
 return 0;_LL2F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp38=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp38->tag != 35)goto _LL31;else{_tmp39=_tmp38->f2;}}_LL30:
# 253
 for(0;_tmp39 != 0;_tmp39=_tmp39->tl){
struct Cyc_Absyn_Exp*_tmp58=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp39->hd)).f2);if(_tmp58 != 0)(*((struct _tuple10*)_tmp39->hd)).f2=_tmp58;}
# 256
return 0;_LL31:;_LL32: {
# 258
struct Cyc_Core_Impossible_exn_struct _tmp1A6;const char*_tmp1A5;struct Cyc_Core_Impossible_exn_struct*_tmp1A4;(int)_throw((void*)((_tmp1A4=_cycalloc(sizeof(*_tmp1A4)),((_tmp1A4[0]=((_tmp1A6.tag=Cyc_Core_Impossible,((_tmp1A6.f1=((_tmp1A5="bad exp form in Tocv::box_free_vars_exp",_tag_dyneither(_tmp1A5,sizeof(char),40))),_tmp1A6)))),_tmp1A4)))));}_LL0:;}}
# 262
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 266
while(1){
void*_tmp5C=s->r;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Exp**_tmp64;struct Cyc_Absyn_Exp**_tmp66;struct Cyc_Absyn_Stmt*_tmp68;struct Cyc_Absyn_Stmt*_tmp69;struct Cyc_Absyn_Exp**_tmp6B;struct Cyc_Absyn_Stmt*_tmp6C;struct Cyc_Absyn_Stmt*_tmp6D;struct Cyc_Absyn_Exp**_tmp6F;struct Cyc_Absyn_Stmt*_tmp70;struct Cyc_Absyn_Exp**_tmp72;struct Cyc_Absyn_Exp**_tmp73;struct Cyc_Absyn_Exp**_tmp74;struct Cyc_Absyn_Stmt*_tmp75;struct Cyc_Absyn_Stmt*_tmp77;struct Cyc_Absyn_Exp**_tmp78;struct Cyc_Absyn_Exp**_tmp7A;struct Cyc_List_List*_tmp7B;struct Cyc_Absyn_Decl*_tmp7D;struct Cyc_Absyn_Stmt*_tmp7E;_LL39: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp5D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp5D->tag != 0)goto _LL3B;}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp5E=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp5E->tag != 6)goto _LL3D;}_LL3C:
 goto _LL3E;_LL3D: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp5F=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp5F->tag != 7)goto _LL3F;}_LL3E:
 return;_LL3F: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp60=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp60->tag != 8)goto _LL41;}_LL40:
# 273
 return;_LL41: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp61=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp61->tag != 13)goto _LL43;else{_tmp62=_tmp61->f2;}}_LL42:
# 275
 s=_tmp62;
continue;_LL43: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp63=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp63->tag != 3)goto _LL45;else{_tmp64=(struct Cyc_Absyn_Exp**)& _tmp63->f1;}}_LL44:
# 279
 if(*_tmp64 != 0){
struct Cyc_Absyn_Exp*_tmp7F=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*_tmp64));
if(_tmp7F != 0)*_tmp64=_tmp7F;}
# 283
return;_LL45: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp65=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp65->tag != 1)goto _LL47;else{_tmp66=(struct Cyc_Absyn_Exp**)& _tmp65->f1;}}_LL46:
# 285
{struct Cyc_Absyn_Exp*_tmp80=Cyc_Tovc_box_free_vars_exp(env,*_tmp66);if(_tmp80 != 0)*_tmp66=_tmp80;}
return;_LL47: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp67=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp67->tag != 2)goto _LL49;else{_tmp68=_tmp67->f1;_tmp69=_tmp67->f2;}}_LL48:
# 288
 Cyc_Tovc_box_free_vars_stmt(env,_tmp68);
s=_tmp69;
continue;_LL49: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp6A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp6A->tag != 4)goto _LL4B;else{_tmp6B=(struct Cyc_Absyn_Exp**)& _tmp6A->f1;_tmp6C=_tmp6A->f2;_tmp6D=_tmp6A->f3;}}_LL4A:
# 292
{struct Cyc_Absyn_Exp*_tmp81=Cyc_Tovc_box_free_vars_exp(env,*_tmp6B);if(_tmp81 != 0)*_tmp6B=_tmp81;}
Cyc_Tovc_box_free_vars_stmt(env,_tmp6C);
s=_tmp6D;
continue;_LL4B: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp6E=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp6E->tag != 5)goto _LL4D;else{_tmp6F=(struct Cyc_Absyn_Exp**)&(_tmp6E->f1).f1;_tmp70=_tmp6E->f2;}}_LL4C:
# 297
{struct Cyc_Absyn_Exp*_tmp82=Cyc_Tovc_box_free_vars_exp(env,*_tmp6F);if(_tmp82 != 0)*_tmp6F=_tmp82;}
s=_tmp70;
continue;_LL4D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp71=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp71->tag != 9)goto _LL4F;else{_tmp72=(struct Cyc_Absyn_Exp**)& _tmp71->f1;_tmp73=(struct Cyc_Absyn_Exp**)&(_tmp71->f2).f1;_tmp74=(struct Cyc_Absyn_Exp**)&(_tmp71->f3).f1;_tmp75=_tmp71->f4;}}_LL4E:
# 301
{struct Cyc_Absyn_Exp*_tmp83=Cyc_Tovc_box_free_vars_exp(env,*_tmp72);if(_tmp83 != 0)*_tmp72=_tmp83;}
{struct Cyc_Absyn_Exp*_tmp84=Cyc_Tovc_box_free_vars_exp(env,*_tmp73);if(_tmp84 != 0)*_tmp73=_tmp84;}
{struct Cyc_Absyn_Exp*_tmp85=Cyc_Tovc_box_free_vars_exp(env,*_tmp74);if(_tmp85 != 0)*_tmp74=_tmp85;}
s=_tmp75;
continue;_LL4F: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp76=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp76->tag != 14)goto _LL51;else{_tmp77=_tmp76->f1;_tmp78=(struct Cyc_Absyn_Exp**)&(_tmp76->f2).f1;}}_LL50:
# 307
{struct Cyc_Absyn_Exp*_tmp86=Cyc_Tovc_box_free_vars_exp(env,*_tmp78);if(_tmp86 != 0)*_tmp78=_tmp86;}
s=_tmp77;
continue;_LL51: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp79=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp79->tag != 10)goto _LL53;else{_tmp7A=(struct Cyc_Absyn_Exp**)& _tmp79->f1;_tmp7B=_tmp79->f2;}}_LL52:
# 312
{struct Cyc_Absyn_Exp*_tmp87=Cyc_Tovc_box_free_vars_exp(env,*_tmp7A);if(_tmp87 != 0)*_tmp7A=_tmp87;}
for(0;_tmp7B != 0;_tmp7B=_tmp7B->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp7B->hd)->body);}
return;_LL53: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp7C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5C;if(_tmp7C->tag != 12)goto _LL55;else{_tmp7D=_tmp7C->f1;_tmp7E=_tmp7C->f2;}}_LL54:
# 317
{void*_tmp88=_tmp7D->r;struct Cyc_Absyn_Vardecl*_tmp8A;_LL58: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp89=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp88;if(_tmp89->tag != 0)goto _LL5A;else{_tmp8A=_tmp89->f1;}}_LL59:
# 319
{struct Cyc_List_List*_tmp1A7;env.boundvars=((_tmp1A7=_region_malloc(env.rgn,sizeof(*_tmp1A7)),((_tmp1A7->hd=_tmp8A,((_tmp1A7->tl=env.boundvars,_tmp1A7))))));}
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp8A->name).f2);
if(_tmp8A->initializer != 0){
struct Cyc_Absyn_Exp*_tmp8C=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp8A->initializer));
if(_tmp8C != 0)_tmp8A->initializer=_tmp8C;}
# 325
s=_tmp7E;
continue;_LL5A:;_LL5B:
 goto _LL57;_LL57:;}
# 329
goto _LL56;_LL55:;_LL56: {
struct Cyc_Core_Impossible_exn_struct _tmp1AD;const char*_tmp1AC;struct Cyc_Core_Impossible_exn_struct*_tmp1AB;(int)_throw((void*)((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB[0]=((_tmp1AD.tag=Cyc_Core_Impossible,((_tmp1AD.f1=((_tmp1AC="bad stmt after xlation to C",_tag_dyneither(_tmp1AC,sizeof(char),28))),_tmp1AD)))),_tmp1AB)))));}_LL38:;}}
# 340
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 344
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 347
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp1AE;struct Cyc_Tovc_BoxingEnv _tmp90=(_tmp1AE.rgn=env.rgn,((_tmp1AE.all_locals=env.all_locals,((_tmp1AE.varmap=env.varmap,((_tmp1AE.boundvars=0,((_tmp1AE.freevars=& freevars,_tmp1AE)))))))));
Cyc_Tovc_box_free_vars_stmt(_tmp90,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 353
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 355
struct Cyc_Absyn_Vardecl*_tmp1AF;struct Cyc_Absyn_Vardecl*_tmp91=(_tmp1AF=_cycalloc(sizeof(*_tmp1AF)),((_tmp1AF->sc=Cyc_Absyn_Public,((_tmp1AF->name=Cyc_Absyn_exn_name,((_tmp1AF->tq=Cyc_Absyn_empty_tqual(0),((_tmp1AF->type=(void*)& Cyc_Absyn_VoidType_val,((_tmp1AF->initializer=0,((_tmp1AF->rgn=0,((_tmp1AF->attributes=0,((_tmp1AF->escapes=0,_tmp1AF)))))))))))))))));
# 358
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp91)){
void*_tmp92=_tmp91->type;void*_tmp94;_LL5D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp92;if(_tmp93->tag != 8)goto _LL5F;else{_tmp94=(_tmp93->f1).elt_type;}}_LL5E:
# 361
{struct _tuple8*_tmp1B2;struct Cyc_List_List*_tmp1B1;params=((_tmp1B1=_cycalloc(sizeof(*_tmp1B1)),((_tmp1B1->hd=((_tmp1B2=_cycalloc(sizeof(*_tmp1B2)),((_tmp1B2->f1=(*_tmp91->name).f2,((_tmp1B2->f2=_tmp91->tq,((_tmp1B2->f3=
Cyc_Absyn_cstar_typ(_tmp94,Cyc_Absyn_empty_tqual(0)),_tmp1B2)))))))),((_tmp1B1->tl=params,_tmp1B1))))));}
# 364
{struct Cyc_List_List*_tmp1B3;args=((_tmp1B3=_cycalloc(sizeof(*_tmp1B3)),((_tmp1B3->hd=Cyc_Absyn_var_exp(_tmp91->name,0),((_tmp1B3->tl=args,_tmp1B3))))));}
goto _LL5C;_LL5F:;_LL60:
# 367
{struct _tuple8*_tmp1B6;struct Cyc_List_List*_tmp1B5;params=((_tmp1B5=_cycalloc(sizeof(*_tmp1B5)),((_tmp1B5->hd=((_tmp1B6=_cycalloc(sizeof(*_tmp1B6)),((_tmp1B6->f1=(*_tmp91->name).f2,((_tmp1B6->f2=_tmp91->tq,((_tmp1B6->f3=
Cyc_Absyn_cstar_typ(_tmp91->type,Cyc_Absyn_empty_tqual(0)),_tmp1B6)))))))),((_tmp1B5->tl=params,_tmp1B5))))));}
# 370
{struct Cyc_List_List*_tmp1B7;args=((_tmp1B7=_cycalloc(sizeof(*_tmp1B7)),((_tmp1B7->hd=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp91->name,0),0),((_tmp1B7->tl=args,_tmp1B7))))));}
goto _LL5C;_LL5C:;}{
# 373
struct _tuple0*funname=Cyc_Toc_temp_var();
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp1C1;struct Cyc_Absyn_Fndecl*_tmp1C0;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp1BF;struct Cyc_List_List*_tmp1BE;*env.gen_ds=((_tmp1BE=_cycalloc(sizeof(*_tmp1BE)),((_tmp1BE->hd=Cyc_Absyn_new_decl((void*)((_tmp1C1=_cycalloc(sizeof(*_tmp1C1)),((_tmp1C1[0]=((_tmp1BF.tag=1,((_tmp1BF.f1=((_tmp1C0=_cycalloc(sizeof(*_tmp1C0)),((_tmp1C0->sc=Cyc_Absyn_Static,((_tmp1C0->is_inline=0,((_tmp1C0->name=funname,((_tmp1C0->tvs=0,((_tmp1C0->effect=0,((_tmp1C0->ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp1C0->ret_type=rettype,((_tmp1C0->args=params,((_tmp1C0->c_varargs=0,((_tmp1C0->cyc_varargs=0,((_tmp1C0->rgn_po=0,((_tmp1C0->body=s,((_tmp1C0->cached_typ=0,((_tmp1C0->param_vardecls=0,((_tmp1C0->fn_vardecl=0,((_tmp1C0->attributes=0,_tmp1C0)))))))))))))))))))))))))))))))))),_tmp1BF)))),_tmp1C1)))),0),((_tmp1BE->tl=*env.gen_ds,_tmp1BE))))));}
# 380
return Cyc_Absyn_fncall_exp(Cyc_Absyn_var_exp(funname,0),args,0);};};}
# 383
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmpA1=e->r;struct _dyneither_ptr*_tmpA3;struct Cyc_Absyn_Exp*_tmpA7;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_List_List*_tmpD4;_LL62: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpA2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpA2->tag != 1)goto _LL64;else{if((((_tmpA2->f1)->f1).Loc_n).tag != 4)goto _LL64;_tmpA3=(_tmpA2->f1)->f2;}}_LL63:
# 388
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmpA3))
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmpA3))->r;
return;_LL64: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpA4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpA4->tag != 1)goto _LL66;}_LL65:
 return;_LL66: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpA5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpA5->tag != 0)goto _LL68;}_LL67:
# 394
 return;_LL68: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA6=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpA6->tag != 9)goto _LL6A;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;}}_LL69:
# 396
 Cyc_Tovc_apply_varmap(varmap,_tmpA7);_tmpAA=_tmpA8;goto _LL6B;_LL6A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpA9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpA9->tag != 2)goto _LL6C;else{_tmpAA=_tmpA9->f2;}}_LL6B:
# 398
 for(0;_tmpAA != 0;_tmpAA=_tmpAA->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmpAA->hd);}
return;_LL6C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpAB=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpAB->tag != 5)goto _LL6E;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;_tmpAE=_tmpAB->f3;}}_LL6D:
# 403
 Cyc_Tovc_apply_varmap(varmap,_tmpAE);_tmpB0=_tmpAC;_tmpB1=_tmpAD;goto _LL6F;_LL6E: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpAF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpAF->tag != 6)goto _LL70;else{_tmpB0=_tmpAF->f1;_tmpB1=_tmpAF->f2;}}_LL6F:
 _tmpB3=_tmpB0;_tmpB4=_tmpB1;goto _LL71;_LL70: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpB2->tag != 7)goto _LL72;else{_tmpB3=_tmpB2->f1;_tmpB4=_tmpB2->f2;}}_LL71:
 _tmpB6=_tmpB3;_tmpB7=_tmpB4;goto _LL73;_LL72: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpB5=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpB5->tag != 22)goto _LL74;else{_tmpB6=_tmpB5->f1;_tmpB7=_tmpB5->f2;}}_LL73:
 _tmpB9=_tmpB6;_tmpBA=_tmpB7;goto _LL75;_LL74: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpB8->tag != 8)goto _LL76;else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;}}_LL75:
 _tmpBC=_tmpB9;_tmpBD=_tmpBA;goto _LL77;_LL76: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpBB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpBB->tag != 3)goto _LL78;else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f3;}}_LL77:
# 409
 Cyc_Tovc_apply_varmap(varmap,_tmpBD);_tmpBF=_tmpBC;goto _LL79;_LL78: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpBE->tag != 11)goto _LL7A;else{_tmpBF=_tmpBE->f1;}}_LL79:
# 411
 _tmpC1=_tmpBF;goto _LL7B;_LL7A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpC0->tag != 12)goto _LL7C;else{_tmpC1=_tmpC0->f1;}}_LL7B:
 _tmpC3=_tmpC1;goto _LL7D;_LL7C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpC2->tag != 13)goto _LL7E;else{_tmpC3=_tmpC2->f2;}}_LL7D:
 _tmpC5=_tmpC3;goto _LL7F;_LL7E: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpC4=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpC4->tag != 14)goto _LL80;else{_tmpC5=_tmpC4->f1;}}_LL7F:
 _tmpC7=_tmpC5;goto _LL81;_LL80: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpC6->tag != 17)goto _LL82;else{_tmpC7=_tmpC6->f1;}}_LL81:
 _tmpC9=_tmpC7;goto _LL83;_LL82: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpC8->tag != 19)goto _LL84;else{_tmpC9=_tmpC8->f1;}}_LL83:
 _tmpCB=_tmpC9;goto _LL85;_LL84: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpCA->tag != 20)goto _LL86;else{_tmpCB=_tmpCA->f1;}}_LL85:
 _tmpCD=_tmpCB;goto _LL87;_LL86: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpCC->tag != 21)goto _LL88;else{_tmpCD=_tmpCC->f1;}}_LL87:
 _tmpCF=_tmpCD;goto _LL89;_LL88: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpCE=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpCE->tag != 4)goto _LL8A;else{_tmpCF=_tmpCE->f1;}}_LL89:
# 420
 Cyc_Tovc_apply_varmap(varmap,_tmpCF);
return;_LL8A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpD0->tag != 16)goto _LL8C;}_LL8B:
# 423
 goto _LL8D;_LL8C: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpD1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpD1->tag != 18)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD2=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpD2->tag != 31)goto _LL90;}_LL8F:
 return;_LL90: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA1;if(_tmpD3->tag != 35)goto _LL92;else{_tmpD4=_tmpD3->f2;}}_LL91:
# 428
 for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmpD4->hd)).f2);}
return;_LL92:;_LL93: {
# 432
struct Cyc_Core_Impossible_exn_struct _tmp1C7;const char*_tmp1C6;struct Cyc_Core_Impossible_exn_struct*_tmp1C5;(int)_throw((void*)((_tmp1C5=_cycalloc(sizeof(*_tmp1C5)),((_tmp1C5[0]=((_tmp1C7.tag=Cyc_Core_Impossible,((_tmp1C7.f1=((_tmp1C6="bad exp form in Tocv::apply_varmap",_tag_dyneither(_tmp1C6,sizeof(char),35))),_tmp1C7)))),_tmp1C5)))));}_LL61:;}
# 436
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmpD8=s->r;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Stmt*_tmpDD;struct Cyc_Absyn_Stmt*_tmpDE;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Stmt*_tmpE1;struct Cyc_Absyn_Stmt*_tmpE2;struct Cyc_Absyn_Decl*_tmpE4;struct Cyc_Absyn_Stmt*_tmpE5;_LL95: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpD9=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmpD8;if(_tmpD9->tag != 0)goto _LL97;}_LL96:
 return Cyc_Absyn_true_exp(0);_LL97: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpDA=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD8;if(_tmpDA->tag != 1)goto _LL99;else{_tmpDB=_tmpDA->f1;}}_LL98:
# 440
 Cyc_Tovc_apply_varmap(env.varmap,_tmpDB);
return _tmpDB;_LL99: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpDC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD8;if(_tmpDC->tag != 2)goto _LL9B;else{_tmpDD=_tmpDC->f1;_tmpDE=_tmpDC->f2;}}_LL9A: {
# 444
struct Cyc_Absyn_Exp*_tmpE6=Cyc_Tovc_stmt_to_exp(env,_tmpDD);
struct Cyc_Absyn_Exp*_tmpE7=Cyc_Tovc_stmt_to_exp(env,_tmpDE);
return Cyc_Absyn_seq_exp(_tmpE6,_tmpE7,0);}_LL9B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpDF=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpD8;if(_tmpDF->tag != 4)goto _LL9D;else{_tmpE0=_tmpDF->f1;_tmpE1=_tmpDF->f2;_tmpE2=_tmpDF->f3;}}_LL9C:
# 448
 Cyc_Tovc_apply_varmap(env.varmap,_tmpE0);{
# 450
struct Cyc_Absyn_Exp*_tmpE8=Cyc_Tovc_stmt_to_exp(env,_tmpE1);
struct Cyc_Absyn_Exp*_tmpE9=Cyc_Tovc_stmt_to_exp(env,_tmpE2);
return Cyc_Absyn_conditional_exp(_tmpE0,_tmpE8,_tmpE9,0);};_LL9D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpE3=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD8;if(_tmpE3->tag != 12)goto _LL9F;else{_tmpE4=_tmpE3->f1;_tmpE5=_tmpE3->f2;}}_LL9E: {
# 454
void*_tmpEA=_tmpE4->r;struct Cyc_Absyn_Vardecl*_tmpEC;_LLA2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpEB=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpEA;if(_tmpEB->tag != 0)goto _LLA4;else{_tmpEC=_tmpEB->f1;}}_LLA3: {
# 457
struct _dyneither_ptr*_tmpED=(*_tmpEC->name).f2;
struct _tuple0*_tmpEE=Cyc_Toc_temp_var();
_tmpEC->name=_tmpEE;
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmpED,Cyc_Absyn_var_exp(_tmpEE,0));{
struct Cyc_List_List*_tmpEF=0;
if(_tmpEC->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpEC->initializer));{
void*_tmpF0=((struct Cyc_Absyn_Exp*)_check_null(_tmpEC->initializer))->r;struct Cyc_List_List*_tmpF2;_LLA7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF1=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF0;if(_tmpF1->tag != 35)goto _LLA9;else{_tmpF2=_tmpF1->f2;}}_LLA8: {
# 476 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpF3=Cyc_Absyn_var_exp(_tmpEC->name,0);
{unsigned int i=0;for(0;_tmpF2 != 0;(_tmpF2=_tmpF2->tl,++ i)){
struct Cyc_List_List*_tmp1C8;_tmpEF=((_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((_tmp1C8->hd=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(_tmpF3,Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmpF2->hd)).f2,0),((_tmp1C8->tl=_tmpEF,_tmp1C8))))));}}
# 482
_tmpEC->initializer=0;
goto _LLA6;}_LLA9:;_LLAA:
 goto _LLA6;_LLA6:;};}
# 487
{struct Cyc_List_List*_tmp1C9;env.all_locals=((_tmp1C9=_region_malloc(env.rgn,sizeof(*_tmp1C9)),((_tmp1C9->hd=_tmpEC,((_tmp1C9->tl=env.all_locals,_tmp1C9))))));}
# 489
(env.encloser)->r=(Cyc_Absyn_decl_stmt(_tmpE4,Cyc_Absyn_new_stmt((env.encloser)->r,0),0))->r;{
struct Cyc_Absyn_Exp*_tmpF6=Cyc_Tovc_stmt_to_exp(env,_tmpE5);
if(_tmpEC->initializer != 0){
_tmpF6=Cyc_Absyn_seq_exp(Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmpEC->name,0),(struct Cyc_Absyn_Exp*)_check_null(_tmpEC->initializer),0),_tmpF6,0);
# 494
_tmpEC->initializer=0;}
# 496
for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){
_tmpF6=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpEF->hd,_tmpF6,0);}
return _tmpF6;};};}_LLA4:;_LLA5: {
struct Cyc_Core_Impossible_exn_struct _tmp1CF;const char*_tmp1CE;struct Cyc_Core_Impossible_exn_struct*_tmp1CD;(int)_throw((void*)((_tmp1CD=_cycalloc(sizeof(*_tmp1CD)),((_tmp1CD[0]=((_tmp1CF.tag=Cyc_Core_Impossible,((_tmp1CF.f1=((_tmp1CE="bad local decl in Tovc::stmt_to_exp",_tag_dyneither(_tmp1CE,sizeof(char),36))),_tmp1CF)))),_tmp1CD)))));}_LLA1:;}_LL9F:;_LLA0:
# 502
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);_LL94:;}
# 506
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 508
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
void*_tmpFA=e->r;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_List_List*_tmp12B;struct Cyc_Absyn_Stmt*_tmp12D;_LLAC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmpFB->tag != 0)goto _LLAE;}_LLAD:
 return;_LLAE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmpFC->tag != 1)goto _LLB0;}_LLAF:
 return;_LLB0: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmpFD->tag != 9)goto _LLB2;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;}}_LLB1:
# 514
 Cyc_Tovc_exp_to_vc(env,_tmpFE);_tmp101=_tmpFF;goto _LLB3;_LLB2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp100->tag != 2)goto _LLB4;else{_tmp101=_tmp100->f2;}}_LLB3:
# 516
 for(0;_tmp101 != 0;_tmp101=_tmp101->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmp101->hd);}
return;_LLB4: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp102->tag != 5)goto _LLB6;else{_tmp103=_tmp102->f1;_tmp104=_tmp102->f2;_tmp105=_tmp102->f3;}}_LLB5:
# 521
 Cyc_Tovc_exp_to_vc(env,_tmp105);_tmp107=_tmp103;_tmp108=_tmp104;goto _LLB7;_LLB6: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp106->tag != 6)goto _LLB8;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}_LLB7:
 _tmp10A=_tmp107;_tmp10B=_tmp108;goto _LLB9;_LLB8: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp109->tag != 7)goto _LLBA;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}_LLB9:
 _tmp10D=_tmp10A;_tmp10E=_tmp10B;goto _LLBB;_LLBA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp10C->tag != 22)goto _LLBC;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LLBB:
 _tmp110=_tmp10D;_tmp111=_tmp10E;goto _LLBD;_LLBC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp10F->tag != 8)goto _LLBE;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;}}_LLBD:
 _tmp113=_tmp110;_tmp114=_tmp111;goto _LLBF;_LLBE: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp112=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp112->tag != 3)goto _LLC0;else{_tmp113=_tmp112->f1;_tmp114=_tmp112->f3;}}_LLBF:
# 527
 Cyc_Tovc_exp_to_vc(env,_tmp114);_tmp116=_tmp113;goto _LLC1;_LLC0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp115->tag != 11)goto _LLC2;else{_tmp116=_tmp115->f1;}}_LLC1:
# 529
 _tmp118=_tmp116;goto _LLC3;_LLC2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp117->tag != 12)goto _LLC4;else{_tmp118=_tmp117->f1;}}_LLC3:
 _tmp11A=_tmp118;goto _LLC5;_LLC4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp119->tag != 13)goto _LLC6;else{_tmp11A=_tmp119->f2;}}_LLC5:
 _tmp11C=_tmp11A;goto _LLC7;_LLC6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp11B->tag != 14)goto _LLC8;else{_tmp11C=_tmp11B->f1;}}_LLC7:
 _tmp11E=_tmp11C;goto _LLC9;_LLC8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp11D->tag != 17)goto _LLCA;else{_tmp11E=_tmp11D->f1;}}_LLC9:
 _tmp120=_tmp11E;goto _LLCB;_LLCA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp11F=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp11F->tag != 19)goto _LLCC;else{_tmp120=_tmp11F->f1;}}_LLCB:
 _tmp122=_tmp120;goto _LLCD;_LLCC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp121->tag != 20)goto _LLCE;else{_tmp122=_tmp121->f1;}}_LLCD:
 _tmp124=_tmp122;goto _LLCF;_LLCE: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp123->tag != 21)goto _LLD0;else{_tmp124=_tmp123->f1;}}_LLCF:
 _tmp126=_tmp124;goto _LLD1;_LLD0: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp125->tag != 4)goto _LLD2;else{_tmp126=_tmp125->f1;}}_LLD1:
# 538
 Cyc_Tovc_exp_to_vc(env,_tmp126);
return;_LLD2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp127=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp127->tag != 16)goto _LLD4;}_LLD3:
# 541
 goto _LLD5;_LLD4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp128->tag != 18)goto _LLD6;}_LLD5:
 goto _LLD7;_LLD6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp129=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp129->tag != 31)goto _LLD8;}_LLD7:
 return;_LLD8: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp12A->tag != 35)goto _LLDA;else{_tmp12B=_tmp12A->f2;}}_LLD9:
# 549
 for(0;_tmp12B != 0;_tmp12B=_tmp12B->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmp12B->hd)).f2);}
return;_LLDA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp12C->tag != 36)goto _LLDC;else{_tmp12D=_tmp12C->f1;}}_LLDB:
# 557
 Cyc_Tovc_stmt_to_vc(env,_tmp12D);
# 559
e->r=(Cyc_Tovc_stmt_to_exp(env,_tmp12D))->r;
return;_LLDC: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpFA;if(_tmp12E->tag != 39)goto _LLDE;}_LLDD:
# 562
 return;_LLDE:;_LLDF: {
# 564
struct Cyc_Core_Impossible_exn_struct _tmp1D5;const char*_tmp1D4;struct Cyc_Core_Impossible_exn_struct*_tmp1D3;(int)_throw((void*)((_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3[0]=((_tmp1D5.tag=Cyc_Core_Impossible,((_tmp1D5.f1=((_tmp1D4="bad exp form after xlation to C",_tag_dyneither(_tmp1D4,sizeof(char),32))),_tmp1D5)))),_tmp1D3)))));}_LLAB:;}
# 568
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 570
while(1){
env.encloser=s;{
void*_tmp132=s->r;struct Cyc_Absyn_Stmt*_tmp138;struct Cyc_Absyn_Exp*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Absyn_Stmt*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Stmt*_tmp142;struct Cyc_Absyn_Stmt*_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14A;struct Cyc_Absyn_Stmt*_tmp14B;struct Cyc_Absyn_Stmt*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_Decl*_tmp153;struct Cyc_Absyn_Stmt*_tmp154;_LLE1: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp133=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp133->tag != 0)goto _LLE3;}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp134=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp134->tag != 6)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp135=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp135->tag != 7)goto _LLE7;}_LLE6:
 goto _LLE8;_LLE7: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp136=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp136->tag != 8)goto _LLE9;}_LLE8:
 return;_LLE9: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp137=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp137->tag != 13)goto _LLEB;else{_tmp138=_tmp137->f2;}}_LLEA:
# 578
 s=_tmp138;
continue;_LLEB: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp139=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp139->tag != 3)goto _LLED;else{_tmp13A=_tmp139->f1;}}_LLEC:
# 581
 if(_tmp13A == 0)
return;
_tmp13C=_tmp13A;goto _LLEE;_LLED: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp13B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp13B->tag != 1)goto _LLEF;else{_tmp13C=_tmp13B->f1;}}_LLEE:
# 585
 Cyc_Tovc_exp_to_vc(env,_tmp13C);
return;_LLEF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp13D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp13D->tag != 2)goto _LLF1;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LLF0:
# 588
 Cyc_Tovc_stmt_to_vc(env,_tmp13E);
s=_tmp13F;
continue;_LLF1: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp140=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp140->tag != 4)goto _LLF3;else{_tmp141=_tmp140->f1;_tmp142=_tmp140->f2;_tmp143=_tmp140->f3;}}_LLF2:
# 592
 Cyc_Tovc_exp_to_vc(env,_tmp141);
Cyc_Tovc_stmt_to_vc(env,_tmp142);
s=_tmp143;
continue;_LLF3: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp144=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp144->tag != 5)goto _LLF5;else{_tmp145=(_tmp144->f1).f1;_tmp146=_tmp144->f2;}}_LLF4:
# 597
 Cyc_Tovc_exp_to_vc(env,_tmp145);
s=_tmp146;
continue;_LLF5: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp147=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp147->tag != 9)goto _LLF7;else{_tmp148=_tmp147->f1;_tmp149=(_tmp147->f2).f1;_tmp14A=(_tmp147->f3).f1;_tmp14B=_tmp147->f4;}}_LLF6:
# 601
 Cyc_Tovc_exp_to_vc(env,_tmp148);
Cyc_Tovc_exp_to_vc(env,_tmp149);
Cyc_Tovc_exp_to_vc(env,_tmp14A);
s=_tmp14B;
continue;_LLF7: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp14C=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp14C->tag != 14)goto _LLF9;else{_tmp14D=_tmp14C->f1;_tmp14E=(_tmp14C->f2).f1;}}_LLF8:
# 607
 Cyc_Tovc_exp_to_vc(env,_tmp14E);
s=_tmp14D;
continue;_LLF9: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp14F=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp14F->tag != 10)goto _LLFB;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;}}_LLFA:
# 613
 Cyc_Tovc_exp_to_vc(env,_tmp150);
for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp151->hd)->body);}
return;_LLFB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp152=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp132;if(_tmp152->tag != 12)goto _LLFD;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;}}_LLFC:
# 618
{void*_tmp155=_tmp153->r;struct Cyc_Absyn_Vardecl*_tmp157;_LL100: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp156=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp155;if(_tmp156->tag != 0)goto _LL102;else{_tmp157=_tmp156->f1;}}_LL101:
# 620
 if(Cyc_Tovc_elim_array_initializers){
# 625
void*_tmp158=Cyc_Tcutil_compress(_tmp157->type);_LL105: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp159->tag != 8)goto _LL107;}_LL106:
# 627
 if(_tmp157->initializer != 0){
void*_tmp15A=((struct Cyc_Absyn_Exp*)_check_null(_tmp157->initializer))->r;struct Cyc_List_List*_tmp15C;_LL10A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp15B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp15A;if(_tmp15B->tag != 35)goto _LL10C;else{_tmp15C=_tmp15B->f2;}}_LL10B:
# 630
 _tmp157->initializer=0;{
# 632
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp157->name,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp154->r,0);
{unsigned int i=0;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,++ i)){
s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,
Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmp15C->hd)).f2,0),0),s3,0);}}
# 640
_tmp154->r=s3->r;
goto _LL109;};_LL10C:;_LL10D: {
# 643
const char*_tmp1D9;void*_tmp1D8[1];struct Cyc_String_pa_PrintArg_struct _tmp1D7;struct _dyneither_ptr _tmp15D=(_tmp1D7.tag=0,((_tmp1D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmp1D8[0]=& _tmp1D7,Cyc_aprintf(((_tmp1D9="|%s|: array type bad initializer",_tag_dyneither(_tmp1D9,sizeof(char),33))),_tag_dyneither(_tmp1D8,sizeof(void*),1)))))));
struct Cyc_Core_Impossible_exn_struct _tmp1DC;struct Cyc_Core_Impossible_exn_struct*_tmp1DB;(int)_throw((void*)((_tmp1DB=_cycalloc(sizeof(*_tmp1DB)),((_tmp1DB[0]=((_tmp1DC.tag=Cyc_Core_Impossible,((_tmp1DC.f1=(struct _dyneither_ptr)_tmp15D,_tmp1DC)))),_tmp1DB)))));}_LL109:;}
# 647
goto _LL104;_LL107:;_LL108:
 goto _LL104;_LL104:;}
# 651
if(_tmp157->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp157->initializer));
{struct Cyc_List_List*_tmp1DD;env.all_locals=((_tmp1DD=_region_malloc(env.rgn,sizeof(*_tmp1DD)),((_tmp1DD->hd=_tmp157,((_tmp1DD->tl=env.all_locals,_tmp1DD))))));}
s=_tmp154;
continue;_LL102:;_LL103:
 goto _LLFF;_LLFF:;}
# 658
goto _LLFE;_LLFD:;_LLFE: {
struct Cyc_Core_Impossible_exn_struct _tmp1E3;const char*_tmp1E2;struct Cyc_Core_Impossible_exn_struct*_tmp1E1;(int)_throw((void*)((_tmp1E1=_cycalloc(sizeof(*_tmp1E1)),((_tmp1E1[0]=((_tmp1E3.tag=Cyc_Core_Impossible,((_tmp1E3.f1=((_tmp1E2="bad stmt after xlation to C",_tag_dyneither(_tmp1E2,sizeof(char),28))),_tmp1E3)))),_tmp1E1)))));}_LLE0:;};}}
# 664
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmp167=0;
for(0;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmp168=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_tmp169=_tmp168->r;struct Cyc_Absyn_Fndecl*_tmp16B;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp178;struct Cyc_List_List*_tmp17A;struct Cyc_List_List*_tmp17C;_LL10F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp16A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp16A->tag != 1)goto _LL111;else{_tmp16B=_tmp16A->f1;}}_LL110:
# 673
 _tmp16B->cached_typ=0;
# 680
if(_tmp16B->param_vardecls != 0){
struct _RegionHandle _tmp17F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp17F;_push_region(rgn);
{struct Cyc_List_List**_tmp1E4;struct Cyc_List_List**_tmp180=(_tmp1E4=_region_malloc(rgn,sizeof(*_tmp1E4)),((_tmp1E4[0]=0,_tmp1E4)));
if(_tmp16B->param_vardecls == 0){
{const char*_tmp1E8;void*_tmp1E7[1];struct Cyc_String_pa_PrintArg_struct _tmp1E6;(_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp16B->name).f2),((_tmp1E7[0]=& _tmp1E6,Cyc_fprintf(Cyc_stderr,((_tmp1E8="No param vardecls for function %s\n",_tag_dyneither(_tmp1E8,sizeof(char),35))),_tag_dyneither(_tmp1E7,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);}
# 687
{struct Cyc_Tovc_ToExpEnv _tmp1E9;Cyc_Tovc_stmt_to_vc(((_tmp1E9.rgn=rgn,((_tmp1E9.all_locals=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp16B->param_vardecls))->v),((_tmp1E9.varmap=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp),((_tmp1E9.encloser=_tmp16B->body,((_tmp1E9.gen_ds=_tmp180,_tmp1E9)))))))))),_tmp16B->body);}
# 693
if(*_tmp180 != 0){
struct Cyc_Absyn_Vardecl*_tmp1EA;struct Cyc_Absyn_Vardecl*vd=(_tmp1EA=_cycalloc(sizeof(*_tmp1EA)),((_tmp1EA->sc=_tmp16B->sc,((_tmp1EA->name=_tmp16B->name,((_tmp1EA->tq=Cyc_Absyn_const_tqual(0),((_tmp1EA->type=
Cyc_Tcutil_fndecl2typ(_tmp16B),((_tmp1EA->initializer=0,((_tmp1EA->rgn=0,((_tmp1EA->attributes=_tmp16B->attributes,((_tmp1EA->escapes=0,_tmp1EA)))))))))))))))));
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1F0;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1EF;struct Cyc_List_List*_tmp1EE;_tmp167=((_tmp1EE=_cycalloc(sizeof(*_tmp1EE)),((_tmp1EE->hd=Cyc_Absyn_new_decl((void*)((_tmp1F0=_cycalloc(sizeof(*_tmp1F0)),((_tmp1F0[0]=((_tmp1EF.tag=0,((_tmp1EF.f1=vd,_tmp1EF)))),_tmp1F0)))),0),((_tmp1EE->tl=_tmp167,_tmp1EE))))));}
# 698
_tmp167=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp180,_tmp167);}}
# 682
;_pop_region(rgn);}
# 702
goto _LL112;_LL111: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp16C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp16C->tag != 0)goto _LL113;}_LL112:
 goto _LL114;_LL113: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp16D=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp16D->tag != 2)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp16E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp16E->tag != 3)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp16F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp16F->tag != 4)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp170=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp170->tag != 5)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp171=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp171->tag != 6)goto _LL11D;}_LL11C:
 goto _LL11E;_LL11D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp172=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp172->tag != 7)goto _LL11F;}_LL11E:
 goto _LL120;_LL11F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp173=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp173->tag != 8)goto _LL121;}_LL120:
 goto _LL122;_LL121: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp174=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp174->tag != 9)goto _LL123;}_LL122:
# 712
{struct Cyc_List_List*_tmp1F1;_tmp167=((_tmp1F1=_cycalloc(sizeof(*_tmp1F1)),((_tmp1F1->hd=_tmp168,((_tmp1F1->tl=_tmp167,_tmp1F1))))));}
goto _LL10E;_LL123: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp175=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp175->tag != 10)goto _LL125;else{_tmp176=_tmp175->f2;}}_LL124:
 _tmp178=_tmp176;goto _LL126;_LL125: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp177=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp177->tag != 11)goto _LL127;else{_tmp178=_tmp177->f2;}}_LL126:
 _tmp17A=_tmp178;goto _LL128;_LL127: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp179=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp179->tag != 12)goto _LL129;else{_tmp17A=_tmp179->f1;}}_LL128:
 _tmp17C=_tmp17A;goto _LL12A;_LL129: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp17B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp17B->tag != 13)goto _LL12B;else{_tmp17C=_tmp17B->f1;}}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp17D=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp17D->tag != 14)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp17E=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp169;if(_tmp17E->tag != 15)goto _LL10E;}_LL12E: {
# 720
struct Cyc_Core_Impossible_exn_struct _tmp1F7;const char*_tmp1F6;struct Cyc_Core_Impossible_exn_struct*_tmp1F5;(int)_throw((void*)((_tmp1F5=_cycalloc(sizeof(*_tmp1F5)),((_tmp1F5[0]=((_tmp1F7.tag=Cyc_Core_Impossible,((_tmp1F7.f1=((_tmp1F6="nested translation unit after translation to C",_tag_dyneither(_tmp1F6,sizeof(char),47))),_tmp1F7)))),_tmp1F5)))));}_LL10E:;}
# 723
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp167);}
