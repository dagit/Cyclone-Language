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
_zero_arr_plus_char_fn(char *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
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
  _zero_arr_inplace_plus_char_fn(x,i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn(x,i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn(x,i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn(x,i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn(x,i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn(x,i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn(x,i,__FILE__,__LINE__)

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
typedef int Cyc_bool;
# 69 "core.h"
void exit(int);struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
# 272
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char*getenv(const char*);
# 318
int system(const char*);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 234 "core.h"
struct _dyneither_ptr Cstring_to_string(char*);
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
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 319
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 347
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 374
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 50
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 51
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
# 63
extern int Cyc_Position_max_errors;
# 65
int Cyc_Position_error_p();
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1172 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1174
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
# 33
extern int Cyc_Parse_no_register;struct _tuple10{struct _tuple10*tl;};struct Cyc_FlatList{struct _tuple10*tl;};
# 36
typedef struct _tuple10*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd;};
# 53
typedef struct _tuple12*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 75 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 31 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
# 32
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 33
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 114 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();
# 83
extern int Cyc_Tcutil_warn_alias_coerce;
# 86
extern int Cyc_Tcutil_warn_region_coerce;
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
# 28
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple19{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple19 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple20{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple20 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 35 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple21{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple21*info);
# 76 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_Lex_compile_for_boot_r;
# 46
void Cyc_Lex_pos_init();
# 47
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 50
int Cyc_Toc_warn_bounds_checks;
# 51
int Cyc_Toc_warn_all_null_deref;
# 60
static int Cyc_pp_r=0;
# 61
int Cyc_noexpand_r=0;
# 62
static int Cyc_noshake_r=0;
# 63
static int Cyc_stop_after_cpp_r=0;
# 64
static int Cyc_parseonly_r=0;
# 65
static int Cyc_tc_r=0;
# 66
static int Cyc_cf_r=0;
# 67
static int Cyc_noprint_r=0;
# 68
static int Cyc_ic_r=0;
# 69
static int Cyc_toc_r=0;
# 70
static int Cyc_stop_after_objectfile_r=0;
# 71
static int Cyc_stop_after_asmfile_r=0;
# 72
static int Cyc_inline_functions_r=0;
# 73
static int Cyc_elim_se_r=0;
# 74
static int Cyc_v_r=0;
# 75
static int Cyc_save_temps_r=0;
# 76
static int Cyc_save_c_r=0;
# 77
static int Cyc_nogc_r=0;
# 78
static int Cyc_pa_r=0;
# 79
static int Cyc_pg_r=0;
# 80
static int Cyc_nocheck_r=0;
# 81
static int Cyc_add_cyc_namespace_r=1;
# 82
static int Cyc_generate_line_directives_r=1;
# 83
static int Cyc_print_full_evars_r=0;
# 84
static int Cyc_print_all_tvars_r=0;
# 85
static int Cyc_print_all_kinds_r=0;
# 86
static int Cyc_print_all_effects_r=0;
# 87
static int Cyc_nocyc_setjmp_r=0;
# 88
static int Cyc_generate_interface_r=0;
# 91
static struct Cyc_List_List*Cyc_ccargs=0;
# 92
static void Cyc_add_ccarg(struct _dyneither_ptr s){
# 93
struct _dyneither_ptr*_tmp3CC;struct Cyc_List_List*_tmp3CB;Cyc_ccargs=((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB->hd=((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC[0]=s,_tmp3CC)))),((_tmp3CB->tl=Cyc_ccargs,_tmp3CB))))));}
# 99
void Cyc_set_c_compiler(struct _dyneither_ptr s){
# 100
const char*_tmp3CD;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3CD="vc",_tag_dyneither(_tmp3CD,sizeof(char),3))))== 0){
# 101
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
# 102
const char*_tmp3CE;Cyc_add_ccarg(((_tmp3CE="-DVC_C",_tag_dyneither(_tmp3CE,sizeof(char),7))));};}else{
# 104
const char*_tmp3CF;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3CF="gcc",_tag_dyneither(_tmp3CF,sizeof(char),4))))== 0){
# 105
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
# 106
const char*_tmp3D0;Cyc_add_ccarg(((_tmp3D0="-DGCC_C",_tag_dyneither(_tmp3D0,sizeof(char),8))));};}}}
# 112
void Cyc_set_port_c_code(){
# 113
Cyc_Absyn_porting_c_code=1;
# 114
Cyc_Position_max_errors=65535;
# 115
Cyc_save_c_r=1;
# 116
Cyc_parseonly_r=1;}
# 120
static struct _dyneither_ptr*Cyc_output_file=0;
# 121
static void Cyc_set_output_file(struct _dyneither_ptr s){
# 122
struct _dyneither_ptr*_tmp3D1;Cyc_output_file=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=s,_tmp3D1))));}
# 125
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
# 126
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
# 127
Cyc_specified_interface=s;}
# 134
extern char*Cdef_inc_path;
# 135
extern char*Cdef_lib_path;
# 136
extern char*Carch;
# 137
extern char*Ccomp;
# 138
extern char*Cversion;static char _tmp7[1]="";
# 141
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
# 142
static void Cyc_set_cpp(struct _dyneither_ptr s){
# 143
Cyc_cpp=s;}
# 146
static struct Cyc_List_List*Cyc_cppargs=0;
# 147
static void Cyc_add_cpparg(struct _dyneither_ptr s){
# 148
struct _dyneither_ptr*_tmp3D4;struct Cyc_List_List*_tmp3D3;Cyc_cppargs=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->hd=((_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4[0]=s,_tmp3D4)))),((_tmp3D3->tl=Cyc_cppargs,_tmp3D3))))));}
# 151
static void Cyc_add_iprefix(struct _dyneither_ptr s){
# 152
const char*_tmp3D8;void*_tmp3D7[1];struct Cyc_String_pa_PrintArg_struct _tmp3D6;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3D6.tag=0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3D7[0]=& _tmp3D6,Cyc_aprintf(((_tmp3D8="-iprefix %s",_tag_dyneither(_tmp3D8,sizeof(char),12))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))))));}
# 154
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
# 155
const char*_tmp3DC;void*_tmp3DB[1];struct Cyc_String_pa_PrintArg_struct _tmp3DA;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DA.tag=0,((_tmp3DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3DB[0]=& _tmp3DA,Cyc_aprintf(((_tmp3DC="-iwithprefix %s",_tag_dyneither(_tmp3DC,sizeof(char),16))),_tag_dyneither(_tmp3DB,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
# 158
const char*_tmp3E0;void*_tmp3DF[1];struct Cyc_String_pa_PrintArg_struct _tmp3DE;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DE.tag=0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3DF[0]=& _tmp3DE,Cyc_aprintf(((_tmp3E0="-iwithprefixbefore %s",_tag_dyneither(_tmp3E0,sizeof(char),22))),_tag_dyneither(_tmp3DF,sizeof(void*),1)))))))));}
# 160
static void Cyc_add_isystem(struct _dyneither_ptr s){
# 161
const char*_tmp3E4;void*_tmp3E3[1];struct Cyc_String_pa_PrintArg_struct _tmp3E2;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E2.tag=0,((_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E3[0]=& _tmp3E2,Cyc_aprintf(((_tmp3E4="-isystem %s",_tag_dyneither(_tmp3E4,sizeof(char),12))),_tag_dyneither(_tmp3E3,sizeof(void*),1)))))))));}
# 163
static void Cyc_add_idirafter(struct _dyneither_ptr s){
# 164
const char*_tmp3E8;void*_tmp3E7[1];struct Cyc_String_pa_PrintArg_struct _tmp3E6;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E6.tag=0,((_tmp3E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E7[0]=& _tmp3E6,Cyc_aprintf(((_tmp3E8="-idirafter %s",_tag_dyneither(_tmp3E8,sizeof(char),14))),_tag_dyneither(_tmp3E7,sizeof(void*),1)))))))));}
# 167
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
# 168
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 169
Cyc_toc_r=1;
# 170
Cyc_target_arch=s;}
# 174
static void Cyc_print_version(){
# 175
{const char*_tmp3EC;void*_tmp3EB[1];struct Cyc_String_pa_PrintArg_struct _tmp3EA;(_tmp3EA.tag=0,((_tmp3EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3EB[0]=& _tmp3EA,Cyc_printf(((_tmp3EC="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3EC,sizeof(char),34))),_tag_dyneither(_tmp3EB,sizeof(void*),1)))))));}
# 176
{const char*_tmp3F1;void*_tmp3F0[2];struct Cyc_String_pa_PrintArg_struct _tmp3EF;struct Cyc_String_pa_PrintArg_struct _tmp3EE;(_tmp3EE.tag=0,((_tmp3EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 177
Cstring_to_string(Ccomp)),((_tmp3EF.tag=0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3F0[0]=& _tmp3EF,((_tmp3F0[1]=& _tmp3EE,Cyc_printf(((_tmp3F1="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3F1,sizeof(char),39))),_tag_dyneither(_tmp3F0,sizeof(void*),2)))))))))))));}
# 178
{const char*_tmp3F5;void*_tmp3F4[1];struct Cyc_String_pa_PrintArg_struct _tmp3F3;(_tmp3F3.tag=0,((_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3F4[0]=& _tmp3F3,Cyc_printf(((_tmp3F5="Standard library directory: %s\n",_tag_dyneither(_tmp3F5,sizeof(char),32))),_tag_dyneither(_tmp3F4,sizeof(void*),1)))))));}
# 179
{const char*_tmp3F9;void*_tmp3F8[1];struct Cyc_String_pa_PrintArg_struct _tmp3F7;(_tmp3F7.tag=0,((_tmp3F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3F8[0]=& _tmp3F7,Cyc_printf(((_tmp3F9="Standard include directory: %s\n",_tag_dyneither(_tmp3F9,sizeof(char),32))),_tag_dyneither(_tmp3F8,sizeof(void*),1)))))));}
# 180
exit(0);}
# 183
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
# 184
unsigned long _tmp26=Cyc_strlen((struct _dyneither_ptr)s);
# 185
if(_tmp26 <= 4)return 0;else{
# 186
const char*_tmp3FA;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp26 - 4)),((_tmp3FA=".cyc",_tag_dyneither(_tmp3FA,sizeof(char),5))))== 0;}}
# 189
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
# 190
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
# 191
unsigned long _tmp28=Cyc_strlen((struct _dyneither_ptr)s);
# 192
if(_tmp28 <= 2)return;{
# 193
struct _dyneither_ptr _tmp29=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp28 - 2);
# 194
struct _dyneither_ptr*_tmp3FD;struct Cyc_List_List*_tmp3FC;Cyc_cyclone_exec_path=((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->hd=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD[0]=_tmp29,_tmp3FD)))),((_tmp3FC->tl=Cyc_cyclone_exec_path,_tmp3FC))))));};}
# 202
static struct Cyc_List_List*Cyc_libargs=0;
# 203
static void Cyc_add_libarg(struct _dyneither_ptr s){
# 204
const char*_tmp3FE;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3FE="-lxml",_tag_dyneither(_tmp3FE,sizeof(char),6))))== 0){
# 205
if(!Cyc_pa_r)
# 206
Cyc_add_ccarg(s);else{
# 208
const char*_tmp3FF;Cyc_add_ccarg(((_tmp3FF="-lxml_a",_tag_dyneither(_tmp3FF,sizeof(char),8))));}}else{
# 210
struct _dyneither_ptr*_tmp402;struct Cyc_List_List*_tmp401;Cyc_libargs=((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->hd=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=s,_tmp402)))),((_tmp401->tl=Cyc_libargs,_tmp401))))));}}
# 213
static void Cyc_add_marg(struct _dyneither_ptr s){
# 214
Cyc_add_ccarg(s);}
# 217
static void Cyc_set_save_temps(){
# 218
Cyc_save_temps_r=1;{
# 219
const char*_tmp403;Cyc_add_ccarg(((_tmp403="-save-temps",_tag_dyneither(_tmp403,sizeof(char),12))));};}
# 222
static int Cyc_produce_dependencies=0;
# 223
static void Cyc_set_produce_dependencies(){
# 224
Cyc_stop_after_cpp_r=1;
# 225
Cyc_produce_dependencies=1;{
# 226
const char*_tmp404;Cyc_add_cpparg(((_tmp404="-M",_tag_dyneither(_tmp404,sizeof(char),3))));};}
# 229
static struct _dyneither_ptr*Cyc_dependencies_target=0;
# 230
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
# 231
struct _dyneither_ptr*_tmp405;Cyc_dependencies_target=((_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405[0]=s,_tmp405))));}
# 234
static void Cyc_set_stop_after_objectfile(){
# 235
Cyc_stop_after_objectfile_r=1;{
# 236
const char*_tmp406;Cyc_add_ccarg(((_tmp406="-c",_tag_dyneither(_tmp406,sizeof(char),3))));};}
# 239
static void Cyc_set_nocppprecomp(){
# 240
{const char*_tmp407;Cyc_add_cpparg(((_tmp407="-no-cpp-precomp",_tag_dyneither(_tmp407,sizeof(char),16))));}{
# 241
const char*_tmp408;Cyc_add_ccarg(((_tmp408="-no-cpp-precomp",_tag_dyneither(_tmp408,sizeof(char),16))));};}
# 244
static void Cyc_clear_lineno(){
# 245
Cyc_generate_line_directives_r=0;
# 246
Cyc_set_save_temps();}
# 248
static void Cyc_set_inline_functions(){
# 249
Cyc_inline_functions_r=1;}
# 251
static void Cyc_set_elim_se(){
# 252
Cyc_elim_se_r=1;
# 253
Cyc_set_inline_functions();}
# 255
static void Cyc_set_tovc(){
# 256
Cyc_Cyclone_tovc_r=1;
# 257
{const char*_tmp409;Cyc_add_ccarg(((_tmp409="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp409,sizeof(char),18))));}
# 258
Cyc_set_elim_se();}
# 260
static void Cyc_set_noboundschecks(){
# 261
const char*_tmp40A;Cyc_add_ccarg(((_tmp40A="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp40A,sizeof(char),23))));}
# 263
static void Cyc_set_nonullchecks(){
# 264
const char*_tmp40B;Cyc_add_ccarg(((_tmp40B="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp40B,sizeof(char),21))));}
# 266
static void Cyc_set_nochecks(){
# 267
Cyc_set_noboundschecks();
# 268
Cyc_set_nonullchecks();
# 269
Cyc_nocheck_r=1;}
# 272
static void Cyc_set_nocyc(){
# 273
Cyc_add_cyc_namespace_r=0;{
# 274
const char*_tmp40C;Cyc_add_ccarg(((_tmp40C="-DNO_CYC_PREFIX",_tag_dyneither(_tmp40C,sizeof(char),16))));};}
# 277
static void Cyc_set_pa(){
# 278
Cyc_pa_r=1;
# 279
{const char*_tmp40D;Cyc_add_ccarg(((_tmp40D="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp40D,sizeof(char),21))));}{
# 280
const char*_tmp40E;Cyc_add_cpparg(((_tmp40E="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp40E,sizeof(char),21))));};}
# 283
static void Cyc_set_pg(){
# 284
Cyc_pg_r=1;{
# 285
const char*_tmp40F;Cyc_add_ccarg(((_tmp40F="-pg",_tag_dyneither(_tmp40F,sizeof(char),4))));};}
# 288
static void Cyc_set_stop_after_asmfile(){
# 289
Cyc_stop_after_asmfile_r=1;{
# 290
const char*_tmp410;Cyc_add_ccarg(((_tmp410="-S",_tag_dyneither(_tmp410,sizeof(char),3))));};}
# 293
static void Cyc_set_all_warnings(){
# 294
Cyc_Toc_warn_bounds_checks=1;
# 295
Cyc_Toc_warn_all_null_deref=1;
# 296
Cyc_NewControlFlow_warn_lose_unique=1;
# 297
Cyc_Tcutil_warn_alias_coerce=1;
# 298
Cyc_Tcutil_warn_region_coerce=1;}
# 302
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 306
typedef enum Cyc_inputtype Cyc_inputtype_t;
# 307
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
# 308
static void Cyc_set_inputtype(struct _dyneither_ptr s){
# 309
const char*_tmp411;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp411="cyc",_tag_dyneither(_tmp411,sizeof(char),4))))== 0)
# 310
Cyc_intype=Cyc_CYCLONEFILE;else{
# 311
const char*_tmp412;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp412="none",_tag_dyneither(_tmp412,sizeof(char),5))))== 0)
# 312
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 314
const char*_tmp416;void*_tmp415[1];struct Cyc_String_pa_PrintArg_struct _tmp414;(_tmp414.tag=0,((_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp415[0]=& _tmp414,Cyc_fprintf(Cyc_stderr,((_tmp416="Input type '%s' not supported\n",_tag_dyneither(_tmp416,sizeof(char),31))),_tag_dyneither(_tmp415,sizeof(void*),1)))))));}}}
# 318
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
# 319
struct _dyneither_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);
# 320
struct _dyneither_ptr _tmp44=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 324
struct _dyneither_ptr _tmp45=Cyc_strlen((struct _dyneither_ptr)_tmp43)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp43,(struct _dyneither_ptr)_tmp44): _tmp44;
# 325
return(struct _dyneither_ptr)_tmp45;}
# 330
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 332
static void Cyc_add_other(struct _dyneither_ptr s){
# 333
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 335
{struct _dyneither_ptr*_tmp419;struct Cyc_List_List*_tmp418;Cyc_cyclone_files=((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->hd=((_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419[0]=s,_tmp419)))),((_tmp418->tl=Cyc_cyclone_files,_tmp418))))));}{
# 338
struct _dyneither_ptr _tmp48=Cyc_make_base_filename(s,Cyc_output_file);
# 339
const char*_tmp41A;struct _dyneither_ptr _tmp49=Cyc_strconcat((struct _dyneither_ptr)_tmp48,((_tmp41A=".c",_tag_dyneither(_tmp41A,sizeof(char),3))));
# 340
Cyc_add_ccarg((struct _dyneither_ptr)_tmp49);};}else{
# 342
Cyc_add_ccarg(s);}}
# 345
static void Cyc_remove_file(struct _dyneither_ptr s){
# 346
if(Cyc_save_temps_r)return;else{
# 347
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple22{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 356
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
# 357
struct Cyc_List_List*_tmp4B=0;
# 358
int c;
# 359
int i;
# 360
char strname[256];
# 361
char strvalue[4096];
# 362
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),(const char*)"r");
# 364
if(spec_file == 0){
# 365
{const char*_tmp41E;void*_tmp41D[1];struct Cyc_String_pa_PrintArg_struct _tmp41C;(_tmp41C.tag=0,((_tmp41C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp41D[0]=& _tmp41C,Cyc_fprintf(Cyc_stderr,((_tmp41E="Error opening spec file %s\n",_tag_dyneither(_tmp41E,sizeof(char),28))),_tag_dyneither(_tmp41D,sizeof(void*),1)))))));}
# 366
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 367
return 0;}
# 370
while(1){
# 371
while(1){
# 372
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 373
if(isspace(c))continue;
# 374
if(c == '*')break;
# 375
if(c != - 1){
# 376
{const char*_tmp423;void*_tmp422[2];struct Cyc_String_pa_PrintArg_struct _tmp421;struct Cyc_Int_pa_PrintArg_struct _tmp420;(_tmp420.tag=1,((_tmp420.f1=(unsigned long)c,((_tmp421.tag=0,((_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp422[0]=& _tmp421,((_tmp422[1]=& _tmp420,Cyc_fprintf(Cyc_stderr,((_tmp423="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp423,sizeof(char),55))),_tag_dyneither(_tmp422,sizeof(void*),2)))))))))))));}
# 379
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 381
goto CLEANUP_AND_RETURN;}
# 383
JUST_AFTER_STAR:
# 384
 i=0;
# 385
while(1){
# 386
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 387
if(c == - 1){
# 388
{const char*_tmp427;void*_tmp426[1];struct Cyc_String_pa_PrintArg_struct _tmp425;(_tmp425.tag=0,((_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp426[0]=& _tmp425,Cyc_fprintf(Cyc_stderr,((_tmp427="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp427,sizeof(char),44))),_tag_dyneither(_tmp426,sizeof(void*),1)))))));}
# 391
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 392
goto CLEANUP_AND_RETURN;}
# 394
if(c == ':'){
# 395
strname[_check_known_subscript_notnull(256,i)]='\000';
# 396
break;}
# 398
strname[_check_known_subscript_notnull(256,i)]=(char)c;
# 399
++ i;
# 400
if(i >= 256){
# 401
{const char*_tmp42B;void*_tmp42A[1];struct Cyc_String_pa_PrintArg_struct _tmp429;(_tmp429.tag=0,((_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42A[0]=& _tmp429,Cyc_fprintf(Cyc_stderr,((_tmp42B="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp42B,sizeof(char),50))),_tag_dyneither(_tmp42A,sizeof(void*),1)))))));}
# 404
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 405
goto CLEANUP_AND_RETURN;}}
# 408
while(1){
# 409
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 410
if(isspace(c))continue;
# 411
break;}
# 413
if(c == '*'){
# 415
{struct _tuple22*_tmp438;struct _dyneither_ptr*_tmp437;const char*_tmp436;struct _dyneither_ptr*_tmp435;struct Cyc_List_List*_tmp434;_tmp4B=((_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434->hd=((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->f1=((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp437)))),((_tmp438->f2=(
# 416
(_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp436="",_tag_dyneither(_tmp436,sizeof(char),1)))),_tmp435)))),_tmp438)))))),((_tmp434->tl=_tmp4B,_tmp434))))));}
# 417
goto JUST_AFTER_STAR;}
# 420
strvalue[0]=(char)c;
# 421
i=1;
# 422
while(1){
# 423
c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);
# 424
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
# 425
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
# 426
++ i;
# 427
if(i >= 4096){
# 428
{const char*_tmp43D;void*_tmp43C[2];struct Cyc_String_pa_PrintArg_struct _tmp43B;struct Cyc_String_pa_PrintArg_struct _tmp43A;(_tmp43A.tag=0,((_tmp43A.f1=(struct _dyneither_ptr)
# 430
_tag_dyneither(strname,sizeof(char),256),((_tmp43B.tag=0,((_tmp43B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp43C[0]=& _tmp43B,((_tmp43C[1]=& _tmp43A,Cyc_fprintf(Cyc_stderr,((_tmp43D="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp43D,sizeof(char),50))),_tag_dyneither(_tmp43C,sizeof(void*),2)))))))))))));}
# 431
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 432
goto CLEANUP_AND_RETURN;}}
# 436
{struct _tuple22*_tmp446;struct _dyneither_ptr*_tmp445;struct _dyneither_ptr*_tmp444;struct Cyc_List_List*_tmp443;_tmp4B=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->hd=((_tmp446=_cycalloc(sizeof(*_tmp446)),((_tmp446->f1=((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp445)))),((_tmp446->f2=(
# 437
(_tmp444=_cycalloc(sizeof(*_tmp444)),((_tmp444[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp444)))),_tmp446)))))),((_tmp443->tl=_tmp4B,_tmp443))))));}
# 438
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 441
CLEANUP_AND_RETURN:
# 442
 Cyc_fclose((struct Cyc___cycFILE*)spec_file);
# 443
return _tmp4B;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp450(struct _dyneither_ptr*_tmp6C,unsigned int*_tmp44F,unsigned int*_tmp44E,struct _dyneither_ptr**_tmp44C){for(*_tmp44F=0;*_tmp44F < *_tmp44E;(*_tmp44F)++){(*_tmp44C)[*_tmp44F]=*((struct _dyneither_ptr**)(*_tmp6C).curr)[(int)*_tmp44F];}}
# 447
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
# 448
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
# 449
unsigned long _tmp66=Cyc_strlen((struct _dyneither_ptr)cmdline);
# 450
struct Cyc_List_List*_tmp67=0;
# 451
char buf[4096];
# 452
int i=0;
# 453
int j=0;
# 454
if(_tmp66 > 4096)goto DONE;
# 455
while(1){
# 457
while(1){
# 458
if(i >= _tmp66)goto DONE;
# 459
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
# 460
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
# 461
++ i;}
# 463
j=0;
# 468
while(1){
# 469
if(i >= _tmp66)break;
# 470
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
# 471
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
# 472
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 474
++ i;
# 475
if(i >= _tmp66)break;
# 476
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
# 477
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
# 478
++ j;}else{
# 481
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
# 482
++ j;}
# 484
++ i;}
# 486
if(j < 4096)
# 487
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 489
struct _dyneither_ptr*_tmp449;struct Cyc_List_List*_tmp448;_tmp67=((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->hd=((_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp449)))),((_tmp448->tl=_tmp67,_tmp448))))));};}
# 491
DONE:
# 492
 _tmp67=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp67);
# 493
{struct Cyc_List_List*_tmp44A;_tmp67=((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp44A->tl=_tmp67,_tmp44A))))));}{
# 494
struct _dyneither_ptr _tmp6C=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp67);
# 495
unsigned int _tmp44F;unsigned int _tmp44E;struct _dyneither_ptr _tmp44D;struct _dyneither_ptr*_tmp44C;unsigned int _tmp44B;struct _dyneither_ptr _tmp6D=(_tmp44B=_get_dyneither_size(_tmp6C,sizeof(struct _dyneither_ptr*)),((_tmp44C=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp44B)),((_tmp44D=_tag_dyneither(_tmp44C,sizeof(struct _dyneither_ptr),_tmp44B),((((_tmp44E=_tmp44B,_tmp450(& _tmp6C,& _tmp44F,& _tmp44E,& _tmp44C))),_tmp44D)))))));
# 496
return _tmp6D;};};}
# 501
int Cyc_compile_failure=0;
# 503
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
# 504
struct _handler_cons _tmp72;_push_handler(& _tmp72);{int _tmp74=0;if(setjmp(_tmp72.handler))_tmp74=1;if(!_tmp74){{struct Cyc___cycFILE*_tmp75=(struct Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp75;};_pop_handler();}else{void*_tmp73=(void*)_exn_thrown;void*_tmp77=_tmp73;_LL1:;_LL2:
# 507
 Cyc_compile_failure=1;
# 508
{const char*_tmp455;void*_tmp454[2];struct Cyc_String_pa_PrintArg_struct _tmp453;struct Cyc_String_pa_PrintArg_struct _tmp452;(_tmp452.tag=0,((_tmp452.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp453.tag=0,((_tmp453.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp454[0]=& _tmp453,((_tmp454[1]=& _tmp452,Cyc_fprintf(Cyc_stderr,((_tmp455="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp455,sizeof(char),29))),_tag_dyneither(_tmp454,sizeof(void*),2)))))))))))));}
# 509
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 510
return 0;_LL3:;_LL4:(void)_throw(_tmp77);_LL0:;}};}
# 514
void CYCALLOCPROFILE_mark(const char*s);
# 516
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 520
const char*_tmp456;struct _dyneither_ptr exn_string=(_tmp456="",_tag_dyneither(_tmp456,sizeof(char),1));
# 521
const char*_tmp457;struct _dyneither_ptr explain_string=(_tmp457="",_tag_dyneither(_tmp457,sizeof(char),1));
# 522
int other_exn=0;
# 523
struct Cyc_Core_Impossible_exn_struct _tmp45D;const char*_tmp45C;struct Cyc_Core_Impossible_exn_struct*_tmp45B;void*ex=(void*)((_tmp45B=_cycalloc(sizeof(*_tmp45B)),((_tmp45B[0]=((_tmp45D.tag=Cyc_Core_Impossible,((_tmp45D.f1=((_tmp45C="",_tag_dyneither(_tmp45C,sizeof(char),1))),_tmp45D)))),_tmp45B))));
# 524
struct Cyc_List_List*_tmp7C=0;
# 526
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 528
{struct _handler_cons _tmp7D;_push_handler(& _tmp7D);{int _tmp7F=0;if(setjmp(_tmp7D.handler))_tmp7F=1;if(!_tmp7F){_tmp7C=f(env,tds);;_pop_handler();}else{void*_tmp7E=(void*)_exn_thrown;void*_tmp81=_tmp7E;struct _dyneither_ptr _tmp83;struct _dyneither_ptr _tmp86;struct _dyneither_ptr _tmp88;_LL6: {struct Cyc_Core_Impossible_exn_struct*_tmp82=(struct Cyc_Core_Impossible_exn_struct*)_tmp81;if(_tmp82->tag != Cyc_Core_Impossible)goto _LL8;else{_tmp83=_tmp82->f1;}}_LL7:
# 531
{const char*_tmp45E;exn_string=((_tmp45E="Exception Core::Impossible",_tag_dyneither(_tmp45E,sizeof(char),27)));}
# 532
explain_string=_tmp83;
# 533
goto _LL5;_LL8: {struct Cyc_Dict_Absent_exn_struct*_tmp84=(struct Cyc_Dict_Absent_exn_struct*)_tmp81;if(_tmp84->tag != Cyc_Dict_Absent)goto _LLA;}_LL9:
# 535
{const char*_tmp45F;exn_string=((_tmp45F="Exception Dict::Absent",_tag_dyneither(_tmp45F,sizeof(char),23)));}
# 536
goto _LL5;_LLA: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp85=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp81;if(_tmp85->tag != Cyc_Core_Invalid_argument)goto _LLC;else{_tmp86=_tmp85->f1;}}_LLB:
# 538
{const char*_tmp460;exn_string=((_tmp460="Exception Core::Invalid_argument",_tag_dyneither(_tmp460,sizeof(char),33)));}
# 539
explain_string=_tmp86;
# 540
goto _LL5;_LLC: {struct Cyc_Core_Failure_exn_struct*_tmp87=(struct Cyc_Core_Failure_exn_struct*)_tmp81;if(_tmp87->tag != Cyc_Core_Failure)goto _LLE;else{_tmp88=_tmp87->f1;}}_LLD:
# 542
{const char*_tmp461;exn_string=((_tmp461="Exception Core::Failure",_tag_dyneither(_tmp461,sizeof(char),24)));}
# 543
explain_string=_tmp88;
# 544
goto _LL5;_LLE:;_LLF:
# 546
 ex=_tmp81;
# 547
other_exn=1;
# 548
{const char*_tmp462;exn_string=((_tmp462="Uncaught exception",_tag_dyneither(_tmp462,sizeof(char),19)));}
# 549
goto _LL5;_LL10:;_LL11:(void)_throw(_tmp81);_LL5:;}};}
# 551
if(Cyc_Position_error_p())
# 552
Cyc_compile_failure=1;
# 553
{const char*_tmp463;if(Cyc_strcmp((struct _dyneither_ptr)exn_string,((_tmp463="",_tag_dyneither(_tmp463,sizeof(char),1))))!= 0){
# 554
Cyc_compile_failure=1;{
# 555
const char*_tmp469;void*_tmp468[3];struct Cyc_String_pa_PrintArg_struct _tmp467;struct Cyc_String_pa_PrintArg_struct _tmp466;struct Cyc_String_pa_PrintArg_struct _tmp465;(_tmp465.tag=0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)explain_string),((_tmp466.tag=0,((_tmp466.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp467.tag=0,((_tmp467.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)exn_string),((_tmp468[0]=& _tmp467,((_tmp468[1]=& _tmp466,((_tmp468[2]=& _tmp465,Cyc_fprintf(Cyc_stderr,((_tmp469="\n%s thrown during %s: %s",_tag_dyneither(_tmp469,sizeof(char),25))),_tag_dyneither(_tmp468,sizeof(void*),3)))))))))))))))))));};}}
# 558
if(Cyc_compile_failure){
# 559
{const char*_tmp46C;void*_tmp46B;(_tmp46B=0,Cyc_fprintf(Cyc_stderr,((_tmp46C="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp46C,sizeof(char),22))),_tag_dyneither(_tmp46B,sizeof(void*),0)));}
# 560
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 561
if(other_exn)
# 562
(int)_throw(ex);
# 563
return _tmp7C;}else{
# 565
if(Cyc_v_r){
# 566
{const char*_tmp470;void*_tmp46F[1];struct Cyc_String_pa_PrintArg_struct _tmp46E;(_tmp46E.tag=0,((_tmp46E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp46F[0]=& _tmp46E,Cyc_fprintf(Cyc_stderr,((_tmp470="%s completed.\n",_tag_dyneither(_tmp470,sizeof(char),15))),_tag_dyneither(_tmp46F,sizeof(void*),1)))))));}
# 567
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 568
return _tmp7C;}}
# 570
return _tmp7C;}
# 573
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
# 574
Cyc_Lex_lex_init(1);
# 575
Cyc_Lex_pos_init();{
# 576
struct Cyc_List_List*_tmp9E=Cyc_Parse_parse_file(f);
# 577
Cyc_Lex_lex_init(1);
# 578
return _tmp9E;};}struct _tuple23{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 581
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple23*tcenv,struct Cyc_List_List*tds){
# 583
struct _RegionHandle*_tmpA0;struct Cyc_Tcenv_Tenv*_tmpA1;struct _tuple23 _tmp9F=*tcenv;_tmpA0=_tmp9F.f1;_tmpA1=_tmp9F.f2;
# 584
Cyc_Tc_tc(_tmpA0,_tmpA1,1,tds);
# 585
if(!Cyc_noshake_r)
# 586
tds=Cyc_Tc_treeshake(_tmpA1,tds);
# 587
return tds;}
# 590
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
# 591
Cyc_NewControlFlow_cf_check(tds);
# 592
return tds;}struct _tuple24{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 597
struct Cyc_List_List*Cyc_do_interface(struct _tuple24*params,struct Cyc_List_List*tds){
# 598
struct _tuple24 _tmpA3;struct Cyc_Tcenv_Tenv*_tmpA4;struct Cyc___cycFILE*_tmpA5;struct Cyc___cycFILE*_tmpA6;struct _tuple24*_tmpA2=params;_tmpA3=*_tmpA2;_tmpA4=_tmpA3.f1;_tmpA5=_tmpA3.f2;_tmpA6=_tmpA3.f3;{
# 599
struct Cyc_Interface_I*_tmpA7=Cyc_Interface_extract(_tmpA4->ae);
# 600
if(_tmpA5 == 0)
# 601
Cyc_Interface_save(_tmpA7,_tmpA6);else{
# 603
struct Cyc_Interface_I*_tmpA8=Cyc_Interface_parse((struct Cyc___cycFILE*)_tmpA5);
# 604
const char*_tmp475;const char*_tmp474;struct _tuple21*_tmp473;if(!Cyc_Interface_is_subinterface(_tmpA8,_tmpA7,((_tmp473=_cycalloc(sizeof(*_tmp473)),((_tmp473->f1=((_tmp474="written interface",_tag_dyneither(_tmp474,sizeof(char),18))),((_tmp473->f2=((_tmp475="maximal interface",_tag_dyneither(_tmp475,sizeof(char),18))),_tmp473))))))))
# 605
Cyc_compile_failure=1;else{
# 607
Cyc_Interface_save(_tmpA8,_tmpA6);}}
# 609
return tds;};}
# 612
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
# 613
return Cyc_Toc_toc(tds);}
# 615
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
# 616
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
# 617
return Cyc_Tovc_tovc(tds);}
# 620
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 622
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 624
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
# 625
Cyc_cyc_include=f;}
# 629
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 632
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
# 633
{const char*_tmp478;void*_tmp477;(_tmp477=0,Cyc_fprintf(Cyc_stderr,((_tmp478="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp478,sizeof(char),53))),_tag_dyneither(_tmp477,sizeof(void*),0)));}
# 634
return 1;}{
# 636
const char*_tmp47A;const char*_tmp479;struct Cyc___cycFILE*_tmpAE=Cyc_try_file_open(file,((_tmp479="r",_tag_dyneither(_tmp479,sizeof(char),2))),((_tmp47A="internal compiler file",_tag_dyneither(_tmp47A,sizeof(char),23))));
# 637
if(_tmpAE == 0)return 1;{
# 638
unsigned long n_read=1024;
# 639
unsigned long n_written;
# 640
char buf[1024];
# 641
{unsigned int _tmpB5=1024;unsigned int i;for(i=0;i < _tmpB5;i ++){buf[i]='\000';}}while(n_read == 1024){
# 642
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)_tmpAE);
# 643
if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmpAE)){
# 644
Cyc_compile_failure=1;
# 645
{const char*_tmp47E;void*_tmp47D[1];struct Cyc_String_pa_PrintArg_struct _tmp47C;(_tmp47C.tag=0,((_tmp47C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp47D[0]=& _tmp47C,Cyc_fprintf(Cyc_stderr,((_tmp47E="\nError: problem copying %s\n",_tag_dyneither(_tmp47E,sizeof(char),28))),_tag_dyneither(_tmp47D,sizeof(void*),1)))))));}
# 646
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 647
return 1;}
# 649
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
# 650
if(n_read != n_written){
# 651
Cyc_compile_failure=1;
# 652
{const char*_tmp482;void*_tmp481[1];struct Cyc_String_pa_PrintArg_struct _tmp480;(_tmp480.tag=0,((_tmp480.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp481[0]=& _tmp480,Cyc_fprintf(Cyc_stderr,((_tmp482="\nError: problem copying %s\n",_tag_dyneither(_tmp482,sizeof(char),28))),_tag_dyneither(_tmp481,sizeof(void*),1)))))));}
# 653
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);
# 654
return 1;}}
# 657
Cyc_fclose((struct Cyc___cycFILE*)_tmpAE);
# 658
return 0;};};}
# 662
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
# 663
struct Cyc_Absynpp_Params _tmpB8=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
# 664
_tmpB8.expand_typedefs=!Cyc_noexpand_r;
# 665
_tmpB8.to_VC=Cyc_Cyclone_tovc_r;
# 666
_tmpB8.add_cyc_prefix=Cyc_add_cyc_namespace_r;
# 667
_tmpB8.generate_line_directives=Cyc_generate_line_directives_r;
# 668
_tmpB8.print_full_evars=Cyc_print_full_evars_r;
# 669
_tmpB8.print_all_tvars=Cyc_print_all_tvars_r;
# 670
_tmpB8.print_all_kinds=Cyc_print_all_kinds_r;
# 671
_tmpB8.print_all_effects=Cyc_print_all_effects_r;
# 673
if(Cyc_inline_functions_r){
# 674
const char*_tmp485;void*_tmp484;(_tmp484=0,Cyc_fprintf(out_file,((_tmp485="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp485,sizeof(char),27))),_tag_dyneither(_tmp484,sizeof(void*),0)));}
# 677
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
# 678
if(!Cyc_nocyc_setjmp_r){
# 679
if(Cyc_Lex_compile_for_boot_r){
# 680
const char*_tmp488;void*_tmp487;(_tmp487=0,Cyc_fprintf(out_file,((_tmp488="#include <setjmp.h>\n",_tag_dyneither(_tmp488,sizeof(char),21))),_tag_dyneither(_tmp487,sizeof(void*),0)));}else{
# 681
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 683
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 685
if(Cyc_pp_r){
# 686
Cyc_Absynpp_set_params(& _tmpB8);
# 687
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 689
Cyc_Absyndump_set_params(& _tmpB8);
# 690
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 692
Cyc_fflush((struct Cyc___cycFILE*)out_file);
# 693
return tds;}
# 698
static struct Cyc_List_List*Cyc_cfiles=0;
# 699
static void Cyc_remove_cfiles(){
# 700
if(!Cyc_save_c_r)
# 701
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
# 702
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 706
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
# 707
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
# 708
struct Cyc_List_List*_tmpBD=0;
# 709
unsigned long _tmpBE=Cyc_strlen((struct _dyneither_ptr)s);
# 711
while(_tmpBE > 0){
# 712
struct _dyneither_ptr _tmpBF=Cyc_strchr(s,c);
# 713
if((char*)_tmpBF.curr == (char*)(_tag_dyneither(0,0,0)).curr){
# 714
{struct _dyneither_ptr*_tmp48B;struct Cyc_List_List*_tmp48A;_tmpBD=((_tmp48A=_cycalloc(sizeof(*_tmp48A)),((_tmp48A->hd=((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((_tmp48B[0]=s,_tmp48B)))),((_tmp48A->tl=_tmpBD,_tmp48A))))));}
# 715
break;}else{
# 718
{struct _dyneither_ptr*_tmp48E;struct Cyc_List_List*_tmp48D;_tmpBD=((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D->hd=((_tmp48E=_cycalloc(sizeof(*_tmp48E)),((_tmp48E[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char))),_tmp48E)))),((_tmp48D->tl=_tmpBD,_tmp48D))))));}
# 719
_tmpBE -=(((struct _dyneither_ptr)_tmpBF).curr - s.curr)/ sizeof(char);
# 720
s=_dyneither_ptr_plus(_tmpBF,sizeof(char),1);}}
# 723
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBD);};}
# 727
static int Cyc_file_exists(struct _dyneither_ptr file){
# 728
struct Cyc___cycFILE*f=0;
# 729
{struct _handler_cons _tmpC4;_push_handler(& _tmpC4);{int _tmpC6=0;if(setjmp(_tmpC4.handler))_tmpC6=1;if(!_tmpC6){{const char*_tmp48F;f=(struct Cyc___cycFILE*)Cyc_file_open(file,((_tmp48F="r",_tag_dyneither(_tmp48F,sizeof(char),2))));};_pop_handler();}else{void*_tmpC5=(void*)_exn_thrown;void*_tmpC9=_tmpC5;_LL13:;_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmpC9);_LL12:;}};}
# 730
if(f == 0)return 0;else{
# 731
Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}
# 735
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
# 736
const char*_tmp490;struct _dyneither_ptr tmp=(_tmp490="",_tag_dyneither(_tmp490,sizeof(char),1));
# 737
for(0;dirs != 0;dirs=dirs->tl){
# 738
struct _dyneither_ptr _tmpCA=*((struct _dyneither_ptr*)dirs->hd);
# 739
if((char*)_tmpCA.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCA)== 0)continue;
# 740
{const char*_tmp491;_tmpCA=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCA,((_tmp491=":",_tag_dyneither(_tmp491,sizeof(char),2))));}
# 741
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpCA,(struct _dyneither_ptr)tmp);}
# 743
return tmp;}
# 748
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
# 749
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
# 750
for(0;dirs != 0;dirs=dirs->tl){
# 751
struct _dyneither_ptr _tmpCD=*((struct _dyneither_ptr*)dirs->hd);
# 752
if((char*)_tmpCD.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpCD)== 0)continue;{
# 753
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpCD,file);
# 754
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp492;return(_tmp492=_cycalloc(sizeof(*_tmp492)),((_tmp492[0]=s,_tmp492)));}};}
# 756
return 0;}
# 759
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
# 760
struct _dyneither_ptr*_tmpCF=Cyc_find(dirs,file);
# 761
if(_tmpCF == 0){
# 762
Cyc_compile_failure=1;
# 763
Cyc_remove_cfiles();
# 764
{const char*_tmp497;void*_tmp496[2];struct Cyc_String_pa_PrintArg_struct _tmp495;struct Cyc_String_pa_PrintArg_struct _tmp494;(_tmp494.tag=0,((_tmp494.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 765
Cyc_sprint_list(dirs)),((_tmp495.tag=0,((_tmp495.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp496[0]=& _tmp495,((_tmp496[1]=& _tmp494,Cyc_fprintf(Cyc_stderr,((_tmp497="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp497,sizeof(char),56))),_tag_dyneither(_tmp496,sizeof(void*),2)))))))))))));}
# 766
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
# 767
struct Cyc_Core_Failure_exn_struct _tmp4A7;const char*_tmp4A6;void*_tmp4A5[2];struct Cyc_String_pa_PrintArg_struct _tmp4A4;struct Cyc_String_pa_PrintArg_struct _tmp4A3;struct Cyc_Core_Failure_exn_struct*_tmp4A2;(int)_throw((void*)((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2[0]=((_tmp4A7.tag=Cyc_Core_Failure,((_tmp4A7.f1=(struct _dyneither_ptr)((_tmp4A3.tag=0,((_tmp4A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp4A4.tag=0,((_tmp4A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp4A5[0]=& _tmp4A4,((_tmp4A5[1]=& _tmp4A3,Cyc_aprintf(((_tmp4A6="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp4A6,sizeof(char),56))),_tag_dyneither(_tmp4A5,sizeof(void*),2)))))))))))))),_tmp4A7)))),_tmp4A2)))));};}
# 771
return*_tmpCF;}
# 776
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 778
struct Cyc_List_List*_tmpDA=0;
# 779
for(0;dirs != 0;dirs=dirs->tl){
# 780
struct _dyneither_ptr*_tmp4AA;struct Cyc_List_List*_tmp4A9;_tmpDA=((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9->hd=((_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4AA)))),((_tmp4A9->tl=_tmpDA,_tmp4A9))))));}
# 782
_tmpDA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDA);
# 783
return _tmpDA;}
# 788
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 790
struct Cyc_List_List*_tmpDD=0;
# 791
for(0;dirs != 0;dirs=dirs->tl){
# 792
{struct Cyc_List_List*_tmp4AB;_tmpDD=((_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp4AB->tl=_tmpDD,_tmp4AB))))));}{
# 793
struct _dyneither_ptr*_tmp4AE;struct Cyc_List_List*_tmp4AD;_tmpDD=((_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD->hd=((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4AE)))),((_tmp4AD->tl=_tmpDD,_tmp4AD))))));};}
# 795
_tmpDD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDD);
# 796
return _tmpDD;}
# 800
static int Cyc_is_other_special(char c){
# 801
switch(c){case '\\': _LL17:
# 802
 goto _LL18;case '"': _LL18:
# 803
 goto _LL19;case ';': _LL19:
# 804
 goto _LL1A;case '&': _LL1A:
# 805
 goto _LL1B;case '(': _LL1B:
# 806
 goto _LL1C;case ')': _LL1C:
# 807
 goto _LL1D;case '|': _LL1D:
# 808
 goto _LL1E;case '^': _LL1E:
# 809
 goto _LL1F;case '<': _LL1F:
# 810
 goto _LL20;case '>': _LL20:
# 811
 goto _LL21;case '\n': _LL21:
# 815
 goto _LL22;case '\t': _LL22:
# 816
 return 1;default: _LL23:
# 817
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4B7(unsigned int*_tmp4B6,unsigned int*_tmp4B5,char**_tmp4B3){for(*_tmp4B6=0;*_tmp4B6 < *_tmp4B5;(*_tmp4B6)++){(*_tmp4B3)[*_tmp4B6]='\000';}}
# 822
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
# 823
unsigned long _tmpE1=Cyc_strlen((struct _dyneither_ptr)s);
# 826
int _tmpE2=0;
# 827
int _tmpE3=0;
# 828
{int i=0;for(0;i < _tmpE1;++ i){
# 829
char _tmpE4=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 830
if(_tmpE4 == '\'')++ _tmpE2;else{
# 831
if(Cyc_is_other_special(_tmpE4))++ _tmpE3;}}}
# 835
if(_tmpE2 == 0  && _tmpE3 == 0)
# 836
return s;
# 839
if(_tmpE2 == 0){
# 840
struct _dyneither_ptr*_tmp4B1;struct _dyneither_ptr*_tmp4B0[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4B0[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4B0[1]=((_tmp4B1=_cycalloc(sizeof(*_tmp4B1)),((_tmp4B1[0]=(struct _dyneither_ptr)s,_tmp4B1)))),((_tmp4B0[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B0,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 843
unsigned long _tmpE9=(_tmpE1 + _tmpE2)+ _tmpE3;
# 844
unsigned int _tmp4B6;unsigned int _tmp4B5;struct _dyneither_ptr _tmp4B4;char*_tmp4B3;unsigned int _tmp4B2;struct _dyneither_ptr s2=(_tmp4B2=_tmpE9 + 1,((_tmp4B3=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4B2 + 1)),((_tmp4B4=_tag_dyneither(_tmp4B3,sizeof(char),_tmp4B2 + 1),((((_tmp4B5=_tmp4B2,((_tmp4B7(& _tmp4B6,& _tmp4B5,& _tmp4B3),_tmp4B3[_tmp4B5]=(char)0)))),_tmp4B4)))))));
# 845
int _tmpEA=0;
# 846
int _tmpEB=0;
# 847
for(0;_tmpEA < _tmpE1;++ _tmpEA){
# 848
char _tmpEC=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpEA));
# 849
if(_tmpEC == '\''  || Cyc_is_other_special(_tmpEC)){
# 850
char _tmp4BA;char _tmp4B9;struct _dyneither_ptr _tmp4B8;(_tmp4B8=_dyneither_ptr_plus(s2,sizeof(char),_tmpEB ++),((_tmp4B9=*((char*)_check_dyneither_subscript(_tmp4B8,sizeof(char),0)),((_tmp4BA='\\',((_get_dyneither_size(_tmp4B8,sizeof(char))== 1  && (_tmp4B9 == '\000'  && _tmp4BA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4B8.curr)=_tmp4BA)))))));}{
# 851
char _tmp4BD;char _tmp4BC;struct _dyneither_ptr _tmp4BB;(_tmp4BB=_dyneither_ptr_plus(s2,sizeof(char),_tmpEB ++),((_tmp4BC=*((char*)_check_dyneither_subscript(_tmp4BB,sizeof(char),0)),((_tmp4BD=_tmpEC,((_get_dyneither_size(_tmp4BB,sizeof(char))== 1  && (_tmp4BC == '\000'  && _tmp4BD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4BB.curr)=_tmp4BD)))))));};}
# 853
return(struct _dyneither_ptr)s2;};}
# 855
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
# 856
struct _dyneither_ptr*_tmp4BE;return(_tmp4BE=_cycalloc(sizeof(*_tmp4BE)),((_tmp4BE[0]=Cyc_sh_escape_string(*sp),_tmp4BE)));}
# 859
static void Cyc_process_file(struct _dyneither_ptr filename){
# 861
struct _dyneither_ptr _tmpF8=Cyc_make_base_filename(filename,Cyc_output_file);
# 862
const char*_tmp4BF;struct _dyneither_ptr _tmpF9=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4BF=".cyp",_tag_dyneither(_tmp4BF,sizeof(char),5))));
# 863
const char*_tmp4C0;struct _dyneither_ptr _tmpFA=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
# 864
Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4C0=".cyci",_tag_dyneither(_tmp4C0,sizeof(char),6))));
# 865
const char*_tmp4C1;struct _dyneither_ptr _tmpFB=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4C1=".cycio",_tag_dyneither(_tmp4C1,sizeof(char),7))));
# 866
const char*_tmp4C2;struct _dyneither_ptr _tmpFC=Cyc_strconcat((struct _dyneither_ptr)_tmpF8,((_tmp4C2=".c",_tag_dyneither(_tmp4C2,sizeof(char),3))));
# 868
if(Cyc_v_r){const char*_tmp4C6;void*_tmp4C5[1];struct Cyc_String_pa_PrintArg_struct _tmp4C4;(_tmp4C4.tag=0,((_tmp4C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4C5[0]=& _tmp4C4,Cyc_fprintf(Cyc_stderr,((_tmp4C6="Compiling %s\n",_tag_dyneither(_tmp4C6,sizeof(char),14))),_tag_dyneither(_tmp4C5,sizeof(void*),1)))))));}{
# 871
const char*_tmp4C8;const char*_tmp4C7;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4C7="r",_tag_dyneither(_tmp4C7,sizeof(char),2))),((_tmp4C8="input file",_tag_dyneither(_tmp4C8,sizeof(char),11))));
# 872
if(Cyc_compile_failure  || !((unsigned int)f0))
# 873
return;
# 874
Cyc_fclose((struct Cyc___cycFILE*)f0);{
# 878
const char*_tmp4CF;struct _dyneither_ptr*_tmp4CE;const char*_tmp4CD;struct Cyc_List_List*_tmp4CC;struct _dyneither_ptr _tmp100=
# 879
Cyc_str_sepstr(((_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((_tmp4CC->hd=((_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE[0]=(struct _dyneither_ptr)((_tmp4CD="",_tag_dyneither(_tmp4CD,sizeof(char),1))),_tmp4CE)))),((_tmp4CC->tl=
# 880
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4CC)))))),(
# 881
(_tmp4CF=" ",_tag_dyneither(_tmp4CF,sizeof(char),2))));
# 888
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
# 889
struct Cyc_List_List*_tmp101=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
# 890
{const char*_tmp4D0;_tmp101=Cyc_add_subdir(_tmp101,((_tmp4D0="include",_tag_dyneither(_tmp4D0,sizeof(char),8))));}
# 891
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
# 892
struct _dyneither_ptr*_tmp4D3;struct Cyc_List_List*_tmp4D2;_tmp101=((_tmp4D2=_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2->hd=((_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((_tmp4D3[0]=def_inc_path,_tmp4D3)))),((_tmp4D2->tl=_tmp101,_tmp4D2))))));}{
# 894
char*_tmp105=getenv((const char*)"CYCLONE_INCLUDE_PATH");
# 895
if((char*)_tmp105 != 0){
# 896
char*_tmp4D4;_tmp101=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4D4=_tmp105,_tag_dyneither(_tmp4D4,sizeof(char),_get_zero_arr_size_char((void*)_tmp4D4,1)))),':'),_tmp101);}{
# 899
const char*_tmp4DB;struct _dyneither_ptr*_tmp4DA;const char*_tmp4D9;struct Cyc_List_List*_tmp4D8;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
# 900
Cyc_str_sepstr(((_tmp4D8=_cycalloc(sizeof(*_tmp4D8)),((_tmp4D8->hd=((_tmp4DA=_cycalloc(sizeof(*_tmp4DA)),((_tmp4DA[0]=(struct _dyneither_ptr)((_tmp4D9="",_tag_dyneither(_tmp4D9,sizeof(char),1))),_tmp4DA)))),((_tmp4D8->tl=
# 901
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmp101),_tmp4D8)))))),(
# 902
(_tmp4DB=" -I",_tag_dyneither(_tmp4DB,sizeof(char),4))));
# 903
struct _dyneither_ptr ofile_string;
# 904
if(Cyc_stop_after_cpp_r){
# 905
if(Cyc_output_file != 0){
# 906
const char*_tmp4DF;void*_tmp4DE[1];struct Cyc_String_pa_PrintArg_struct _tmp4DD;ofile_string=(struct _dyneither_ptr)((_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4DE[0]=& _tmp4DD,Cyc_aprintf(((_tmp4DF=" > %s",_tag_dyneither(_tmp4DF,sizeof(char),6))),_tag_dyneither(_tmp4DE,sizeof(void*),1))))))));}else{
# 908
const char*_tmp4E0;ofile_string=((_tmp4E0="",_tag_dyneither(_tmp4E0,sizeof(char),1)));}}else{
# 910
const char*_tmp4E4;void*_tmp4E3[1];struct Cyc_String_pa_PrintArg_struct _tmp4E2;ofile_string=(struct _dyneither_ptr)((_tmp4E2.tag=0,((_tmp4E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF9)),((_tmp4E3[0]=& _tmp4E2,Cyc_aprintf(((_tmp4E4=" > %s",_tag_dyneither(_tmp4E4,sizeof(char),6))),_tag_dyneither(_tmp4E3,sizeof(void*),1))))))));}{
# 912
struct _dyneither_ptr fixup_string;
# 913
if(Cyc_produce_dependencies){
# 916
if(Cyc_dependencies_target == 0){
# 920
const char*_tmp4E5;fixup_string=((_tmp4E5=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4E5,sizeof(char),35)));}else{
# 925
const char*_tmp4E9;void*_tmp4E8[1];struct Cyc_String_pa_PrintArg_struct _tmp4E7;fixup_string=(struct _dyneither_ptr)(
# 926
(_tmp4E7.tag=0,((_tmp4E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4E8[0]=& _tmp4E7,Cyc_aprintf(((_tmp4E9=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4E9,sizeof(char),29))),_tag_dyneither(_tmp4E8,sizeof(void*),1))))))));}}else{
# 929
const char*_tmp4EA;fixup_string=((_tmp4EA="",_tag_dyneither(_tmp4EA,sizeof(char),1)));}{
# 931
const char*_tmp4F3;void*_tmp4F2[6];struct Cyc_String_pa_PrintArg_struct _tmp4F1;struct Cyc_String_pa_PrintArg_struct _tmp4F0;struct Cyc_String_pa_PrintArg_struct _tmp4EF;struct Cyc_String_pa_PrintArg_struct _tmp4EE;struct Cyc_String_pa_PrintArg_struct _tmp4ED;struct Cyc_String_pa_PrintArg_struct _tmp4EC;struct _dyneither_ptr _tmp113=(_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4ED.tag=0,((_tmp4ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4EE.tag=0,((_tmp4EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 934
Cyc_sh_escape_string(filename)),((_tmp4EF.tag=0,((_tmp4EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4F0.tag=0,((_tmp4F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp100),((_tmp4F1.tag=0,((_tmp4F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4F2[0]=& _tmp4F1,((_tmp4F2[1]=& _tmp4F0,((_tmp4F2[2]=& _tmp4EF,((_tmp4F2[3]=& _tmp4EE,((_tmp4F2[4]=& _tmp4ED,((_tmp4F2[5]=& _tmp4EC,Cyc_aprintf(((_tmp4F3="%s %s%s %s%s%s",_tag_dyneither(_tmp4F3,sizeof(char),15))),_tag_dyneither(_tmp4F2,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 937
if(Cyc_v_r){const char*_tmp4F7;void*_tmp4F6[1];struct Cyc_String_pa_PrintArg_struct _tmp4F5;(_tmp4F5.tag=0,((_tmp4F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp113),((_tmp4F6[0]=& _tmp4F5,Cyc_fprintf(Cyc_stderr,((_tmp4F7="%s\n",_tag_dyneither(_tmp4F7,sizeof(char),4))),_tag_dyneither(_tmp4F6,sizeof(void*),1)))))));}
# 938
if(system((const char*)_untag_dyneither_ptr(_tmp113,sizeof(char),1))!= 0){
# 939
Cyc_compile_failure=1;
# 940
{const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,Cyc_fprintf(Cyc_stderr,((_tmp4FA="\nError: preprocessing\n",_tag_dyneither(_tmp4FA,sizeof(char),23))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}
# 941
return;}
# 943
if(Cyc_stop_after_cpp_r)return;
# 946
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF9);{
# 947
const char*_tmp4FC;const char*_tmp4FB;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF9,((_tmp4FB="r",_tag_dyneither(_tmp4FB,sizeof(char),2))),((_tmp4FC="file",_tag_dyneither(_tmp4FC,sizeof(char),5))));
# 948
if(Cyc_compile_failure)return;{
# 950
struct Cyc_List_List*tds=0;
# 953
{struct _handler_cons _tmp119;_push_handler(& _tmp119);{int _tmp11B=0;if(setjmp(_tmp119.handler))_tmp11B=1;if(!_tmp11B){
# 954
{const char*_tmp4FD;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FD="parsing",_tag_dyneither(_tmp4FD,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp11A=(void*)_exn_thrown;void*_tmp11E=_tmp11A;_LL26:;_LL27:
# 957
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
# 958
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 959
(int)_throw(_tmp11E);_LL28:;_LL29:(void)_throw(_tmp11E);_LL25:;}};}
# 961
Cyc_file_close((struct Cyc___cycFILE*)in_file);
# 962
if(Cyc_compile_failure){
# 963
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 964
return;}
# 967
if(Cyc_Absyn_porting_c_code){
# 968
Cyc_Port_port(tds);
# 969
return;}{
# 972
struct _RegionHandle _tmp11F=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11F;_push_region(tc_rgn);{
# 973
struct Cyc_Tcenv_Tenv*_tmp120=Cyc_Tcenv_tc_init(tc_rgn);
# 974
if(Cyc_parseonly_r)goto PRINTC;{
# 977
struct _tuple23 _tmp4FE;struct _tuple23 _tmp121=(_tmp4FE.f1=tc_rgn,((_tmp4FE.f2=_tmp120,_tmp4FE)));
# 978
{const char*_tmp4FF;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FF="type checking",_tag_dyneither(_tmp4FF,sizeof(char),14))),Cyc_do_typecheck,& _tmp121,tds);}
# 980
if(Cyc_compile_failure){
# 981
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 982
_npop_handler(0);return;}
# 984
if(Cyc_tc_r)goto PRINTC;
# 985
{const char*_tmp500;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp500="control-flow checking",_tag_dyneither(_tmp500,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
# 986
if(Cyc_compile_failure){
# 987
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 988
_npop_handler(0);return;}
# 992
if(Cyc_generate_interface_r){
# 993
const char*_tmp503;const char*_tmp502;const char*_tmp501;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp501=(const char*)_untag_dyneither_ptr(_tmpFA,sizeof(char),1),_tag_dyneither(_tmp501,sizeof(char),_get_zero_arr_size_char((void*)_tmp501,1)))),((_tmp502="w",_tag_dyneither(_tmp502,sizeof(char),2))),((_tmp503="interface file",_tag_dyneither(_tmp503,sizeof(char),15))));
# 994
if(inter_file == 0){
# 995
Cyc_compile_failure=1;_npop_handler(0);return;}
# 997
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
# 998
Cyc_Absyndump_dump_interface(tds,(struct Cyc___cycFILE*)inter_file);
# 999
Cyc_fclose((struct Cyc___cycFILE*)inter_file);
# 1000
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 1004
if(Cyc_ic_r){
# 1005
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpFA,sizeof(char),1),(const char*)"r");
# 1006
const char*_tmp505;const char*_tmp504;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpFB,((_tmp504="w",_tag_dyneither(_tmp504,sizeof(char),2))),((_tmp505="interface object file",_tag_dyneither(_tmp505,sizeof(char),22))));
# 1007
if(inter_objfile == 0){
# 1008
Cyc_compile_failure=1;
# 1009
_npop_handler(0);return;}
# 1011
Cyc_Position_reset_position(_tmpFA);{
# 1012
struct _tuple24 _tmp506;struct _tuple24 _tmp127=(_tmp506.f1=_tmp120,((_tmp506.f2=inter_file,((_tmp506.f3=(struct Cyc___cycFILE*)inter_objfile,_tmp506)))));
# 1013
{const char*_tmp507;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_List_List*),struct _tuple24*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp507="interface checking",_tag_dyneither(_tmp507,sizeof(char),19))),Cyc_do_interface,& _tmp127,tds);}
# 1014
if(inter_file != 0)
# 1015
Cyc_file_close((struct Cyc___cycFILE*)inter_file);
# 1016
Cyc_file_close((struct Cyc___cycFILE*)inter_objfile);};}};}
# 1019
if(Cyc_cf_r)goto PRINTC;
# 1022
{const char*_tmp508;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp508="translation to C",_tag_dyneither(_tmp508,sizeof(char),17))),Cyc_do_translate,1,tds);}
# 1023
if(Cyc_compile_failure){
# 1024
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 1025
_npop_handler(0);return;}
# 1028
if(!Cyc_compile_failure)
# 1029
Cyc_Tcutil_flush_warnings();
# 1030
Cyc_remove_file((struct _dyneither_ptr)_tmpF9);
# 1031
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1032
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1036
const char*_tmp509;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp509="post-pass to VC",_tag_dyneither(_tmp509,sizeof(char),16))),Cyc_do_tovc,1,tds);}
# 1037
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1043
PRINTC: {
# 1044
struct Cyc___cycFILE*out_file;
# 1045
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
# 1046
if(Cyc_output_file != 0){
# 1047
const char*_tmp50B;const char*_tmp50A;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp50A="w",_tag_dyneither(_tmp50A,sizeof(char),2))),((_tmp50B="output file",_tag_dyneither(_tmp50B,sizeof(char),12))));}else{
# 1049
out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}else{
# 1050
if(Cyc_toc_r  && Cyc_output_file != 0){
# 1051
const char*_tmp50D;const char*_tmp50C;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp50C="w",_tag_dyneither(_tmp50C,sizeof(char),2))),((_tmp50D="output file",_tag_dyneither(_tmp50D,sizeof(char),12))));}else{
# 1053
const char*_tmp50F;const char*_tmp50E;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpFC,((_tmp50E="w",_tag_dyneither(_tmp50E,sizeof(char),2))),((_tmp50F="output file",_tag_dyneither(_tmp50F,sizeof(char),12))));}}
# 1055
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1057
{struct _handler_cons _tmp135;_push_handler(& _tmp135);{int _tmp137=0;if(setjmp(_tmp135.handler))_tmp137=1;if(!_tmp137){
# 1058
if(!Cyc_noprint_r){
# 1059
const char*_tmp510;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp510="printing",_tag_dyneither(_tmp510,sizeof(char),9))),Cyc_do_print,(struct Cyc___cycFILE*)out_file,tds);}
# 1058
;_pop_handler();}else{void*_tmp136=(void*)_exn_thrown;void*_tmp13A=_tmp136;_LL2B:;_LL2C:
# 1062
 Cyc_compile_failure=1;
# 1063
Cyc_file_close((struct Cyc___cycFILE*)out_file);
# 1064
{struct _dyneither_ptr*_tmp513;struct Cyc_List_List*_tmp512;Cyc_cfiles=((_tmp512=_cycalloc(sizeof(*_tmp512)),((_tmp512->hd=((_tmp513=_cycalloc(sizeof(*_tmp513)),((_tmp513[0]=(struct _dyneither_ptr)_tmpFC,_tmp513)))),((_tmp512->tl=Cyc_cfiles,_tmp512))))));}
# 1065
(int)_throw(_tmp13A);_LL2D:;_LL2E:(void)_throw(_tmp13A);_LL2A:;}};}
# 1067
Cyc_file_close((struct Cyc___cycFILE*)out_file);{
# 1068
struct _dyneither_ptr*_tmp516;struct Cyc_List_List*_tmp515;Cyc_cfiles=((_tmp515=_cycalloc(sizeof(*_tmp515)),((_tmp515->hd=((_tmp516=_cycalloc(sizeof(*_tmp516)),((_tmp516[0]=(struct _dyneither_ptr)_tmpFC,_tmp516)))),((_tmp515->tl=Cyc_cfiles,_tmp515))))));};}
# 972
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp157[8]="<final>";
# 1072
static struct _dyneither_ptr Cyc_final_str={_tmp157,_tmp157,_tmp157 + 8};
# 1073
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1075
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
# 1076
if(n == (struct _dyneither_ptr*)Cyc_final_strptr)
# 1077
return Cyc_Interface_final();{
# 1079
struct _dyneither_ptr basename;
# 1080
{struct _handler_cons _tmp158;_push_handler(& _tmp158);{int _tmp15A=0;if(setjmp(_tmp158.handler))_tmp15A=1;if(!_tmp15A){
# 1081
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp159=(void*)_exn_thrown;void*_tmp15C=_tmp159;_LL30: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp15D=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp15C;if(_tmp15D->tag != Cyc_Core_Invalid_argument)goto _LL32;}_LL31:
# 1083
 basename=*n;goto _LL2F;_LL32:;_LL33:(void)_throw(_tmp15C);_LL2F:;}};}{
# 1085
const char*_tmp517;struct _dyneither_ptr _tmp15E=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp517=".cycio",_tag_dyneither(_tmp517,sizeof(char),7))));
# 1086
const char*_tmp519;const char*_tmp518;struct Cyc___cycFILE*_tmp15F=Cyc_try_file_open((struct _dyneither_ptr)_tmp15E,((_tmp518="rb",_tag_dyneither(_tmp518,sizeof(char),3))),((_tmp519="interface object file",_tag_dyneither(_tmp519,sizeof(char),22))));
# 1087
if(_tmp15F == 0){
# 1088
Cyc_compile_failure=1;
# 1089
Cyc_remove_cfiles();
# 1090
exit(1);}
# 1092
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp15E);{
# 1094
struct Cyc_Interface_I*_tmp160=Cyc_Interface_load((struct Cyc___cycFILE*)_tmp15F);
# 1095
Cyc_file_close((struct Cyc___cycFILE*)_tmp15F);
# 1096
return _tmp160;};};};}
# 1099
static int Cyc_is_cfile(struct _dyneither_ptr*n){
# 1100
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1103
void GC_blacklist_warn_clear();struct _tuple25{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp51F(unsigned int*_tmp51E,unsigned int*_tmp51D,int**_tmp51B){for(*_tmp51E=0;*_tmp51E < *_tmp51D;(*_tmp51E)++){(*_tmp51B)[*_tmp51E]=0;}}static void _tmp527(unsigned int*_tmp526,unsigned int*_tmp525,struct _dyneither_ptr**_tmp523){for(*_tmp526=0;*_tmp526 < *_tmp525;(*_tmp526)++){(*_tmp523)[*_tmp526]=(struct _dyneither_ptr)
# 1140
_tag_dyneither(0,0,0);}}static void _tmp52D(unsigned int*_tmp52C,unsigned int*_tmp52B,struct _dyneither_ptr**_tmp529){for(*_tmp52C=0;*_tmp52C < *_tmp52B;(*_tmp52C)++){(*_tmp529)[*_tmp52C]=(struct _dyneither_ptr)
# 1141
_tag_dyneither(0,0,0);}}
# 1106
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1111
GC_blacklist_warn_clear();{
# 1113
struct Cyc_List_List*cyclone_arch_path;
# 1114
struct _dyneither_ptr def_lib_path;
# 1115
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1118
struct _RegionHandle _tmp164=_new_region("r");struct _RegionHandle*r=& _tmp164;_push_region(r);{
# 1120
unsigned int _tmp51E;unsigned int _tmp51D;struct _dyneither_ptr _tmp51C;int*_tmp51B;unsigned int _tmp51A;struct _dyneither_ptr _tmp165=(_tmp51A=(unsigned int)argc,((_tmp51B=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp51A)),((_tmp51C=_tag_dyneither(_tmp51B,sizeof(int),_tmp51A),((((_tmp51D=_tmp51A,_tmp51F(& _tmp51E,& _tmp51D,& _tmp51B))),_tmp51C)))))));
# 1121
int _tmp166=0;
# 1122
int i;int j;int k;
# 1123
for(i=1;i < argc;++ i){
# 1124
const char*_tmp520;if(Cyc_strncmp(((_tmp520="-B",_tag_dyneither(_tmp520,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
# 1125
*((int*)_check_dyneither_subscript(_tmp165,sizeof(int),i))=1;
# 1126
++ _tmp166;}else{
# 1128
const char*_tmp521;if(Cyc_strcmp(((_tmp521="-b",_tag_dyneither(_tmp521,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
# 1129
*((int*)_check_dyneither_subscript(_tmp165,sizeof(int),i))=1;
# 1130
++ _tmp166;
# 1131
++ i;if(i >= argc)break;
# 1132
*((int*)_check_dyneither_subscript(_tmp165,sizeof(int),i))=1;
# 1133
++ _tmp166;}}}{
# 1140
unsigned int _tmp526;unsigned int _tmp525;struct _dyneither_ptr _tmp524;struct _dyneither_ptr*_tmp523;unsigned int _tmp522;struct _dyneither_ptr _tmp169=(_tmp522=(unsigned int)(_tmp166 + 1),((_tmp523=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp522)),((_tmp524=_tag_dyneither(_tmp523,sizeof(struct _dyneither_ptr),_tmp522),((((_tmp525=_tmp522,_tmp527(& _tmp526,& _tmp525,& _tmp523))),_tmp524)))))));
# 1141
unsigned int _tmp52C;unsigned int _tmp52B;struct _dyneither_ptr _tmp52A;struct _dyneither_ptr*_tmp529;unsigned int _tmp528;struct _dyneither_ptr _tmp16A=(_tmp528=(unsigned int)(argc - _tmp166),((_tmp529=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp528)),((_tmp52A=_tag_dyneither(_tmp529,sizeof(struct _dyneither_ptr),_tmp528),((((_tmp52B=_tmp528,_tmp52D(& _tmp52C,& _tmp52B,& _tmp529))),_tmp52A)))))));
# 1142
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp169,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp16A,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
# 1143
for(i=(j=(k=1));i < argc;++ i){
# 1144
if(*((int*)_check_dyneither_subscript(_tmp165,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp169,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
# 1145
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp16A,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1149
struct _tuple25*_tmpA86;const char*_tmpA85;const char*_tmpA84;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA83;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA82;const char*_tmpA81;struct _tuple25*_tmpA80;const char*_tmpA7F;const char*_tmpA7E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA7D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA7C;const char*_tmpA7B;struct _tuple25*_tmpA7A;const char*_tmpA79;const char*_tmpA78;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA77;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA76;const char*_tmpA75;struct _tuple25*_tmpA74;const char*_tmpA73;const char*_tmpA72;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA71;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA70;const char*_tmpA6F;struct _tuple25*_tmpA6E;const char*_tmpA6D;const char*_tmpA6C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6A;const char*_tmpA69;struct _tuple25*_tmpA68;const char*_tmpA67;const char*_tmpA66;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA65;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA64;const char*_tmpA63;struct _tuple25*_tmpA62;const char*_tmpA61;const char*_tmpA60;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA5F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA5E;const char*_tmpA5D;struct _tuple25*_tmpA5C;const char*_tmpA5B;const char*_tmpA5A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA59;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA58;const char*_tmpA57;struct _tuple25*_tmpA56;const char*_tmpA55;const char*_tmpA54;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA53;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA52;const char*_tmpA51;struct _tuple25*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA4D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA4C;const char*_tmpA4B;struct _tuple25*_tmpA4A;const char*_tmpA49;const char*_tmpA48;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA47;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA46;const char*_tmpA45;struct _tuple25*_tmpA44;const char*_tmpA43;const char*_tmpA42;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA41;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA40;const char*_tmpA3F;struct _tuple25*_tmpA3E;const char*_tmpA3D;const char*_tmpA3C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA3B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA3A;const char*_tmpA39;struct _tuple25*_tmpA38;const char*_tmpA37;const char*_tmpA36;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA35;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA34;const char*_tmpA33;struct _tuple25*_tmpA32;const char*_tmpA31;const char*_tmpA30;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA2F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA2E;const char*_tmpA2D;struct _tuple25*_tmpA2C;const char*_tmpA2B;const char*_tmpA2A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA29;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA28;const char*_tmpA27;struct _tuple25*_tmpA26;const char*_tmpA25;const char*_tmpA24;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA23;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA22;const char*_tmpA21;struct _tuple25*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA1D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA1C;const char*_tmpA1B;struct _tuple25*_tmpA1A;const char*_tmpA19;const char*_tmpA18;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA17;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA16;const char*_tmpA15;struct _tuple25*_tmpA14;const char*_tmpA13;const char*_tmpA12;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA11;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA10;const char*_tmpA0F;struct _tuple25*_tmpA0E;const char*_tmpA0D;const char*_tmpA0C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA0B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA0A;const char*_tmpA09;struct _tuple25*_tmpA08;const char*_tmpA07;const char*_tmpA06;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA05;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA04;const char*_tmpA03;struct _tuple25*_tmpA02;const char*_tmpA01;const char*_tmpA00;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9FF;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9FE;const char*_tmp9FD;struct _tuple25*_tmp9FC;const char*_tmp9FB;const char*_tmp9FA;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp9F9;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp9F8;const char*_tmp9F7;struct _tuple25*_tmp9F6;const char*_tmp9F5;const char*_tmp9F4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9F3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9F2;const char*_tmp9F1;struct _tuple25*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9ED;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9EC;const char*_tmp9EB;struct _tuple25*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E6;const char*_tmp9E5;struct _tuple25*_tmp9E4;const char*_tmp9E3;const char*_tmp9E2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9E1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9E0;const char*_tmp9DF;struct _tuple25*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DA;const char*_tmp9D9;struct _tuple25*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9D5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D4;const char*_tmp9D3;struct _tuple25*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9CF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9CE;const char*_tmp9CD;struct _tuple25*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C8;const char*_tmp9C7;struct _tuple25*_tmp9C6;const char*_tmp9C5;const char*_tmp9C4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9C3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9C2;const char*_tmp9C1;struct _tuple25*_tmp9C0;const char*_tmp9BF;const char*_tmp9BE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9BD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9BC;const char*_tmp9BB;struct _tuple25*_tmp9BA;const char*_tmp9B9;const char*_tmp9B8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9B7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9B6;const char*_tmp9B5;struct _tuple25*_tmp9B4;const char*_tmp9B3;const char*_tmp9B2;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9B1;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9B0;const char*_tmp9AF;struct _tuple25*_tmp9AE;const char*_tmp9AD;const char*_tmp9AC;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9AB;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9AA;const char*_tmp9A9;struct _tuple25*_tmp9A8;const char*_tmp9A7;const char*_tmp9A6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9A5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9A4;const char*_tmp9A3;struct _tuple25*_tmp9A2;const char*_tmp9A1;const char*_tmp9A0;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp99F;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp99E;const char*_tmp99D;struct _tuple25*_tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp999;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp998;const char*_tmp997;struct _tuple25*_tmp996;const char*_tmp995;const char*_tmp994;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp993;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp992;const char*_tmp991;struct _tuple25*_tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp98D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp98C;const char*_tmp98B;struct _tuple25*_tmp98A;const char*_tmp989;const char*_tmp988;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp987;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp986;const char*_tmp985;struct _tuple25*_tmp984;const char*_tmp983;const char*_tmp982;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp981;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp980;const char*_tmp97F;struct _tuple25*_tmp97E;const char*_tmp97D;const char*_tmp97C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp97B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp97A;const char*_tmp979;struct _tuple25*_tmp978;const char*_tmp977;const char*_tmp976;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp975;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp974;const char*_tmp973;struct _tuple25*_tmp972;const char*_tmp971;const char*_tmp970;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp96F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp96E;const char*_tmp96D;struct _tuple25*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp969;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp968;const char*_tmp967;struct _tuple25*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp963;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp962;const char*_tmp961;struct _tuple25*_tmp960;const char*_tmp95F;const char*_tmp95E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp95D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp95C;const char*_tmp95B;struct _tuple25*_tmp95A;const char*_tmp959;const char*_tmp958;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp957;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp956;const char*_tmp955;struct _tuple25*_tmp954;const char*_tmp953;const char*_tmp952;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp951;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp950;const char*_tmp94F;struct _tuple25*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94A;const char*_tmp949;struct _tuple25*_tmp948;const char*_tmp947;const char*_tmp946;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp945;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp944;const char*_tmp943;struct _tuple25*_tmp942;const char*_tmp941;const char*_tmp940;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93E;const char*_tmp93D;struct _tuple25*_tmp93C;const char*_tmp93B;const char*_tmp93A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp939;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp938;const char*_tmp937;struct _tuple25*_tmp936;const char*_tmp935;const char*_tmp934;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp933;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp932;const char*_tmp931;struct _tuple25*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92C;const char*_tmp92B;struct _tuple25*_tmp92A;const char*_tmp929;const char*_tmp928;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp927;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp926;const char*_tmp925;struct _tuple25*_tmp924;const char*_tmp923;const char*_tmp922;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp921;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp920;const char*_tmp91F;struct _tuple25*_tmp91E;const char*_tmp91D;const char*_tmp91C;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp91B;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp91A;const char*_tmp919;struct _tuple25*_tmp918;const char*_tmp917;const char*_tmp916;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp915;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp914;const char*_tmp913;struct _tuple25*_tmp912;const char*_tmp911;const char*_tmp910;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp90F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp90E;const char*_tmp90D;struct _tuple25*_tmp90C;const char*_tmp90B;const char*_tmp90A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp909;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp908;const char*_tmp907;struct _tuple25*_tmp906;const char*_tmp905;const char*_tmp904;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp903;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp902;const char*_tmp901;struct _tuple25*_tmp900;const char*_tmp8FF;const char*_tmp8FE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8FD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8FC;const char*_tmp8FB;struct _tuple25*_tmp8FA;const char*_tmp8F9;const char*_tmp8F8;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp8F7;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp8F6;const char*_tmp8F5;struct _tuple25*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8F1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8F0;const char*_tmp8EF;struct _tuple25*_tmp8EE;const char*_tmp8ED;const char*_tmp8EC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8EB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8EA;const char*_tmp8E9;struct _tuple25*_tmp8E8;const char*_tmp8E7;const char*_tmp8E6;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E5;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E4;const char*_tmp8E3;struct _tuple25*_tmp8E2;const char*_tmp8E1;const char*_tmp8E0;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8DF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8DE;const char*_tmp8DD;struct _tuple25*_tmp8DC;const char*_tmp8DB;const char*_tmp8DA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8D9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8D8;const char*_tmp8D7;struct _tuple25*_tmp8D6;const char*_tmp8D5;const char*_tmp8D4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8D3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8D2;const char*_tmp8D1;struct _tuple25*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8CD;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8CC;const char*_tmp8CB;struct _tuple25*_tmp8CA;const char*_tmp8C9;const char*_tmp8C8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8C7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8C6;const char*_tmp8C5;struct _tuple25*_tmp8C4;const char*_tmp8C3;const char*_tmp8C2;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8C1;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8C0;const char*_tmp8BF;struct _tuple25*_tmp8BE[76];struct Cyc_List_List*options=
# 1150
(_tmp8BE[75]=(
# 1377
(_tmp8C4=_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4->f1=((_tmp8C3="--interface",_tag_dyneither(_tmp8C3,sizeof(char),12))),((_tmp8C4->f2=0,((_tmp8C4->f3=((_tmp8C2=" <file>",_tag_dyneither(_tmp8C2,sizeof(char),8))),((_tmp8C4->f4=(void*)(
# 1378
(_tmp8C0=_region_malloc(r,sizeof(*_tmp8C0)),((_tmp8C0[0]=((_tmp8C1.tag=5,((_tmp8C1.f1=Cyc_set_specified_interface,_tmp8C1)))),_tmp8C0)))),((_tmp8C4->f5=(
# 1379
(_tmp8BF="Set the interface file to be <file>.",_tag_dyneither(_tmp8BF,sizeof(char),37))),_tmp8C4)))))))))))),((_tmp8BE[74]=(
# 1374
(_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((_tmp8CA->f1=((_tmp8C9="--geninterface",_tag_dyneither(_tmp8C9,sizeof(char),15))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C8="",_tag_dyneither(_tmp8C8,sizeof(char),1))),((_tmp8CA->f4=(void*)(
# 1375
(_tmp8C6=_region_malloc(r,sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C7.tag=3,((_tmp8C7.f1=& Cyc_generate_interface_r,_tmp8C7)))),_tmp8C6)))),((_tmp8CA->f5=(
# 1376
(_tmp8C5="Generate interface files",_tag_dyneither(_tmp8C5,sizeof(char),25))),_tmp8CA)))))))))))),((_tmp8BE[73]=(
# 1371
(_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CF="-idirafter",_tag_dyneither(_tmp8CF,sizeof(char),11))),((_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CE="<dir>",_tag_dyneither(_tmp8CE,sizeof(char),6))),((_tmp8D0->f4=(void*)(
# 1372
(_tmp8CC=_region_malloc(r,sizeof(*_tmp8CC)),((_tmp8CC[0]=((_tmp8CD.tag=5,((_tmp8CD.f1=Cyc_add_idirafter,_tmp8CD)))),_tmp8CC)))),((_tmp8D0->f5=(
# 1373
(_tmp8CB="Add the directory to the second include path.",_tag_dyneither(_tmp8CB,sizeof(char),46))),_tmp8D0)))))))))))),((_tmp8BE[72]=(
# 1368
(_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((_tmp8D6->f1=((_tmp8D5="-isystem",_tag_dyneither(_tmp8D5,sizeof(char),9))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D4="<dir>",_tag_dyneither(_tmp8D4,sizeof(char),6))),((_tmp8D6->f4=(void*)(
# 1369
(_tmp8D2=_region_malloc(r,sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=5,((_tmp8D3.f1=Cyc_add_isystem,_tmp8D3)))),_tmp8D2)))),((_tmp8D6->f5=(
# 1370
(_tmp8D1="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp8D1,sizeof(char),90))),_tmp8D6)))))))))))),((_tmp8BE[71]=(
# 1365
(_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8DB="-iwithprefixbefore",_tag_dyneither(_tmp8DB,sizeof(char),19))),((_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8DA="<dir>",_tag_dyneither(_tmp8DA,sizeof(char),6))),((_tmp8DC->f4=(void*)(
# 1366
(_tmp8D8=_region_malloc(r,sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=5,((_tmp8D9.f1=Cyc_add_iwithprefixbefore,_tmp8D9)))),_tmp8D8)))),((_tmp8DC->f5=(
# 1367
(_tmp8D7="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8D7,sizeof(char),45))),_tmp8DC)))))))))))),((_tmp8BE[70]=(
# 1362
(_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((_tmp8E2->f1=((_tmp8E1="-iwithprefix",_tag_dyneither(_tmp8E1,sizeof(char),13))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8E0="<dir>",_tag_dyneither(_tmp8E0,sizeof(char),6))),((_tmp8E2->f4=(void*)(
# 1363
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE[0]=((_tmp8DF.tag=5,((_tmp8DF.f1=Cyc_add_iwithprefix,_tmp8DF)))),_tmp8DE)))),((_tmp8E2->f5=(
# 1364
(_tmp8DD="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp8DD,sizeof(char),47))),_tmp8E2)))))))))))),((_tmp8BE[69]=(
# 1359
(_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((_tmp8E8->f1=((_tmp8E7="-iprefix",_tag_dyneither(_tmp8E7,sizeof(char),9))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E6="<prefix>",_tag_dyneither(_tmp8E6,sizeof(char),9))),((_tmp8E8->f4=(void*)(
# 1360
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=5,((_tmp8E5.f1=Cyc_add_iprefix,_tmp8E5)))),_tmp8E4)))),((_tmp8E8->f5=(
# 1361
(_tmp8E3="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp8E3,sizeof(char),67))),_tmp8E8)))))))))))),((_tmp8BE[68]=(
# 1356
(_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((_tmp8EE->f1=((_tmp8ED="-noregions",_tag_dyneither(_tmp8ED,sizeof(char),11))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EC="",_tag_dyneither(_tmp8EC,sizeof(char),1))),((_tmp8EE->f4=(void*)(
# 1357
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=3,((_tmp8EB.f1=& Cyc_Absyn_no_regions,_tmp8EB)))),_tmp8EA)))),((_tmp8EE->f5=(
# 1358
(_tmp8E9="Generate code that doesn't use regions",_tag_dyneither(_tmp8E9,sizeof(char),39))),_tmp8EE)))))))))))),((_tmp8BE[67]=(
# 1353
(_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8F3="-port",_tag_dyneither(_tmp8F3,sizeof(char),6))),((_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F2="",_tag_dyneither(_tmp8F2,sizeof(char),1))),((_tmp8F4->f4=(void*)(
# 1354
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=0,((_tmp8F1.f1=Cyc_set_port_c_code,_tmp8F1)))),_tmp8F0)))),((_tmp8F4->f5=(
# 1355
(_tmp8EF="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp8EF,sizeof(char),38))),_tmp8F4)))))))))))),((_tmp8BE[66]=(
# 1350
(_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((_tmp8FA->f1=((_tmp8F9="-detailedlocation",_tag_dyneither(_tmp8F9,sizeof(char),18))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F8="",_tag_dyneither(_tmp8F8,sizeof(char),1))),((_tmp8FA->f4=(void*)(
# 1351
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=4,((_tmp8F7.f1=& Cyc_Position_use_gcc_style_location,_tmp8F7)))),_tmp8F6)))),((_tmp8FA->f5=(
# 1352
(_tmp8F5="Try to give more detailed location information for errors",_tag_dyneither(_tmp8F5,sizeof(char),58))),_tmp8FA)))))))))))),((_tmp8BE[65]=(
# 1347
(_tmp900=_region_malloc(r,sizeof(*_tmp900)),((_tmp900->f1=((_tmp8FF="-noregister",_tag_dyneither(_tmp8FF,sizeof(char),12))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FE="",_tag_dyneither(_tmp8FE,sizeof(char),1))),((_tmp900->f4=(void*)(
# 1348
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FD.tag=3,((_tmp8FD.f1=& Cyc_Parse_no_register,_tmp8FD)))),_tmp8FC)))),((_tmp900->f5=(
# 1349
(_tmp8FB="Treat register storage class as public",_tag_dyneither(_tmp8FB,sizeof(char),39))),_tmp900)))))))))))),((_tmp8BE[64]=(
# 1344
(_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp905="-warnregioncoerce",_tag_dyneither(_tmp905,sizeof(char),18))),((_tmp906->f2=0,((_tmp906->f3=((_tmp904="",_tag_dyneither(_tmp904,sizeof(char),1))),((_tmp906->f4=(void*)(
# 1345
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902[0]=((_tmp903.tag=3,((_tmp903.f1=& Cyc_Tcutil_warn_region_coerce,_tmp903)))),_tmp902)))),((_tmp906->f5=(
# 1346
(_tmp901="Warn when any region coercion is inserted",_tag_dyneither(_tmp901,sizeof(char),42))),_tmp906)))))))))))),((_tmp8BE[63]=(
# 1341
(_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((_tmp90C->f1=((_tmp90B="-warnaliascoercion",_tag_dyneither(_tmp90B,sizeof(char),19))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp90A="",_tag_dyneither(_tmp90A,sizeof(char),1))),((_tmp90C->f4=(void*)(
# 1342
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908[0]=((_tmp909.tag=3,((_tmp909.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp909)))),_tmp908)))),((_tmp90C->f5=(
# 1343
(_tmp907="Warn when any alias coercion is inserted",_tag_dyneither(_tmp907,sizeof(char),41))),_tmp90C)))))))))))),((_tmp8BE[62]=(
# 1338
(_tmp912=_region_malloc(r,sizeof(*_tmp912)),((_tmp912->f1=((_tmp911="-warnnullchecks",_tag_dyneither(_tmp911,sizeof(char),16))),((_tmp912->f2=0,((_tmp912->f3=((_tmp910="",_tag_dyneither(_tmp910,sizeof(char),1))),((_tmp912->f4=(void*)(
# 1339
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E[0]=((_tmp90F.tag=3,((_tmp90F.f1=& Cyc_Toc_warn_all_null_deref,_tmp90F)))),_tmp90E)))),((_tmp912->f5=(
# 1340
(_tmp90D="Warn when any null check can't be eliminated",_tag_dyneither(_tmp90D,sizeof(char),45))),_tmp912)))))))))))),((_tmp8BE[61]=(
# 1335
(_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp917="-warnboundschecks",_tag_dyneither(_tmp917,sizeof(char),18))),((_tmp918->f2=0,((_tmp918->f3=((_tmp916="",_tag_dyneither(_tmp916,sizeof(char),1))),((_tmp918->f4=(void*)(
# 1336
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=3,((_tmp915.f1=& Cyc_Toc_warn_bounds_checks,_tmp915)))),_tmp914)))),((_tmp918->f5=(
# 1337
(_tmp913="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp913,sizeof(char),44))),_tmp918)))))))))))),((_tmp8BE[60]=(
# 1332
(_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((_tmp91D="-CI",_tag_dyneither(_tmp91D,sizeof(char),4))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91C=" <file>",_tag_dyneither(_tmp91C,sizeof(char),8))),((_tmp91E->f4=(void*)(
# 1333
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=5,((_tmp91B.f1=Cyc_set_cyc_include,_tmp91B)))),_tmp91A)))),((_tmp91E->f5=(
# 1334
(_tmp919="Set cyc_include.h to be <file>",_tag_dyneither(_tmp919,sizeof(char),31))),_tmp91E)))))))))))),((_tmp8BE[59]=(
# 1329
(_tmp924=_region_malloc(r,sizeof(*_tmp924)),((_tmp924->f1=((_tmp923="-compile-for-boot",_tag_dyneither(_tmp923,sizeof(char),18))),((_tmp924->f2=0,((_tmp924->f3=((_tmp922="",_tag_dyneither(_tmp922,sizeof(char),1))),((_tmp924->f4=(void*)(
# 1330
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920[0]=((_tmp921.tag=3,((_tmp921.f1=& Cyc_Lex_compile_for_boot_r,_tmp921)))),_tmp920)))),((_tmp924->f5=(
# 1331
(_tmp91F="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp91F,sizeof(char),71))),_tmp924)))))))))))),((_tmp8BE[58]=(
# 1326
(_tmp92A=_region_malloc(r,sizeof(*_tmp92A)),((_tmp92A->f1=((_tmp929="-nocyc_setjmp",_tag_dyneither(_tmp929,sizeof(char),14))),((_tmp92A->f2=0,((_tmp92A->f3=((_tmp928="",_tag_dyneither(_tmp928,sizeof(char),1))),((_tmp92A->f4=(void*)(
# 1327
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=3,((_tmp927.f1=& Cyc_nocyc_setjmp_r,_tmp927)))),_tmp926)))),((_tmp92A->f5=(
# 1328
(_tmp925="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp925,sizeof(char),46))),_tmp92A)))))))))))),((_tmp8BE[57]=(
# 1323
(_tmp930=_region_malloc(r,sizeof(*_tmp930)),((_tmp930->f1=((_tmp92F="-printalleffects",_tag_dyneither(_tmp92F,sizeof(char),17))),((_tmp930->f2=0,((_tmp930->f3=((_tmp92E="",_tag_dyneither(_tmp92E,sizeof(char),1))),((_tmp930->f4=(void*)(
# 1324
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C[0]=((_tmp92D.tag=3,((_tmp92D.f1=& Cyc_print_all_effects_r,_tmp92D)))),_tmp92C)))),((_tmp930->f5=(
# 1325
(_tmp92B="Print effects for functions (type debugging)",_tag_dyneither(_tmp92B,sizeof(char),45))),_tmp930)))))))))))),((_tmp8BE[56]=(
# 1320
(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->f1=((_tmp935="-printfullevars",_tag_dyneither(_tmp935,sizeof(char),16))),((_tmp936->f2=0,((_tmp936->f3=((_tmp934="",_tag_dyneither(_tmp934,sizeof(char),1))),((_tmp936->f4=(void*)(
# 1321
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932[0]=((_tmp933.tag=3,((_tmp933.f1=& Cyc_print_full_evars_r,_tmp933)))),_tmp932)))),((_tmp936->f5=(
# 1322
(_tmp931="Print full information for evars (type debugging)",_tag_dyneither(_tmp931,sizeof(char),50))),_tmp936)))))))))))),((_tmp8BE[55]=(
# 1317
(_tmp93C=_region_malloc(r,sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B="-printallkinds",_tag_dyneither(_tmp93B,sizeof(char),15))),((_tmp93C->f2=0,((_tmp93C->f3=((_tmp93A="",_tag_dyneither(_tmp93A,sizeof(char),1))),((_tmp93C->f4=(void*)(
# 1318
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((_tmp939.tag=3,((_tmp939.f1=& Cyc_print_all_kinds_r,_tmp939)))),_tmp938)))),((_tmp93C->f5=(
# 1319
(_tmp937="Always print kinds of type variables",_tag_dyneither(_tmp937,sizeof(char),37))),_tmp93C)))))))))))),((_tmp8BE[54]=(
# 1314
(_tmp942=_region_malloc(r,sizeof(*_tmp942)),((_tmp942->f1=((_tmp941="-printalltvars",_tag_dyneither(_tmp941,sizeof(char),15))),((_tmp942->f2=0,((_tmp942->f3=((_tmp940="",_tag_dyneither(_tmp940,sizeof(char),1))),((_tmp942->f4=(void*)(
# 1315
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E[0]=((_tmp93F.tag=3,((_tmp93F.f1=& Cyc_print_all_tvars_r,_tmp93F)))),_tmp93E)))),((_tmp942->f5=(
# 1316
(_tmp93D="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp93D,sizeof(char),57))),_tmp942)))))))))))),((_tmp8BE[53]=(
# 1311
(_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948->f1=((_tmp947="-noexpandtypedefs",_tag_dyneither(_tmp947,sizeof(char),18))),((_tmp948->f2=0,((_tmp948->f3=((_tmp946="",_tag_dyneither(_tmp946,sizeof(char),1))),((_tmp948->f4=(void*)(
# 1312
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944[0]=((_tmp945.tag=3,((_tmp945.f1=& Cyc_noexpand_r,_tmp945)))),_tmp944)))),((_tmp948->f5=(
# 1313
(_tmp943="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp943,sizeof(char),41))),_tmp948)))))))))))),((_tmp8BE[52]=(
# 1308
(_tmp94E=_region_malloc(r,sizeof(*_tmp94E)),((_tmp94E->f1=((_tmp94D="-noremoveunused",_tag_dyneither(_tmp94D,sizeof(char),16))),((_tmp94E->f2=0,((_tmp94E->f3=((_tmp94C="",_tag_dyneither(_tmp94C,sizeof(char),1))),((_tmp94E->f4=(void*)(
# 1309
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=((_tmp94B.tag=3,((_tmp94B.f1=& Cyc_noshake_r,_tmp94B)))),_tmp94A)))),((_tmp94E->f5=(
# 1310
(_tmp949="Don't remove externed variables that aren't used",_tag_dyneither(_tmp949,sizeof(char),49))),_tmp94E)))))))))))),((_tmp8BE[51]=(
# 1305
(_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->f1=((_tmp953="-nogc",_tag_dyneither(_tmp953,sizeof(char),6))),((_tmp954->f2=0,((_tmp954->f3=((_tmp952="",_tag_dyneither(_tmp952,sizeof(char),1))),((_tmp954->f4=(void*)(
# 1306
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950[0]=((_tmp951.tag=3,((_tmp951.f1=& Cyc_nogc_r,_tmp951)))),_tmp950)))),((_tmp954->f5=(
# 1307
(_tmp94F="Don't link in the garbage collector",_tag_dyneither(_tmp94F,sizeof(char),36))),_tmp954)))))))))))),((_tmp8BE[50]=(
# 1302
(_tmp95A=_region_malloc(r,sizeof(*_tmp95A)),((_tmp95A->f1=((_tmp959="-nocyc",_tag_dyneither(_tmp959,sizeof(char),7))),((_tmp95A->f2=0,((_tmp95A->f3=((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),((_tmp95A->f4=(void*)(
# 1303
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=((_tmp957.tag=0,((_tmp957.f1=Cyc_set_nocyc,_tmp957)))),_tmp956)))),((_tmp95A->f5=(
# 1304
(_tmp955="Don't add implicit namespace Cyc",_tag_dyneither(_tmp955,sizeof(char),33))),_tmp95A)))))))))))),((_tmp8BE[49]=(
# 1299
(_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960->f1=((_tmp95F="-no-cpp-precomp",_tag_dyneither(_tmp95F,sizeof(char),16))),((_tmp960->f2=0,((_tmp960->f3=((_tmp95E="",_tag_dyneither(_tmp95E,sizeof(char),1))),((_tmp960->f4=(void*)(
# 1300
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C[0]=((_tmp95D.tag=0,((_tmp95D.f1=Cyc_set_nocppprecomp,_tmp95D)))),_tmp95C)))),((_tmp960->f5=(
# 1301
(_tmp95B="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp95B,sizeof(char),40))),_tmp960)))))))))))),((_tmp8BE[48]=(
# 1296
(_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966->f1=((_tmp965="-use-cpp",_tag_dyneither(_tmp965,sizeof(char),9))),((_tmp966->f2=0,((_tmp966->f3=((_tmp964="<path>",_tag_dyneither(_tmp964,sizeof(char),7))),((_tmp966->f4=(void*)(
# 1297
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=((_tmp963.tag=5,((_tmp963.f1=Cyc_set_cpp,_tmp963)))),_tmp962)))),((_tmp966->f5=(
# 1298
(_tmp961="Indicate which preprocessor to use",_tag_dyneither(_tmp961,sizeof(char),35))),_tmp966)))))))))))),((_tmp8BE[47]=(
# 1293
(_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C->f1=((_tmp96B="--inline-checks",_tag_dyneither(_tmp96B,sizeof(char),16))),((_tmp96C->f2=0,((_tmp96C->f3=((_tmp96A="",_tag_dyneither(_tmp96A,sizeof(char),1))),((_tmp96C->f4=(void*)(
# 1294
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968[0]=((_tmp969.tag=0,((_tmp969.f1=Cyc_set_inline_functions,_tmp969)))),_tmp968)))),((_tmp96C->f5=(
# 1295
(_tmp967="Inline bounds checks instead of #define",_tag_dyneither(_tmp967,sizeof(char),40))),_tmp96C)))))))))))),((_tmp8BE[46]=(
# 1290
(_tmp972=_region_malloc(r,sizeof(*_tmp972)),((_tmp972->f1=((_tmp971="--noboundschecks",_tag_dyneither(_tmp971,sizeof(char),17))),((_tmp972->f2=0,((_tmp972->f3=((_tmp970="",_tag_dyneither(_tmp970,sizeof(char),1))),((_tmp972->f4=(void*)(
# 1291
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=0,((_tmp96F.f1=Cyc_set_noboundschecks,_tmp96F)))),_tmp96E)))),((_tmp972->f5=(
# 1292
(_tmp96D="Disable bounds checks",_tag_dyneither(_tmp96D,sizeof(char),22))),_tmp972)))))))))))),((_tmp8BE[45]=(
# 1287
(_tmp978=_region_malloc(r,sizeof(*_tmp978)),((_tmp978->f1=((_tmp977="--nonullchecks",_tag_dyneither(_tmp977,sizeof(char),15))),((_tmp978->f2=0,((_tmp978->f3=((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp978->f4=(void*)(
# 1288
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974[0]=((_tmp975.tag=0,((_tmp975.f1=Cyc_set_nonullchecks,_tmp975)))),_tmp974)))),((_tmp978->f5=(
# 1289
(_tmp973="Disable null checks",_tag_dyneither(_tmp973,sizeof(char),20))),_tmp978)))))))))))),((_tmp8BE[44]=(
# 1284
(_tmp97E=_region_malloc(r,sizeof(*_tmp97E)),((_tmp97E->f1=((_tmp97D="--nochecks",_tag_dyneither(_tmp97D,sizeof(char),11))),((_tmp97E->f2=0,((_tmp97E->f3=((_tmp97C="",_tag_dyneither(_tmp97C,sizeof(char),1))),((_tmp97E->f4=(void*)(
# 1285
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A[0]=((_tmp97B.tag=0,((_tmp97B.f1=Cyc_set_nochecks,_tmp97B)))),_tmp97A)))),((_tmp97E->f5=(
# 1286
(_tmp979="Disable bounds/null checks",_tag_dyneither(_tmp979,sizeof(char),27))),_tmp97E)))))))))))),((_tmp8BE[43]=(
# 1281
(_tmp984=_region_malloc(r,sizeof(*_tmp984)),((_tmp984->f1=((_tmp983="--nolineno",_tag_dyneither(_tmp983,sizeof(char),11))),((_tmp984->f2=0,((_tmp984->f3=((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),((_tmp984->f4=(void*)(
# 1282
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=0,((_tmp981.f1=Cyc_clear_lineno,_tmp981)))),_tmp980)))),((_tmp984->f5=(
# 1283
(_tmp97F="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp97F,sizeof(char),63))),_tmp984)))))))))))),((_tmp8BE[42]=(
# 1278
(_tmp98A=_region_malloc(r,sizeof(*_tmp98A)),((_tmp98A->f1=((_tmp989="-save-c",_tag_dyneither(_tmp989,sizeof(char),8))),((_tmp98A->f2=0,((_tmp98A->f3=((_tmp988="",_tag_dyneither(_tmp988,sizeof(char),1))),((_tmp98A->f4=(void*)(
# 1279
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986[0]=((_tmp987.tag=3,((_tmp987.f1=& Cyc_save_c_r,_tmp987)))),_tmp986)))),((_tmp98A->f5=(
# 1280
(_tmp985="Don't delete temporary C files",_tag_dyneither(_tmp985,sizeof(char),31))),_tmp98A)))))))))))),((_tmp8BE[41]=(
# 1275
(_tmp990=_region_malloc(r,sizeof(*_tmp990)),((_tmp990->f1=((_tmp98F="-save-temps",_tag_dyneither(_tmp98F,sizeof(char),12))),((_tmp990->f2=0,((_tmp990->f3=((_tmp98E="",_tag_dyneither(_tmp98E,sizeof(char),1))),((_tmp990->f4=(void*)(
# 1276
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=0,((_tmp98D.f1=Cyc_set_save_temps,_tmp98D)))),_tmp98C)))),((_tmp990->f5=(
# 1277
(_tmp98B="Don't delete temporary files",_tag_dyneither(_tmp98B,sizeof(char),29))),_tmp990)))))))))))),((_tmp8BE[40]=(
# 1272
(_tmp996=_region_malloc(r,sizeof(*_tmp996)),((_tmp996->f1=((_tmp995="-c-comp",_tag_dyneither(_tmp995,sizeof(char),8))),((_tmp996->f2=0,((_tmp996->f3=((_tmp994=" <compiler>",_tag_dyneither(_tmp994,sizeof(char),12))),((_tmp996->f4=(void*)(
# 1273
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992[0]=((_tmp993.tag=5,((_tmp993.f1=Cyc_set_c_compiler,_tmp993)))),_tmp992)))),((_tmp996->f5=(
# 1274
(_tmp991="Produce C output for the given compiler",_tag_dyneither(_tmp991,sizeof(char),40))),_tmp996)))))))))))),((_tmp8BE[39]=(
# 1269
(_tmp99C=_region_malloc(r,sizeof(*_tmp99C)),((_tmp99C->f1=((_tmp99B="-un-gcc",_tag_dyneither(_tmp99B,sizeof(char),8))),((_tmp99C->f2=0,((_tmp99C->f3=((_tmp99A="",_tag_dyneither(_tmp99A,sizeof(char),1))),((_tmp99C->f4=(void*)(
# 1270
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998[0]=((_tmp999.tag=0,((_tmp999.f1=Cyc_set_tovc,_tmp999)))),_tmp998)))),((_tmp99C->f5=(
# 1271
(_tmp997="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp997,sizeof(char),57))),_tmp99C)))))))))))),((_tmp8BE[38]=(
# 1266
(_tmp9A2=_region_malloc(r,sizeof(*_tmp9A2)),((_tmp9A2->f1=((_tmp9A1="-no-elim-statement-expressions",_tag_dyneither(_tmp9A1,sizeof(char),31))),((_tmp9A2->f2=0,((_tmp9A2->f3=((_tmp9A0="",_tag_dyneither(_tmp9A0,sizeof(char),1))),((_tmp9A2->f4=(void*)(
# 1267
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E[0]=((_tmp99F.tag=4,((_tmp99F.f1=& Cyc_elim_se_r,_tmp99F)))),_tmp99E)))),((_tmp9A2->f5=(
# 1268
(_tmp99D="Do not avoid statement expressions in C output",_tag_dyneither(_tmp99D,sizeof(char),47))),_tmp9A2)))))))))))),((_tmp8BE[37]=(
# 1262
(_tmp9A8=_region_malloc(r,sizeof(*_tmp9A8)),((_tmp9A8->f1=((_tmp9A7="-elim-statement-expressions",_tag_dyneither(_tmp9A7,sizeof(char),28))),((_tmp9A8->f2=0,((_tmp9A8->f3=((_tmp9A6="",_tag_dyneither(_tmp9A6,sizeof(char),1))),((_tmp9A8->f4=(void*)(
# 1263
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4[0]=((_tmp9A5.tag=3,((_tmp9A5.f1=& Cyc_elim_se_r,_tmp9A5)))),_tmp9A4)))),((_tmp9A8->f5=(
# 1265
(_tmp9A3="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9A3,sizeof(char),66))),_tmp9A8)))))))))))),((_tmp8BE[36]=(
# 1259
(_tmp9AE=_region_malloc(r,sizeof(*_tmp9AE)),((_tmp9AE->f1=((_tmp9AD="-up",_tag_dyneither(_tmp9AD,sizeof(char),4))),((_tmp9AE->f2=0,((_tmp9AE->f3=((_tmp9AC="",_tag_dyneither(_tmp9AC,sizeof(char),1))),((_tmp9AE->f4=(void*)(
# 1260
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA[0]=((_tmp9AB.tag=4,((_tmp9AB.f1=& Cyc_pp_r,_tmp9AB)))),_tmp9AA)))),((_tmp9AE->f5=(
# 1261
(_tmp9A9="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9A9,sizeof(char),55))),_tmp9AE)))))))))))),((_tmp8BE[35]=(
# 1256
(_tmp9B4=_region_malloc(r,sizeof(*_tmp9B4)),((_tmp9B4->f1=((_tmp9B3="-pp",_tag_dyneither(_tmp9B3,sizeof(char),4))),((_tmp9B4->f2=0,((_tmp9B4->f3=((_tmp9B2="",_tag_dyneither(_tmp9B2,sizeof(char),1))),((_tmp9B4->f4=(void*)(
# 1257
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0[0]=((_tmp9B1.tag=3,((_tmp9B1.f1=& Cyc_pp_r,_tmp9B1)))),_tmp9B0)))),((_tmp9B4->f5=(
# 1258
(_tmp9AF="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9AF,sizeof(char),47))),_tmp9B4)))))))))))),((_tmp8BE[34]=(
# 1253
(_tmp9BA=_region_malloc(r,sizeof(*_tmp9BA)),((_tmp9BA->f1=((_tmp9B9="-ic",_tag_dyneither(_tmp9B9,sizeof(char),4))),((_tmp9BA->f2=0,((_tmp9BA->f3=((_tmp9B8="",_tag_dyneither(_tmp9B8,sizeof(char),1))),((_tmp9BA->f4=(void*)(
# 1254
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B7.tag=3,((_tmp9B7.f1=& Cyc_ic_r,_tmp9B7)))),_tmp9B6)))),((_tmp9BA->f5=(
# 1255
(_tmp9B5="Activate the link-checker",_tag_dyneither(_tmp9B5,sizeof(char),26))),_tmp9BA)))))))))))),((_tmp8BE[33]=(
# 1250
(_tmp9C0=_region_malloc(r,sizeof(*_tmp9C0)),((_tmp9C0->f1=((_tmp9BF="-stopafter-toc",_tag_dyneither(_tmp9BF,sizeof(char),15))),((_tmp9C0->f2=0,((_tmp9C0->f3=((_tmp9BE="",_tag_dyneither(_tmp9BE,sizeof(char),1))),((_tmp9C0->f4=(void*)(
# 1251
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC[0]=((_tmp9BD.tag=3,((_tmp9BD.f1=& Cyc_toc_r,_tmp9BD)))),_tmp9BC)))),((_tmp9C0->f5=(
# 1252
(_tmp9BB="Stop after translation to C",_tag_dyneither(_tmp9BB,sizeof(char),28))),_tmp9C0)))))))))))),((_tmp8BE[32]=(
# 1247
(_tmp9C6=_region_malloc(r,sizeof(*_tmp9C6)),((_tmp9C6->f1=((_tmp9C5="-stopafter-cf",_tag_dyneither(_tmp9C5,sizeof(char),14))),((_tmp9C6->f2=0,((_tmp9C6->f3=((_tmp9C4="",_tag_dyneither(_tmp9C4,sizeof(char),1))),((_tmp9C6->f4=(void*)(
# 1248
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=3,((_tmp9C3.f1=& Cyc_cf_r,_tmp9C3)))),_tmp9C2)))),((_tmp9C6->f5=(
# 1249
(_tmp9C1="Stop after control-flow checking",_tag_dyneither(_tmp9C1,sizeof(char),33))),_tmp9C6)))))))))))),((_tmp8BE[31]=(
# 1244
(_tmp9CC=_region_malloc(r,sizeof(*_tmp9CC)),((_tmp9CC->f1=((_tmp9CB="-noprint",_tag_dyneither(_tmp9CB,sizeof(char),9))),((_tmp9CC->f2=0,((_tmp9CC->f3=((_tmp9CA="",_tag_dyneither(_tmp9CA,sizeof(char),1))),((_tmp9CC->f4=(void*)(
# 1245
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=3,((_tmp9C9.f1=& Cyc_noprint_r,_tmp9C9)))),_tmp9C8)))),((_tmp9CC->f5=(
# 1246
(_tmp9C7="Do not print output (when stopping early)",_tag_dyneither(_tmp9C7,sizeof(char),42))),_tmp9CC)))))))))))),((_tmp8BE[30]=(
# 1241
(_tmp9D2=_region_malloc(r,sizeof(*_tmp9D2)),((_tmp9D2->f1=((_tmp9D1="-stopafter-tc",_tag_dyneither(_tmp9D1,sizeof(char),14))),((_tmp9D2->f2=0,((_tmp9D2->f3=((_tmp9D0="",_tag_dyneither(_tmp9D0,sizeof(char),1))),((_tmp9D2->f4=(void*)(
# 1242
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE[0]=((_tmp9CF.tag=3,((_tmp9CF.f1=& Cyc_tc_r,_tmp9CF)))),_tmp9CE)))),((_tmp9D2->f5=(
# 1243
(_tmp9CD="Stop after type checking",_tag_dyneither(_tmp9CD,sizeof(char),25))),_tmp9D2)))))))))))),((_tmp8BE[29]=(
# 1238
(_tmp9D8=_region_malloc(r,sizeof(*_tmp9D8)),((_tmp9D8->f1=((_tmp9D7="-stopafter-parse",_tag_dyneither(_tmp9D7,sizeof(char),17))),((_tmp9D8->f2=0,((_tmp9D8->f3=((_tmp9D6="",_tag_dyneither(_tmp9D6,sizeof(char),1))),((_tmp9D8->f4=(void*)(
# 1239
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4[0]=((_tmp9D5.tag=3,((_tmp9D5.f1=& Cyc_parseonly_r,_tmp9D5)))),_tmp9D4)))),((_tmp9D8->f5=(
# 1240
(_tmp9D3="Stop after parsing",_tag_dyneither(_tmp9D3,sizeof(char),19))),_tmp9D8)))))))))))),((_tmp8BE[28]=(
# 1235
(_tmp9DE=_region_malloc(r,sizeof(*_tmp9DE)),((_tmp9DE->f1=((_tmp9DD="-E",_tag_dyneither(_tmp9DD,sizeof(char),3))),((_tmp9DE->f2=0,((_tmp9DE->f3=((_tmp9DC="",_tag_dyneither(_tmp9DC,sizeof(char),1))),((_tmp9DE->f4=(void*)(
# 1236
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=3,((_tmp9DB.f1=& Cyc_stop_after_cpp_r,_tmp9DB)))),_tmp9DA)))),((_tmp9DE->f5=(
# 1237
(_tmp9D9="Stop after preprocessing",_tag_dyneither(_tmp9D9,sizeof(char),25))),_tmp9DE)))))))))))),((_tmp8BE[27]=(
# 1232
(_tmp9E4=_region_malloc(r,sizeof(*_tmp9E4)),((_tmp9E4->f1=((_tmp9E3="-Wall",_tag_dyneither(_tmp9E3,sizeof(char),6))),((_tmp9E4->f2=0,((_tmp9E4->f3=((_tmp9E2="",_tag_dyneither(_tmp9E2,sizeof(char),1))),((_tmp9E4->f4=(void*)(
# 1233
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=0,((_tmp9E1.f1=Cyc_set_all_warnings,_tmp9E1)))),_tmp9E0)))),((_tmp9E4->f5=(
# 1234
(_tmp9DF="Turn on all warnings",_tag_dyneither(_tmp9DF,sizeof(char),21))),_tmp9E4)))))))))))),((_tmp8BE[26]=(
# 1229
(_tmp9EA=_region_malloc(r,sizeof(*_tmp9EA)),((_tmp9EA->f1=((_tmp9E9="-Wlose-unique",_tag_dyneither(_tmp9E9,sizeof(char),14))),((_tmp9EA->f2=0,((_tmp9EA->f3=((_tmp9E8="",_tag_dyneither(_tmp9E8,sizeof(char),1))),((_tmp9EA->f4=(void*)(
# 1230
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6[0]=((_tmp9E7.tag=3,((_tmp9E7.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmp9E7)))),_tmp9E6)))),((_tmp9EA->f5=(
# 1231
(_tmp9E5="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmp9E5,sizeof(char),49))),_tmp9EA)))))))))))),((_tmp8BE[25]=(
# 1226
(_tmp9F0=_region_malloc(r,sizeof(*_tmp9F0)),((_tmp9F0->f1=((_tmp9EF="-b",_tag_dyneither(_tmp9EF,sizeof(char),3))),((_tmp9F0->f2=0,((_tmp9F0->f3=((_tmp9EE="<arch>",_tag_dyneither(_tmp9EE,sizeof(char),7))),((_tmp9F0->f4=(void*)(
# 1227
(_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC[0]=((_tmp9ED.tag=5,((_tmp9ED.f1=Cyc_set_target_arch,_tmp9ED)))),_tmp9EC)))),((_tmp9F0->f5=(
# 1228
(_tmp9EB="Set target architecture; implies -stopafter-toc",_tag_dyneither(_tmp9EB,sizeof(char),48))),_tmp9F0)))))))))))),((_tmp8BE[24]=(
# 1223
(_tmp9F6=_region_malloc(r,sizeof(*_tmp9F6)),((_tmp9F6->f1=((_tmp9F5="-MT",_tag_dyneither(_tmp9F5,sizeof(char),4))),((_tmp9F6->f2=0,((_tmp9F6->f3=((_tmp9F4=" <target>",_tag_dyneither(_tmp9F4,sizeof(char),10))),((_tmp9F6->f4=(void*)(
# 1224
(_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2[0]=((_tmp9F3.tag=5,((_tmp9F3.f1=Cyc_set_dependencies_target,_tmp9F3)))),_tmp9F2)))),((_tmp9F6->f5=(
# 1225
(_tmp9F1="Give target for dependencies",_tag_dyneither(_tmp9F1,sizeof(char),29))),_tmp9F6)))))))))))),((_tmp8BE[23]=(
# 1219
(_tmp9FC=_region_malloc(r,sizeof(*_tmp9FC)),((_tmp9FC->f1=((_tmp9FB="-MG",_tag_dyneither(_tmp9FB,sizeof(char),4))),((_tmp9FC->f2=0,((_tmp9FC->f3=((_tmp9FA="",_tag_dyneither(_tmp9FA,sizeof(char),1))),((_tmp9FC->f4=(void*)(
# 1220
(_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8[0]=((_tmp9F9.tag=1,((_tmp9F9.f1=Cyc_add_cpparg,_tmp9F9)))),_tmp9F8)))),((_tmp9FC->f5=(
# 1222
(_tmp9F7="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmp9F7,sizeof(char),68))),_tmp9FC)))))))))))),((_tmp8BE[22]=(
# 1216
(_tmpA02=_region_malloc(r,sizeof(*_tmpA02)),((_tmpA02->f1=((_tmpA01="-M",_tag_dyneither(_tmpA01,sizeof(char),3))),((_tmpA02->f2=0,((_tmpA02->f3=((_tmpA00="",_tag_dyneither(_tmpA00,sizeof(char),1))),((_tmpA02->f4=(void*)(
# 1217
(_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=0,((_tmp9FF.f1=Cyc_set_produce_dependencies,_tmp9FF)))),_tmp9FE)))),((_tmpA02->f5=(
# 1218
(_tmp9FD="Produce dependencies",_tag_dyneither(_tmp9FD,sizeof(char),21))),_tmpA02)))))))))))),((_tmp8BE[21]=(
# 1213
(_tmpA08=_region_malloc(r,sizeof(*_tmpA08)),((_tmpA08->f1=((_tmpA07="-S",_tag_dyneither(_tmpA07,sizeof(char),3))),((_tmpA08->f2=0,((_tmpA08->f3=((_tmpA06="",_tag_dyneither(_tmpA06,sizeof(char),1))),((_tmpA08->f4=(void*)(
# 1214
(_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04[0]=((_tmpA05.tag=0,((_tmpA05.f1=Cyc_set_stop_after_asmfile,_tmpA05)))),_tmpA04)))),((_tmpA08->f5=(
# 1215
(_tmpA03="Stop after producing assembly code",_tag_dyneither(_tmpA03,sizeof(char),35))),_tmpA08)))))))))))),((_tmp8BE[20]=(
# 1210
(_tmpA0E=_region_malloc(r,sizeof(*_tmpA0E)),((_tmpA0E->f1=((_tmpA0D="-pa",_tag_dyneither(_tmpA0D,sizeof(char),4))),((_tmpA0E->f2=0,((_tmpA0E->f3=((_tmpA0C="",_tag_dyneither(_tmpA0C,sizeof(char),1))),((_tmpA0E->f4=(void*)(
# 1211
(_tmpA0A=_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=0,((_tmpA0B.f1=Cyc_set_pa,_tmpA0B)))),_tmpA0A)))),((_tmpA0E->f5=(
# 1212
(_tmpA09="Compile for profiling with aprof",_tag_dyneither(_tmpA09,sizeof(char),33))),_tmpA0E)))))))))))),((_tmp8BE[19]=(
# 1207
(_tmpA14=_region_malloc(r,sizeof(*_tmpA14)),((_tmpA14->f1=((_tmpA13="-pg",_tag_dyneither(_tmpA13,sizeof(char),4))),((_tmpA14->f2=0,((_tmpA14->f3=((_tmpA12="",_tag_dyneither(_tmpA12,sizeof(char),1))),((_tmpA14->f4=(void*)(
# 1208
(_tmpA10=_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10[0]=((_tmpA11.tag=0,((_tmpA11.f1=Cyc_set_pg,_tmpA11)))),_tmpA10)))),((_tmpA14->f5=(
# 1209
(_tmpA0F="Compile for profiling with gprof",_tag_dyneither(_tmpA0F,sizeof(char),33))),_tmpA14)))))))))))),((_tmp8BE[18]=(
# 1204
(_tmpA1A=_region_malloc(r,sizeof(*_tmpA1A)),((_tmpA1A->f1=((_tmpA19="-p",_tag_dyneither(_tmpA19,sizeof(char),3))),((_tmpA1A->f2=0,((_tmpA1A->f3=((_tmpA18="",_tag_dyneither(_tmpA18,sizeof(char),1))),((_tmpA1A->f4=(void*)(
# 1205
(_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA17.tag=1,((_tmpA17.f1=Cyc_add_ccarg,_tmpA17)))),_tmpA16)))),((_tmpA1A->f5=(
# 1206
(_tmpA15="Compile for profiling with prof",_tag_dyneither(_tmpA15,sizeof(char),32))),_tmpA1A)))))))))))),((_tmp8BE[17]=(
# 1201
(_tmpA20=_region_malloc(r,sizeof(*_tmpA20)),((_tmpA20->f1=((_tmpA1F="-g",_tag_dyneither(_tmpA1F,sizeof(char),3))),((_tmpA20->f2=0,((_tmpA20->f3=((_tmpA1E="",_tag_dyneither(_tmpA1E,sizeof(char),1))),((_tmpA20->f4=(void*)(
# 1202
(_tmpA1C=_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1D.tag=1,((_tmpA1D.f1=Cyc_add_ccarg,_tmpA1D)))),_tmpA1C)))),((_tmpA20->f5=(
# 1203
(_tmpA1B="Compile for debugging",_tag_dyneither(_tmpA1B,sizeof(char),22))),_tmpA20)))))))))))),((_tmp8BE[16]=(
# 1198
(_tmpA26=_region_malloc(r,sizeof(*_tmpA26)),((_tmpA26->f1=((_tmpA25="-fomit-frame-pointer",_tag_dyneither(_tmpA25,sizeof(char),21))),((_tmpA26->f2=0,((_tmpA26->f3=((_tmpA24="",_tag_dyneither(_tmpA24,sizeof(char),1))),((_tmpA26->f4=(void*)(
# 1199
(_tmpA22=_region_malloc(r,sizeof(*_tmpA22)),((_tmpA22[0]=((_tmpA23.tag=1,((_tmpA23.f1=Cyc_add_ccarg,_tmpA23)))),_tmpA22)))),((_tmpA26->f5=(
# 1200
(_tmpA21="Omit frame pointer",_tag_dyneither(_tmpA21,sizeof(char),19))),_tmpA26)))))))))))),((_tmp8BE[15]=(
# 1195
(_tmpA2C=_region_malloc(r,sizeof(*_tmpA2C)),((_tmpA2C->f1=((_tmpA2B="-O3",_tag_dyneither(_tmpA2B,sizeof(char),4))),((_tmpA2C->f2=0,((_tmpA2C->f3=((_tmpA2A="",_tag_dyneither(_tmpA2A,sizeof(char),1))),((_tmpA2C->f4=(void*)(
# 1196
(_tmpA28=_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=1,((_tmpA29.f1=Cyc_add_ccarg,_tmpA29)))),_tmpA28)))),((_tmpA2C->f5=(
# 1197
(_tmpA27="Even more optimization",_tag_dyneither(_tmpA27,sizeof(char),23))),_tmpA2C)))))))))))),((_tmp8BE[14]=(
# 1192
(_tmpA32=_region_malloc(r,sizeof(*_tmpA32)),((_tmpA32->f1=((_tmpA31="-O2",_tag_dyneither(_tmpA31,sizeof(char),4))),((_tmpA32->f2=0,((_tmpA32->f3=((_tmpA30="",_tag_dyneither(_tmpA30,sizeof(char),1))),((_tmpA32->f4=(void*)(
# 1193
(_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E[0]=((_tmpA2F.tag=1,((_tmpA2F.f1=Cyc_add_ccarg,_tmpA2F)))),_tmpA2E)))),((_tmpA32->f5=(
# 1194
(_tmpA2D="A higher level of optimization",_tag_dyneither(_tmpA2D,sizeof(char),31))),_tmpA32)))))))))))),((_tmp8BE[13]=(
# 1189
(_tmpA38=_region_malloc(r,sizeof(*_tmpA38)),((_tmpA38->f1=((_tmpA37="-O",_tag_dyneither(_tmpA37,sizeof(char),3))),((_tmpA38->f2=0,((_tmpA38->f3=((_tmpA36="",_tag_dyneither(_tmpA36,sizeof(char),1))),((_tmpA38->f4=(void*)(
# 1190
(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34[0]=((_tmpA35.tag=1,((_tmpA35.f1=Cyc_add_ccarg,_tmpA35)))),_tmpA34)))),((_tmpA38->f5=(
# 1191
(_tmpA33="Optimize",_tag_dyneither(_tmpA33,sizeof(char),9))),_tmpA38)))))))))))),((_tmp8BE[12]=(
# 1186
(_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->f1=((_tmpA3D="-O0",_tag_dyneither(_tmpA3D,sizeof(char),4))),((_tmpA3E->f2=0,((_tmpA3E->f3=((_tmpA3C="",_tag_dyneither(_tmpA3C,sizeof(char),1))),((_tmpA3E->f4=(void*)(
# 1187
(_tmpA3A=_region_malloc(r,sizeof(*_tmpA3A)),((_tmpA3A[0]=((_tmpA3B.tag=1,((_tmpA3B.f1=Cyc_add_ccarg,_tmpA3B)))),_tmpA3A)))),((_tmpA3E->f5=(
# 1188
(_tmpA39="Don't optimize",_tag_dyneither(_tmpA39,sizeof(char),15))),_tmpA3E)))))))))))),((_tmp8BE[11]=(
# 1183
(_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->f1=((_tmpA43="-s",_tag_dyneither(_tmpA43,sizeof(char),3))),((_tmpA44->f2=0,((_tmpA44->f3=((_tmpA42="",_tag_dyneither(_tmpA42,sizeof(char),1))),((_tmpA44->f4=(void*)(
# 1184
(_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40[0]=((_tmpA41.tag=1,((_tmpA41.f1=Cyc_add_ccarg,_tmpA41)))),_tmpA40)))),((_tmpA44->f5=(
# 1185
(_tmpA3F="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA3F,sizeof(char),60))),_tmpA44)))))))))))),((_tmp8BE[10]=(
# 1180
(_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A->f1=((_tmpA49="-x",_tag_dyneither(_tmpA49,sizeof(char),3))),((_tmpA4A->f2=0,((_tmpA4A->f3=((_tmpA48=" <language>",_tag_dyneither(_tmpA48,sizeof(char),12))),((_tmpA4A->f4=(void*)(
# 1181
(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=5,((_tmpA47.f1=Cyc_set_inputtype,_tmpA47)))),_tmpA46)))),((_tmpA4A->f5=(
# 1182
(_tmpA45="Specify <language> for the following input files",_tag_dyneither(_tmpA45,sizeof(char),49))),_tmpA4A)))))))))))),((_tmp8BE[9]=(
# 1177
(_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->f1=((_tmpA4F="-c",_tag_dyneither(_tmpA4F,sizeof(char),3))),((_tmpA50->f2=0,((_tmpA50->f3=((_tmpA4E="",_tag_dyneither(_tmpA4E,sizeof(char),1))),((_tmpA50->f4=(void*)(
# 1178
(_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=0,((_tmpA4D.f1=Cyc_set_stop_after_objectfile,_tmpA4D)))),_tmpA4C)))),((_tmpA50->f5=(
# 1179
(_tmpA4B="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA4B,sizeof(char),61))),_tmpA50)))))))))))),((_tmp8BE[8]=(
# 1174
(_tmpA56=_region_malloc(r,sizeof(*_tmpA56)),((_tmpA56->f1=((_tmpA55="-m",_tag_dyneither(_tmpA55,sizeof(char),3))),((_tmpA56->f2=1,((_tmpA56->f3=((_tmpA54="<option>",_tag_dyneither(_tmpA54,sizeof(char),9))),((_tmpA56->f4=(void*)(
# 1175
(_tmpA52=_region_malloc(r,sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=1,((_tmpA53.f1=Cyc_add_marg,_tmpA53)))),_tmpA52)))),((_tmpA56->f5=(
# 1176
(_tmpA51="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA51,sizeof(char),52))),_tmpA56)))))))))))),((_tmp8BE[7]=(
# 1171
(_tmpA5C=_region_malloc(r,sizeof(*_tmpA5C)),((_tmpA5C->f1=((_tmpA5B="-l",_tag_dyneither(_tmpA5B,sizeof(char),3))),((_tmpA5C->f2=1,((_tmpA5C->f3=((_tmpA5A="<libname>",_tag_dyneither(_tmpA5A,sizeof(char),10))),((_tmpA5C->f4=(void*)(
# 1172
(_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=1,((_tmpA59.f1=Cyc_add_libarg,_tmpA59)))),_tmpA58)))),((_tmpA5C->f5=(
# 1173
(_tmpA57="Library file",_tag_dyneither(_tmpA57,sizeof(char),13))),_tmpA5C)))))))))))),((_tmp8BE[6]=(
# 1168
(_tmpA62=_region_malloc(r,sizeof(*_tmpA62)),((_tmpA62->f1=((_tmpA61="-L",_tag_dyneither(_tmpA61,sizeof(char),3))),((_tmpA62->f2=1,((_tmpA62->f3=((_tmpA60="<dir>",_tag_dyneither(_tmpA60,sizeof(char),6))),((_tmpA62->f4=(void*)(
# 1169
(_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=1,((_tmpA5F.f1=Cyc_add_ccarg,_tmpA5F)))),_tmpA5E)))),((_tmpA62->f5=(
# 1170
(_tmpA5D="Add to the list of directories for -l",_tag_dyneither(_tmpA5D,sizeof(char),38))),_tmpA62)))))))))))),((_tmp8BE[5]=(
# 1165
(_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68->f1=((_tmpA67="-I",_tag_dyneither(_tmpA67,sizeof(char),3))),((_tmpA68->f2=1,((_tmpA68->f3=((_tmpA66="<dir>",_tag_dyneither(_tmpA66,sizeof(char),6))),((_tmpA68->f4=(void*)(
# 1166
(_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=1,((_tmpA65.f1=Cyc_add_cpparg,_tmpA65)))),_tmpA64)))),((_tmpA68->f5=(
# 1167
(_tmpA63="Add to the list of directories to search for include files",_tag_dyneither(_tmpA63,sizeof(char),59))),_tmpA68)))))))))))),((_tmp8BE[4]=(
# 1162
(_tmpA6E=_region_malloc(r,sizeof(*_tmpA6E)),((_tmpA6E->f1=((_tmpA6D="-B",_tag_dyneither(_tmpA6D,sizeof(char),3))),((_tmpA6E->f2=1,((_tmpA6E->f3=((_tmpA6C="<file>",_tag_dyneither(_tmpA6C,sizeof(char),7))),((_tmpA6E->f4=(void*)(
# 1163
(_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=1,((_tmpA6B.f1=Cyc_add_cyclone_exec_path,_tmpA6B)))),_tmpA6A)))),((_tmpA6E->f5=(
# 1164
(_tmpA69="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA69,sizeof(char),60))),_tmpA6E)))))))))))),((_tmp8BE[3]=(
# 1159
(_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74->f1=((_tmpA73="-D",_tag_dyneither(_tmpA73,sizeof(char),3))),((_tmpA74->f2=1,((_tmpA74->f3=((_tmpA72="<name>[=<value>]",_tag_dyneither(_tmpA72,sizeof(char),17))),((_tmpA74->f4=(void*)(
# 1160
(_tmpA70=_region_malloc(r,sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=1,((_tmpA71.f1=Cyc_add_cpparg,_tmpA71)))),_tmpA70)))),((_tmpA74->f5=(
# 1161
(_tmpA6F="Pass definition to preprocessor",_tag_dyneither(_tmpA6F,sizeof(char),32))),_tmpA74)))))))))))),((_tmp8BE[2]=(
# 1156
(_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A->f1=((_tmpA79="-o",_tag_dyneither(_tmpA79,sizeof(char),3))),((_tmpA7A->f2=0,((_tmpA7A->f3=((_tmpA78=" <file>",_tag_dyneither(_tmpA78,sizeof(char),8))),((_tmpA7A->f4=(void*)(
# 1157
(_tmpA76=_region_malloc(r,sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA77.tag=5,((_tmpA77.f1=Cyc_set_output_file,_tmpA77)))),_tmpA76)))),((_tmpA7A->f5=(
# 1158
(_tmpA75="Set the output file name to <file>",_tag_dyneither(_tmpA75,sizeof(char),35))),_tmpA7A)))))))))))),((_tmp8BE[1]=(
# 1153
(_tmpA80=_region_malloc(r,sizeof(*_tmpA80)),((_tmpA80->f1=((_tmpA7F="--version",_tag_dyneither(_tmpA7F,sizeof(char),10))),((_tmpA80->f2=0,((_tmpA80->f3=((_tmpA7E="",_tag_dyneither(_tmpA7E,sizeof(char),1))),((_tmpA80->f4=(void*)(
# 1154
(_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C[0]=((_tmpA7D.tag=0,((_tmpA7D.f1=Cyc_print_version,_tmpA7D)))),_tmpA7C)))),((_tmpA80->f5=(
# 1155
(_tmpA7B="Print version information and exit",_tag_dyneither(_tmpA7B,sizeof(char),35))),_tmpA80)))))))))))),((_tmp8BE[0]=(
# 1150
(_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86->f1=((_tmpA85="-v",_tag_dyneither(_tmpA85,sizeof(char),3))),((_tmpA86->f2=0,((_tmpA86->f3=((_tmpA84="",_tag_dyneither(_tmpA84,sizeof(char),1))),((_tmpA86->f4=(void*)(
# 1151
(_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA83.tag=3,((_tmpA83.f1=& Cyc_v_r,_tmpA83)))),_tmpA82)))),((_tmpA86->f5=(
# 1152
(_tmpA81="Print compilation stages verbosely",_tag_dyneither(_tmpA81,sizeof(char),35))),_tmpA86)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8BE,sizeof(struct _tuple25*),76)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1382
const char*_tmpA87;struct _dyneither_ptr optstring=(_tmpA87="Options:",_tag_dyneither(_tmpA87,sizeof(char),9));
# 1383
Cyc_Arg_current=0;
# 1384
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp169);
# 1387
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1395
char*_tmpA88;struct _dyneither_ptr cyclone_exec_prefix=(_tmpA88=getenv((const char*)"CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpA88,sizeof(char),_get_zero_arr_size_char((void*)_tmpA88,1)));
# 1396
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
# 1397
struct _dyneither_ptr*_tmpA8B;struct Cyc_List_List*_tmpA8A;Cyc_cyclone_exec_path=((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->hd=((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=cyclone_exec_prefix,_tmpA8B)))),((_tmpA8A->tl=Cyc_cyclone_exec_path,_tmpA8A))))));}
# 1398
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
# 1399
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
# 1400
struct _dyneither_ptr*_tmpA91;const char*_tmpA90;struct Cyc_List_List*_tmpA8F;Cyc_cyclone_exec_path=(
# 1401
(_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->hd=((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpA90="cyc-lib",_tag_dyneither(_tmpA90,sizeof(char),8)))),_tmpA91)))),((_tmpA8F->tl=Cyc_cyclone_exec_path,_tmpA8F))))));}
# 1403
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
# 1404
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1411
const char*_tmpA92;struct _dyneither_ptr _tmp170=Cyc_do_find(cyclone_arch_path,((_tmpA92="cycspecs",_tag_dyneither(_tmpA92,sizeof(char),9))));
# 1412
if(Cyc_v_r){const char*_tmpA96;void*_tmpA95[1];struct Cyc_String_pa_PrintArg_struct _tmpA94;(_tmpA94.tag=0,((_tmpA94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp170),((_tmpA95[0]=& _tmpA94,Cyc_fprintf(Cyc_stderr,((_tmpA96="Reading from specs file %s\n",_tag_dyneither(_tmpA96,sizeof(char),28))),_tag_dyneither(_tmpA95,sizeof(void*),1)))))));}{
# 1413
struct Cyc_List_List*_tmp174=Cyc_read_specs(_tmp170);
# 1414
struct _dyneither_ptr _tmp175=_tag_dyneither(0,0,0);
# 1415
{struct _handler_cons _tmp176;_push_handler(& _tmp176);{int _tmp178=0;if(setjmp(_tmp176.handler))_tmp178=1;if(!_tmp178){
# 1416
{struct _dyneither_ptr _tmp179=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp174,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
# 1417
_tmp175=Cyc_split_specs(_tmp179);}
# 1416
;_pop_handler();}else{void*_tmp177=(void*)_exn_thrown;void*_tmp17C=_tmp177;_LL35: {struct Cyc_Core_Not_found_exn_struct*_tmp17D=(struct Cyc_Core_Not_found_exn_struct*)_tmp17C;if(_tmp17D->tag != Cyc_Core_Not_found)goto _LL37;}_LL36:
# 1419
 goto _LL34;_LL37:;_LL38:(void)_throw(_tmp17C);_LL34:;}};}
# 1421
if((struct _dyneither_ptr*)_tmp175.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
# 1422
Cyc_Arg_current=0;
# 1423
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp175);}
# 1426
Cyc_Arg_current=0;
# 1427
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp16A);{
# 1428
const char*_tmpA97;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpA97="",_tag_dyneither(_tmpA97,sizeof(char),1))))== 0){
# 1444 "cyclone.cyc"
const char*_tmp17F=Cyc_produce_dependencies?"":(const char*)" -E";
# 1445
const char*_tmpA9F;void*_tmpA9E[3];struct Cyc_String_pa_PrintArg_struct _tmpA9D;const char*_tmpA9C;struct Cyc_String_pa_PrintArg_struct _tmpA9B;struct Cyc_String_pa_PrintArg_struct _tmpA9A;Cyc_set_cpp((struct _dyneither_ptr)((_tmpA9A.tag=0,((_tmpA9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp170),((_tmpA9B.tag=0,((_tmpA9B.f1=(struct _dyneither_ptr)((_tmpA9C=_tmp17F,_tag_dyneither(_tmpA9C,sizeof(char),_get_zero_arr_size_char((void*)_tmpA9C,1)))),((_tmpA9D.tag=0,((_tmpA9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpA9E[0]=& _tmpA9D,((_tmpA9E[1]=& _tmpA9B,((_tmpA9E[2]=& _tmpA9A,Cyc_aprintf(((_tmpA9F="%s -w -x c%s -specs %s",_tag_dyneither(_tmpA9F,sizeof(char),23))),_tag_dyneither(_tmpA9E,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1448
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
# 1449
{const char*_tmpAA2;void*_tmpAA1;(_tmpAA1=0,Cyc_fprintf(Cyc_stderr,((_tmpAA2="missing file\n",_tag_dyneither(_tmpAA2,sizeof(char),14))),_tag_dyneither(_tmpAA1,sizeof(void*),0)));}
# 1450
exit(1);}
# 1457
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
# 1458
const char*_tmpAA3;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpAA3="cyc_setjmp.h",_tag_dyneither(_tmpAA3,sizeof(char),13))));}
# 1459
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
# 1460
const char*_tmpAA4;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAA4="cyc_include.h",_tag_dyneither(_tmpAA4,sizeof(char),14))));}
# 1463
{struct Cyc_List_List*_tmp362=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp362 != 0;_tmp362=_tmp362->tl){
# 1464
Cyc_process_file(*((struct _dyneither_ptr*)_tmp362->hd));
# 1465
if(Cyc_compile_failure){int _tmp363=1;_npop_handler(0);return _tmp363;}}}
# 1468
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp364=0;_npop_handler(0);return _tmp364;}
# 1474
if(Cyc_ccargs == 0){int _tmp365=0;_npop_handler(0);return _tmp365;}
# 1477
{const char*_tmpAA5;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAA5="-L",_tag_dyneither(_tmpAA5,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1479
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
# 1480
const char*_tmpAA6;struct _dyneither_ptr _tmp367=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAA6=" ",_tag_dyneither(_tmpAA6,sizeof(char),2))));
# 1481
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
# 1482
const char*_tmpAA8;struct Cyc_List_List*_tmpAA7;struct _dyneither_ptr _tmp368=Cyc_str_sepstr(((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAA7->tl=
# 1483
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAA7)))))),((_tmpAA8=" ",_tag_dyneither(_tmpAA8,sizeof(char),2))));
# 1485
struct Cyc_List_List*stdlib;
# 1486
struct _dyneither_ptr stdlib_string;
# 1487
const char*_tmpAAA;const char*_tmpAA9;int _tmp369=
# 1488
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1490
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAAA=".a",_tag_dyneither(_tmpAAA,sizeof(char),3))))) || 
# 1491
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAA9=".lib",_tag_dyneither(_tmpAA9,sizeof(char),5))));
# 1492
if(_tmp369){
# 1493
stdlib=0;{
# 1494
const char*_tmpAAB;stdlib_string=(struct _dyneither_ptr)((_tmpAAB="",_tag_dyneither(_tmpAAB,sizeof(char),1)));};}else{
# 1498
struct _dyneither_ptr libcyc_flag;
# 1499
struct _dyneither_ptr nogc_filename;
# 1500
struct _dyneither_ptr runtime_filename;
# 1501
struct _dyneither_ptr gc_filename;
# 1502
if(Cyc_pa_r){
# 1503
{const char*_tmpAAC;libcyc_flag=((_tmpAAC="-lcyc_a",_tag_dyneither(_tmpAAC,sizeof(char),8)));}
# 1504
{const char*_tmpAAD;nogc_filename=((_tmpAAD="nogc_a.a",_tag_dyneither(_tmpAAD,sizeof(char),9)));}{
# 1505
const char*_tmpAAE;runtime_filename=((_tmpAAE="runtime_cyc_a.o",_tag_dyneither(_tmpAAE,sizeof(char),16)));};}else{
# 1506
if(Cyc_nocheck_r){
# 1507
{const char*_tmpAAF;libcyc_flag=((_tmpAAF="-lcyc_nocheck",_tag_dyneither(_tmpAAF,sizeof(char),14)));}
# 1508
{const char*_tmpAB0;nogc_filename=((_tmpAB0="nogc.a",_tag_dyneither(_tmpAB0,sizeof(char),7)));}{
# 1509
const char*_tmpAB1;runtime_filename=((_tmpAB1="runtime_cyc.o",_tag_dyneither(_tmpAB1,sizeof(char),14)));};}else{
# 1510
if(Cyc_pg_r){
# 1511
{const char*_tmpAB2;libcyc_flag=((_tmpAB2="-lcyc_pg",_tag_dyneither(_tmpAB2,sizeof(char),9)));}
# 1512
{const char*_tmpAB3;runtime_filename=((_tmpAB3="runtime_cyc_pg.o",_tag_dyneither(_tmpAB3,sizeof(char),17)));}{
# 1513
const char*_tmpAB4;nogc_filename=((_tmpAB4="nogc_pg.a",_tag_dyneither(_tmpAB4,sizeof(char),10)));};}else{
# 1514
if(Cyc_Lex_compile_for_boot_r){
# 1516
{const char*_tmpAB5;libcyc_flag=((_tmpAB5="-lcycboot",_tag_dyneither(_tmpAB5,sizeof(char),10)));}
# 1517
{const char*_tmpAB6;nogc_filename=((_tmpAB6="nogc.a",_tag_dyneither(_tmpAB6,sizeof(char),7)));}{
# 1518
const char*_tmpAB7;runtime_filename=((_tmpAB7="runtime_cyc.o",_tag_dyneither(_tmpAB7,sizeof(char),14)));};}else{
# 1520
{const char*_tmpAB8;libcyc_flag=((_tmpAB8="-lcyc",_tag_dyneither(_tmpAB8,sizeof(char),6)));}
# 1521
{const char*_tmpAB9;nogc_filename=((_tmpAB9="nogc.a",_tag_dyneither(_tmpAB9,sizeof(char),7)));}{
# 1522
const char*_tmpABA;runtime_filename=((_tmpABA="runtime_cyc.o",_tag_dyneither(_tmpABA,sizeof(char),14)));};}}}}
# 1524
{const char*_tmpABB;gc_filename=((_tmpABB="gc.a",_tag_dyneither(_tmpABB,sizeof(char),5)));}{
# 1526
struct _dyneither_ptr _tmp37B=Cyc_nogc_r?
# 1527
Cyc_do_find(cyclone_arch_path,nogc_filename):
# 1528
 Cyc_do_find(cyclone_arch_path,gc_filename);
# 1529
struct _dyneither_ptr _tmp37C=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1533
stdlib=0;{
# 1534
const char*_tmpAC1;void*_tmpAC0[3];struct Cyc_String_pa_PrintArg_struct _tmpABF;struct Cyc_String_pa_PrintArg_struct _tmpABE;struct Cyc_String_pa_PrintArg_struct _tmpABD;stdlib_string=(struct _dyneither_ptr)((_tmpABD.tag=0,((_tmpABD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37B),((_tmpABE.tag=0,((_tmpABE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpABF.tag=0,((_tmpABF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp37C),((_tmpAC0[0]=& _tmpABF,((_tmpAC0[1]=& _tmpABE,((_tmpAC0[2]=& _tmpABD,Cyc_aprintf(((_tmpAC1=" %s %s %s",_tag_dyneither(_tmpAC1,sizeof(char),10))),_tag_dyneither(_tmpAC0,sizeof(void*),3))))))))))))))))))));};};}
# 1537
if(Cyc_ic_r){struct _handler_cons _tmp382;_push_handler(& _tmp382);{int _tmp384=0;if(setjmp(_tmp382.handler))_tmp384=1;if(!_tmp384){
# 1538
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
# 1539
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
# 1540
struct Cyc_List_List*_tmp385=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
# 1541
if(!_tmp369){
# 1542
struct Cyc_List_List*_tmpAC2;_tmp385=((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((_tmpAC2->hd=Cyc_final_strptr,((_tmpAC2->tl=_tmp385,_tmpAC2))))));}{
# 1545
struct Cyc_Interface_I*_tmp387=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp385,_tmp385);
# 1546
if(_tmp387 == 0){
# 1547
{const char*_tmpAC5;void*_tmpAC4;(_tmpAC4=0,Cyc_fprintf(Cyc_stderr,((_tmpAC5="Error: interfaces incompatible\n",_tag_dyneither(_tmpAC5,sizeof(char),32))),_tag_dyneither(_tmpAC4,sizeof(void*),0)));}
# 1548
Cyc_compile_failure=1;
# 1549
Cyc_remove_cfiles();{
# 1550
int _tmp38A=1;_npop_handler(1);return _tmp38A;};}
# 1552
if(_tmp369){
# 1553
if(Cyc_output_file != 0){
# 1554
const char*_tmpAC9;void*_tmpAC8[1];struct Cyc_String_pa_PrintArg_struct _tmpAC7;struct _dyneither_ptr _tmp38B=(_tmpAC7.tag=0,((_tmpAC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAC8[0]=& _tmpAC7,Cyc_aprintf(((_tmpAC9="%s.cycio",_tag_dyneither(_tmpAC9,sizeof(char),9))),_tag_dyneither(_tmpAC8,sizeof(void*),1)))))));
# 1555
const char*_tmpACB;const char*_tmpACA;struct Cyc___cycFILE*_tmp38C=Cyc_try_file_open((struct _dyneither_ptr)_tmp38B,((_tmpACA="wb",_tag_dyneither(_tmpACA,sizeof(char),3))),((_tmpACB="interface object file",_tag_dyneither(_tmpACB,sizeof(char),22))));
# 1556
if(_tmp38C == 0){
# 1557
Cyc_compile_failure=1;
# 1558
Cyc_remove_cfiles();{
# 1559
int _tmp38D=1;_npop_handler(1);return _tmp38D;};}
# 1561
Cyc_Interface_save((struct Cyc_Interface_I*)_tmp387,(struct Cyc___cycFILE*)_tmp38C);
# 1562
Cyc_file_close((struct Cyc___cycFILE*)_tmp38C);}}else{
# 1565
const char*_tmpAD0;const char*_tmpACF;struct _tuple21*_tmpACE;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct Cyc_Interface_I*)_tmp387,(
# 1566
(_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE->f1=((_tmpACF="empty interface",_tag_dyneither(_tmpACF,sizeof(char),16))),((_tmpACE->f2=((_tmpAD0="global interface",_tag_dyneither(_tmpAD0,sizeof(char),17))),_tmpACE)))))))){
# 1567
{const char*_tmpAD3;void*_tmpAD2;(_tmpAD2=0,Cyc_fprintf(Cyc_stderr,((_tmpAD3="Error: some objects are still undefined\n",_tag_dyneither(_tmpAD3,sizeof(char),41))),_tag_dyneither(_tmpAD2,sizeof(void*),0)));}
# 1568
Cyc_compile_failure=1;
# 1569
Cyc_remove_cfiles();{
# 1570
int _tmp398=1;_npop_handler(1);return _tmp398;};}}};};
# 1538
;_pop_handler();}else{void*_tmp383=(void*)_exn_thrown;void*_tmp39A=_tmp383;_LL3A:;_LL3B:
# 1574
{void*_tmp39B=_tmp39A;struct _dyneither_ptr _tmp39D;struct _dyneither_ptr _tmp39F;struct _dyneither_ptr _tmp3A2;_LL3F: {struct Cyc_Core_Failure_exn_struct*_tmp39C=(struct Cyc_Core_Failure_exn_struct*)_tmp39B;if(_tmp39C->tag != Cyc_Core_Failure)goto _LL41;else{_tmp39D=_tmp39C->f1;}}_LL40:
# 1576
{const char*_tmpAD7;void*_tmpAD6[1];struct Cyc_String_pa_PrintArg_struct _tmpAD5;(_tmpAD5.tag=0,((_tmpAD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39D),((_tmpAD6[0]=& _tmpAD5,Cyc_fprintf(Cyc_stderr,((_tmpAD7="Exception Core::Failure %s\n",_tag_dyneither(_tmpAD7,sizeof(char),28))),_tag_dyneither(_tmpAD6,sizeof(void*),1)))))));}goto _LL3E;_LL41: {struct Cyc_Core_Impossible_exn_struct*_tmp39E=(struct Cyc_Core_Impossible_exn_struct*)_tmp39B;if(_tmp39E->tag != Cyc_Core_Impossible)goto _LL43;else{_tmp39F=_tmp39E->f1;}}_LL42:
# 1578
{const char*_tmpADB;void*_tmpADA[1];struct Cyc_String_pa_PrintArg_struct _tmpAD9;(_tmpAD9.tag=0,((_tmpAD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39F),((_tmpADA[0]=& _tmpAD9,Cyc_fprintf(Cyc_stderr,((_tmpADB="Exception Core::Impossible %s\n",_tag_dyneither(_tmpADB,sizeof(char),31))),_tag_dyneither(_tmpADA,sizeof(void*),1)))))));}goto _LL3E;_LL43: {struct Cyc_Dict_Absent_exn_struct*_tmp3A0=(struct Cyc_Dict_Absent_exn_struct*)_tmp39B;if(_tmp3A0->tag != Cyc_Dict_Absent)goto _LL45;}_LL44:
# 1580
{const char*_tmpADE;void*_tmpADD;(_tmpADD=0,Cyc_fprintf(Cyc_stderr,((_tmpADE="Exception Dict::Absent\n",_tag_dyneither(_tmpADE,sizeof(char),24))),_tag_dyneither(_tmpADD,sizeof(void*),0)));}goto _LL3E;_LL45: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp3A1=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp39B;if(_tmp3A1->tag != Cyc_Core_Invalid_argument)goto _LL47;else{_tmp3A2=_tmp3A1->f1;}}_LL46:
# 1582
{const char*_tmpAE2;void*_tmpAE1[1];struct Cyc_String_pa_PrintArg_struct _tmpAE0;(_tmpAE0.tag=0,((_tmpAE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A2),((_tmpAE1[0]=& _tmpAE0,Cyc_fprintf(Cyc_stderr,((_tmpAE2="Exception Core::Invalid_argument %s\n",_tag_dyneither(_tmpAE2,sizeof(char),37))),_tag_dyneither(_tmpAE1,sizeof(void*),1)))))));}goto _LL3E;_LL47:;_LL48:
# 1584
{const char*_tmpAE5;void*_tmpAE4;(_tmpAE4=0,Cyc_fprintf(Cyc_stderr,((_tmpAE5="Uncaught exception\n",_tag_dyneither(_tmpAE5,sizeof(char),20))),_tag_dyneither(_tmpAE4,sizeof(void*),0)));}goto _LL3E;_LL3E:;}
# 1586
Cyc_compile_failure=1;
# 1587
Cyc_remove_cfiles();{
# 1588
int _tmp3B0=1;_npop_handler(0);return _tmp3B0;};_LL3C:;_LL3D:(void)_throw(_tmp39A);_LL39:;}};}{
# 1591
const char*_tmpAE6;struct _dyneither_ptr outfile_str=(_tmpAE6="",_tag_dyneither(_tmpAE6,sizeof(char),1));
# 1592
if(Cyc_output_file != 0){
# 1593
const char*_tmpAEA;void*_tmpAE9[1];struct Cyc_String_pa_PrintArg_struct _tmpAE8;outfile_str=(struct _dyneither_ptr)((_tmpAE8.tag=0,((_tmpAE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAE9[0]=& _tmpAE8,Cyc_aprintf(((_tmpAEA=" -o %s",_tag_dyneither(_tmpAEA,sizeof(char),7))),_tag_dyneither(_tmpAE9,sizeof(void*),1))))))));}{
# 1595
const char*_tmpAF2;void*_tmpAF1[5];struct Cyc_String_pa_PrintArg_struct _tmpAF0;struct Cyc_String_pa_PrintArg_struct _tmpAEF;struct Cyc_String_pa_PrintArg_struct _tmpAEE;struct Cyc_String_pa_PrintArg_struct _tmpAED;struct Cyc_String_pa_PrintArg_struct _tmpAEC;struct _dyneither_ptr _tmp3B4=
# 1596
(_tmpAEC.tag=0,((_tmpAEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp368),((_tmpAED.tag=0,((_tmpAED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpAEE.tag=0,((_tmpAEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp367),((_tmpAEF.tag=0,((_tmpAEF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpAF0.tag=0,((_tmpAF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpAF1[0]=& _tmpAF0,((_tmpAF1[1]=& _tmpAEF,((_tmpAF1[2]=& _tmpAEE,((_tmpAF1[3]=& _tmpAED,((_tmpAF1[4]=& _tmpAEC,Cyc_aprintf(((_tmpAF2="%s %s %s%s%s",_tag_dyneither(_tmpAF2,sizeof(char),13))),_tag_dyneither(_tmpAF1,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1598
if(Cyc_v_r){{const char*_tmpAF6;void*_tmpAF5[1];struct Cyc_String_pa_PrintArg_struct _tmpAF4;(_tmpAF4.tag=0,((_tmpAF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B4),((_tmpAF5[0]=& _tmpAF4,Cyc_fprintf(Cyc_stderr,((_tmpAF6="%s\n",_tag_dyneither(_tmpAF6,sizeof(char),4))),_tag_dyneither(_tmpAF5,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
# 1599
if(system((const char*)_untag_dyneither_ptr(_tmp3B4,sizeof(char),1))!= 0){
# 1600
{const char*_tmpAF9;void*_tmpAF8;(_tmpAF8=0,Cyc_fprintf(Cyc_stderr,((_tmpAF9="Error: C compiler failed\n",_tag_dyneither(_tmpAF9,sizeof(char),26))),_tag_dyneither(_tmpAF8,sizeof(void*),0)));}
# 1601
Cyc_compile_failure=1;
# 1602
Cyc_remove_cfiles();{
# 1603
int _tmp3BA=1;_npop_handler(0);return _tmp3BA;};}
# 1605
Cyc_remove_cfiles();{
# 1607
int _tmp3BB=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3BB;};};};};};
# 1118 "cyclone.cyc"
;_pop_region(r);};}
