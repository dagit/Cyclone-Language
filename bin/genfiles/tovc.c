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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 905 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
# 982
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1097
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
# 167
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 341 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
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
void*_tmp0=e->r;struct _dyneither_ptr*_tmp2;struct Cyc_Absyn_Exp**_tmp6;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp9;struct Cyc_Absyn_Exp**_tmpB;struct Cyc_Absyn_Exp**_tmpC;struct Cyc_Absyn_Exp**_tmpD;struct Cyc_Absyn_Exp**_tmpF;struct Cyc_Absyn_Exp**_tmp10;struct Cyc_Absyn_Exp**_tmp12;struct Cyc_Absyn_Exp**_tmp13;struct Cyc_Absyn_Exp**_tmp15;struct Cyc_Absyn_Exp**_tmp16;struct Cyc_Absyn_Exp**_tmp18;struct Cyc_Absyn_Exp**_tmp19;struct Cyc_Absyn_Exp**_tmp1B;struct Cyc_Absyn_Exp**_tmp1C;struct Cyc_Absyn_Exp**_tmp1E;struct Cyc_Absyn_Exp**_tmp20;struct Cyc_Absyn_Exp**_tmp22;struct Cyc_Absyn_Exp**_tmp24;struct Cyc_Absyn_Exp**_tmp26;struct Cyc_Absyn_Exp**_tmp28;struct Cyc_Absyn_Exp**_tmp2A;struct Cyc_Absyn_Exp**_tmp2C;struct Cyc_Absyn_Exp**_tmp2E;struct Cyc_List_List*_tmp33;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1->tag != 1)goto _LL3;else{if((((_tmp1->f1)->f1).Loc_n).tag != 4)goto _LL3;_tmp2=(_tmp1->f1)->f2;}}_LL2:
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
struct Cyc_Absyn_Vardecl*_tmp34=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmp2,(*_tmp34->name).f2)== 0){
*env.freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp34);{
# 131
void*_tmp35=_tmp34->type;_LL34: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp35;if(_tmp36->tag != 8)goto _LL36;}_LL35:
 return 0;_LL36:;_LL37:
 return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp34->name,0),0);_LL33:;};}}
# 140
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
if(Cyc_strptrcmp(_tmp2,(*((struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd)->name).f2)== 0){
struct Cyc_Core_Impossible_exn_struct _tmp183;const char*_tmp182;void*_tmp181[1];struct Cyc_String_pa_PrintArg_struct _tmp180;struct Cyc_Core_Impossible_exn_struct*_tmp17F;(int)_throw((void*)((_tmp17F=_cycalloc(sizeof(*_tmp17F)),((_tmp17F[0]=((_tmp183.tag=Cyc_Core_Impossible,((_tmp183.f1=(struct _dyneither_ptr)((_tmp180.tag=0,((_tmp180.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2),((_tmp181[0]=& _tmp180,Cyc_aprintf(((_tmp182="unbound variable %s found in box_free_vars_exp",_tag_dyneither(_tmp182,sizeof(char),47))),_tag_dyneither(_tmp181,sizeof(void*),1)))))))),_tmp183)))),_tmp17F)))));}}
# 147
return 0;_LL3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp3->tag != 1)goto _LL5;}_LL4:
# 150
 return 0;_LL5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp4->tag != 0)goto _LL7;}_LL6:
# 153
 return 0;_LL7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp5=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp5->tag != 9)goto _LL9;else{_tmp6=(struct Cyc_Absyn_Exp**)& _tmp5->f1;_tmp7=_tmp5->f2;}}_LL8:
# 155
{struct Cyc_Absyn_Exp*_tmp3C=Cyc_Tovc_box_free_vars_exp(env,*_tmp6);if(_tmp3C != 0)*_tmp6=_tmp3C;}
_tmp9=_tmp7;goto _LLA;_LL9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp8->tag != 2)goto _LLB;else{_tmp9=_tmp8->f2;}}_LLA:
# 158
 for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Absyn_Exp*_tmp3D=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp9->hd);if(_tmp3D != 0)_tmp9->hd=(void*)_tmp3D;}
# 161
return 0;_LLB: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpA->tag != 5)goto _LLD;else{_tmpB=(struct Cyc_Absyn_Exp**)& _tmpA->f1;_tmpC=(struct Cyc_Absyn_Exp**)& _tmpA->f2;_tmpD=(struct Cyc_Absyn_Exp**)& _tmpA->f3;}}_LLC:
# 164
{struct Cyc_Absyn_Exp*_tmp3E=Cyc_Tovc_box_free_vars_exp(env,*_tmpD);if(_tmp3E != 0)*_tmpD=_tmp3E;}
_tmpF=_tmpB;_tmp10=_tmpC;goto _LLE;_LLD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmpE->tag != 6)goto _LLF;else{_tmpF=(struct Cyc_Absyn_Exp**)& _tmpE->f1;_tmp10=(struct Cyc_Absyn_Exp**)& _tmpE->f2;}}_LLE:
 _tmp12=_tmpF;_tmp13=_tmp10;goto _LL10;_LLF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp11->tag != 7)goto _LL11;else{_tmp12=(struct Cyc_Absyn_Exp**)& _tmp11->f1;_tmp13=(struct Cyc_Absyn_Exp**)& _tmp11->f2;}}_LL10:
 _tmp15=_tmp12;_tmp16=_tmp13;goto _LL12;_LL11: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp14=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp14->tag != 22)goto _LL13;else{_tmp15=(struct Cyc_Absyn_Exp**)& _tmp14->f1;_tmp16=(struct Cyc_Absyn_Exp**)& _tmp14->f2;}}_LL12:
 _tmp18=_tmp15;_tmp19=_tmp16;goto _LL14;_LL13: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp17=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp17->tag != 8)goto _LL15;else{_tmp18=(struct Cyc_Absyn_Exp**)& _tmp17->f1;_tmp19=(struct Cyc_Absyn_Exp**)& _tmp17->f2;}}_LL14:
 _tmp1B=_tmp18;_tmp1C=_tmp19;goto _LL16;_LL15: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1A->tag != 3)goto _LL17;else{_tmp1B=(struct Cyc_Absyn_Exp**)& _tmp1A->f1;_tmp1C=(struct Cyc_Absyn_Exp**)& _tmp1A->f3;}}_LL16:
# 171
{struct Cyc_Absyn_Exp*_tmp3F=Cyc_Tovc_box_free_vars_exp(env,*_tmp1C);if(_tmp3F != 0)*_tmp1C=_tmp3F;}
_tmp1E=_tmp1B;goto _LL18;_LL17: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1D->tag != 11)goto _LL19;else{_tmp1E=(struct Cyc_Absyn_Exp**)& _tmp1D->f1;}}_LL18:
# 174
 _tmp20=_tmp1E;goto _LL1A;_LL19: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp1F->tag != 12)goto _LL1B;else{_tmp20=(struct Cyc_Absyn_Exp**)& _tmp1F->f1;}}_LL1A:
 _tmp22=_tmp20;goto _LL1C;_LL1B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp21=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp21->tag != 13)goto _LL1D;else{_tmp22=(struct Cyc_Absyn_Exp**)& _tmp21->f2;}}_LL1C:
 _tmp24=_tmp22;goto _LL1E;_LL1D: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp23=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp23->tag != 14)goto _LL1F;else{_tmp24=(struct Cyc_Absyn_Exp**)& _tmp23->f1;}}_LL1E:
 _tmp26=_tmp24;goto _LL20;_LL1F: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp25=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp25->tag != 17)goto _LL21;else{_tmp26=(struct Cyc_Absyn_Exp**)& _tmp25->f1;}}_LL20:
 _tmp28=_tmp26;goto _LL22;_LL21: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp27=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp27->tag != 19)goto _LL23;else{_tmp28=(struct Cyc_Absyn_Exp**)& _tmp27->f1;}}_LL22:
 _tmp2A=_tmp28;goto _LL24;_LL23: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp29=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp29->tag != 20)goto _LL25;else{_tmp2A=(struct Cyc_Absyn_Exp**)& _tmp29->f1;}}_LL24:
 _tmp2C=_tmp2A;goto _LL26;_LL25: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2B->tag != 21)goto _LL27;else{_tmp2C=(struct Cyc_Absyn_Exp**)& _tmp2B->f1;}}_LL26:
 _tmp2E=_tmp2C;goto _LL28;_LL27: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2D=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2D->tag != 4)goto _LL29;else{_tmp2E=(struct Cyc_Absyn_Exp**)& _tmp2D->f1;}}_LL28:
# 183
{struct Cyc_Absyn_Exp*_tmp40=Cyc_Tovc_box_free_vars_exp(env,*_tmp2E);if(_tmp40 != 0)*_tmp2E=_tmp40;}
return 0;_LL29: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp2F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp2F->tag != 16)goto _LL2B;}_LL2A:
# 186
 goto _LL2C;_LL2B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp30=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp30->tag != 18)goto _LL2D;}_LL2C:
 goto _LL2E;_LL2D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp31=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp31->tag != 30)goto _LL2F;}_LL2E:
 return 0;_LL2F: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp32=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp0;if(_tmp32->tag != 34)goto _LL31;else{_tmp33=_tmp32->f2;}}_LL30:
# 191
 for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
struct Cyc_Absyn_Exp*_tmp41=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp33->hd)).f2);if(_tmp41 != 0)(*((struct _tuple10*)_tmp33->hd)).f2=_tmp41;}
# 194
return 0;_LL31:;_LL32: {
# 196
struct Cyc_Core_Impossible_exn_struct _tmp189;const char*_tmp188;struct Cyc_Core_Impossible_exn_struct*_tmp187;(int)_throw((void*)((_tmp187=_cycalloc(sizeof(*_tmp187)),((_tmp187[0]=((_tmp189.tag=Cyc_Core_Impossible,((_tmp189.f1=((_tmp188="bad exp form in Tocv::box_free_vars_exp",_tag_dyneither(_tmp188,sizeof(char),40))),_tmp189)))),_tmp187)))));}_LL0:;}}
# 200
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 204
while(1){
void*_tmp45=s->r;struct Cyc_Absyn_Stmt*_tmp4B;struct Cyc_Absyn_Exp**_tmp4D;struct Cyc_Absyn_Exp**_tmp4F;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Stmt*_tmp52;struct Cyc_Absyn_Exp**_tmp54;struct Cyc_Absyn_Stmt*_tmp55;struct Cyc_Absyn_Stmt*_tmp56;struct Cyc_Absyn_Exp**_tmp58;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Exp**_tmp5B;struct Cyc_Absyn_Exp**_tmp5C;struct Cyc_Absyn_Exp**_tmp5D;struct Cyc_Absyn_Stmt*_tmp5E;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Exp**_tmp61;struct Cyc_Absyn_Exp**_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Decl*_tmp66;struct Cyc_Absyn_Stmt*_tmp67;_LL39: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp46=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp46->tag != 0)goto _LL3B;}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp47=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp47->tag != 6)goto _LL3D;}_LL3C:
 goto _LL3E;_LL3D: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp48=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp48->tag != 7)goto _LL3F;}_LL3E:
 return;_LL3F: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp49=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp49->tag != 8)goto _LL41;}_LL40:
# 211
 return;_LL41: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp4A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp4A->tag != 13)goto _LL43;else{_tmp4B=_tmp4A->f2;}}_LL42:
# 213
 s=_tmp4B;
continue;_LL43: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp4C=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp4C->tag != 3)goto _LL45;else{_tmp4D=(struct Cyc_Absyn_Exp**)& _tmp4C->f1;}}_LL44:
# 217
 if(*_tmp4D == 0)
return;
{struct Cyc_Absyn_Exp*_tmp68=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*_tmp4D));if(_tmp68 != 0)*_tmp4D=_tmp68;}
return;_LL45: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp4E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp4E->tag != 1)goto _LL47;else{_tmp4F=(struct Cyc_Absyn_Exp**)& _tmp4E->f1;}}_LL46:
# 222
{struct Cyc_Absyn_Exp*_tmp69=Cyc_Tovc_box_free_vars_exp(env,*_tmp4F);if(_tmp69 != 0)*_tmp4F=_tmp69;}
return;_LL47: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp50=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp50->tag != 2)goto _LL49;else{_tmp51=_tmp50->f1;_tmp52=_tmp50->f2;}}_LL48:
# 225
 Cyc_Tovc_box_free_vars_stmt(env,_tmp51);
s=_tmp52;
continue;_LL49: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp53=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp53->tag != 4)goto _LL4B;else{_tmp54=(struct Cyc_Absyn_Exp**)& _tmp53->f1;_tmp55=_tmp53->f2;_tmp56=_tmp53->f3;}}_LL4A:
# 229
{struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tovc_box_free_vars_exp(env,*_tmp54);if(_tmp6A != 0)*_tmp54=_tmp6A;}
Cyc_Tovc_box_free_vars_stmt(env,_tmp55);
s=_tmp56;
continue;_LL4B: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp57=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp57->tag != 5)goto _LL4D;else{_tmp58=(struct Cyc_Absyn_Exp**)&(_tmp57->f1).f1;_tmp59=_tmp57->f2;}}_LL4C:
# 234
{struct Cyc_Absyn_Exp*_tmp6B=Cyc_Tovc_box_free_vars_exp(env,*_tmp58);if(_tmp6B != 0)*_tmp58=_tmp6B;}
s=_tmp59;
continue;_LL4D: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp5A=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp5A->tag != 9)goto _LL4F;else{_tmp5B=(struct Cyc_Absyn_Exp**)& _tmp5A->f1;_tmp5C=(struct Cyc_Absyn_Exp**)&(_tmp5A->f2).f1;_tmp5D=(struct Cyc_Absyn_Exp**)&(_tmp5A->f3).f1;_tmp5E=_tmp5A->f4;}}_LL4E:
# 238
{struct Cyc_Absyn_Exp*_tmp6C=Cyc_Tovc_box_free_vars_exp(env,*_tmp5B);if(_tmp6C != 0)*_tmp5B=_tmp6C;}
{struct Cyc_Absyn_Exp*_tmp6D=Cyc_Tovc_box_free_vars_exp(env,*_tmp5C);if(_tmp6D != 0)*_tmp5C=_tmp6D;}
{struct Cyc_Absyn_Exp*_tmp6E=Cyc_Tovc_box_free_vars_exp(env,*_tmp5D);if(_tmp6E != 0)*_tmp5D=_tmp6E;}
s=_tmp5E;
continue;_LL4F: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp5F=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp5F->tag != 14)goto _LL51;else{_tmp60=_tmp5F->f1;_tmp61=(struct Cyc_Absyn_Exp**)&(_tmp5F->f2).f1;}}_LL50:
# 244
{struct Cyc_Absyn_Exp*_tmp6F=Cyc_Tovc_box_free_vars_exp(env,*_tmp61);if(_tmp6F != 0)*_tmp61=_tmp6F;}
s=_tmp60;
continue;_LL51: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp62=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp62->tag != 10)goto _LL53;else{_tmp63=(struct Cyc_Absyn_Exp**)& _tmp62->f1;_tmp64=_tmp62->f2;}}_LL52:
# 249
{struct Cyc_Absyn_Exp*_tmp70=Cyc_Tovc_box_free_vars_exp(env,*_tmp63);if(_tmp70 != 0)*_tmp63=_tmp70;}
for(0;_tmp64 != 0;_tmp64=_tmp64->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp64->hd)->body);}
return;_LL53: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp65=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45;if(_tmp65->tag != 12)goto _LL55;else{_tmp66=_tmp65->f1;_tmp67=_tmp65->f2;}}_LL54:
# 254
{void*_tmp71=_tmp66->r;struct Cyc_Absyn_Vardecl*_tmp73;_LL58: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp72=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp71;if(_tmp72->tag != 0)goto _LL5A;else{_tmp73=_tmp72->f1;}}_LL59:
# 256
{struct Cyc_List_List*_tmp18A;env.boundvars=((_tmp18A=_region_malloc(env.rgn,sizeof(*_tmp18A)),((_tmp18A->hd=_tmp73,((_tmp18A->tl=env.boundvars,_tmp18A))))));}
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp73->name).f2);
if(_tmp73->initializer != 0){
struct Cyc_Absyn_Exp*_tmp75=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp73->initializer));
if(_tmp75 != 0)_tmp73->initializer=_tmp75;}
# 262
s=_tmp67;
continue;_LL5A:;_LL5B:
 goto _LL57;_LL57:;}
# 266
goto _LL56;_LL55:;_LL56: {
struct Cyc_Core_Impossible_exn_struct _tmp190;const char*_tmp18F;struct Cyc_Core_Impossible_exn_struct*_tmp18E;(int)_throw((void*)((_tmp18E=_cycalloc(sizeof(*_tmp18E)),((_tmp18E[0]=((_tmp190.tag=Cyc_Core_Impossible,((_tmp190.f1=((_tmp18F="bad stmt after xlation to C",_tag_dyneither(_tmp18F,sizeof(char),28))),_tmp190)))),_tmp18E)))));}_LL38:;}}
# 277
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 281
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 284
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp191;struct Cyc_Tovc_BoxingEnv _tmp79=(_tmp191.rgn=env.rgn,((_tmp191.all_locals=env.all_locals,((_tmp191.varmap=env.varmap,((_tmp191.boundvars=0,((_tmp191.freevars=& freevars,_tmp191)))))))));
Cyc_Tovc_box_free_vars_stmt(_tmp79,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 290
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 292
struct Cyc_Absyn_Vardecl*_tmp192;struct Cyc_Absyn_Vardecl*_tmp7A=(_tmp192=_cycalloc(sizeof(*_tmp192)),((_tmp192->sc=Cyc_Absyn_Public,((_tmp192->name=Cyc_Absyn_exn_name,((_tmp192->tq=Cyc_Absyn_empty_tqual(0),((_tmp192->type=(void*)& Cyc_Absyn_VoidType_val,((_tmp192->initializer=0,((_tmp192->rgn=0,((_tmp192->attributes=0,((_tmp192->escapes=0,_tmp192)))))))))))))))));
# 295
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp7A)){
void*_tmp7B=_tmp7A->type;void*_tmp7D;_LL5D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7B;if(_tmp7C->tag != 8)goto _LL5F;else{_tmp7D=(_tmp7C->f1).elt_type;}}_LL5E:
# 298
{struct _tuple8*_tmp195;struct Cyc_List_List*_tmp194;params=((_tmp194=_cycalloc(sizeof(*_tmp194)),((_tmp194->hd=((_tmp195=_cycalloc(sizeof(*_tmp195)),((_tmp195->f1=(*_tmp7A->name).f2,((_tmp195->f2=_tmp7A->tq,((_tmp195->f3=
Cyc_Absyn_cstar_typ(_tmp7D,Cyc_Absyn_empty_tqual(0)),_tmp195)))))))),((_tmp194->tl=params,_tmp194))))));}
# 301
{struct Cyc_List_List*_tmp196;args=((_tmp196=_cycalloc(sizeof(*_tmp196)),((_tmp196->hd=Cyc_Absyn_var_exp(_tmp7A->name,0),((_tmp196->tl=args,_tmp196))))));}
goto _LL5C;_LL5F:;_LL60:
# 304
{struct _tuple8*_tmp199;struct Cyc_List_List*_tmp198;params=((_tmp198=_cycalloc(sizeof(*_tmp198)),((_tmp198->hd=((_tmp199=_cycalloc(sizeof(*_tmp199)),((_tmp199->f1=(*_tmp7A->name).f2,((_tmp199->f2=_tmp7A->tq,((_tmp199->f3=
Cyc_Absyn_cstar_typ(_tmp7A->type,Cyc_Absyn_empty_tqual(0)),_tmp199)))))))),((_tmp198->tl=params,_tmp198))))));}
# 307
{struct Cyc_List_List*_tmp19A;args=((_tmp19A=_cycalloc(sizeof(*_tmp19A)),((_tmp19A->hd=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp7A->name,0),0),((_tmp19A->tl=args,_tmp19A))))));}
goto _LL5C;_LL5C:;}{
# 310
struct _tuple0*funname=Cyc_Toc_temp_var();
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp1A4;struct Cyc_Absyn_Fndecl*_tmp1A3;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmp1A2;struct Cyc_List_List*_tmp1A1;*env.gen_ds=((_tmp1A1=_cycalloc(sizeof(*_tmp1A1)),((_tmp1A1->hd=Cyc_Absyn_new_decl((void*)((_tmp1A4=_cycalloc(sizeof(*_tmp1A4)),((_tmp1A4[0]=((_tmp1A2.tag=1,((_tmp1A2.f1=((_tmp1A3=_cycalloc(sizeof(*_tmp1A3)),((_tmp1A3->sc=Cyc_Absyn_Static,((_tmp1A3->is_inline=0,((_tmp1A3->name=funname,((_tmp1A3->tvs=0,((_tmp1A3->effect=0,((_tmp1A3->ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp1A3->ret_type=rettype,((_tmp1A3->args=params,((_tmp1A3->c_varargs=0,((_tmp1A3->cyc_varargs=0,((_tmp1A3->rgn_po=0,((_tmp1A3->body=s,((_tmp1A3->cached_typ=0,((_tmp1A3->param_vardecls=0,((_tmp1A3->fn_vardecl=0,((_tmp1A3->attributes=0,_tmp1A3)))))))))))))))))))))))))))))))))),_tmp1A2)))),_tmp1A4)))),0),((_tmp1A1->tl=*env.gen_ds,_tmp1A1))))));}
# 317
return Cyc_Absyn_fncall_exp(Cyc_Absyn_var_exp(funname,0),args,0);};};}
# 320
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp8A=e->r;struct _dyneither_ptr*_tmp8C;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp93;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpAA;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpB0;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_List_List*_tmpBD;_LL62: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp8B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp8B->tag != 1)goto _LL64;else{if((((_tmp8B->f1)->f1).Loc_n).tag != 4)goto _LL64;_tmp8C=(_tmp8B->f1)->f2;}}_LL63:
# 325
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmp8C))
e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmp8C))->r;
return;_LL64: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp8D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp8D->tag != 1)goto _LL66;}_LL65:
 return;_LL66: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp8E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp8E->tag != 0)goto _LL68;}_LL67:
# 331
 return;_LL68: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp8F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp8F->tag != 9)goto _LL6A;else{_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;}}_LL69:
# 333
 Cyc_Tovc_apply_varmap(varmap,_tmp90);_tmp93=_tmp91;goto _LL6B;_LL6A: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp92=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp92->tag != 2)goto _LL6C;else{_tmp93=_tmp92->f2;}}_LL6B:
# 335
 for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp93->hd);}
return;_LL6C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp94=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp94->tag != 5)goto _LL6E;else{_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;_tmp97=_tmp94->f3;}}_LL6D:
# 340
 Cyc_Tovc_apply_varmap(varmap,_tmp97);_tmp99=_tmp95;_tmp9A=_tmp96;goto _LL6F;_LL6E: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp98=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp98->tag != 6)goto _LL70;else{_tmp99=_tmp98->f1;_tmp9A=_tmp98->f2;}}_LL6F:
 _tmp9C=_tmp99;_tmp9D=_tmp9A;goto _LL71;_LL70: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp9B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp9B->tag != 7)goto _LL72;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL71:
 _tmp9F=_tmp9C;_tmpA0=_tmp9D;goto _LL73;_LL72: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp9E=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmp9E->tag != 22)goto _LL74;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;}}_LL73:
 _tmpA2=_tmp9F;_tmpA3=_tmpA0;goto _LL75;_LL74: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpA1=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpA1->tag != 8)goto _LL76;else{_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f2;}}_LL75:
 _tmpA5=_tmpA2;_tmpA6=_tmpA3;goto _LL77;_LL76: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpA4=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpA4->tag != 3)goto _LL78;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f3;}}_LL77:
# 346
 Cyc_Tovc_apply_varmap(varmap,_tmpA6);_tmpA8=_tmpA5;goto _LL79;_LL78: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpA7=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpA7->tag != 11)goto _LL7A;else{_tmpA8=_tmpA7->f1;}}_LL79:
# 348
 _tmpAA=_tmpA8;goto _LL7B;_LL7A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA9=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpA9->tag != 12)goto _LL7C;else{_tmpAA=_tmpA9->f1;}}_LL7B:
 _tmpAC=_tmpAA;goto _LL7D;_LL7C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpAB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpAB->tag != 13)goto _LL7E;else{_tmpAC=_tmpAB->f2;}}_LL7D:
 _tmpAE=_tmpAC;goto _LL7F;_LL7E: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpAD=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpAD->tag != 14)goto _LL80;else{_tmpAE=_tmpAD->f1;}}_LL7F:
 _tmpB0=_tmpAE;goto _LL81;_LL80: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpAF=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpAF->tag != 17)goto _LL82;else{_tmpB0=_tmpAF->f1;}}_LL81:
 _tmpB2=_tmpB0;goto _LL83;_LL82: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpB1=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpB1->tag != 19)goto _LL84;else{_tmpB2=_tmpB1->f1;}}_LL83:
 _tmpB4=_tmpB2;goto _LL85;_LL84: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpB3=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpB3->tag != 20)goto _LL86;else{_tmpB4=_tmpB3->f1;}}_LL85:
 _tmpB6=_tmpB4;goto _LL87;_LL86: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpB5=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpB5->tag != 21)goto _LL88;else{_tmpB6=_tmpB5->f1;}}_LL87:
 _tmpB8=_tmpB6;goto _LL89;_LL88: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpB7=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpB7->tag != 4)goto _LL8A;else{_tmpB8=_tmpB7->f1;}}_LL89:
# 357
 Cyc_Tovc_apply_varmap(varmap,_tmpB8);
return;_LL8A: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB9=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpB9->tag != 16)goto _LL8C;}_LL8B:
# 360
 goto _LL8D;_LL8C: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBA=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpBA->tag != 18)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpBB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpBB->tag != 30)goto _LL90;}_LL8F:
 return;_LL90: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpBC=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp8A;if(_tmpBC->tag != 34)goto _LL92;else{_tmpBD=_tmpBC->f2;}}_LL91:
# 365
 for(0;_tmpBD != 0;_tmpBD=_tmpBD->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmpBD->hd)).f2);}
return;_LL92:;_LL93: {
# 369
struct Cyc_Core_Impossible_exn_struct _tmp1AA;const char*_tmp1A9;struct Cyc_Core_Impossible_exn_struct*_tmp1A8;(int)_throw((void*)((_tmp1A8=_cycalloc(sizeof(*_tmp1A8)),((_tmp1A8[0]=((_tmp1AA.tag=Cyc_Core_Impossible,((_tmp1AA.f1=((_tmp1A9="bad exp form in Tocv::apply_varmap",_tag_dyneither(_tmp1A9,sizeof(char),35))),_tmp1AA)))),_tmp1A8)))));}_LL61:;}
# 373
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmpC1=s->r;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Stmt*_tmpC6;struct Cyc_Absyn_Stmt*_tmpC7;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Stmt*_tmpCA;struct Cyc_Absyn_Stmt*_tmpCB;struct Cyc_Absyn_Decl*_tmpCD;struct Cyc_Absyn_Stmt*_tmpCE;_LL95: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmpC2=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmpC1;if(_tmpC2->tag != 0)goto _LL97;}_LL96:
 return Cyc_Absyn_true_exp(0);_LL97: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC3=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC1;if(_tmpC3->tag != 1)goto _LL99;else{_tmpC4=_tmpC3->f1;}}_LL98:
# 377
 Cyc_Tovc_apply_varmap(env.varmap,_tmpC4);
return _tmpC4;_LL99: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC5=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC1;if(_tmpC5->tag != 2)goto _LL9B;else{_tmpC6=_tmpC5->f1;_tmpC7=_tmpC5->f2;}}_LL9A: {
# 381
struct Cyc_Absyn_Exp*_tmpCF=Cyc_Tovc_stmt_to_exp(env,_tmpC6);
struct Cyc_Absyn_Exp*_tmpD0=Cyc_Tovc_stmt_to_exp(env,_tmpC7);
return Cyc_Absyn_seq_exp(_tmpCF,_tmpD0,0);}_LL9B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmpC8=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpC1;if(_tmpC8->tag != 4)goto _LL9D;else{_tmpC9=_tmpC8->f1;_tmpCA=_tmpC8->f2;_tmpCB=_tmpC8->f3;}}_LL9C:
# 385
 Cyc_Tovc_apply_varmap(env.varmap,_tmpC9);{
# 387
struct Cyc_Absyn_Exp*_tmpD1=Cyc_Tovc_stmt_to_exp(env,_tmpCA);
struct Cyc_Absyn_Exp*_tmpD2=Cyc_Tovc_stmt_to_exp(env,_tmpCB);
return Cyc_Absyn_conditional_exp(_tmpC9,_tmpD1,_tmpD2,0);};_LL9D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpCC=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC1;if(_tmpCC->tag != 12)goto _LL9F;else{_tmpCD=_tmpCC->f1;_tmpCE=_tmpCC->f2;}}_LL9E: {
# 391
void*_tmpD3=_tmpCD->r;struct Cyc_Absyn_Vardecl*_tmpD5;_LLA2: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD4=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpD3;if(_tmpD4->tag != 0)goto _LLA4;else{_tmpD5=_tmpD4->f1;}}_LLA3: {
# 394
struct _dyneither_ptr*_tmpD6=(*_tmpD5->name).f2;
struct _tuple0*_tmpD7=Cyc_Toc_temp_var();
_tmpD5->name=_tmpD7;
env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmpD6,Cyc_Absyn_var_exp(_tmpD7,0));{
struct Cyc_List_List*_tmpD8=0;
if(_tmpD5->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpD5->initializer));{
void*_tmpD9=((struct Cyc_Absyn_Exp*)_check_null(_tmpD5->initializer))->r;struct Cyc_List_List*_tmpDB;_LLA7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpDA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD9;if(_tmpDA->tag != 34)goto _LLA9;else{_tmpDB=_tmpDA->f2;}}_LLA8: {
# 413 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpDC=Cyc_Absyn_var_exp(_tmpD5->name,0);
{unsigned int i=0;for(0;_tmpDB != 0;(_tmpDB=_tmpDB->tl,++ i)){
struct Cyc_List_List*_tmp1AB;_tmpD8=((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB->hd=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(_tmpDC,Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmpDB->hd)).f2,0),((_tmp1AB->tl=_tmpD8,_tmp1AB))))));}}
# 419
_tmpD5->initializer=0;
goto _LLA6;}_LLA9:;_LLAA:
 goto _LLA6;_LLA6:;};}
# 424
{struct Cyc_List_List*_tmp1AC;env.all_locals=((_tmp1AC=_region_malloc(env.rgn,sizeof(*_tmp1AC)),((_tmp1AC->hd=_tmpD5,((_tmp1AC->tl=env.all_locals,_tmp1AC))))));}
# 426
(env.encloser)->r=(Cyc_Absyn_decl_stmt(_tmpCD,Cyc_Absyn_new_stmt((env.encloser)->r,0),0))->r;{
struct Cyc_Absyn_Exp*_tmpDF=Cyc_Tovc_stmt_to_exp(env,_tmpCE);
if(_tmpD5->initializer != 0){
_tmpDF=Cyc_Absyn_seq_exp(Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmpD5->name,0),(struct Cyc_Absyn_Exp*)_check_null(_tmpD5->initializer),0),_tmpDF,0);
# 431
_tmpD5->initializer=0;}
# 433
for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){
_tmpDF=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpD8->hd,_tmpDF,0);}
return _tmpDF;};};}_LLA4:;_LLA5: {
struct Cyc_Core_Impossible_exn_struct _tmp1B2;const char*_tmp1B1;struct Cyc_Core_Impossible_exn_struct*_tmp1B0;(int)_throw((void*)((_tmp1B0=_cycalloc(sizeof(*_tmp1B0)),((_tmp1B0[0]=((_tmp1B2.tag=Cyc_Core_Impossible,((_tmp1B2.f1=((_tmp1B1="bad local decl in Tovc::stmt_to_exp",_tag_dyneither(_tmp1B1,sizeof(char),36))),_tmp1B2)))),_tmp1B0)))));}_LLA1:;}_LL9F:;_LLA0:
# 439
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);_LL94:;}
# 443
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 445
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
void*_tmpE3=e->r;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_List_List*_tmp114;struct Cyc_Absyn_Stmt*_tmp116;_LLAC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpE4->tag != 0)goto _LLAE;}_LLAD:
 return;_LLAE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpE5->tag != 1)goto _LLB0;}_LLAF:
 return;_LLB0: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE6=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpE6->tag != 9)goto _LLB2;else{_tmpE7=_tmpE6->f1;_tmpE8=_tmpE6->f2;}}_LLB1:
# 451
 Cyc_Tovc_exp_to_vc(env,_tmpE7);_tmpEA=_tmpE8;goto _LLB3;_LLB2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE9=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpE9->tag != 2)goto _LLB4;else{_tmpEA=_tmpE9->f2;}}_LLB3:
# 453
 for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmpEA->hd);}
return;_LLB4: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpEB=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpEB->tag != 5)goto _LLB6;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;_tmpEE=_tmpEB->f3;}}_LLB5:
# 458
 Cyc_Tovc_exp_to_vc(env,_tmpEE);_tmpF0=_tmpEC;_tmpF1=_tmpED;goto _LLB7;_LLB6: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpEF=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpEF->tag != 6)goto _LLB8;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}}_LLB7:
 _tmpF3=_tmpF0;_tmpF4=_tmpF1;goto _LLB9;_LLB8: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpF2=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpF2->tag != 7)goto _LLBA;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;}}_LLB9:
 _tmpF6=_tmpF3;_tmpF7=_tmpF4;goto _LLBB;_LLBA: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpF5->tag != 22)goto _LLBC;else{_tmpF6=_tmpF5->f1;_tmpF7=_tmpF5->f2;}}_LLBB:
 _tmpF9=_tmpF6;_tmpFA=_tmpF7;goto _LLBD;_LLBC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpF8->tag != 8)goto _LLBE;else{_tmpF9=_tmpF8->f1;_tmpFA=_tmpF8->f2;}}_LLBD:
 _tmpFC=_tmpF9;_tmpFD=_tmpFA;goto _LLBF;_LLBE: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpFB->tag != 3)goto _LLC0;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFB->f3;}}_LLBF:
# 464
 Cyc_Tovc_exp_to_vc(env,_tmpFD);_tmpFF=_tmpFC;goto _LLC1;_LLC0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpFE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmpFE->tag != 11)goto _LLC2;else{_tmpFF=_tmpFE->f1;}}_LLC1:
# 466
 _tmp101=_tmpFF;goto _LLC3;_LLC2: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp100->tag != 12)goto _LLC4;else{_tmp101=_tmp100->f1;}}_LLC3:
 _tmp103=_tmp101;goto _LLC5;_LLC4: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp102->tag != 13)goto _LLC6;else{_tmp103=_tmp102->f2;}}_LLC5:
 _tmp105=_tmp103;goto _LLC7;_LLC6: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp104->tag != 14)goto _LLC8;else{_tmp105=_tmp104->f1;}}_LLC7:
 _tmp107=_tmp105;goto _LLC9;_LLC8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp106->tag != 17)goto _LLCA;else{_tmp107=_tmp106->f1;}}_LLC9:
 _tmp109=_tmp107;goto _LLCB;_LLCA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp108->tag != 19)goto _LLCC;else{_tmp109=_tmp108->f1;}}_LLCB:
 _tmp10B=_tmp109;goto _LLCD;_LLCC: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp10A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp10A->tag != 20)goto _LLCE;else{_tmp10B=_tmp10A->f1;}}_LLCD:
 _tmp10D=_tmp10B;goto _LLCF;_LLCE: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp10C->tag != 21)goto _LLD0;else{_tmp10D=_tmp10C->f1;}}_LLCF:
 _tmp10F=_tmp10D;goto _LLD1;_LLD0: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp10E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp10E->tag != 4)goto _LLD2;else{_tmp10F=_tmp10E->f1;}}_LLD1:
# 475
 Cyc_Tovc_exp_to_vc(env,_tmp10F);
return;_LLD2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp110=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp110->tag != 16)goto _LLD4;}_LLD3:
# 478
 goto _LLD5;_LLD4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp111->tag != 18)goto _LLD6;}_LLD5:
 goto _LLD7;_LLD6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp112=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp112->tag != 30)goto _LLD8;}_LLD7:
 return;_LLD8: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp113->tag != 34)goto _LLDA;else{_tmp114=_tmp113->f2;}}_LLD9:
# 486
 for(0;_tmp114 != 0;_tmp114=_tmp114->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmp114->hd)).f2);}
return;_LLDA: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp115->tag != 35)goto _LLDC;else{_tmp116=_tmp115->f1;}}_LLDB:
# 494
 Cyc_Tovc_stmt_to_vc(env,_tmp116);
# 496
e->r=(Cyc_Tovc_stmt_to_exp(env,_tmp116))->r;
return;_LLDC: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE3;if(_tmp117->tag != 38)goto _LLDE;}_LLDD:
# 499
 return;_LLDE:;_LLDF: {
# 501
struct Cyc_Core_Impossible_exn_struct _tmp1B8;const char*_tmp1B7;struct Cyc_Core_Impossible_exn_struct*_tmp1B6;(int)_throw((void*)((_tmp1B6=_cycalloc(sizeof(*_tmp1B6)),((_tmp1B6[0]=((_tmp1B8.tag=Cyc_Core_Impossible,((_tmp1B8.f1=((_tmp1B7="bad exp form after xlation to C",_tag_dyneither(_tmp1B7,sizeof(char),32))),_tmp1B8)))),_tmp1B6)))));}_LLAB:;}
# 505
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 507
while(1){
env.encloser=s;{
void*_tmp11B=s->r;struct Cyc_Absyn_Stmt*_tmp121;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Stmt*_tmp127;struct Cyc_Absyn_Stmt*_tmp128;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Absyn_Stmt*_tmp12C;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Stmt*_tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Absyn_Stmt*_tmp136;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Decl*_tmp13C;struct Cyc_Absyn_Stmt*_tmp13D;_LLE1: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp11C=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp11C->tag != 0)goto _LLE3;}_LLE2:
 goto _LLE4;_LLE3: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp11D=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp11D->tag != 6)goto _LLE5;}_LLE4:
 goto _LLE6;_LLE5: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp11E=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp11E->tag != 7)goto _LLE7;}_LLE6:
 goto _LLE8;_LLE7: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp11F=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp11F->tag != 8)goto _LLE9;}_LLE8:
 return;_LLE9: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp120=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp120->tag != 13)goto _LLEB;else{_tmp121=_tmp120->f2;}}_LLEA:
# 515
 s=_tmp121;
continue;_LLEB: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp122=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp122->tag != 3)goto _LLED;else{_tmp123=_tmp122->f1;}}_LLEC:
# 518
 if(_tmp123 == 0)
return;
_tmp125=_tmp123;goto _LLEE;_LLED: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp124=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp124->tag != 1)goto _LLEF;else{_tmp125=_tmp124->f1;}}_LLEE:
# 522
 Cyc_Tovc_exp_to_vc(env,_tmp125);
return;_LLEF: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp126=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp126->tag != 2)goto _LLF1;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}_LLF0:
# 525
 Cyc_Tovc_stmt_to_vc(env,_tmp127);
s=_tmp128;
continue;_LLF1: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp129=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp129->tag != 4)goto _LLF3;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;_tmp12C=_tmp129->f3;}}_LLF2:
# 529
 Cyc_Tovc_exp_to_vc(env,_tmp12A);
Cyc_Tovc_stmt_to_vc(env,_tmp12B);
s=_tmp12C;
continue;_LLF3: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp12D=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp12D->tag != 5)goto _LLF5;else{_tmp12E=(_tmp12D->f1).f1;_tmp12F=_tmp12D->f2;}}_LLF4:
# 534
 Cyc_Tovc_exp_to_vc(env,_tmp12E);
s=_tmp12F;
continue;_LLF5: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp130=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp130->tag != 9)goto _LLF7;else{_tmp131=_tmp130->f1;_tmp132=(_tmp130->f2).f1;_tmp133=(_tmp130->f3).f1;_tmp134=_tmp130->f4;}}_LLF6:
# 538
 Cyc_Tovc_exp_to_vc(env,_tmp131);
Cyc_Tovc_exp_to_vc(env,_tmp132);
Cyc_Tovc_exp_to_vc(env,_tmp133);
s=_tmp134;
continue;_LLF7: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp135=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp135->tag != 14)goto _LLF9;else{_tmp136=_tmp135->f1;_tmp137=(_tmp135->f2).f1;}}_LLF8:
# 544
 Cyc_Tovc_exp_to_vc(env,_tmp137);
s=_tmp136;
continue;_LLF9: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp138=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp138->tag != 10)goto _LLFB;else{_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;}}_LLFA:
# 550
 Cyc_Tovc_exp_to_vc(env,_tmp139);
for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp13A->hd)->body);}
return;_LLFB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp13B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp11B;if(_tmp13B->tag != 12)goto _LLFD;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13B->f2;}}_LLFC:
# 555
{void*_tmp13E=_tmp13C->r;struct Cyc_Absyn_Vardecl*_tmp140;_LL100: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp13F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp13E;if(_tmp13F->tag != 0)goto _LL102;else{_tmp140=_tmp13F->f1;}}_LL101:
# 557
 if(Cyc_Tovc_elim_array_initializers){
# 562
void*_tmp141=Cyc_Tcutil_compress(_tmp140->type);_LL105: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp142=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp141;if(_tmp142->tag != 8)goto _LL107;}_LL106:
# 564
 if(_tmp140->initializer != 0){
void*_tmp143=((struct Cyc_Absyn_Exp*)_check_null(_tmp140->initializer))->r;struct Cyc_List_List*_tmp145;_LL10A: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp143;if(_tmp144->tag != 34)goto _LL10C;else{_tmp145=_tmp144->f2;}}_LL10B:
# 567
 _tmp140->initializer=0;{
# 569
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp140->name,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp13D->r,0);
{unsigned int i=0;for(0;_tmp145 != 0;(_tmp145=_tmp145->tl,++ i)){
s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,
Cyc_Absyn_uint_exp(i,0),0),(*((struct _tuple10*)_tmp145->hd)).f2,0),0),s3,0);}}
# 577
_tmp13D->r=s3->r;
goto _LL109;};_LL10C:;_LL10D: {
# 580
const char*_tmp1BC;void*_tmp1BB[1];struct Cyc_String_pa_PrintArg_struct _tmp1BA;struct _dyneither_ptr _tmp146=(_tmp1BA.tag=0,((_tmp1BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_stmt2string(s)),((_tmp1BB[0]=& _tmp1BA,Cyc_aprintf(((_tmp1BC="|%s|: array type bad initializer",_tag_dyneither(_tmp1BC,sizeof(char),33))),_tag_dyneither(_tmp1BB,sizeof(void*),1)))))));
struct Cyc_Core_Impossible_exn_struct _tmp1BF;struct Cyc_Core_Impossible_exn_struct*_tmp1BE;(int)_throw((void*)((_tmp1BE=_cycalloc(sizeof(*_tmp1BE)),((_tmp1BE[0]=((_tmp1BF.tag=Cyc_Core_Impossible,((_tmp1BF.f1=(struct _dyneither_ptr)_tmp146,_tmp1BF)))),_tmp1BE)))));}_LL109:;}
# 584
goto _LL104;_LL107:;_LL108:
 goto _LL104;_LL104:;}
# 588
if(_tmp140->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp140->initializer));
{struct Cyc_List_List*_tmp1C0;env.all_locals=((_tmp1C0=_region_malloc(env.rgn,sizeof(*_tmp1C0)),((_tmp1C0->hd=_tmp140,((_tmp1C0->tl=env.all_locals,_tmp1C0))))));}
s=_tmp13D;
continue;_LL102:;_LL103:
 goto _LLFF;_LLFF:;}
# 595
goto _LLFE;_LLFD:;_LLFE: {
struct Cyc_Core_Impossible_exn_struct _tmp1C6;const char*_tmp1C5;struct Cyc_Core_Impossible_exn_struct*_tmp1C4;(int)_throw((void*)((_tmp1C4=_cycalloc(sizeof(*_tmp1C4)),((_tmp1C4[0]=((_tmp1C6.tag=Cyc_Core_Impossible,((_tmp1C6.f1=((_tmp1C5="bad stmt after xlation to C",_tag_dyneither(_tmp1C5,sizeof(char),28))),_tmp1C6)))),_tmp1C4)))));}_LLE0:;};}}
# 601
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmp150=0;
for(0;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmp151=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_tmp152=_tmp151->r;struct Cyc_Absyn_Fndecl*_tmp154;struct Cyc_List_List*_tmp15F;struct Cyc_List_List*_tmp161;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp165;_LL10F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp153=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp153->tag != 1)goto _LL111;else{_tmp154=_tmp153->f1;}}_LL110:
# 610
 _tmp154->cached_typ=0;
# 617
if(_tmp154->param_vardecls != 0){
struct _RegionHandle _tmp168=_new_region("rgn");struct _RegionHandle*rgn=& _tmp168;_push_region(rgn);
{struct Cyc_List_List**_tmp1C7;struct Cyc_List_List**_tmp169=(_tmp1C7=_region_malloc(rgn,sizeof(*_tmp1C7)),((_tmp1C7[0]=0,_tmp1C7)));
if(_tmp154->param_vardecls == 0){
{const char*_tmp1CB;void*_tmp1CA[1];struct Cyc_String_pa_PrintArg_struct _tmp1C9;(_tmp1C9.tag=0,((_tmp1C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp154->name).f2),((_tmp1CA[0]=& _tmp1C9,Cyc_fprintf(Cyc_stderr,((_tmp1CB="No param vardecls for function %s\n",_tag_dyneither(_tmp1CB,sizeof(char),35))),_tag_dyneither(_tmp1CA,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);}
# 624
{struct Cyc_Tovc_ToExpEnv _tmp1CC;Cyc_Tovc_stmt_to_vc(((_tmp1CC.rgn=rgn,((_tmp1CC.all_locals=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp154->param_vardecls))->v),((_tmp1CC.varmap=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp),((_tmp1CC.encloser=_tmp154->body,((_tmp1CC.gen_ds=_tmp169,_tmp1CC)))))))))),_tmp154->body);}
# 630
if(*_tmp169 != 0){
struct Cyc_Absyn_Vardecl*_tmp1CD;struct Cyc_Absyn_Vardecl*vd=(_tmp1CD=_cycalloc(sizeof(*_tmp1CD)),((_tmp1CD->sc=_tmp154->sc,((_tmp1CD->name=_tmp154->name,((_tmp1CD->tq=Cyc_Absyn_const_tqual(0),((_tmp1CD->type=
Cyc_Tcutil_fndecl2typ(_tmp154),((_tmp1CD->initializer=0,((_tmp1CD->rgn=0,((_tmp1CD->attributes=_tmp154->attributes,((_tmp1CD->escapes=0,_tmp1CD)))))))))))))))));
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1D3;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp1D2;struct Cyc_List_List*_tmp1D1;_tmp150=((_tmp1D1=_cycalloc(sizeof(*_tmp1D1)),((_tmp1D1->hd=Cyc_Absyn_new_decl((void*)((_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3[0]=((_tmp1D2.tag=0,((_tmp1D2.f1=vd,_tmp1D2)))),_tmp1D3)))),0),((_tmp1D1->tl=_tmp150,_tmp1D1))))));}
# 635
_tmp150=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp169,_tmp150);}}
# 619
;_pop_region(rgn);}
# 639
goto _LL112;_LL111: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp155=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp155->tag != 0)goto _LL113;}_LL112:
 goto _LL114;_LL113: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp156=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp156->tag != 2)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp157=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp157->tag != 3)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp158=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp158->tag != 4)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp159=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp159->tag != 5)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp15A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp15A->tag != 6)goto _LL11D;}_LL11C:
 goto _LL11E;_LL11D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp15B=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp15B->tag != 7)goto _LL11F;}_LL11E:
 goto _LL120;_LL11F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp15C=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp15C->tag != 8)goto _LL121;}_LL120:
 goto _LL122;_LL121: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp15D=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp15D->tag != 9)goto _LL123;}_LL122:
# 649
{struct Cyc_List_List*_tmp1D4;_tmp150=((_tmp1D4=_cycalloc(sizeof(*_tmp1D4)),((_tmp1D4->hd=_tmp151,((_tmp1D4->tl=_tmp150,_tmp1D4))))));}
goto _LL10E;_LL123: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp15E=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp15E->tag != 10)goto _LL125;else{_tmp15F=_tmp15E->f2;}}_LL124:
 _tmp161=_tmp15F;goto _LL126;_LL125: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp160=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp160->tag != 11)goto _LL127;else{_tmp161=_tmp160->f2;}}_LL126:
 _tmp163=_tmp161;goto _LL128;_LL127: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp162=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp162->tag != 12)goto _LL129;else{_tmp163=_tmp162->f1;}}_LL128:
 _tmp165=_tmp163;goto _LL12A;_LL129: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp164=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp164->tag != 13)goto _LL12B;else{_tmp165=_tmp164->f1;}}_LL12A:
 goto _LL12C;_LL12B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp166=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp166->tag != 14)goto _LL12D;}_LL12C:
 goto _LL12E;_LL12D: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp167=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp152;if(_tmp167->tag != 15)goto _LL10E;}_LL12E: {
# 657
struct Cyc_Core_Impossible_exn_struct _tmp1DA;const char*_tmp1D9;struct Cyc_Core_Impossible_exn_struct*_tmp1D8;(int)_throw((void*)((_tmp1D8=_cycalloc(sizeof(*_tmp1D8)),((_tmp1D8[0]=((_tmp1DA.tag=Cyc_Core_Impossible,((_tmp1DA.f1=((_tmp1D9="nested translation unit after translation to C",_tag_dyneither(_tmp1D9,sizeof(char),47))),_tmp1DA)))),_tmp1D8)))));}_LL10E:;}
# 660
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp150);}
