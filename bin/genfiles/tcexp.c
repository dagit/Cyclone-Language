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
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244 "core.h"
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 230
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 234
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 249
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 259
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
# 100
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
# 101
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 431
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
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
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 913 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 922
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 927
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
# 928
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 931
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 945
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 947
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 957
void*Cyc_Absyn_exn_typ();
# 968
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 976
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 979
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1067
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1141
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1143
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
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
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 100
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 123 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
# 124
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 125
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 133
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 134
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 137
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 138
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 159
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 160
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 161
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 166
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 178
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
# 179
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 192
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 196
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 200
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 207
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 53
void*Cyc_Tcutil_copy_type(void*t);
# 55
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(struct Cyc_Absyn_Exp*);
# 58
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 62
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 63
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 65
void*Cyc_Tcutil_compress(void*t);
# 66
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 67
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
# 68
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 69
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 70
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
# 71
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 72
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 74
int Cyc_Tcutil_coerceable(void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 81
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 89
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 90
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 92
int Cyc_Tcutil_is_pointer_type(void*t);
# 93
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 94
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
# 95
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 96
void*Cyc_Tcutil_pointer_region(void*t);
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 116
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 142
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 144
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 148
void Cyc_Tcutil_explain_failure();
# 150
int Cyc_Tcutil_unify(void*,void*);
# 155
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 164
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 169
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 178
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 214 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 224
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 226
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 228
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 232
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 238
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 240
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 242
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 252
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 260
int Cyc_Tcutil_is_noalias_pointer(void*t);
# 265
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 270
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 279
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 335
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 338
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 33
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
# 40
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 49 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 60
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 62
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 67
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple16{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple16 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessEqConst{int tag;unsigned int val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;struct _union_RelnOp_LessEqConst LessEqConst;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple17{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple17 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 158 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 177
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 256 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp6DC;(_tmp6DC=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6DC,sizeof(void*),0)));}
# 57
if(topt == 0)
# 58
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple18*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
# 72
struct _tuple18*_tmp6DD;return(_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD->f1=0,((_tmp6DD->f2=e,_tmp6DD)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
# 77
void*_tmp2=e1->r;struct _tuple1*_tmp4;void*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2;if(_tmp3->tag != 1)goto _LL3;else{_tmp4=_tmp3->f1;_tmp5=(void**)& _tmp3->f2;_tmp6=(void**)((void**)& _tmp3->f2);}}_LL2: {
# 79
struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(_tmp7.handler))_tmp9=1;if(!_tmp9){
# 80
{struct _RegionHandle _tmpA=_new_region("r");struct _RegionHandle*r=& _tmpA;_push_region(r);
# 81
{void*_tmpB=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp4,1);void*_tmpD;void*_tmp10;struct Cyc_Absyn_Datatypedecl*_tmp12;struct Cyc_Absyn_Datatypefield*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp15;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpC=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB;if(_tmpC->tag != 0)goto _LL8;else{_tmpD=(void*)_tmpC->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD;if(_tmpE->tag != 0)goto _LL8;};}}_LL7:
# 83
{const char*_tmp6E1;void*_tmp6E0[1];struct Cyc_String_pa_PrintArg_struct _tmp6DF;(_tmp6DF.tag=0,((_tmp6DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6E0[0]=& _tmp6DF,Cyc_Tcutil_terr(e1->loc,((_tmp6E1="undeclared identifier %s",_tag_dyneither(_tmp6E1,sizeof(char),25))),_tag_dyneither(_tmp6E0,sizeof(void*),1)))))));}{
# 84
int _tmp1B=0;_npop_handler(1);return _tmp1B;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB;if(_tmpF->tag != 0)goto _LLA;else{_tmp10=(void*)_tmpF->f1;}}_LL9:
# 86
*_tmp6=_tmp10;{
# 87
int _tmp1C=1;_npop_handler(1);return _tmp1C;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp11=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp11->tag != 2)goto _LLC;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LLB:
# 89
 if(_tmp13->typs == 0){
# 91
{const char*_tmp6E5;void*_tmp6E4[1];struct Cyc_String_pa_PrintArg_struct _tmp6E3;(_tmp6E3.tag=0,((_tmp6E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 92
Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6E4[0]=& _tmp6E3,Cyc_Tcutil_terr(e->loc,((_tmp6E5="%s is a constant, not a function",_tag_dyneither(_tmp6E5,sizeof(char),33))),_tag_dyneither(_tmp6E4,sizeof(void*),1)))))));}{
# 93
int _tmp20=0;_npop_handler(1);return _tmp20;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6E8;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6E7;e->r=(void*)((_tmp6E7=_cycalloc(sizeof(*_tmp6E7)),((_tmp6E7[0]=((_tmp6E8.tag=29,((_tmp6E8.f1=es,((_tmp6E8.f2=_tmp12,((_tmp6E8.f3=_tmp13,_tmp6E8)))))))),_tmp6E7))));}{
# 96
int _tmp23=1;_npop_handler(1);return _tmp23;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp14->tag != 1)goto _LLE;else{_tmp15=_tmp14->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp24=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
# 99
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6EB;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6EA;e->r=(void*)((_tmp6EA=_cycalloc(sizeof(*_tmp6EA)),((_tmp6EA[0]=((_tmp6EB.tag=27,((_tmp6EB.f1=_tmp15->name,((_tmp6EB.f2=0,((_tmp6EB.f3=_tmp24,((_tmp6EB.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp15,_tmp6EB)))))))))),_tmp6EA))));}{
# 100
int _tmp27=1;_npop_handler(1);return _tmp27;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp16=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp16->tag != 4)goto _LL10;}_LLF:
# 101
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp17=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB;if(_tmp17->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp6EF;void*_tmp6EE[1];struct Cyc_String_pa_PrintArg_struct _tmp6ED;(_tmp6ED.tag=0,((_tmp6ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6EE[0]=& _tmp6ED,Cyc_Tcutil_terr(e->loc,((_tmp6EF="%s is an enum constructor, not a function",_tag_dyneither(_tmp6EF,sizeof(char),42))),_tag_dyneither(_tmp6EE,sizeof(void*),1)))))));}{
# 104
int _tmp2B=0;_npop_handler(1);return _tmp2B;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp8=(void*)_exn_thrown;void*_tmp2D=_tmp8;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6F3;void*_tmp6F2[1];struct Cyc_String_pa_PrintArg_struct _tmp6F1;(_tmp6F1.tag=0,((_tmp6F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4)),((_tmp6F2[0]=& _tmp6F1,Cyc_Tcutil_terr(e1->loc,((_tmp6F3="undeclared identifier %s",_tag_dyneither(_tmp6F3,sizeof(char),25))),_tag_dyneither(_tmp6F2,sizeof(void*),1)))))));}
# 109
return 0;_LL15:;_LL16:(void)_throw(_tmp2D);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6F6;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6F5;e->r=(void*)((_tmp6F5=_cycalloc(sizeof(*_tmp6F5)),((_tmp6F5[0]=((_tmp6F6.tag=25,((_tmp6F6.f1=des,_tmp6F6)))),_tmp6F5))));}
# 123
return;}{
# 125
void*t=*topt;
# 126
void*_tmp34=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp36;union Cyc_Absyn_AggrInfoU _tmp37;struct Cyc_Absyn_ArrayInfo _tmp39;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp35=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp34;if(_tmp35->tag != 11)goto _LL1A;else{_tmp36=_tmp35->f1;_tmp37=_tmp36.aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3D=_tmp37;struct _tuple3 _tmp3E;struct Cyc_Absyn_Aggrdecl**_tmp3F;struct Cyc_Absyn_Aggrdecl*_tmp40;_LL21: if((_tmp3D.UnknownAggr).tag != 1)goto _LL23;_tmp3E=(struct _tuple3)(_tmp3D.UnknownAggr).val;_LL22: {
# 129
const char*_tmp6F9;void*_tmp6F8;(_tmp6F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6F9="struct type not properly set",_tag_dyneither(_tmp6F9,sizeof(char),29))),_tag_dyneither(_tmp6F8,sizeof(void*),0)));}_LL23: if((_tmp3D.KnownAggr).tag != 2)goto _LL20;_tmp3F=(struct Cyc_Absyn_Aggrdecl**)(_tmp3D.KnownAggr).val;_tmp40=*_tmp3F;_LL24: {
# 130
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6FC;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6FB;e->r=(void*)((_tmp6FB=_cycalloc(sizeof(*_tmp6FB)),((_tmp6FB[0]=((_tmp6FC.tag=27,((_tmp6FC.f1=_tmp40->name,((_tmp6FC.f2=0,((_tmp6FC.f3=des,((_tmp6FC.f4=(struct Cyc_Absyn_Aggrdecl*)_tmp40,_tmp6FC)))))))))),_tmp6FB))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34;if(_tmp38->tag != 8)goto _LL1C;else{_tmp39=_tmp38->f1;_tmp3A=_tmp39.elt_type;_tmp3B=_tmp39.tq;}}_LL1B:
# 133
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6FF;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6FE;e->r=(void*)((_tmp6FE=_cycalloc(sizeof(*_tmp6FE)),((_tmp6FE[0]=((_tmp6FF.tag=25,((_tmp6FF.f1=des,_tmp6FF)))),_tmp6FE))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp34;if(_tmp3C->tag != 12)goto _LL1E;}_LL1D:
# 134
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp702;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp701;e->r=(void*)((_tmp701=_cycalloc(sizeof(*_tmp701)),((_tmp701[0]=((_tmp702.tag=28,((_tmp702.f1=(void*)t,((_tmp702.f2=des,_tmp702)))))),_tmp701))));}goto _LL17;_LL1E:;_LL1F:
# 135
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp705;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp704;e->r=(void*)((_tmp704=_cycalloc(sizeof(*_tmp704)),((_tmp704[0]=((_tmp705.tag=25,((_tmp705.f1=des,_tmp705)))),_tmp704))));}goto _LL17;_LL17:;};}
# 142
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 143
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 146
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
# 147
for(0;es != 0;es=es->tl){
# 148
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 152
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
# 153
Cyc_Tcutil_check_contains_assign(e);
# 154
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);
# 155
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
# 156
const char*_tmp70A;void*_tmp709[2];struct Cyc_String_pa_PrintArg_struct _tmp708;struct Cyc_String_pa_PrintArg_struct _tmp707;(_tmp707.tag=0,((_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 157
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp709[0]=& _tmp708,((_tmp709[1]=& _tmp707,Cyc_Tcutil_terr(e->loc,((_tmp70A="test of %s has type %s instead of integral or * type",_tag_dyneither(_tmp70A,sizeof(char),53))),_tag_dyneither(_tmp709,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4F=e->r;enum Cyc_Absyn_Primop _tmp51;struct Cyc_List_List*_tmp52;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp50=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4F;if(_tmp50->tag != 2)goto _LL28;else{_tmp51=_tmp50->f1;_tmp52=_tmp50->f2;}}_LL27:
# 163
 if(_tmp51 == Cyc_Absyn_Eq  || _tmp51 == Cyc_Absyn_Neq){
# 164
struct _tuple0 _tmp70B;struct _tuple0 _tmp54=(_tmp70B.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp52))->hd)->topt),((_tmp70B.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp52))->tl))->hd)->topt),_tmp70B)));void*_tmp55;void*_tmp57;void*_tmp58;void*_tmp5A;_LL2B: _tmp55=_tmp54.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp55;if(_tmp56->tag != 15)goto _LL2D;else{_tmp57=(void*)_tmp56->f1;}};_tmp58=_tmp54.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp59=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp58;if(_tmp59->tag != 15)goto _LL2D;else{_tmp5A=(void*)_tmp59->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp70E;struct Cyc_Tcexp_TestEnv _tmp70D;return(_tmp70D.eq=((_tmp70E=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp70E)),((_tmp70E->f1=_tmp57,((_tmp70E->f2=_tmp5A,_tmp70E)))))),((_tmp70D.isTrue=_tmp51 == Cyc_Absyn_Eq,_tmp70D)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
# 172
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp70F;return(_tmp70F.eq=0,((_tmp70F.isTrue=0,_tmp70F)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
# 196
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
# 201
void*t;
# 202
void*string_elt_typ=Cyc_Absyn_char_typ;
# 203
int string_numelts=0;
# 204
{union Cyc_Absyn_Cnst _tmp5E=*((union Cyc_Absyn_Cnst*)_check_null(c));struct _tuple4 _tmp5F;enum Cyc_Absyn_Sign _tmp60;struct _tuple4 _tmp61;enum Cyc_Absyn_Sign _tmp62;struct _tuple4 _tmp63;enum Cyc_Absyn_Sign _tmp64;struct _dyneither_ptr _tmp65;struct _tuple5 _tmp66;enum Cyc_Absyn_Sign _tmp67;struct _tuple7 _tmp68;enum Cyc_Absyn_Sign _tmp69;struct _tuple8 _tmp6A;int _tmp6B;struct _tuple6 _tmp6C;enum Cyc_Absyn_Sign _tmp6D;int _tmp6E;struct _dyneither_ptr _tmp6F;struct _dyneither_ptr _tmp70;int _tmp71;_LL30: if((_tmp5E.Char_c).tag != 2)goto _LL32;_tmp5F=(struct _tuple4)(_tmp5E.Char_c).val;_tmp60=_tmp5F.f1;if(_tmp60 != Cyc_Absyn_Signed)goto _LL32;_LL31:
# 205
 t=Cyc_Absyn_schar_typ;goto _LL2F;_LL32: if((_tmp5E.Char_c).tag != 2)goto _LL34;_tmp61=(struct _tuple4)(_tmp5E.Char_c).val;_tmp62=_tmp61.f1;if(_tmp62 != Cyc_Absyn_Unsigned)goto _LL34;_LL33:
# 206
 t=Cyc_Absyn_uchar_typ;goto _LL2F;_LL34: if((_tmp5E.Char_c).tag != 2)goto _LL36;_tmp63=(struct _tuple4)(_tmp5E.Char_c).val;_tmp64=_tmp63.f1;if(_tmp64 != Cyc_Absyn_None)goto _LL36;_LL35:
# 207
 t=Cyc_Absyn_char_typ;goto _LL2F;_LL36: if((_tmp5E.Wchar_c).tag != 3)goto _LL38;_tmp65=(struct _dyneither_ptr)(_tmp5E.Wchar_c).val;_LL37:
# 208
 t=Cyc_Absyn_wchar_typ();goto _LL2F;_LL38: if((_tmp5E.Short_c).tag != 4)goto _LL3A;_tmp66=(struct _tuple5)(_tmp5E.Short_c).val;_tmp67=_tmp66.f1;_LL39:
# 210
 t=_tmp67 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;_LL3A: if((_tmp5E.LongLong_c).tag != 6)goto _LL3C;_tmp68=(struct _tuple7)(_tmp5E.LongLong_c).val;_tmp69=_tmp68.f1;_LL3B:
# 212
 t=_tmp69 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;_LL3C: if((_tmp5E.Float_c).tag != 7)goto _LL3E;_tmp6A=(struct _tuple8)(_tmp5E.Float_c).val;_tmp6B=_tmp6A.f2;_LL3D:
# 213
 t=Cyc_Absyn_float_typ(_tmp6B);goto _LL2F;_LL3E: if((_tmp5E.Int_c).tag != 5)goto _LL40;_tmp6C=(struct _tuple6)(_tmp5E.Int_c).val;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;_LL3F:
# 215
 if(topt == 0)
# 216
t=_tmp6D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp72=Cyc_Tcutil_compress(*topt);enum Cyc_Absyn_Sign _tmp74;enum Cyc_Absyn_Size_of _tmp75;enum Cyc_Absyn_Sign _tmp77;enum Cyc_Absyn_Size_of _tmp78;enum Cyc_Absyn_Sign _tmp7A;enum Cyc_Absyn_Size_of _tmp7B;enum Cyc_Absyn_Sign _tmp7D;enum Cyc_Absyn_Size_of _tmp7E;struct Cyc_Absyn_PtrInfo _tmp80;void*_tmp81;struct Cyc_Absyn_Tqual _tmp82;struct Cyc_Absyn_PtrAtts _tmp83;void*_tmp84;union Cyc_Absyn_Constraint*_tmp85;union Cyc_Absyn_Constraint*_tmp86;union Cyc_Absyn_Constraint*_tmp87;void*_tmp89;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72;if(_tmp73->tag != 6)goto _LL49;else{_tmp74=_tmp73->f1;_tmp75=_tmp73->f2;if(_tmp75 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
# 224
 switch(_tmp74){case Cyc_Absyn_Unsigned: _LL55:
# 225
 t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL56:
# 226
 t=Cyc_Absyn_schar_typ;break;case Cyc_Absyn_None: _LL57:
# 227
 t=Cyc_Absyn_char_typ;break;}
# 229
*c=Cyc_Absyn_Char_c(_tmp74,(char)_tmp6E);
# 230
goto _LL46;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72;if(_tmp76->tag != 6)goto _LL4B;else{_tmp77=_tmp76->f1;_tmp78=_tmp76->f2;if(_tmp78 != Cyc_Absyn_Short_sz)goto _LL4B;}}_LL4A:
# 232
 t=_tmp77 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
# 233
*c=Cyc_Absyn_Short_c(_tmp77,(short)_tmp6E);
# 234
goto _LL46;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72;if(_tmp79->tag != 6)goto _LL4D;else{_tmp7A=_tmp79->f1;_tmp7B=_tmp79->f2;if(_tmp7B != Cyc_Absyn_Int_sz)goto _LL4D;}}_LL4C:
# 236
 t=_tmp7A == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 237
goto _LL46;_LL4D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp72;if(_tmp7C->tag != 6)goto _LL4F;else{_tmp7D=_tmp7C->f1;_tmp7E=_tmp7C->f2;if(_tmp7E != Cyc_Absyn_Long_sz)goto _LL4F;}}_LL4E:
# 239
 t=_tmp7D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 240
goto _LL46;_LL4F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp7F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72;if(_tmp7F->tag != 5)goto _LL51;else{_tmp80=_tmp7F->f1;_tmp81=_tmp80.elt_typ;_tmp82=_tmp80.elt_tq;_tmp83=_tmp80.ptr_atts;_tmp84=_tmp83.rgn;_tmp85=_tmp83.nullable;_tmp86=_tmp83.bounds;_tmp87=_tmp83.zero_term;}}if(!(_tmp6E == 0))goto _LL51;_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
# 244
e->r=(void*)& nullc;
# 245
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp85))return*topt;{
# 246
struct Cyc_List_List*_tmp8A=Cyc_Tcenv_lookup_type_vars(te);
# 247
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp719;struct Cyc_Absyn_PtrAtts _tmp718;struct Cyc_Absyn_PtrInfo _tmp717;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp716;t=(void*)((_tmp716=_cycalloc(sizeof(*_tmp716)),((_tmp716[0]=((_tmp719.tag=5,((_tmp719.f1=((_tmp717.elt_typ=_tmp81,((_tmp717.elt_tq=_tmp82,((_tmp717.ptr_atts=(
# 248
(_tmp718.rgn=_tmp84,((_tmp718.nullable=Cyc_Absyn_true_conref,((_tmp718.bounds=_tmp86,((_tmp718.zero_term=_tmp87,((_tmp718.ptrloc=0,_tmp718)))))))))),_tmp717)))))),_tmp719)))),_tmp716))));}
# 249
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp72;if(_tmp88->tag != 19)goto _LL53;else{_tmp89=(void*)_tmp88->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp71C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp71B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp90=(_tmp71B=_cycalloc(sizeof(*_tmp71B)),((_tmp71B[0]=((_tmp71C.tag=18,((_tmp71C.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp6E,0),_tmp71C)))),_tmp71B)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp71F;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp71E;t=(void*)((_tmp71E=_cycalloc(sizeof(*_tmp71E)),((_tmp71E[0]=((_tmp71F.tag=19,((_tmp71F.f1=(void*)((void*)_tmp90),_tmp71F)))),_tmp71E))));}
# 259
goto _LL46;}_LL53:;_LL54:
# 261
 t=_tmp6D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 262
goto _LL46;_LL46:;}
# 264
goto _LL2F;_LL40: if((_tmp5E.String_c).tag != 8)goto _LL42;_tmp6F=(struct _dyneither_ptr)(_tmp5E.String_c).val;_LL41:
# 266
 string_numelts=(int)_get_dyneither_size(_tmp6F,sizeof(char));
# 267
_tmp70=_tmp6F;goto _LL43;_LL42: if((_tmp5E.Wstring_c).tag != 9)goto _LL44;_tmp70=(struct _dyneither_ptr)(_tmp5E.Wstring_c).val;_LL43:
# 269
 if(string_numelts == 0){
# 270
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp70);
# 271
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 273
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
# 274
elen->topt=(void*)Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp722;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp721;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp95=(_tmp721=_cycalloc(sizeof(*_tmp721)),((_tmp721[0]=((_tmp722.tag=1,((_tmp722.f1=elen,_tmp722)))),_tmp721)));
# 279
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp95,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
# 282
void*_tmp96=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp98;struct Cyc_Absyn_Tqual _tmp99;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96;if(_tmp97->tag != 8)goto _LL5C;else{_tmp98=_tmp97->f1;_tmp99=_tmp98.tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp99,(struct Cyc_Absyn_Exp*)elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp96;if(_tmp9A->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
# 289
e->topt=(void*)t;
# 290
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 291
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
# 295
Cyc_Absyn_const_tqual(0),(void*)_tmp95,Cyc_Absyn_true_conref);
# 296
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
# 297
e->topt=(void*)t;
# 298
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 299
t=*topt;}}
# 302
goto _LL59;_LL5E:;_LL5F:
# 303
 goto _LL59;_LL59:;}
# 306
return t;};};_LL44: if((_tmp5E.Null_c).tag != 1)goto _LL2F;_tmp71=(int)(_tmp5E.Null_c).val;_LL45:
# 308
 if(topt != 0){
# 309
void*_tmp9D=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp9F;void*_tmpA0;struct Cyc_Absyn_Tqual _tmpA1;struct Cyc_Absyn_PtrAtts _tmpA2;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA4;union Cyc_Absyn_Constraint*_tmpA5;union Cyc_Absyn_Constraint*_tmpA6;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D;if(_tmp9E->tag != 5)goto _LL63;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.elt_typ;_tmpA1=_tmp9F.elt_tq;_tmpA2=_tmp9F.ptr_atts;_tmpA3=_tmpA2.rgn;_tmpA4=_tmpA2.nullable;_tmpA5=_tmpA2.bounds;_tmpA6=_tmpA2.zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpA4))return*topt;{
# 313
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp72C;struct Cyc_Absyn_PtrAtts _tmp72B;struct Cyc_Absyn_PtrInfo _tmp72A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp729;return(void*)((_tmp729=_cycalloc(sizeof(*_tmp729)),((_tmp729[0]=((_tmp72C.tag=5,((_tmp72C.f1=((_tmp72A.elt_typ=_tmpA0,((_tmp72A.elt_tq=_tmpA1,((_tmp72A.ptr_atts=((_tmp72B.rgn=_tmpA3,((_tmp72B.nullable=Cyc_Absyn_true_conref,((_tmp72B.bounds=_tmpA5,((_tmp72B.zero_term=_tmpA6,((_tmp72B.ptrloc=0,_tmp72B)))))))))),_tmp72A)))))),_tmp72C)))),_tmp729))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmpAB=Cyc_Tcenv_lookup_type_vars(te);
# 319
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp73F;struct Cyc_Absyn_PtrAtts _tmp73E;struct Cyc_Core_Opt*_tmp73D;struct Cyc_Core_Opt*_tmp73C;struct Cyc_Absyn_PtrInfo _tmp73B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp73A;t=(void*)((_tmp73A=_cycalloc(sizeof(*_tmp73A)),((_tmp73A[0]=((_tmp73F.tag=5,((_tmp73F.f1=((_tmp73B.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tako,((_tmp73C=_cycalloc(sizeof(*_tmp73C)),((_tmp73C->v=_tmpAB,_tmp73C))))),((_tmp73B.elt_tq=
# 320
Cyc_Absyn_empty_tqual(0),((_tmp73B.ptr_atts=(
# 321
(_tmp73E.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp73D=_cycalloc(sizeof(*_tmp73D)),((_tmp73D->v=_tmpAB,_tmp73D))))),((_tmp73E.nullable=Cyc_Absyn_true_conref,((_tmp73E.bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp73E.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp73E.ptrloc=0,_tmp73E)))))))))),_tmp73B)))))),_tmp73F)))),_tmp73A))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmpB2=*((void**)_check_null(b));struct Cyc_Absyn_Vardecl*_tmpB5;struct Cyc_Absyn_Fndecl*_tmpB7;struct Cyc_Absyn_Vardecl*_tmpB9;struct Cyc_Absyn_Vardecl*_tmpBB;struct Cyc_Absyn_Vardecl*_tmpBD;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpB3=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpB2;if(_tmpB3->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmpBE;_push_handler(& _tmpBE);{int _tmpC0=0;if(setjmp(_tmpBE.handler))_tmpC0=1;if(!_tmpC0){
# 343
{struct _RegionHandle _tmpC1=_new_region("r");struct _RegionHandle*r=& _tmpC1;_push_region(r);
# 344
{void*_tmpC2=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpC4;void*_tmpC7;struct Cyc_Absyn_Enumdecl*_tmpC9;struct Cyc_Absyn_Enumfield*_tmpCA;void*_tmpCC;struct Cyc_Absyn_Enumfield*_tmpCD;struct Cyc_Absyn_Datatypedecl*_tmpCF;struct Cyc_Absyn_Datatypefield*_tmpD0;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpC3=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC2;if(_tmpC3->tag != 0)goto _LL75;else{_tmpC4=(void*)_tmpC3->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC5=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC4;if(_tmpC5->tag != 0)goto _LL75;};}}_LL74: {
# 346
const char*_tmp743;void*_tmp742[1];struct Cyc_String_pa_PrintArg_struct _tmp741;void*_tmpD5=(_tmp741.tag=0,((_tmp741.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp742[0]=& _tmp741,Cyc_Tcexp_expr_err(te,loc,0,((_tmp743="undeclared identifier %s",_tag_dyneither(_tmp743,sizeof(char),25))),_tag_dyneither(_tmp742,sizeof(void*),1)))))));_npop_handler(1);return _tmpD5;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpC6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC2;if(_tmpC6->tag != 0)goto _LL77;else{_tmpC7=(void*)_tmpC6->f1;}}_LL76:
# 348
*b=_tmpC7;{
# 349
void*_tmpD6=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpD6;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpC8=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpC2;if(_tmpC8->tag != 3)goto _LL79;else{_tmpC9=_tmpC8->f1;_tmpCA=_tmpC8->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp746;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp745;e->r=(void*)((_tmp745=_cycalloc(sizeof(*_tmp745)),((_tmp745[0]=((_tmp746.tag=30,((_tmp746.f1=_tmpCA->name,((_tmp746.f2=(struct Cyc_Absyn_Enumdecl*)_tmpC9,((_tmp746.f3=(struct Cyc_Absyn_Enumfield*)_tmpCA,_tmp746)))))))),_tmp745))));}{
# 352
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp749;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp748;void*_tmpDB=(void*)((_tmp748=_cycalloc(sizeof(*_tmp748)),((_tmp748[0]=((_tmp749.tag=13,((_tmp749.f1=_tmpC9->name,((_tmp749.f2=(struct Cyc_Absyn_Enumdecl*)_tmpC9,_tmp749)))))),_tmp748))));_npop_handler(1);return _tmpDB;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpCB=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpC2;if(_tmpCB->tag != 4)goto _LL7B;else{_tmpCC=(void*)_tmpCB->f1;_tmpCD=_tmpCB->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp74C;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp74B;e->r=(void*)((_tmp74B=_cycalloc(sizeof(*_tmp74B)),((_tmp74B[0]=((_tmp74C.tag=31,((_tmp74C.f1=_tmpCD->name,((_tmp74C.f2=(void*)_tmpCC,((_tmp74C.f3=(struct Cyc_Absyn_Enumfield*)_tmpCD,_tmp74C)))))))),_tmp74B))));}{
# 355
void*_tmpDE=_tmpCC;_npop_handler(1);return _tmpDE;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpCE=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpC2;if(_tmpCE->tag != 2)goto _LL7D;else{_tmpCF=_tmpCE->f1;_tmpD0=_tmpCE->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp74F;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp74E;e->r=(void*)((_tmp74E=_cycalloc(sizeof(*_tmp74E)),((_tmp74E[0]=((_tmp74F.tag=29,((_tmp74F.f1=0,((_tmp74F.f2=_tmpCF,((_tmp74F.f3=_tmpD0,_tmp74F)))))))),_tmp74E))));}{
# 358
void*_tmpE1=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpCF,_tmpD0);_npop_handler(1);return _tmpE1;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpD1=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpC2;if(_tmpD1->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp753;void*_tmp752[1];struct Cyc_String_pa_PrintArg_struct _tmp751;void*_tmpE5=(_tmp751.tag=0,((_tmp751.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp752[0]=& _tmp751,Cyc_Tcexp_expr_err(te,loc,0,((_tmp753="bad occurrence of type name %s",_tag_dyneither(_tmp753,sizeof(char),31))),_tag_dyneither(_tmp752,sizeof(void*),1)))))));_npop_handler(1);return _tmpE5;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpBF=(void*)_exn_thrown;void*_tmpE7=_tmpBF;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpE8=(struct Cyc_Dict_Absent_exn_struct*)_tmpE7;if(_tmpE8->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp757;void*_tmp756[1];struct Cyc_String_pa_PrintArg_struct _tmp755;return(_tmp755.tag=0,((_tmp755.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp756[0]=& _tmp755,Cyc_Tcexp_expr_err(te,loc,0,((_tmp757="undeclared identifier %s",_tag_dyneither(_tmp757,sizeof(char),25))),_tag_dyneither(_tmp756,sizeof(void*),1)))))));}_LL82:;_LL83:(void)_throw(_tmpE7);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpB4=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB2;if(_tmpB4->tag != 1)goto _LL6A;else{_tmpB5=_tmpB4->f1;}}_LL69:
# 368
*q=*_tmpB5->name;
# 369
return _tmpB5->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB6=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpB2;if(_tmpB6->tag != 2)goto _LL6C;else{_tmpB7=_tmpB6->f1;}}_LL6B:
# 370
*q=*_tmpB7->name;return Cyc_Tcutil_fndecl2typ(_tmpB7);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpB8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB2;if(_tmpB8->tag != 5)goto _LL6E;else{_tmpB9=_tmpB8->f1;}}_LL6D:
# 371
 _tmpBB=_tmpB9;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBA=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB2;if(_tmpBA->tag != 4)goto _LL70;else{_tmpBB=_tmpBA->f1;}}_LL6F:
# 372
 _tmpBD=_tmpBB;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpBC=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB2;if(_tmpBC->tag != 3)goto _LL65;else{_tmpBD=_tmpBC->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp758;(*q).f1=(((_tmp758.Loc_n).val=0,(((_tmp758.Loc_n).tag=4,_tmp758))));}
# 375
if(te->allow_valueof){
# 376
void*_tmpED=Cyc_Tcutil_compress(_tmpBD->type);void*_tmpEF;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpEE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpED;if(_tmpEE->tag != 19)goto _LL87;else{_tmpEF=(void*)_tmpEE->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp75B;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp75A;e->r=(void*)((_tmp75A=_cycalloc(sizeof(*_tmp75A)),((_tmp75A[0]=((_tmp75B.tag=37,((_tmp75B.f1=(void*)_tmpEF,_tmp75B)))),_tmp75A))));}
# 379
goto _LL84;_LL87:;_LL88:
# 380
 goto _LL84;_LL84:;}
# 383
return _tmpBD->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
# 397
{void*_tmpF2=fmt->r;union Cyc_Absyn_Cnst _tmpF4;struct _dyneither_ptr _tmpF5;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp _tmpF8;void*_tmpF9;union Cyc_Absyn_Cnst _tmpFB;struct _dyneither_ptr _tmpFC;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF2;if(_tmpF3->tag != 0)goto _LL8C;else{_tmpF4=_tmpF3->f1;if((_tmpF4.String_c).tag != 8)goto _LL8C;_tmpF5=(struct _dyneither_ptr)(_tmpF4.String_c).val;}}_LL8B:
# 398
 _tmpFC=_tmpF5;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpF6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF2;if(_tmpF6->tag != 13)goto _LL8E;else{_tmpF7=_tmpF6->f2;_tmpF8=*_tmpF7;_tmpF9=_tmpF8.r;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF9;if(_tmpFA->tag != 0)goto _LL8E;else{_tmpFB=_tmpFA->f1;if((_tmpFB.String_c).tag != 8)goto _LL8E;_tmpFC=(struct _dyneither_ptr)(_tmpFB.String_c).val;}};}}_LL8D:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpFC,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 404
 if(opt_args != 0){
# 405
struct Cyc_List_List*_tmpFD=(struct Cyc_List_List*)opt_args->v;
# 406
for(0;_tmpFD != 0;_tmpFD=_tmpFD->tl){
# 407
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpFD->hd);{
# 408
struct _RegionHandle _tmpFE=_new_region("temp");struct _RegionHandle*temp=& _tmpFE;_push_region(temp);
# 409
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpFD->hd)->topt)) && !
# 410
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpFD->hd)){
# 411
const char*_tmp75E;void*_tmp75D;(_tmp75D=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpFD->hd)->loc,((_tmp75E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp75E,sizeof(char),49))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;_LL89:;}
# 417
if(opt_args != 0){
# 418
struct Cyc_List_List*_tmp101=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmp101 != 0;
# 421
(((desc_types=desc_types->tl,_tmp101=_tmp101->tl)),arg_cnt ++)){
# 422
int alias_coercion=0;
# 423
void*t=(void*)desc_types->hd;
# 424
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp101->hd;
# 425
Cyc_Tcexp_tcExp(te,(void**)& t,e);
# 426
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
# 427
{const char*_tmp763;void*_tmp762[2];struct Cyc_String_pa_PrintArg_struct _tmp761;struct Cyc_String_pa_PrintArg_struct _tmp760;(_tmp760.tag=0,((_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 428
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp761.tag=0,((_tmp761.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp762[0]=& _tmp761,((_tmp762[1]=& _tmp760,Cyc_Tcutil_terr(e->loc,((_tmp763="descriptor has type \n%s\n but argument has type \n%s",_tag_dyneither(_tmp763,sizeof(char),51))),_tag_dyneither(_tmp762,sizeof(void*),2)))))))))))));}
# 429
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
# 432
struct Cyc_List_List*_tmp764;*alias_arg_exps=(struct Cyc_List_List*)((_tmp764=_cycalloc(sizeof(*_tmp764)),((_tmp764->hd=(void*)arg_cnt,((_tmp764->tl=*alias_arg_exps,_tmp764))))));}{
# 433
struct _RegionHandle _tmp107=_new_region("temp");struct _RegionHandle*temp=& _tmp107;_push_region(temp);
# 434
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 435
const char*_tmp767;void*_tmp766;(_tmp766=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp101->hd)->loc,((_tmp767="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp767,sizeof(char),49))),_tag_dyneither(_tmp766,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
# 440
const char*_tmp76A;void*_tmp769;(_tmp769=0,Cyc_Tcutil_terr(fmt->loc,((_tmp76A="too few arguments",_tag_dyneither(_tmp76A,sizeof(char),18))),_tag_dyneither(_tmp769,sizeof(void*),0)));}
# 441
if(_tmp101 != 0){
# 442
const char*_tmp76D;void*_tmp76C;(_tmp76C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp101->hd)->loc,((_tmp76D="too many arguments",_tag_dyneither(_tmp76D,sizeof(char),19))),_tag_dyneither(_tmp76C,sizeof(void*),0)));}}}
# 445
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 447
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 448
switch(p){case Cyc_Absyn_Plus: _LL90:
# 449
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 451
 if(!Cyc_Tcutil_is_numeric(e)){
# 452
const char*_tmp771;void*_tmp770[1];struct Cyc_String_pa_PrintArg_struct _tmp76F;(_tmp76F.tag=0,((_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp770[0]=& _tmp76F,Cyc_Tcutil_terr(loc,((_tmp771="expecting arithmetic type but found %s",_tag_dyneither(_tmp771,sizeof(char),39))),_tag_dyneither(_tmp770,sizeof(void*),1)))))));}
# 453
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 455
 Cyc_Tcutil_check_contains_assign(e);
# 456
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
# 457
const char*_tmp775;void*_tmp774[1];struct Cyc_String_pa_PrintArg_struct _tmp773;(_tmp773.tag=0,((_tmp773.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp774[0]=& _tmp773,Cyc_Tcutil_terr(loc,((_tmp775="expecting integral or * type but found %s",_tag_dyneither(_tmp775,sizeof(char),42))),_tag_dyneither(_tmp774,sizeof(void*),1)))))));}
# 458
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 460
 if(!Cyc_Tcutil_is_integral(e)){
# 461
const char*_tmp779;void*_tmp778[1];struct Cyc_String_pa_PrintArg_struct _tmp777;(_tmp777.tag=0,((_tmp777.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp778[0]=& _tmp777,Cyc_Tcutil_terr(loc,((_tmp779="expecting integral type but found %s",_tag_dyneither(_tmp779,sizeof(char),37))),_tag_dyneither(_tmp778,sizeof(void*),1)))))));}
# 462
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 464
{void*_tmp117=t;struct Cyc_Absyn_PtrInfo _tmp11A;struct Cyc_Absyn_PtrAtts _tmp11B;union Cyc_Absyn_Constraint*_tmp11C;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp118=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp117;if(_tmp118->tag != 8)goto _LL99;}_LL98:
# 465
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp119=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp117;if(_tmp119->tag != 5)goto _LL9B;else{_tmp11A=_tmp119->f1;_tmp11B=_tmp11A.ptr_atts;_tmp11C=_tmp11B.bounds;}}_LL9A:
# 467
{void*_tmp11D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp11C);struct Cyc_Absyn_Exp*_tmp120;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp11E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp11D;if(_tmp11E->tag != 0)goto _LLA0;}_LL9F:
# 468
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp11D;if(_tmp11F->tag != 1)goto _LL9D;else{_tmp120=_tmp11F->f1;}}_LLA1:
# 470
 if(!Cyc_Evexp_c_can_eval(_tmp120)){
# 471
const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_terr(loc,((_tmp77C="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp77C,sizeof(char),55))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
# 472
goto _LL9D;_LL9D:;}
# 474
goto _LL96;_LL9B:;_LL9C: {
# 476
const char*_tmp780;void*_tmp77F[1];struct Cyc_String_pa_PrintArg_struct _tmp77E;(_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp77F[0]=& _tmp77E,Cyc_Tcutil_terr(loc,((_tmp780="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp780,sizeof(char),47))),_tag_dyneither(_tmp77F,sizeof(void*),1)))))));}_LL96:;}
# 478
return Cyc_Absyn_uint_typ;default: _LL95: {
# 479
const char*_tmp783;void*_tmp782;(_tmp782=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp783="Non-unary primop",_tag_dyneither(_tmp783,sizeof(char),17))),_tag_dyneither(_tmp782,sizeof(void*),0)));}}}
# 484
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 487
if(!checker(e1)){
# 488
{const char*_tmp787;void*_tmp786[1];struct Cyc_String_pa_PrintArg_struct _tmp785;(_tmp785.tag=0,((_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp786[0]=& _tmp785,Cyc_Tcutil_terr(e1->loc,((_tmp787="type %s cannot be used here",_tag_dyneither(_tmp787,sizeof(char),28))),_tag_dyneither(_tmp786,sizeof(void*),1)))))));}
# 489
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 491
if(!checker(e2)){
# 492
{const char*_tmp78B;void*_tmp78A[1];struct Cyc_String_pa_PrintArg_struct _tmp789;(_tmp789.tag=0,((_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp78A[0]=& _tmp789,Cyc_Tcutil_terr(e2->loc,((_tmp78B="type %s cannot be used here",_tag_dyneither(_tmp78B,sizeof(char),28))),_tag_dyneither(_tmp78A,sizeof(void*),1)))))));}
# 493
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 495
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 496
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 497
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 500
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 501
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 502
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 503
void*_tmp12E=t1;struct Cyc_Absyn_PtrInfo _tmp130;void*_tmp131;struct Cyc_Absyn_Tqual _tmp132;struct Cyc_Absyn_PtrAtts _tmp133;void*_tmp134;union Cyc_Absyn_Constraint*_tmp135;union Cyc_Absyn_Constraint*_tmp136;union Cyc_Absyn_Constraint*_tmp137;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E;if(_tmp12F->tag != 5)goto _LLA6;else{_tmp130=_tmp12F->f1;_tmp131=_tmp130.elt_typ;_tmp132=_tmp130.elt_tq;_tmp133=_tmp130.ptr_atts;_tmp134=_tmp133.rgn;_tmp135=_tmp133.nullable;_tmp136=_tmp133.bounds;_tmp137=_tmp133.zero_term;}}_LLA5:
# 505
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp131),& Cyc_Tcutil_tmk)){
# 506
const char*_tmp78E;void*_tmp78D;(_tmp78D=0,Cyc_Tcutil_terr(e1->loc,((_tmp78E="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp78E,sizeof(char),50))),_tag_dyneither(_tmp78D,sizeof(void*),0)));}
# 507
if(Cyc_Tcutil_is_noalias_pointer(t1)){
# 508
const char*_tmp791;void*_tmp790;(_tmp790=0,Cyc_Tcutil_terr(e1->loc,((_tmp791="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp791,sizeof(char),54))),_tag_dyneither(_tmp790,sizeof(void*),0)));}
# 509
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
# 510
const char*_tmp795;void*_tmp794[1];struct Cyc_String_pa_PrintArg_struct _tmp793;(_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp794[0]=& _tmp793,Cyc_Tcutil_terr(e2->loc,((_tmp795="expecting int but found %s",_tag_dyneither(_tmp795,sizeof(char),27))),_tag_dyneither(_tmp794,sizeof(void*),1)))))));}{
# 511
void*_tmp13F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp136);struct Cyc_Absyn_Exp*_tmp142;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp140=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp13F;if(_tmp140->tag != 0)goto _LLAB;}_LLAA:
# 512
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp141=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp13F;if(_tmp141->tag != 1)goto _LLA8;else{_tmp142=_tmp141->f1;}}_LLAC:
# 516
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp137)){
# 517
const char*_tmp798;void*_tmp797;(_tmp797=0,Cyc_Tcutil_warn(e1->loc,((_tmp798="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp798,sizeof(char),70))),_tag_dyneither(_tmp797,sizeof(void*),0)));}{
# 524
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp7A2;struct Cyc_Absyn_PtrAtts _tmp7A1;struct Cyc_Absyn_PtrInfo _tmp7A0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp79F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp145=(_tmp79F=_cycalloc(sizeof(*_tmp79F)),((_tmp79F[0]=((_tmp7A2.tag=5,((_tmp7A2.f1=((_tmp7A0.elt_typ=_tmp131,((_tmp7A0.elt_tq=_tmp132,((_tmp7A0.ptr_atts=(
# 525
(_tmp7A1.rgn=_tmp134,((_tmp7A1.nullable=Cyc_Absyn_true_conref,((_tmp7A1.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp7A1.zero_term=_tmp137,((_tmp7A1.ptrloc=0,_tmp7A1)))))))))),_tmp7A0)))))),_tmp7A2)))),_tmp79F)));
# 528
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp145,Cyc_Absyn_Other_coercion);
# 529
return(void*)_tmp145;};_LLA8:;};_LLA6:;_LLA7:
# 531
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 536
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 537
void*t1=(void*)_check_null(e1->topt);
# 538
void*t2=(void*)_check_null(e2->topt);
# 539
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
# 540
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
# 541
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
# 542
if(!Cyc_Tcutil_unify(t1,t2)){
# 543
{const char*_tmp7A7;void*_tmp7A6[2];struct Cyc_String_pa_PrintArg_struct _tmp7A5;struct Cyc_String_pa_PrintArg_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 545
Cyc_Absynpp_typ2string(t2)),((_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7A6[0]=& _tmp7A5,((_tmp7A6[1]=& _tmp7A4,Cyc_Tcutil_terr(e1->loc,((_tmp7A7="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp7A7,sizeof(char),59))),_tag_dyneither(_tmp7A6,sizeof(void*),2)))))))))))));}
# 546
Cyc_Tcutil_explain_failure();}
# 548
return Cyc_Absyn_sint_typ;}else{
# 550
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
# 551
const char*_tmp7AA;void*_tmp7A9;(_tmp7A9=0,Cyc_Tcutil_terr(e1->loc,((_tmp7AA="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7AA,sizeof(char),50))),_tag_dyneither(_tmp7A9,sizeof(void*),0)));}
# 552
if(Cyc_Tcutil_is_noalias_pointer(t1)){
# 553
const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,Cyc_Tcutil_terr(e1->loc,((_tmp7AD="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7AD,sizeof(char),54))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}
# 554
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
# 555
{const char*_tmp7B2;void*_tmp7B1[2];struct Cyc_String_pa_PrintArg_struct _tmp7B0;struct Cyc_String_pa_PrintArg_struct _tmp7AF;(_tmp7AF.tag=0,((_tmp7AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp7B0.tag=0,((_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B1[0]=& _tmp7B0,((_tmp7B1[1]=& _tmp7AF,Cyc_Tcutil_terr(e2->loc,((_tmp7B2="expecting either %s or int but found %s",_tag_dyneither(_tmp7B2,sizeof(char),40))),_tag_dyneither(_tmp7B1,sizeof(void*),2)))))))))))));}
# 557
Cyc_Tcutil_explain_failure();}
# 559
return t1;}}
# 563
if(Cyc_Tcutil_is_pointer_type(t1))
# 564
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
# 565
if(Cyc_Tcutil_is_pointer_type(t2))
# 566
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);
# 567
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 570
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 571
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
# 572
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
# 573
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 574
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 575
if(e1_is_num  && e2_is_num)
# 576
return Cyc_Absyn_sint_typ;else{
# 578
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
# 579
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 580
if(Cyc_Tcutil_unify(t1,t2))
# 581
return Cyc_Absyn_sint_typ;else{
# 583
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
# 584
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
# 585
return Cyc_Absyn_sint_typ;}else{
# 586
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
# 587
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
# 588
return Cyc_Absyn_sint_typ;}else{
# 589
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
# 590
return Cyc_Absyn_sint_typ;else{
# 591
goto pointer_cmp;}}}}}else{
# 596
pointer_cmp: {
# 597
struct _tuple0 _tmp7B3;struct _tuple0 _tmp157=(_tmp7B3.f1=Cyc_Tcutil_compress(t1),((_tmp7B3.f2=Cyc_Tcutil_compress(t2),_tmp7B3)));void*_tmp158;struct Cyc_Absyn_PtrInfo _tmp15A;void*_tmp15B;void*_tmp15C;struct Cyc_Absyn_PtrInfo _tmp15E;void*_tmp15F;void*_tmp160;void*_tmp162;_LLAE: _tmp158=_tmp157.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp158;if(_tmp159->tag != 5)goto _LLB0;else{_tmp15A=_tmp159->f1;_tmp15B=_tmp15A.elt_typ;}};_tmp15C=_tmp157.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15C;if(_tmp15D->tag != 5)goto _LLB0;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15E.elt_typ;}};_LLAF:
# 600
 if(Cyc_Tcutil_unify(_tmp15B,_tmp15F))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0: _tmp160=_tmp157.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp160;if(_tmp161->tag != 15)goto _LLB2;};_tmp162=_tmp157.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp162;if(_tmp163->tag != 15)goto _LLB2;};_LLB1:
# 602
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
# 603
 goto _LLAD;_LLAD:;}
# 605
{const char*_tmp7B8;void*_tmp7B7[2];struct Cyc_String_pa_PrintArg_struct _tmp7B6;struct Cyc_String_pa_PrintArg_struct _tmp7B5;(_tmp7B5.tag=0,((_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 606
Cyc_Absynpp_typ2string(t2)),((_tmp7B6.tag=0,((_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B7[0]=& _tmp7B6,((_tmp7B7[1]=& _tmp7B5,Cyc_Tcutil_terr(loc,((_tmp7B8="comparison not allowed between %s and %s",_tag_dyneither(_tmp7B8,sizeof(char),41))),_tag_dyneither(_tmp7B7,sizeof(void*),2)))))))))))));}
# 607
Cyc_Tcutil_explain_failure();
# 608
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 612
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 614
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
# 615
return Cyc_Absyn_sint_typ;{
# 620
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 621
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 622
{struct _tuple0 _tmp7B9;struct _tuple0 _tmp169=(_tmp7B9.f1=t1,((_tmp7B9.f2=t2,_tmp7B9)));void*_tmp16A;void*_tmp16C;void*_tmp16D;void*_tmp16F;_LLB5: _tmp16A=_tmp169.f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp16A;if(_tmp16B->tag != 15)goto _LLB7;else{_tmp16C=(void*)_tmp16B->f1;}};_tmp16D=_tmp169.f2;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp16D;if(_tmp16E->tag != 15)goto _LLB7;else{_tmp16F=(void*)_tmp16E->f1;}};_LLB6:
# 625
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
# 626
 goto _LLB4;_LLB4:;}
# 628
{const char*_tmp7BE;void*_tmp7BD[2];struct Cyc_String_pa_PrintArg_struct _tmp7BC;struct Cyc_String_pa_PrintArg_struct _tmp7BB;(_tmp7BB.tag=0,((_tmp7BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 629
Cyc_Absynpp_typ2string(t2)),((_tmp7BC.tag=0,((_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7BD[0]=& _tmp7BC,((_tmp7BD[1]=& _tmp7BB,Cyc_Tcutil_terr(loc,((_tmp7BE="comparison not allowed between %s and %s",_tag_dyneither(_tmp7BE,sizeof(char),41))),_tag_dyneither(_tmp7BD,sizeof(void*),2)))))))))))));}
# 630
Cyc_Tcutil_explain_failure();
# 631
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 636
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 638
switch(p){case Cyc_Absyn_Plus: _LLB9:
# 639
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
# 640
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 642
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
# 643
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 645
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
# 646
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
# 647
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
# 648
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
# 649
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
# 650
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
# 651
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 655
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
# 656
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 658
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
# 659
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
# 660
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
# 661
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 663
const char*_tmp7C1;void*_tmp7C0;(_tmp7C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7C1="bad binary primop",_tag_dyneither(_tmp7C1,sizeof(char),18))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));}}}
# 667
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 675
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)
# 676
return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
# 677
Cyc_Tcexp_tcExpList(te,es);{
# 678
void*t;
# 679
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLCC: {
# 680
const char*_tmp7C4;void*_tmp7C3;return(_tmp7C3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C4="primitive operator has 0 arguments",_tag_dyneither(_tmp7C4,sizeof(char),35))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}case 1: _LLCD:
# 681
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
# 682
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
# 683
const char*_tmp7C7;void*_tmp7C6;return(_tmp7C6=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C7="primitive operator has > 2 arguments",_tag_dyneither(_tmp7C7,sizeof(char),37))),_tag_dyneither(_tmp7C6,sizeof(void*),0)));}}
# 685
return t;};}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 688
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
# 689
void*_tmp17A=Cyc_Tcutil_compress(t);
# 690
void*_tmp17B=_tmp17A;struct Cyc_Absyn_AggrInfo _tmp17D;union Cyc_Absyn_AggrInfoU _tmp17E;struct Cyc_Absyn_Aggrdecl**_tmp17F;struct Cyc_Absyn_Aggrdecl*_tmp180;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_DatatypeFieldInfo _tmp184;union Cyc_Absyn_DatatypeFieldInfoU _tmp185;struct _tuple2 _tmp186;struct Cyc_Absyn_Datatypefield*_tmp187;struct Cyc_Absyn_ArrayInfo _tmp189;void*_tmp18A;struct Cyc_Absyn_Tqual _tmp18B;struct Cyc_List_List*_tmp18D;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp17B;if(_tmp17C->tag != 11)goto _LLD4;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17D.aggr_info;if((_tmp17E.KnownAggr).tag != 2)goto _LLD4;_tmp17F=(struct Cyc_Absyn_Aggrdecl**)(_tmp17E.KnownAggr).val;_tmp180=*_tmp17F;}}_LLD3:
# 692
 _tmp182=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp180->impl))->fields;goto _LLD5;_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17B;if(_tmp181->tag != 12)goto _LLD6;else{_tmp182=_tmp181->f2;}}_LLD5:
# 694
 for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
# 695
struct Cyc_Absyn_Aggrfield*_tmp18E=(struct Cyc_Absyn_Aggrfield*)_tmp182->hd;
# 696
if((_tmp18E->tq).real_const){
# 697
{const char*_tmp7CB;void*_tmp7CA[1];struct Cyc_String_pa_PrintArg_struct _tmp7C9;(_tmp7C9.tag=0,((_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp18E->name),((_tmp7CA[0]=& _tmp7C9,Cyc_Tcutil_terr(loc,((_tmp7CB="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp7CB,sizeof(char),56))),_tag_dyneither(_tmp7CA,sizeof(void*),1)))))));}
# 698
return 0;}
# 700
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp18E->type))return 0;}
# 702
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp17B;if(_tmp183->tag != 4)goto _LLD8;else{_tmp184=_tmp183->f1;_tmp185=_tmp184.field_info;if((_tmp185.KnownDatatypefield).tag != 2)goto _LLD8;_tmp186=(struct _tuple2)(_tmp185.KnownDatatypefield).val;_tmp187=_tmp186.f2;}}_LLD7:
# 704
{struct Cyc_List_List*_tmp192=_tmp187->typs;for(0;_tmp192 != 0;_tmp192=_tmp192->tl){
# 705
struct _tuple19 _tmp194;struct Cyc_Absyn_Tqual _tmp195;void*_tmp196;struct _tuple19*_tmp193=(struct _tuple19*)_tmp192->hd;_tmp194=*_tmp193;_tmp195=_tmp194.f1;_tmp196=_tmp194.f2;
# 706
if(_tmp195.real_const){
# 707
{const char*_tmp7CF;void*_tmp7CE[1];struct Cyc_String_pa_PrintArg_struct _tmp7CD;(_tmp7CD.tag=0,((_tmp7CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp187->name)),((_tmp7CE[0]=& _tmp7CD,Cyc_Tcutil_terr(loc,((_tmp7CF="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp7CF,sizeof(char),64))),_tag_dyneither(_tmp7CE,sizeof(void*),1)))))));}
# 708
return 0;}
# 710
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp196))return 0;}}
# 712
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17B;if(_tmp188->tag != 8)goto _LLDA;else{_tmp189=_tmp188->f1;_tmp18A=_tmp189.elt_type;_tmp18B=_tmp189.tq;}}_LLD9:
# 714
 if(_tmp18B.real_const){
# 715
{const char*_tmp7D2;void*_tmp7D1;(_tmp7D1=0,Cyc_Tcutil_terr(loc,((_tmp7D2="attempt to over-write a const array",_tag_dyneither(_tmp7D2,sizeof(char),36))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}
# 716
return 0;}
# 718
return Cyc_Tcexp_check_writable_aggr(loc,_tmp18A);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17B;if(_tmp18C->tag != 10)goto _LLDC;else{_tmp18D=_tmp18C->f1;}}_LLDB:
# 720
 for(0;_tmp18D != 0;_tmp18D=_tmp18D->tl){
# 721
struct _tuple19 _tmp19D;struct Cyc_Absyn_Tqual _tmp19E;void*_tmp19F;struct _tuple19*_tmp19C=(struct _tuple19*)_tmp18D->hd;_tmp19D=*_tmp19C;_tmp19E=_tmp19D.f1;_tmp19F=_tmp19D.f2;
# 722
if(_tmp19E.real_const){
# 723
{const char*_tmp7D5;void*_tmp7D4;(_tmp7D4=0,Cyc_Tcutil_terr(loc,((_tmp7D5="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp7D5,sizeof(char),56))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}
# 724
return 0;}
# 726
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp19F))return 0;}
# 728
return 1;_LLDC:;_LLDD:
# 729
 return 1;_LLD1:;}
# 736
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 739
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
# 740
{void*_tmp1A2=e->r;void*_tmp1A4;struct Cyc_Absyn_Vardecl*_tmp1A6;void*_tmp1A8;struct Cyc_Absyn_Vardecl*_tmp1AA;void*_tmp1AC;struct Cyc_Absyn_Vardecl*_tmp1AE;void*_tmp1B0;struct Cyc_Absyn_Vardecl*_tmp1B2;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B7;struct _dyneither_ptr*_tmp1B8;struct Cyc_Absyn_Exp*_tmp1BA;struct _dyneither_ptr*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1A3->tag != 1)goto _LLE1;else{_tmp1A4=(void*)_tmp1A3->f2;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1A5=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp1A4;if(_tmp1A5->tag != 3)goto _LLE1;else{_tmp1A6=_tmp1A5->f1;}};}}_LLE0:
# 741
 _tmp1AA=_tmp1A6;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1A7->tag != 1)goto _LLE3;else{_tmp1A8=(void*)_tmp1A7->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1A9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp1A8;if(_tmp1A9->tag != 4)goto _LLE3;else{_tmp1AA=_tmp1A9->f1;}};}}_LLE2:
# 742
 _tmp1AE=_tmp1AA;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1AB->tag != 1)goto _LLE5;else{_tmp1AC=(void*)_tmp1AB->f2;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1AD=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp1AC;if(_tmp1AD->tag != 5)goto _LLE5;else{_tmp1AE=_tmp1AD->f1;}};}}_LLE4:
# 743
 _tmp1B2=_tmp1AE;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1AF->tag != 1)goto _LLE7;else{_tmp1B0=(void*)_tmp1AF->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1B1=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp1B0;if(_tmp1B1->tag != 1)goto _LLE7;else{_tmp1B2=_tmp1B1->f1;}};}}_LLE6:
# 744
 if(!(_tmp1B2->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1B3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1B3->tag != 22)goto _LLE9;else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B3->f2;}}_LLE8:
# 746
{void*_tmp1C2=Cyc_Tcutil_compress((void*)_check_null(_tmp1B4->topt));struct Cyc_Absyn_PtrInfo _tmp1C4;struct Cyc_Absyn_Tqual _tmp1C5;struct Cyc_Absyn_ArrayInfo _tmp1C7;struct Cyc_Absyn_Tqual _tmp1C8;struct Cyc_List_List*_tmp1CA;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C2;if(_tmp1C3->tag != 5)goto _LLF8;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C4.elt_tq;}}_LLF7:
# 747
 _tmp1C8=_tmp1C5;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C2;if(_tmp1C6->tag != 8)goto _LLFA;else{_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C7.tq;}}_LLF9:
# 748
 if(!_tmp1C8.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1C9=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1C2;if(_tmp1C9->tag != 10)goto _LLFC;else{_tmp1CA=_tmp1C9->f1;}}_LLFB: {
# 750
unsigned int _tmp1CC;int _tmp1CD;struct _tuple15 _tmp1CB=Cyc_Evexp_eval_const_uint_exp(_tmp1B5);_tmp1CC=_tmp1CB.f1;_tmp1CD=_tmp1CB.f2;
# 751
if(!_tmp1CD){
# 752
{const char*_tmp7D8;void*_tmp7D7;(_tmp7D7=0,Cyc_Tcutil_terr(e->loc,((_tmp7D8="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7D8,sizeof(char),47))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}
# 753
return;}
# 755
{struct _handler_cons _tmp1D0;_push_handler(& _tmp1D0);{int _tmp1D2=0;if(setjmp(_tmp1D0.handler))_tmp1D2=1;if(!_tmp1D2){
# 756
{struct _tuple19 _tmp1D4;struct Cyc_Absyn_Tqual _tmp1D5;struct _tuple19*_tmp1D3=((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1CA,(int)_tmp1CC);_tmp1D4=*_tmp1D3;_tmp1D5=_tmp1D4.f1;
# 757
if(!_tmp1D5.real_const){_npop_handler(0);return;}}
# 756
;_pop_handler();}else{void*_tmp1D1=(void*)_exn_thrown;void*_tmp1D7=_tmp1D1;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1D8=(struct Cyc_List_Nth_exn_struct*)_tmp1D7;if(_tmp1D8->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 758
 return;_LL101:;_LL102:(void)_throw(_tmp1D7);_LLFE:;}};}
# 759
goto _LLF5;}_LLFC:;_LLFD:
# 760
 goto _LLF5;_LLF5:;}
# 762
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1B6=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1B6->tag != 20)goto _LLEB;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}_LLEA:
# 764
{void*_tmp1D9=Cyc_Tcutil_compress((void*)_check_null(_tmp1B7->topt));struct Cyc_Absyn_AggrInfo _tmp1DB;union Cyc_Absyn_AggrInfoU _tmp1DC;struct Cyc_Absyn_Aggrdecl**_tmp1DD;struct Cyc_List_List*_tmp1DF;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DA->tag != 11)goto _LL106;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DB.aggr_info;if((_tmp1DC.KnownAggr).tag != 2)goto _LL106;_tmp1DD=(struct Cyc_Absyn_Aggrdecl**)(_tmp1DC.KnownAggr).val;}}_LL105: {
# 766
struct Cyc_Absyn_Aggrfield*sf=
# 767
(struct Cyc_Absyn_Aggrdecl**)_tmp1DD == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1DD,_tmp1B8);
# 768
if(sf == 0  || !(sf->tq).real_const)return;
# 769
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DE->tag != 12)goto _LL108;else{_tmp1DF=_tmp1DE->f2;}}_LL107: {
# 771
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1DF,_tmp1B8);
# 772
if(sf == 0  || !(sf->tq).real_const)return;
# 773
goto _LL103;}_LL108:;_LL109:
# 774
 goto _LL103;_LL103:;}
# 776
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1B9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1B9->tag != 21)goto _LLED;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LLEC:
# 778
{void*_tmp1E0=Cyc_Tcutil_compress((void*)_check_null(_tmp1BA->topt));struct Cyc_Absyn_PtrInfo _tmp1E2;void*_tmp1E3;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E0;if(_tmp1E1->tag != 5)goto _LL10D;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E2.elt_typ;}}_LL10C:
# 780
{void*_tmp1E4=Cyc_Tcutil_compress(_tmp1E3);struct Cyc_Absyn_AggrInfo _tmp1E6;union Cyc_Absyn_AggrInfoU _tmp1E7;struct Cyc_Absyn_Aggrdecl**_tmp1E8;struct Cyc_List_List*_tmp1EA;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1E5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E4;if(_tmp1E5->tag != 11)goto _LL112;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E6.aggr_info;if((_tmp1E7.KnownAggr).tag != 2)goto _LL112;_tmp1E8=(struct Cyc_Absyn_Aggrdecl**)(_tmp1E7.KnownAggr).val;}}_LL111: {
# 782
struct Cyc_Absyn_Aggrfield*sf=
# 783
(struct Cyc_Absyn_Aggrdecl**)_tmp1E8 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1E8,_tmp1BB);
# 784
if(sf == 0  || !(sf->tq).real_const)return;
# 785
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1E9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E4;if(_tmp1E9->tag != 12)goto _LL114;else{_tmp1EA=_tmp1E9->f2;}}_LL113: {
# 787
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1EA,_tmp1BB);
# 788
if(sf == 0  || !(sf->tq).real_const)return;
# 789
goto _LL10F;}_LL114:;_LL115:
# 790
 goto _LL10F;_LL10F:;}
# 792
goto _LL10A;_LL10D:;_LL10E:
# 793
 goto _LL10A;_LL10A:;}
# 795
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1BC->tag != 19)goto _LLEF;else{_tmp1BD=_tmp1BC->f1;}}_LLEE:
# 797
{void*_tmp1EB=Cyc_Tcutil_compress((void*)_check_null(_tmp1BD->topt));struct Cyc_Absyn_PtrInfo _tmp1ED;struct Cyc_Absyn_Tqual _tmp1EE;struct Cyc_Absyn_ArrayInfo _tmp1F0;struct Cyc_Absyn_Tqual _tmp1F1;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EB;if(_tmp1EC->tag != 5)goto _LL119;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1ED.elt_tq;}}_LL118:
# 798
 _tmp1F1=_tmp1EE;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EB;if(_tmp1EF->tag != 8)goto _LL11B;else{_tmp1F0=_tmp1EF->f1;_tmp1F1=_tmp1F0.tq;}}_LL11A:
# 799
 if(!_tmp1F1.real_const)return;goto _LL116;_LL11B:;_LL11C:
# 800
 goto _LL116;_LL116:;}
# 802
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1BE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1BE->tag != 11)goto _LLF1;else{_tmp1BF=_tmp1BE->f1;}}_LLF0:
# 803
 _tmp1C1=_tmp1BF;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1C0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1A2;if(_tmp1C0->tag != 12)goto _LLF3;else{_tmp1C1=_tmp1C0->f1;}}_LLF2:
# 804
 Cyc_Tcexp_check_writable(te,_tmp1C1);return;_LLF3:;_LLF4:
# 805
 goto _LLDE;_LLDE:;}{
# 807
const char*_tmp7DC;void*_tmp7DB[1];struct Cyc_String_pa_PrintArg_struct _tmp7DA;(_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7DB[0]=& _tmp7DA,Cyc_Tcutil_terr(e->loc,((_tmp7DC="attempt to write a const location: %s",_tag_dyneither(_tmp7DC,sizeof(char),38))),_tag_dyneither(_tmp7DB,sizeof(void*),1)))))));};}
# 810
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 813
struct _RegionHandle _tmp1F5=_new_region("temp");struct _RegionHandle*temp=& _tmp1F5;_push_region(temp);
# 814
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 816
if(!Cyc_Absyn_is_lvalue(e)){
# 817
const char*_tmp7DF;void*_tmp7DE;void*_tmp1F8=(_tmp7DE=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7DF="increment/decrement of non-lvalue",_tag_dyneither(_tmp7DF,sizeof(char),34))),_tag_dyneither(_tmp7DE,sizeof(void*),0)));_npop_handler(0);return _tmp1F8;}
# 818
Cyc_Tcexp_check_writable(te,e);{
# 819
void*t=(void*)_check_null(e->topt);
# 821
if(!Cyc_Tcutil_is_numeric(e)){
# 822
void*telt=(void*)& Cyc_Absyn_VoidType_val;
# 823
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
# 824
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
# 825
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
# 826
const char*_tmp7E2;void*_tmp7E1;(_tmp7E1=0,Cyc_Tcutil_terr(e->loc,((_tmp7E2="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7E2,sizeof(char),50))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));}
# 827
if(Cyc_Tcutil_is_noalias_pointer(t)){
# 828
const char*_tmp7E5;void*_tmp7E4;(_tmp7E4=0,Cyc_Tcutil_terr(e->loc,((_tmp7E5="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7E5,sizeof(char),54))),_tag_dyneither(_tmp7E4,sizeof(void*),0)));}}else{
# 830
const char*_tmp7E9;void*_tmp7E8[1];struct Cyc_String_pa_PrintArg_struct _tmp7E7;(_tmp7E7.tag=0,((_tmp7E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7E8[0]=& _tmp7E7,Cyc_Tcutil_terr(e->loc,((_tmp7E9="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7E9,sizeof(char),44))),_tag_dyneither(_tmp7E8,sizeof(void*),1)))))));}}{
# 832
void*_tmp200=t;_npop_handler(0);return _tmp200;};};
# 813
;_pop_region(temp);}
# 836
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 838
struct _tuple0*_tmp203;int _tmp204;const char*_tmp7EA;struct Cyc_Tcexp_TestEnv _tmp202=Cyc_Tcexp_tcTest(te,e1,((_tmp7EA="conditional expression",_tag_dyneither(_tmp7EA,sizeof(char),23))));_tmp203=_tmp202.eq;_tmp204=_tmp202.isTrue;
# 839
Cyc_Tcexp_tcExp(te,topt,e2);
# 840
Cyc_Tcexp_tcExp(te,topt,e3);{
# 841
void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
# 842
struct Cyc_List_List _tmp7EB;struct Cyc_List_List _tmp205=(_tmp7EB.hd=e3,((_tmp7EB.tl=0,_tmp7EB)));
# 843
struct Cyc_List_List _tmp7EC;struct Cyc_List_List _tmp206=(_tmp7EC.hd=e2,((_tmp7EC.tl=(struct Cyc_List_List*)& _tmp205,_tmp7EC)));
# 844
if(!Cyc_Tcutil_coerce_list(te,t,(struct Cyc_List_List*)& _tmp206)){
# 845
{const char*_tmp7F1;void*_tmp7F0[2];struct Cyc_String_pa_PrintArg_struct _tmp7EF;struct Cyc_String_pa_PrintArg_struct _tmp7EE;(_tmp7EE.tag=0,((_tmp7EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 846
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7EF.tag=0,((_tmp7EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7F0[0]=& _tmp7EF,((_tmp7F0[1]=& _tmp7EE,Cyc_Tcutil_terr(loc,((_tmp7F1="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7F1,sizeof(char),48))),_tag_dyneither(_tmp7F0,sizeof(void*),2)))))))))))));}
# 847
Cyc_Tcutil_explain_failure();}
# 849
return t;};}
# 853
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 855
{const char*_tmp7F2;Cyc_Tcexp_tcTest(te,e1,((_tmp7F2="logical-and expression",_tag_dyneither(_tmp7F2,sizeof(char),23))));}
# 856
{const char*_tmp7F3;Cyc_Tcexp_tcTest(te,e2,((_tmp7F3="logical-and expression",_tag_dyneither(_tmp7F3,sizeof(char),23))));}
# 857
return Cyc_Absyn_sint_typ;}
# 861
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 863
{const char*_tmp7F4;Cyc_Tcexp_tcTest(te,e1,((_tmp7F4="logical-or expression",_tag_dyneither(_tmp7F4,sizeof(char),22))));}
# 864
{const char*_tmp7F5;Cyc_Tcexp_tcTest(te,e2,((_tmp7F5="logical-or expression",_tag_dyneither(_tmp7F5,sizeof(char),22))));}
# 865
return Cyc_Absyn_sint_typ;}
# 869
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 876
struct _RegionHandle _tmp211=_new_region("r");struct _RegionHandle*r=& _tmp211;_push_region(r);
# 877
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 879
void*t1=(void*)_check_null(e1->topt);
# 880
Cyc_Tcexp_tcExp(te,(void**)& t1,e2);{
# 881
void*t2=(void*)_check_null(e2->topt);
# 883
{void*_tmp212=Cyc_Tcutil_compress(t1);_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp212;if(_tmp213->tag != 8)goto _LL120;}_LL11F:
# 884
{const char*_tmp7F8;void*_tmp7F7;(_tmp7F7=0,Cyc_Tcutil_terr(loc,((_tmp7F8="cannot assign to an array",_tag_dyneither(_tmp7F8,sizeof(char),26))),_tag_dyneither(_tmp7F7,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
# 885
 goto _LL11D;_LL11D:;}
# 888
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
# 889
const char*_tmp7FB;void*_tmp7FA;(_tmp7FA=0,Cyc_Tcutil_terr(loc,((_tmp7FB="type is abstract (can't determine size).",_tag_dyneither(_tmp7FB,sizeof(char),41))),_tag_dyneither(_tmp7FA,sizeof(void*),0)));}
# 892
if(!Cyc_Absyn_is_lvalue(e1)){
# 893
const char*_tmp7FE;void*_tmp7FD;void*_tmp21A=(_tmp7FD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FE="assignment to non-lvalue",_tag_dyneither(_tmp7FE,sizeof(char),25))),_tag_dyneither(_tmp7FD,sizeof(void*),0)));_npop_handler(0);return _tmp21A;}
# 894
Cyc_Tcexp_check_writable(te,e1);
# 895
if(po == 0){
# 896
struct _RegionHandle _tmp21B=_new_region("temp");struct _RegionHandle*temp=& _tmp21B;_push_region(temp);
# 897
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
# 898
const char*_tmp801;void*_tmp800;(_tmp800=0,Cyc_Tcutil_terr(e2->loc,((_tmp801="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp801,sizeof(char),49))),_tag_dyneither(_tmp800,sizeof(void*),0)));}
# 900
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
# 901
const char*_tmp806;void*_tmp805[2];struct Cyc_String_pa_PrintArg_struct _tmp804;struct Cyc_String_pa_PrintArg_struct _tmp803;void*_tmp21E=(_tmp803.tag=0,((_tmp803.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 902
Cyc_Absynpp_typ2string(t2)),((_tmp804.tag=0,((_tmp804.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp805[0]=& _tmp804,((_tmp805[1]=& _tmp803,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp806="type mismatch: %s != %s",_tag_dyneither(_tmp806,sizeof(char),24))),_tag_dyneither(_tmp805,sizeof(void*),2)))))))))))));
# 903
Cyc_Tcutil_unify(t1,t2);
# 904
Cyc_Tcutil_explain_failure();{
# 905
void*_tmp21F=_tmp21E;_npop_handler(1);return _tmp21F;};}
# 896
;_pop_region(temp);}else{
# 908
enum Cyc_Absyn_Primop _tmp224=(enum Cyc_Absyn_Primop)po->v;
# 909
void*_tmp225=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp224,e1,e2);
# 910
if(!(Cyc_Tcutil_unify(_tmp225,t1) || Cyc_Tcutil_coerceable(_tmp225) && Cyc_Tcutil_coerceable(t1))){
# 911
const char*_tmp80B;void*_tmp80A[2];struct Cyc_String_pa_PrintArg_struct _tmp809;struct Cyc_String_pa_PrintArg_struct _tmp808;void*_tmp226=
# 912
(_tmp808.tag=0,((_tmp808.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 914
Cyc_Absynpp_typ2string(t2)),((_tmp809.tag=0,((_tmp809.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 913
Cyc_Absynpp_typ2string(t1)),((_tmp80A[0]=& _tmp809,((_tmp80A[1]=& _tmp808,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80B="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp80B,sizeof(char),82))),_tag_dyneither(_tmp80A,sizeof(void*),2)))))))))))));
# 915
Cyc_Tcutil_unify(_tmp225,t1);
# 916
Cyc_Tcutil_explain_failure();{
# 917
void*_tmp227=_tmp226;_npop_handler(0);return _tmp227;};}{
# 919
void*_tmp22C=_tmp225;_npop_handler(0);return _tmp22C;};}{
# 921
void*_tmp22D=t1;_npop_handler(0);return _tmp22D;};};};
# 876
;_pop_region(r);}
# 925
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 926
Cyc_Tcexp_tcExp(te,0,e1);
# 927
Cyc_Tcexp_tcExp(te,topt,e2);
# 928
return(void*)_check_null(e2->topt);}
# 932
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 936
struct Cyc_List_List*fields;
# 937
void*t1=(void*)_check_null(e->topt);
# 939
{void*_tmp22E=Cyc_Tcutil_compress(t1);int _tmp230;enum Cyc_Absyn_Size_of _tmp232;enum Cyc_Absyn_Size_of _tmp234;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp22E;if(_tmp22F->tag != 7)goto _LL125;else{_tmp230=_tmp22F->f1;if(_tmp230 != 0)goto _LL125;}}_LL124:
# 940
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp22E;if(_tmp231->tag != 6)goto _LL127;else{_tmp232=_tmp231->f2;if(_tmp232 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
# 941
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp22E;if(_tmp233->tag != 6)goto _LL129;else{_tmp234=_tmp233->f2;if(_tmp234 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
# 942
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
# 943
 goto _LL122;_LL122:;}
# 946
for(fields=fs;fields != 0;fields=fields->tl){
# 947
struct _tuple1*_tmp236;struct Cyc_List_List*_tmp237;unsigned int _tmp238;enum Cyc_Absyn_Scope _tmp239;struct Cyc_Absyn_Datatypefield _tmp235=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp236=_tmp235.name;_tmp237=_tmp235.typs;_tmp238=_tmp235.loc;_tmp239=_tmp235.sc;
# 949
if(_tmp237 == 0  || _tmp237->tl != 0)continue;{
# 950
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp237->hd)).f2);
# 952
if(Cyc_Tcutil_unify(t1,t2))
# 953
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
# 957
for(fields=fs;fields != 0;fields=fields->tl){
# 958
struct _tuple1*_tmp23B;struct Cyc_List_List*_tmp23C;unsigned int _tmp23D;enum Cyc_Absyn_Scope _tmp23E;struct Cyc_Absyn_Datatypefield _tmp23A=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp23B=_tmp23A.name;_tmp23C=_tmp23A.typs;_tmp23D=_tmp23A.loc;_tmp23E=_tmp23A.sc;
# 960
if(_tmp23C == 0  || _tmp23C->tl != 0)continue;{
# 961
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple19*)_tmp23C->hd)).f2);
# 964
int bogus=0;
# 965
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
# 966
return(struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)fields->hd);};}
# 969
{const char*_tmp810;void*_tmp80F[2];struct Cyc_String_pa_PrintArg_struct _tmp80E;struct Cyc_String_pa_PrintArg_struct _tmp80D;(_tmp80D.tag=0,((_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 970
Cyc_Absynpp_typ2string(t1)),((_tmp80E.tag=0,((_tmp80E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp80F[0]=& _tmp80E,((_tmp80F[1]=& _tmp80D,Cyc_Tcutil_terr(e->loc,((_tmp810="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp810,sizeof(char),54))),_tag_dyneither(_tmp80F,sizeof(void*),2)))))))))))));}
# 971
return 0;}
# 975
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 981
struct Cyc_List_List*_tmp243=args;
# 982
int _tmp244=0;
# 983
struct _RegionHandle _tmp245=_new_region("ter");struct _RegionHandle*ter=& _tmp245;_push_region(ter);
# 984
{struct Cyc_Tcenv_Tenv*_tmp246=Cyc_Tcenv_new_block(ter,loc,te_orig);
# 985
Cyc_Tcexp_tcExp(_tmp246,0,e);{
# 986
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 990
void*_tmp247=t;struct Cyc_Absyn_PtrInfo _tmp249;void*_tmp24A;struct Cyc_Absyn_Tqual _tmp24B;struct Cyc_Absyn_PtrAtts _tmp24C;void*_tmp24D;union Cyc_Absyn_Constraint*_tmp24E;union Cyc_Absyn_Constraint*_tmp24F;union Cyc_Absyn_Constraint*_tmp250;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp248=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp247;if(_tmp248->tag != 5)goto _LL12E;else{_tmp249=_tmp248->f1;_tmp24A=_tmp249.elt_typ;_tmp24B=_tmp249.elt_tq;_tmp24C=_tmp249.ptr_atts;_tmp24D=_tmp24C.rgn;_tmp24E=_tmp24C.nullable;_tmp24F=_tmp24C.bounds;_tmp250=_tmp24C.zero_term;}}_LL12D:
# 995
 Cyc_Tcenv_check_rgn_accessible(_tmp246,loc,_tmp24D);
# 997
Cyc_Tcutil_check_nonzero_bound(loc,_tmp24F);{
# 998
void*_tmp251=Cyc_Tcutil_compress(_tmp24A);struct Cyc_Absyn_FnInfo _tmp253;struct Cyc_List_List*_tmp254;void*_tmp255;struct Cyc_Absyn_Tqual _tmp256;void*_tmp257;struct Cyc_List_List*_tmp258;int _tmp259;struct Cyc_Absyn_VarargInfo*_tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_List_List*_tmp25C;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp252=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251;if(_tmp252->tag != 9)goto _LL133;else{_tmp253=_tmp252->f1;_tmp254=_tmp253.tvars;_tmp255=_tmp253.effect;_tmp256=_tmp253.ret_tqual;_tmp257=_tmp253.ret_typ;_tmp258=_tmp253.args;_tmp259=_tmp253.c_varargs;_tmp25A=_tmp253.cyc_varargs;_tmp25B=_tmp253.rgn_po;_tmp25C=_tmp253.attributes;}}_LL132:
# 1004
 if(topt != 0)Cyc_Tcutil_unify(_tmp257,*topt);
# 1006
while(_tmp243 != 0  && _tmp258 != 0){
# 1008
int alias_coercion=0;
# 1009
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp243->hd;
# 1010
void*t2=(*((struct _tuple9*)_tmp258->hd)).f3;
# 1011
Cyc_Tcexp_tcExp(_tmp246,(void**)& t2,e1);
# 1012
if(!Cyc_Tcutil_coerce_arg(_tmp246,e1,t2,& alias_coercion)){
# 1013
{const char*_tmp815;void*_tmp814[2];struct Cyc_String_pa_PrintArg_struct _tmp813;struct Cyc_String_pa_PrintArg_struct _tmp812;(_tmp812.tag=0,((_tmp812.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1016
Cyc_Absynpp_typ2string(t2)),((_tmp813.tag=0,((_tmp813.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp814[0]=& _tmp813,((_tmp814[1]=& _tmp812,Cyc_Tcutil_terr(e1->loc,((_tmp815="actual argument has type \n\t%s\n but formal has type \n\t%s.",_tag_dyneither(_tmp815,sizeof(char),57))),_tag_dyneither(_tmp814,sizeof(void*),2)))))))))))));}
# 1017
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
# 1018
Cyc_Tcutil_explain_failure();}
# 1021
if(alias_coercion){
# 1022
struct Cyc_List_List*_tmp816;*alias_arg_exps=(struct Cyc_List_List*)((_tmp816=_cycalloc(sizeof(*_tmp816)),((_tmp816->hd=(void*)_tmp244,((_tmp816->tl=*alias_arg_exps,_tmp816))))));}
# 1023
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
# 1024
const char*_tmp819;void*_tmp818;(_tmp818=0,Cyc_Tcutil_terr(e1->loc,((_tmp819="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp819,sizeof(char),49))),_tag_dyneither(_tmp818,sizeof(void*),0)));}
# 1025
_tmp243=_tmp243->tl;
# 1026
_tmp258=_tmp258->tl;
# 1027
++ _tmp244;}{
# 1032
int args_already_checked=0;
# 1033
{struct Cyc_List_List*a=_tmp25C;for(0;a != 0;a=a->tl){
# 1034
void*_tmp264=(void*)a->hd;enum Cyc_Absyn_Format_Type _tmp266;int _tmp267;int _tmp268;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp265=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp264;if(_tmp265->tag != 19)goto _LL138;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;_tmp268=_tmp265->f3;}}_LL137:
# 1036
{struct _handler_cons _tmp269;_push_handler(& _tmp269);{int _tmp26B=0;if(setjmp(_tmp269.handler))_tmp26B=1;if(!_tmp26B){
# 1038
{struct Cyc_Absyn_Exp*_tmp26C=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp267 - 1);
# 1040
struct Cyc_Core_Opt*fmt_args;
# 1041
if(_tmp268 == 0)
# 1042
fmt_args=0;else{
# 1044
struct Cyc_Core_Opt*_tmp81A;fmt_args=((_tmp81A=_cycalloc(sizeof(*_tmp81A)),((_tmp81A->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp268 - 1),_tmp81A))));}
# 1045
args_already_checked=1;{
# 1046
struct _RegionHandle _tmp26E=_new_region("temp");struct _RegionHandle*temp=& _tmp26E;_push_region(temp);
# 1047
switch(_tmp266){case Cyc_Absyn_Printf_ft: _LL13A:
# 1049
 Cyc_Tcexp_check_format_args(_tmp246,_tmp26C,fmt_args,_tmp268 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1052
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1054
 Cyc_Tcexp_check_format_args(_tmp246,_tmp26C,fmt_args,_tmp268 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1057
break;}
# 1047
;_pop_region(temp);};}
# 1038
;_pop_handler();}else{void*_tmp26A=(void*)_exn_thrown;void*_tmp270=_tmp26A;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp271=(struct Cyc_List_Nth_exn_struct*)_tmp270;if(_tmp271->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1061
{const char*_tmp81D;void*_tmp81C;(_tmp81C=0,Cyc_Tcutil_terr(loc,((_tmp81D="bad format arguments",_tag_dyneither(_tmp81D,sizeof(char),21))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}goto _LL13D;_LL140:;_LL141:(void)_throw(_tmp270);_LL13D:;}};}
# 1063
goto _LL135;_LL138:;_LL139:
# 1064
 goto _LL135;_LL135:;}}
# 1067
if(_tmp258 != 0){const char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(loc,((_tmp820="too few arguments for function",_tag_dyneither(_tmp820,sizeof(char),31))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}else{
# 1069
if((_tmp243 != 0  || _tmp259) || _tmp25A != 0){
# 1070
if(_tmp259)
# 1071
for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
# 1072
Cyc_Tcexp_tcExp(_tmp246,0,(struct Cyc_Absyn_Exp*)_tmp243->hd);}else{
# 1073
if(_tmp25A == 0){
# 1074
const char*_tmp823;void*_tmp822;(_tmp822=0,Cyc_Tcutil_terr(loc,((_tmp823="too many arguments for function",_tag_dyneither(_tmp823,sizeof(char),32))),_tag_dyneither(_tmp822,sizeof(void*),0)));}else{
# 1076
void*_tmp279;int _tmp27A;struct Cyc_Absyn_VarargInfo _tmp278=*_tmp25A;_tmp279=_tmp278.type;_tmp27A=_tmp278.inject;{
# 1077
struct Cyc_Absyn_VarargCallInfo*_tmp824;struct Cyc_Absyn_VarargCallInfo*_tmp27B=(_tmp824=_cycalloc(sizeof(*_tmp824)),((_tmp824->num_varargs=0,((_tmp824->injectors=0,((_tmp824->vai=(struct Cyc_Absyn_VarargInfo*)_tmp25A,_tmp824)))))));
# 1080
*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp27B;
# 1082
if(!_tmp27A)
# 1084
for(0;_tmp243 != 0;(_tmp243=_tmp243->tl,_tmp244 ++)){
# 1085
int alias_coercion=0;
# 1086
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp243->hd;
# 1087
++ _tmp27B->num_varargs;
# 1088
Cyc_Tcexp_tcExp(_tmp246,(void**)& _tmp279,e1);
# 1089
if(!Cyc_Tcutil_coerce_arg(_tmp246,e1,_tmp279,& alias_coercion)){
# 1090
{const char*_tmp829;void*_tmp828[2];struct Cyc_String_pa_PrintArg_struct _tmp827;struct Cyc_String_pa_PrintArg_struct _tmp826;(_tmp826.tag=0,((_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1091
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp827.tag=0,((_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp279)),((_tmp828[0]=& _tmp827,((_tmp828[1]=& _tmp826,Cyc_Tcutil_terr(loc,((_tmp829="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp829,sizeof(char),49))),_tag_dyneither(_tmp828,sizeof(void*),2)))))))))))));}
# 1092
Cyc_Tcutil_explain_failure();}
# 1094
if(alias_coercion){
# 1095
struct Cyc_List_List*_tmp82A;*alias_arg_exps=(struct Cyc_List_List*)((_tmp82A=_cycalloc(sizeof(*_tmp82A)),((_tmp82A->hd=(void*)_tmp244,((_tmp82A->tl=*alias_arg_exps,_tmp82A))))));}
# 1096
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp279) && !
# 1097
Cyc_Tcutil_is_noalias_path(ter,e1)){
# 1098
const char*_tmp82D;void*_tmp82C;(_tmp82C=0,Cyc_Tcutil_terr(e1->loc,((_tmp82D="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp82D,sizeof(char),49))),_tag_dyneither(_tmp82C,sizeof(void*),0)));}}else{
# 1103
void*_tmp283=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp279));struct Cyc_Absyn_DatatypeInfo _tmp285;union Cyc_Absyn_DatatypeInfoU _tmp286;struct Cyc_Absyn_Datatypedecl**_tmp287;struct Cyc_Absyn_Datatypedecl*_tmp288;struct Cyc_List_List*_tmp289;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp284=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp283;if(_tmp284->tag != 3)goto _LL145;else{_tmp285=_tmp284->f1;_tmp286=_tmp285.datatype_info;if((_tmp286.KnownDatatype).tag != 2)goto _LL145;_tmp287=(struct Cyc_Absyn_Datatypedecl**)(_tmp286.KnownDatatype).val;_tmp288=*_tmp287;_tmp289=_tmp285.targs;}}_LL144: {
# 1107
struct Cyc_Absyn_Datatypedecl*_tmp28A=*Cyc_Tcenv_lookup_datatypedecl(_tmp246,loc,_tmp288->name);
# 1108
struct Cyc_List_List*fields=0;
# 1109
if(_tmp28A->fields == 0){
# 1110
const char*_tmp831;void*_tmp830[1];struct Cyc_String_pa_PrintArg_struct _tmp82F;(_tmp82F.tag=0,((_tmp82F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1111
Cyc_Absynpp_typ2string(_tmp279)),((_tmp830[0]=& _tmp82F,Cyc_Tcutil_terr(loc,((_tmp831="can't inject into abstract datatype %s",_tag_dyneither(_tmp831,sizeof(char),39))),_tag_dyneither(_tmp830,sizeof(void*),1)))))));}else{
# 1112
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp28A->fields))->v;}
# 1119
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp279),Cyc_Tcenv_curr_rgn(_tmp246))){
# 1120
const char*_tmp836;void*_tmp835[2];struct Cyc_String_pa_PrintArg_struct _tmp834;struct Cyc_String_pa_PrintArg_struct _tmp833;(_tmp833.tag=0,((_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1121
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp246))),((_tmp834.tag=0,((_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp279)),((_tmp835[0]=& _tmp834,((_tmp835[1]=& _tmp833,Cyc_Tcutil_terr(loc,((_tmp836="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp836,sizeof(char),49))),_tag_dyneither(_tmp835,sizeof(void*),2)))))))))))));}{
# 1123
struct _RegionHandle _tmp292=_new_region("rgn");struct _RegionHandle*rgn=& _tmp292;_push_region(rgn);{
# 1124
struct Cyc_List_List*_tmp293=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp28A->tvs,_tmp289);
# 1125
for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
# 1126
++ _tmp27B->num_varargs;{
# 1127
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp243->hd;
# 1129
if(!args_already_checked){
# 1130
Cyc_Tcexp_tcExp(_tmp246,0,e1);
# 1131
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
# 1132
Cyc_Tcutil_is_noalias_path(rgn,e1)){
# 1133
const char*_tmp839;void*_tmp838;(_tmp838=0,Cyc_Tcutil_terr(e1->loc,((_tmp839="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp839,sizeof(char),49))),_tag_dyneither(_tmp838,sizeof(void*),0)));}}{
# 1135
struct Cyc_Absyn_Datatypefield*_tmp296=Cyc_Tcexp_tcInjection(_tmp246,e1,Cyc_Tcutil_pointer_elt_type(_tmp279),rgn,_tmp293,fields);
# 1137
if(_tmp296 != 0){
# 1138
struct Cyc_List_List*_tmp83A;_tmp27B->injectors=
# 1139
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp27B->injectors,(
# 1140
(_tmp83A=_cycalloc(sizeof(*_tmp83A)),((_tmp83A->hd=(struct Cyc_Absyn_Datatypefield*)_tmp296,((_tmp83A->tl=0,_tmp83A)))))));}};};}}
# 1143
_npop_handler(0);goto _LL142;
# 1123
;_pop_region(rgn);};}_LL145:;_LL146:
# 1144
{const char*_tmp83D;void*_tmp83C;(_tmp83C=0,Cyc_Tcutil_terr(loc,((_tmp83D="bad inject vararg type",_tag_dyneither(_tmp83D,sizeof(char),23))),_tag_dyneither(_tmp83C,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1151
Cyc_Tcenv_check_effect_accessible(_tmp246,loc,(void*)_check_null(_tmp255));
# 1154
Cyc_Tcenv_check_rgn_partial_order(_tmp246,loc,_tmp25B);{
# 1155
void*_tmp29B=_tmp257;_npop_handler(0);return _tmp29B;};};_LL133:;_LL134: {
# 1156
const char*_tmp840;void*_tmp83F;void*_tmp29E=(_tmp83F=0,Cyc_Tcexp_expr_err(_tmp246,loc,topt,((_tmp840="expected pointer to function",_tag_dyneither(_tmp840,sizeof(char),29))),_tag_dyneither(_tmp83F,sizeof(void*),0)));_npop_handler(0);return _tmp29E;}_LL130:;};_LL12E:;_LL12F: {
# 1158
const char*_tmp843;void*_tmp842;void*_tmp2A1=(_tmp842=0,Cyc_Tcexp_expr_err(_tmp246,loc,topt,((_tmp843="expected pointer to function",_tag_dyneither(_tmp843,sizeof(char),29))),_tag_dyneither(_tmp842,sizeof(void*),0)));_npop_handler(0);return _tmp2A1;}_LL12B:;};}
# 984
;_pop_region(ter);}
# 1164
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
# 1165
int bogus=0;
# 1166
void*_tmp2A2=Cyc_Absyn_exn_typ();
# 1167
Cyc_Tcexp_tcExp(te,(void**)& _tmp2A2,e);
# 1168
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp2A2,& bogus)){
# 1169
const char*_tmp848;void*_tmp847[2];struct Cyc_String_pa_PrintArg_struct _tmp846;struct Cyc_String_pa_PrintArg_struct _tmp845;(_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1170
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp846.tag=0,((_tmp846.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1169
Cyc_Absynpp_typ2string(_tmp2A2)),((_tmp847[0]=& _tmp846,((_tmp847[1]=& _tmp845,Cyc_Tcutil_terr(loc,((_tmp848="expected %s but found %s",_tag_dyneither(_tmp848,sizeof(char),25))),_tag_dyneither(_tmp847,sizeof(void*),2)))))))))))));}
# 1171
if(topt != 0)return*topt;
# 1172
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1176
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1178
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1180
e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
# 1181
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1182
{void*_tmp2A7=t1;struct Cyc_Absyn_PtrInfo _tmp2A9;void*_tmp2AA;struct Cyc_Absyn_Tqual _tmp2AB;struct Cyc_Absyn_PtrAtts _tmp2AC;void*_tmp2AD;union Cyc_Absyn_Constraint*_tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;union Cyc_Absyn_Constraint*_tmp2B0;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A7;if(_tmp2A8->tag != 5)goto _LL14A;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A9.elt_typ;_tmp2AB=_tmp2A9.elt_tq;_tmp2AC=_tmp2A9.ptr_atts;_tmp2AD=_tmp2AC.rgn;_tmp2AE=_tmp2AC.nullable;_tmp2AF=_tmp2AC.bounds;_tmp2B0=_tmp2AC.zero_term;}}_LL149:
# 1184
{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2AA);struct Cyc_Absyn_FnInfo _tmp2B3;struct Cyc_List_List*_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Tqual _tmp2B6;void*_tmp2B7;struct Cyc_List_List*_tmp2B8;int _tmp2B9;struct Cyc_Absyn_VarargInfo*_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BC;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2B2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B1;if(_tmp2B2->tag != 9)goto _LL14F;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B3.tvars;_tmp2B5=_tmp2B3.effect;_tmp2B6=_tmp2B3.ret_tqual;_tmp2B7=_tmp2B3.ret_typ;_tmp2B8=_tmp2B3.args;_tmp2B9=_tmp2B3.c_varargs;_tmp2BA=_tmp2B3.cyc_varargs;_tmp2BB=_tmp2B3.rgn_po;_tmp2BC=_tmp2B3.attributes;}}_LL14E: {
# 1186
struct _RegionHandle _tmp2BD=_new_region("temp");struct _RegionHandle*temp=& _tmp2BD;_push_region(temp);
# 1187
{struct Cyc_List_List*instantiation=0;
# 1189
for(0;ts != 0  && _tmp2B4 != 0;(ts=ts->tl,_tmp2B4=_tmp2B4->tl)){
# 1190
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2B4->hd,& Cyc_Tcutil_bk);
# 1191
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
# 1192
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
# 1193
struct _tuple13*_tmp84B;struct Cyc_List_List*_tmp84A;instantiation=(
# 1194
(_tmp84A=_region_malloc(temp,sizeof(*_tmp84A)),((_tmp84A->hd=((_tmp84B=_region_malloc(temp,sizeof(*_tmp84B)),((_tmp84B->f1=(struct Cyc_Absyn_Tvar*)_tmp2B4->hd,((_tmp84B->f2=(void*)ts->hd,_tmp84B)))))),((_tmp84A->tl=instantiation,_tmp84A))))));};}
# 1196
if(ts != 0){
# 1197
const char*_tmp84E;void*_tmp84D;void*_tmp2C2=
# 1198
(_tmp84D=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp84E="too many type variables in instantiation",_tag_dyneither(_tmp84E,sizeof(char),41))),_tag_dyneither(_tmp84D,sizeof(void*),0)));_npop_handler(0);return _tmp2C2;}{
# 1199
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp854;struct Cyc_Absyn_FnInfo _tmp853;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp852;void*new_fn_typ=
# 1200
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
# 1201
(_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852[0]=((_tmp854.tag=9,((_tmp854.f1=((_tmp853.tvars=_tmp2B4,((_tmp853.effect=_tmp2B5,((_tmp853.ret_tqual=_tmp2B6,((_tmp853.ret_typ=_tmp2B7,((_tmp853.args=_tmp2B8,((_tmp853.c_varargs=_tmp2B9,((_tmp853.cyc_varargs=_tmp2BA,((_tmp853.rgn_po=_tmp2BB,((_tmp853.attributes=_tmp2BC,_tmp853)))))))))))))))))),_tmp854)))),_tmp852)))));
# 1203
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp85E;struct Cyc_Absyn_PtrAtts _tmp85D;struct Cyc_Absyn_PtrInfo _tmp85C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp85B;void*_tmp2C7=(void*)((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85E.tag=5,((_tmp85E.f1=((_tmp85C.elt_typ=new_fn_typ,((_tmp85C.elt_tq=_tmp2AB,((_tmp85C.ptr_atts=((_tmp85D.rgn=_tmp2AD,((_tmp85D.nullable=_tmp2AE,((_tmp85D.bounds=_tmp2AF,((_tmp85D.zero_term=_tmp2B0,((_tmp85D.ptrloc=0,_tmp85D)))))))))),_tmp85C)))))),_tmp85E)))),_tmp85B))));_npop_handler(0);return _tmp2C7;};}
# 1187
;_pop_region(temp);}_LL14F:;_LL150:
# 1205
 goto _LL14C;_LL14C:;}
# 1207
goto _LL147;_LL14A:;_LL14B:
# 1208
 goto _LL147;_LL147:;}{
# 1210
const char*_tmp862;void*_tmp861[1];struct Cyc_String_pa_PrintArg_struct _tmp860;return(_tmp860.tag=0,((_tmp860.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1211
Cyc_Absynpp_typ2string(t1)),((_tmp861[0]=& _tmp860,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp862="expecting polymorphic type but found %s",_tag_dyneither(_tmp862,sizeof(char),40))),_tag_dyneither(_tmp861,sizeof(void*),1)))))));};};}
# 1215
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1217
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
# 1218
Cyc_Tcutil_check_no_qual(loc,t);
# 1220
Cyc_Tcexp_tcExp(te,(void**)& t,e);{
# 1221
void*t2=(void*)_check_null(e->topt);
# 1222
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
# 1223
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1225
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
# 1226
if(crc != Cyc_Absyn_Unknown_coercion)
# 1227
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
# 1228
if(Cyc_Tcutil_zero_to_null(te,t,e))
# 1229
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1232
Cyc_Tcutil_unify(t2,t);{
# 1233
const char*_tmp867;void*_tmp866[2];struct Cyc_String_pa_PrintArg_struct _tmp865;struct Cyc_String_pa_PrintArg_struct _tmp864;void*_tmp2CE=(_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1234
Cyc_Absynpp_typ2string(t)),((_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp866[0]=& _tmp865,((_tmp866[1]=& _tmp864,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp867="cannot cast %s to %s",_tag_dyneither(_tmp867,sizeof(char),21))),_tag_dyneither(_tmp866,sizeof(void*),2)))))))))))));
# 1235
Cyc_Tcutil_explain_failure();
# 1236
return _tmp2CE;};}}}
# 1242
{struct _tuple0 _tmp868;struct _tuple0 _tmp2D4=(_tmp868.f1=e->r,((_tmp868.f2=Cyc_Tcutil_compress(t),_tmp868)));void*_tmp2D5;struct Cyc_Absyn_MallocInfo _tmp2D7;int _tmp2D8;void*_tmp2D9;struct Cyc_Absyn_PtrInfo _tmp2DB;struct Cyc_Absyn_PtrAtts _tmp2DC;union Cyc_Absyn_Constraint*_tmp2DD;union Cyc_Absyn_Constraint*_tmp2DE;union Cyc_Absyn_Constraint*_tmp2DF;_LL152: _tmp2D5=_tmp2D4.f1;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2D6=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D5;if(_tmp2D6->tag != 32)goto _LL154;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D7.fat_result;}};_tmp2D9=_tmp2D4.f2;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9;if(_tmp2DA->tag != 5)goto _LL154;else{_tmp2DB=_tmp2DA->f1;_tmp2DC=_tmp2DB.ptr_atts;_tmp2DD=_tmp2DC.nullable;_tmp2DE=_tmp2DC.bounds;_tmp2DF=_tmp2DC.zero_term;}};_LL153:
# 1246
 if((_tmp2D8  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DF)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DD)){
# 1247
void*_tmp2E0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2DE);struct Cyc_Absyn_Exp*_tmp2E2;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2E1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2E0;if(_tmp2E1->tag != 1)goto _LL159;else{_tmp2E2=_tmp2E1->f1;}}_LL158:
# 1249
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2E2)).f1 == 1){
# 1250
const char*_tmp86B;void*_tmp86A;(_tmp86A=0,Cyc_Tcutil_warn(loc,((_tmp86B="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp86B,sizeof(char),60))),_tag_dyneither(_tmp86A,sizeof(void*),0)));}
# 1251
goto _LL156;_LL159:;_LL15A:
# 1252
 goto _LL156;_LL156:;}
# 1255
goto _LL151;_LL154:;_LL155:
# 1256
 goto _LL151;_LL151:;}
# 1258
return t;};}
# 1262
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1264
void**_tmp2E5=0;
# 1265
struct Cyc_Absyn_Tqual _tmp2E6=Cyc_Absyn_empty_tqual(0);
# 1266
if(topt != 0){
# 1267
void*_tmp2E7=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp2E9;void*_tmp2EA;void**_tmp2EB;struct Cyc_Absyn_Tqual _tmp2EC;struct Cyc_Absyn_PtrAtts _tmp2ED;union Cyc_Absyn_Constraint*_tmp2EE;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7;if(_tmp2E8->tag != 5)goto _LL15E;else{_tmp2E9=_tmp2E8->f1;_tmp2EA=_tmp2E9.elt_typ;_tmp2EB=(void**)&(_tmp2E8->f1).elt_typ;_tmp2EC=_tmp2E9.elt_tq;_tmp2ED=_tmp2E9.ptr_atts;_tmp2EE=_tmp2ED.zero_term;}}_LL15D:
# 1269
 _tmp2E5=(void**)_tmp2EB;
# 1270
_tmp2E6=_tmp2EC;
# 1271
goto _LL15B;_LL15E:;_LL15F:
# 1272
 goto _LL15B;_LL15B:;}{
# 1282
struct _RegionHandle _tmp2EF=_new_region("r");struct _RegionHandle*r=& _tmp2EF;_push_region(r);
# 1283
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),_tmp2E5,e);
# 1285
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
# 1286
const char*_tmp86E;void*_tmp86D;(_tmp86D=0,Cyc_Tcutil_terr(e->loc,((_tmp86E="Cannot take the address of an alias-free path",_tag_dyneither(_tmp86E,sizeof(char),46))),_tag_dyneither(_tmp86D,sizeof(void*),0)));}
# 1292
{void*_tmp2F2=e->r;struct Cyc_Absyn_Exp*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2F3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2F3->tag != 22)goto _LL163;else{_tmp2F4=_tmp2F3->f1;_tmp2F5=_tmp2F3->f2;}}_LL162:
# 1294
{void*_tmp2F6=Cyc_Tcutil_compress((void*)_check_null(_tmp2F4->topt));_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F6;if(_tmp2F7->tag != 10)goto _LL168;}_LL167:
# 1295
 goto _LL165;_LL168:;_LL169:
# 1302
 e0->r=(Cyc_Absyn_add_exp(_tmp2F4,_tmp2F5,0))->r;{
# 1303
void*_tmp2F8=Cyc_Tcexp_tcPlus(te,_tmp2F4,_tmp2F5);_npop_handler(0);return _tmp2F8;};_LL165:;}
# 1305
goto _LL160;_LL163:;_LL164:
# 1306
 goto _LL160;_LL160:;}
# 1310
{void*_tmp2F9=e->r;int _tmp2FB;int _tmp2FD;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2FA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F9;if(_tmp2FA->tag != 20)goto _LL16D;else{_tmp2FB=_tmp2FA->f3;if(_tmp2FB != 1)goto _LL16D;}}_LL16C:
# 1311
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2FC=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F9;if(_tmp2FC->tag != 21)goto _LL16F;else{_tmp2FD=_tmp2FC->f3;if(_tmp2FD != 1)goto _LL16F;}}_LL16E:
# 1313
{const char*_tmp871;void*_tmp870;(_tmp870=0,Cyc_Tcutil_terr(e->loc,((_tmp871="cannot take the address of a @tagged union member",_tag_dyneither(_tmp871,sizeof(char),50))),_tag_dyneither(_tmp870,sizeof(void*),0)));}
# 1314
goto _LL16A;_LL16F:;_LL170:
# 1315
 goto _LL16A;_LL16A:;}{
# 1319
int _tmp301;void*_tmp302;struct _tuple14 _tmp300=Cyc_Tcutil_addressof_props(te,e);_tmp301=_tmp300.f1;_tmp302=_tmp300.f2;
# 1320
if(Cyc_Tcutil_is_noalias_region(_tmp302,0)){
# 1321
const char*_tmp874;void*_tmp873;(_tmp873=0,Cyc_Tcutil_terr(e->loc,((_tmp874="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp874,sizeof(char),60))),_tag_dyneither(_tmp873,sizeof(void*),0)));}{
# 1323
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
# 1324
if(_tmp301){
# 1325
tq.print_const=1;
# 1326
tq.real_const=1;}{
# 1329
void*t=Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp302,tq,Cyc_Absyn_false_conref);
# 1330
void*_tmp305=t;_npop_handler(0);return _tmp305;};};};
# 1282
;_pop_region(r);};}
# 1334
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
# 1335
if(te->allow_valueof)
# 1338
return Cyc_Absyn_uint_typ;
# 1340
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
# 1341
Cyc_Tcutil_check_no_qual(loc,t);
# 1342
if(!Cyc_Evexp_okay_szofarg(t)){
# 1343
const char*_tmp878;void*_tmp877[1];struct Cyc_String_pa_PrintArg_struct _tmp876;(_tmp876.tag=0,((_tmp876.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1344
Cyc_Absynpp_typ2string(t)),((_tmp877[0]=& _tmp876,Cyc_Tcutil_terr(loc,((_tmp878="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp878,sizeof(char),55))),_tag_dyneither(_tmp877,sizeof(void*),1)))))));}
# 1345
if(topt != 0){
# 1346
void*_tmp309=Cyc_Tcutil_compress(*topt);void*_tmp30B;_LL172: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp30A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp309;if(_tmp30A->tag != 19)goto _LL174;else{_tmp30B=(void*)_tmp30A->f1;}}_LL173: {
# 1349
struct Cyc_Absyn_Exp*_tmp30C=Cyc_Absyn_sizeoftyp_exp(t,0);
# 1350
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp87B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp87A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp30D=(_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87B.tag=18,((_tmp87B.f1=_tmp30C,_tmp87B)))),_tmp87A)));
# 1351
if(Cyc_Tcutil_unify(_tmp30B,(void*)_tmp30D))return _tmp309;
# 1352
goto _LL171;}_LL174:;_LL175:
# 1353
 goto _LL171;_LL171:;}
# 1355
return Cyc_Absyn_uint_typ;}
# 1358
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
# 1359
return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}
# 1365
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,void*n){
# 1367
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
# 1368
Cyc_Tcutil_check_no_qual(loc,t);
# 1369
{void*_tmp310=n;struct _dyneither_ptr*_tmp312;unsigned int _tmp314;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp311=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp310;if(_tmp311->tag != 0)goto _LL179;else{_tmp312=_tmp311->f1;}}_LL178: {
# 1371
int bad_type=1;
# 1372
{void*_tmp315=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp317;union Cyc_Absyn_AggrInfoU _tmp318;struct Cyc_Absyn_Aggrdecl**_tmp319;struct Cyc_List_List*_tmp31B;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp315;if(_tmp316->tag != 11)goto _LL17E;else{_tmp317=_tmp316->f1;_tmp318=_tmp317.aggr_info;if((_tmp318.KnownAggr).tag != 2)goto _LL17E;_tmp319=(struct Cyc_Absyn_Aggrdecl**)(_tmp318.KnownAggr).val;}}_LL17D:
# 1374
 if((*_tmp319)->impl == 0)
# 1375
goto _LL17B;
# 1376
if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp312,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp319)->impl))->fields)){
# 1377
const char*_tmp87F;void*_tmp87E[1];struct Cyc_String_pa_PrintArg_struct _tmp87D;(_tmp87D.tag=0,((_tmp87D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp312),((_tmp87E[0]=& _tmp87D,Cyc_Tcutil_terr(loc,((_tmp87F="no field of struct/union has name %s",_tag_dyneither(_tmp87F,sizeof(char),37))),_tag_dyneither(_tmp87E,sizeof(void*),1)))))));}
# 1378
bad_type=0;
# 1379
goto _LL17B;_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp315;if(_tmp31A->tag != 12)goto _LL180;else{_tmp31B=_tmp31A->f2;}}_LL17F:
# 1381
 if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp312,_tmp31B)){
# 1382
const char*_tmp883;void*_tmp882[1];struct Cyc_String_pa_PrintArg_struct _tmp881;(_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp312),((_tmp882[0]=& _tmp881,Cyc_Tcutil_terr(loc,((_tmp883="no field of struct/union has name %s",_tag_dyneither(_tmp883,sizeof(char),37))),_tag_dyneither(_tmp882,sizeof(void*),1)))))));}
# 1383
bad_type=0;
# 1384
goto _LL17B;_LL180:;_LL181:
# 1385
 goto _LL17B;_LL17B:;}
# 1387
if(bad_type){
# 1388
const char*_tmp887;void*_tmp886[1];struct Cyc_String_pa_PrintArg_struct _tmp885;(_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp886[0]=& _tmp885,Cyc_Tcutil_terr(loc,((_tmp887="%s is not a known struct/union type",_tag_dyneither(_tmp887,sizeof(char),36))),_tag_dyneither(_tmp886,sizeof(void*),1)))))));}
# 1389
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp313=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp310;if(_tmp313->tag != 1)goto _LL176;else{_tmp314=_tmp313->f1;}}_LL17A: {
# 1391
int bad_type=1;
# 1392
{void*_tmp325=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp327;union Cyc_Absyn_AggrInfoU _tmp328;struct Cyc_Absyn_Aggrdecl**_tmp329;struct Cyc_List_List*_tmp32B;struct Cyc_List_List*_tmp32D;struct Cyc_Absyn_DatatypeFieldInfo _tmp32F;union Cyc_Absyn_DatatypeFieldInfoU _tmp330;struct _tuple2 _tmp331;struct Cyc_Absyn_Datatypefield*_tmp332;_LL183: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 11)goto _LL185;else{_tmp327=_tmp326->f1;_tmp328=_tmp327.aggr_info;if((_tmp328.KnownAggr).tag != 2)goto _LL185;_tmp329=(struct Cyc_Absyn_Aggrdecl**)(_tmp328.KnownAggr).val;}}_LL184:
# 1394
 if((*_tmp329)->impl == 0)
# 1395
goto _LL182;
# 1396
_tmp32B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp329)->impl))->fields;goto _LL186;_LL185: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp325;if(_tmp32A->tag != 12)goto _LL187;else{_tmp32B=_tmp32A->f2;}}_LL186:
# 1398
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32B)<= _tmp314){
# 1399
const char*_tmp88C;void*_tmp88B[2];struct Cyc_Int_pa_PrintArg_struct _tmp88A;struct Cyc_Int_pa_PrintArg_struct _tmp889;(_tmp889.tag=1,((_tmp889.f1=(unsigned long)((int)_tmp314),((_tmp88A.tag=1,((_tmp88A.f1=(unsigned long)
# 1400
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32B),((_tmp88B[0]=& _tmp88A,((_tmp88B[1]=& _tmp889,Cyc_Tcutil_terr(loc,((_tmp88C="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp88C,sizeof(char),46))),_tag_dyneither(_tmp88B,sizeof(void*),2)))))))))))));}
# 1401
bad_type=0;
# 1402
goto _LL182;_LL187: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp325;if(_tmp32C->tag != 10)goto _LL189;else{_tmp32D=_tmp32C->f1;}}_LL188:
# 1404
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32D)<= _tmp314){
# 1405
const char*_tmp891;void*_tmp890[2];struct Cyc_Int_pa_PrintArg_struct _tmp88F;struct Cyc_Int_pa_PrintArg_struct _tmp88E;(_tmp88E.tag=1,((_tmp88E.f1=(unsigned long)((int)_tmp314),((_tmp88F.tag=1,((_tmp88F.f1=(unsigned long)
# 1406
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32D),((_tmp890[0]=& _tmp88F,((_tmp890[1]=& _tmp88E,Cyc_Tcutil_terr(loc,((_tmp891="tuple has too few components: %d <= %d",_tag_dyneither(_tmp891,sizeof(char),39))),_tag_dyneither(_tmp890,sizeof(void*),2)))))))))))));}
# 1407
bad_type=0;
# 1408
goto _LL182;_LL189: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp325;if(_tmp32E->tag != 4)goto _LL18B;else{_tmp32F=_tmp32E->f1;_tmp330=_tmp32F.field_info;if((_tmp330.KnownDatatypefield).tag != 2)goto _LL18B;_tmp331=(struct _tuple2)(_tmp330.KnownDatatypefield).val;_tmp332=_tmp331.f2;}}_LL18A:
# 1410
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp332->typs)< _tmp314){
# 1411
const char*_tmp896;void*_tmp895[2];struct Cyc_Int_pa_PrintArg_struct _tmp894;struct Cyc_Int_pa_PrintArg_struct _tmp893;(_tmp893.tag=1,((_tmp893.f1=(unsigned long)((int)_tmp314),((_tmp894.tag=1,((_tmp894.f1=(unsigned long)
# 1412
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp332->typs),((_tmp895[0]=& _tmp894,((_tmp895[1]=& _tmp893,Cyc_Tcutil_terr(loc,((_tmp896="datatype field has too few components: %d < %d",_tag_dyneither(_tmp896,sizeof(char),47))),_tag_dyneither(_tmp895,sizeof(void*),2)))))))))))));}
# 1413
bad_type=0;
# 1414
goto _LL182;_LL18B:;_LL18C:
# 1415
 goto _LL182;_LL182:;}
# 1417
if(bad_type){
# 1418
const char*_tmp89A;void*_tmp899[1];struct Cyc_String_pa_PrintArg_struct _tmp898;(_tmp898.tag=0,((_tmp898.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp899[0]=& _tmp898,Cyc_Tcutil_terr(loc,((_tmp89A="%s is not a known type",_tag_dyneither(_tmp89A,sizeof(char),23))),_tag_dyneither(_tmp899,sizeof(void*),1)))))));}
# 1419
goto _LL176;}_LL176:;}
# 1421
return Cyc_Absyn_uint_typ;}
# 1425
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
# 1426
struct _RegionHandle _tmp342=_new_region("r");struct _RegionHandle*r=& _tmp342;_push_region(r);
# 1427
{struct Cyc_Tcenv_Tenv*_tmp343=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
# 1428
Cyc_Tcexp_tcExp(_tmp343,0,e);{
# 1429
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1430
void*_tmp344=t;struct Cyc_Absyn_PtrInfo _tmp347;void*_tmp348;struct Cyc_Absyn_PtrAtts _tmp349;void*_tmp34A;union Cyc_Absyn_Constraint*_tmp34B;union Cyc_Absyn_Constraint*_tmp34C;_LL18E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp344;if(_tmp345->tag != 1)goto _LL190;}_LL18F: {
# 1432
struct Cyc_List_List*_tmp34D=Cyc_Tcenv_lookup_type_vars(_tmp343);
# 1433
struct Cyc_Core_Opt*_tmp89B;void*_tmp34E=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mko,((_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B->v=_tmp34D,_tmp89B)))));
# 1434
struct Cyc_Core_Opt*_tmp89C;void*_tmp34F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,((_tmp89C=_cycalloc(sizeof(*_tmp89C)),((_tmp89C->v=_tmp34D,_tmp89C)))));
# 1435
union Cyc_Absyn_Constraint*_tmp350=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
# 1436
union Cyc_Absyn_Constraint*_tmp351=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
# 1437
struct Cyc_Absyn_PtrAtts _tmp89D;struct Cyc_Absyn_PtrAtts _tmp352=(_tmp89D.rgn=_tmp34F,((_tmp89D.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp89D.bounds=_tmp350,((_tmp89D.zero_term=_tmp351,((_tmp89D.ptrloc=0,_tmp89D)))))))));
# 1438
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8A3;struct Cyc_Absyn_PtrInfo _tmp8A2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8A1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp353=(_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp8A3.tag=5,((_tmp8A3.f1=((_tmp8A2.elt_typ=_tmp34E,((_tmp8A2.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp8A2.ptr_atts=_tmp352,_tmp8A2)))))),_tmp8A3)))),_tmp8A1)));
# 1439
Cyc_Tcutil_unify(t,(void*)_tmp353);
# 1440
_tmp348=_tmp34E;_tmp34A=_tmp34F;_tmp34B=_tmp350;_tmp34C=_tmp351;goto _LL191;}_LL190: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp344;if(_tmp346->tag != 5)goto _LL192;else{_tmp347=_tmp346->f1;_tmp348=_tmp347.elt_typ;_tmp349=_tmp347.ptr_atts;_tmp34A=_tmp349.rgn;_tmp34B=_tmp349.bounds;_tmp34C=_tmp349.zero_term;}}_LL191:
# 1442
 Cyc_Tcenv_check_rgn_accessible(_tmp343,loc,_tmp34A);
# 1443
Cyc_Tcutil_check_nonzero_bound(loc,_tmp34B);
# 1444
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp348),& Cyc_Tcutil_tmk)){
# 1445
const char*_tmp8A6;void*_tmp8A5;(_tmp8A5=0,Cyc_Tcutil_terr(loc,((_tmp8A6="can't dereference abstract pointer type",_tag_dyneither(_tmp8A6,sizeof(char),40))),_tag_dyneither(_tmp8A5,sizeof(void*),0)));}{
# 1446
void*_tmp35C=_tmp348;_npop_handler(0);return _tmp35C;};_LL192:;_LL193: {
# 1448
const char*_tmp8AA;void*_tmp8A9[1];struct Cyc_String_pa_PrintArg_struct _tmp8A8;void*_tmp360=(_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8A9[0]=& _tmp8A8,Cyc_Tcexp_expr_err(_tmp343,loc,topt,((_tmp8AA="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp8AA,sizeof(char),39))),_tag_dyneither(_tmp8A9,sizeof(void*),1)))))));_npop_handler(0);return _tmp360;}_LL18D:;};}
# 1427
;_pop_region(r);}
# 1454
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1458
struct _RegionHandle _tmp361=_new_region("r");struct _RegionHandle*r=& _tmp361;_push_region(r);
# 1459
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_clear_notreadctxt(r,te),0,e);
# 1462
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
# 1463
void*_tmp362=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_AggrInfo _tmp364;union Cyc_Absyn_AggrInfoU _tmp365;struct Cyc_Absyn_Aggrdecl**_tmp366;struct Cyc_Absyn_Aggrdecl*_tmp367;struct Cyc_List_List*_tmp368;enum Cyc_Absyn_AggrKind _tmp36A;struct Cyc_List_List*_tmp36B;_LL195: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp362;if(_tmp363->tag != 11)goto _LL197;else{_tmp364=_tmp363->f1;_tmp365=_tmp364.aggr_info;if((_tmp365.KnownAggr).tag != 2)goto _LL197;_tmp366=(struct Cyc_Absyn_Aggrdecl**)(_tmp365.KnownAggr).val;_tmp367=*_tmp366;_tmp368=_tmp364.targs;}}_LL196: {
# 1465
struct Cyc_Absyn_Aggrfield*_tmp370=Cyc_Absyn_lookup_decl_field(_tmp367,f);
# 1466
if(_tmp370 == 0){
# 1467
const char*_tmp8AF;void*_tmp8AE[2];struct Cyc_String_pa_PrintArg_struct _tmp8AD;struct Cyc_String_pa_PrintArg_struct _tmp8AC;void*_tmp375=(_tmp8AC.tag=0,((_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8AD.tag=0,((_tmp8AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1468
Cyc_Absynpp_qvar2string(_tmp367->name)),((_tmp8AE[0]=& _tmp8AD,((_tmp8AE[1]=& _tmp8AC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8AF="type %s has no %s field",_tag_dyneither(_tmp8AF,sizeof(char),24))),_tag_dyneither(_tmp8AE,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp375;}
# 1470
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp367->impl))->tagged)*is_tagged=1;{
# 1471
void*t2=_tmp370->type;
# 1472
if(_tmp368 != 0){
# 1473
struct _RegionHandle _tmp376=_new_region("rgn");struct _RegionHandle*rgn=& _tmp376;_push_region(rgn);
# 1474
{struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp367->tvs,_tmp368);
# 1475
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp377,_tmp370->type);}
# 1474
;_pop_region(rgn);}
# 1479
if(((_tmp367->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp367->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t2)){
# 1480
const char*_tmp8B3;void*_tmp8B2[1];struct Cyc_String_pa_PrintArg_struct _tmp8B1;void*_tmp37B=(_tmp8B1.tag=0,((_tmp8B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B2[0]=& _tmp8B1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B3="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8B3,sizeof(char),56))),_tag_dyneither(_tmp8B2,sizeof(void*),1)))))));_npop_handler(0);return _tmp37B;}
# 1481
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp367->impl))->exist_vars != 0){
# 1484
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 1485
const char*_tmp8B7;void*_tmp8B6[1];struct Cyc_String_pa_PrintArg_struct _tmp8B5;void*_tmp37F=(_tmp8B5.tag=0,((_tmp8B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8B6[0]=& _tmp8B5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8B7="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8B7,sizeof(char),81))),_tag_dyneither(_tmp8B6,sizeof(void*),1)))))));_npop_handler(0);return _tmp37F;}}{
# 1487
void*_tmp380=t2;_npop_handler(0);return _tmp380;};};}_LL197: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp362;if(_tmp369->tag != 12)goto _LL199;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp369->f2;}}_LL198: {
# 1489
struct Cyc_Absyn_Aggrfield*_tmp381=Cyc_Absyn_lookup_field(_tmp36B,f);
# 1490
if(_tmp381 == 0){
# 1491
const char*_tmp8BB;void*_tmp8BA[1];struct Cyc_String_pa_PrintArg_struct _tmp8B9;void*_tmp385=(_tmp8B9.tag=0,((_tmp8B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8BA[0]=& _tmp8B9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8BB="type has no %s field",_tag_dyneither(_tmp8BB,sizeof(char),21))),_tag_dyneither(_tmp8BA,sizeof(void*),1)))))));_npop_handler(0);return _tmp385;}
# 1492
if((_tmp36A == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp381->type)){
# 1493
const char*_tmp8BF;void*_tmp8BE[1];struct Cyc_String_pa_PrintArg_struct _tmp8BD;void*_tmp389=(_tmp8BD.tag=0,((_tmp8BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8BE[0]=& _tmp8BD,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8BF="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8BF,sizeof(char),56))),_tag_dyneither(_tmp8BE,sizeof(void*),1)))))));_npop_handler(0);return _tmp389;}{
# 1494
void*_tmp38A=_tmp381->type;_npop_handler(0);return _tmp38A;};}_LL199:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp362;if(_tmp36C->tag != 8)goto _LL19B;}{const char*_tmp8C0;if(!(
# 1495
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8C0="size",_tag_dyneither(_tmp8C0,sizeof(char),5))))== 0))goto _LL19B;};_LL19A: goto _LL19C;_LL19B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362;if(_tmp36E->tag != 5)goto _LL19D;}{const char*_tmp8C1;if(!(
# 1496
Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8C1="size",_tag_dyneither(_tmp8C1,sizeof(char),5))))== 0))goto _LL19D;};_LL19C:
# 1497
{const char*_tmp8C5;void*_tmp8C4[1];struct Cyc_String_pa_PrintArg_struct _tmp8C3;(_tmp8C3.tag=0,((_tmp8C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp8C4[0]=& _tmp8C3,Cyc_Tcutil_warn(e->loc,((_tmp8C5="deprecated `.size' used here -- change to numelts(%s)",_tag_dyneither(_tmp8C5,sizeof(char),54))),_tag_dyneither(_tmp8C4,sizeof(void*),1)))))));}
# 1498
{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp8CB;struct Cyc_List_List*_tmp8CA;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp8C9;outer_e->r=(void*)((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CB.tag=2,((_tmp8CB.f1=Cyc_Absyn_Numelts,((_tmp8CB.f2=((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA->hd=e,((_tmp8CA->tl=0,_tmp8CA)))))),_tmp8CB)))))),_tmp8C9))));}{
# 1499
void*_tmp391=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp391;};_LL19D:;_LL19E: {
# 1501
const char*_tmp8CC;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp8CC="size",_tag_dyneither(_tmp8CC,sizeof(char),5))))== 0){
# 1502
const char*_tmp8D0;void*_tmp8CF[1];struct Cyc_String_pa_PrintArg_struct _tmp8CE;void*_tmp396=(_tmp8CE.tag=0,((_tmp8CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1504
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8CF[0]=& _tmp8CE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D0="expecting struct, union, or array, found %s",_tag_dyneither(_tmp8D0,sizeof(char),44))),_tag_dyneither(_tmp8CF,sizeof(void*),1)))))));_npop_handler(0);return _tmp396;}else{
# 1506
const char*_tmp8D4;void*_tmp8D3[1];struct Cyc_String_pa_PrintArg_struct _tmp8D2;void*_tmp39A=(_tmp8D2.tag=0,((_tmp8D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1507
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8D3[0]=& _tmp8D2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D4="expecting struct or union, found %s",_tag_dyneither(_tmp8D4,sizeof(char),36))),_tag_dyneither(_tmp8D3,sizeof(void*),1)))))));_npop_handler(0);return _tmp39A;}}_LL194:;};
# 1458
;_pop_region(r);}
# 1512
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1515
struct _RegionHandle _tmp39B=_new_region("r");struct _RegionHandle*r=& _tmp39B;_push_region(r);
# 1516
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te)),0,e);
# 1519
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
# 1520
{void*_tmp39C=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp39E;void*_tmp39F;struct Cyc_Absyn_PtrAtts _tmp3A0;void*_tmp3A1;union Cyc_Absyn_Constraint*_tmp3A2;union Cyc_Absyn_Constraint*_tmp3A3;_LL1A0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp39D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C;if(_tmp39D->tag != 5)goto _LL1A2;else{_tmp39E=_tmp39D->f1;_tmp39F=_tmp39E.elt_typ;_tmp3A0=_tmp39E.ptr_atts;_tmp3A1=_tmp3A0.rgn;_tmp3A2=_tmp3A0.bounds;_tmp3A3=_tmp3A0.zero_term;}}_LL1A1:
# 1522
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp3A2);
# 1523
{void*_tmp3A4=Cyc_Tcutil_compress(_tmp39F);struct Cyc_Absyn_AggrInfo _tmp3A6;union Cyc_Absyn_AggrInfoU _tmp3A7;struct Cyc_Absyn_Aggrdecl**_tmp3A8;struct Cyc_Absyn_Aggrdecl*_tmp3A9;struct Cyc_List_List*_tmp3AA;enum Cyc_Absyn_AggrKind _tmp3AC;struct Cyc_List_List*_tmp3AD;_LL1A5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3A5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A4;if(_tmp3A5->tag != 11)goto _LL1A7;else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A6.aggr_info;if((_tmp3A7.KnownAggr).tag != 2)goto _LL1A7;_tmp3A8=(struct Cyc_Absyn_Aggrdecl**)(_tmp3A7.KnownAggr).val;_tmp3A9=*_tmp3A8;_tmp3AA=_tmp3A6.targs;}}_LL1A6: {
# 1525
struct Cyc_Absyn_Aggrfield*_tmp3AE=Cyc_Absyn_lookup_decl_field(_tmp3A9,f);
# 1526
if(_tmp3AE == 0){
# 1527
const char*_tmp8D9;void*_tmp8D8[2];struct Cyc_String_pa_PrintArg_struct _tmp8D7;struct Cyc_String_pa_PrintArg_struct _tmp8D6;void*_tmp3B3=(_tmp8D6.tag=0,((_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D7.tag=0,((_tmp8D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1528
Cyc_Absynpp_qvar2string(_tmp3A9->name)),((_tmp8D8[0]=& _tmp8D7,((_tmp8D8[1]=& _tmp8D6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D9="type %s has no %s field",_tag_dyneither(_tmp8D9,sizeof(char),24))),_tag_dyneither(_tmp8D8,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp3B3;}
# 1530
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A9->impl))->tagged)*is_tagged=1;{
# 1531
void*t3=_tmp3AE->type;
# 1532
if(_tmp3AA != 0){
# 1533
struct _RegionHandle _tmp3B4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3B4;_push_region(rgn);
# 1534
{struct Cyc_List_List*_tmp3B5=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp3A9->tvs,_tmp3AA);
# 1535
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp3B5,_tmp3AE->type);}
# 1534
;_pop_region(rgn);}{
# 1538
struct Cyc_Absyn_Kind*_tmp3B6=Cyc_Tcutil_typ_kind(t3);
# 1539
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp3B6)){
# 1540
const char*_tmp8DD;void*_tmp8DC[1];struct Cyc_String_pa_PrintArg_struct _tmp8DB;void*_tmp3BA=(_tmp8DB.tag=0,((_tmp8DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DC[0]=& _tmp8DB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DD="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8DD,sizeof(char),48))),_tag_dyneither(_tmp8DC,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BA;}
# 1542
if(((_tmp3A9->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A9->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){
# 1543
const char*_tmp8E1;void*_tmp8E0[1];struct Cyc_String_pa_PrintArg_struct _tmp8DF;void*_tmp3BE=(_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E0[0]=& _tmp8DF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E1="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8E1,sizeof(char),56))),_tag_dyneither(_tmp8E0,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BE;}
# 1544
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A9->impl))->exist_vars != 0){
# 1545
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 1546
const char*_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_PrintArg_struct _tmp8E3;void*_tmp3C2=(_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E4[0]=& _tmp8E3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E5="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8E5,sizeof(char),72))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C2;}}{
# 1550
void*_tmp3C3=t3;_npop_handler(0);return _tmp3C3;};};};}_LL1A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3AB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A4;if(_tmp3AB->tag != 12)goto _LL1A9;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;}}_LL1A8: {
# 1552
struct Cyc_Absyn_Aggrfield*_tmp3C4=Cyc_Absyn_lookup_field(_tmp3AD,f);
# 1553
if(_tmp3C4 == 0){
# 1554
const char*_tmp8E9;void*_tmp8E8[1];struct Cyc_String_pa_PrintArg_struct _tmp8E7;void*_tmp3C8=(_tmp8E7.tag=0,((_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E8[0]=& _tmp8E7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E9="type has no %s field",_tag_dyneither(_tmp8E9,sizeof(char),21))),_tag_dyneither(_tmp8E8,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C8;}
# 1555
if((_tmp3AC == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(_tmp3C4->type)){
# 1556
const char*_tmp8ED;void*_tmp8EC[1];struct Cyc_String_pa_PrintArg_struct _tmp8EB;void*_tmp3CC=(_tmp8EB.tag=0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8EC[0]=& _tmp8EB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8ED="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8ED,sizeof(char),56))),_tag_dyneither(_tmp8EC,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CC;}{
# 1557
void*_tmp3CD=_tmp3C4->type;_npop_handler(0);return _tmp3CD;};}_LL1A9:;_LL1AA:
# 1558
 goto _LL1A4;_LL1A4:;}
# 1560
goto _LL19F;_LL1A2:;_LL1A3:
# 1561
 goto _LL19F;_LL19F:;}{
# 1563
const char*_tmp8F1;void*_tmp8F0[1];struct Cyc_String_pa_PrintArg_struct _tmp8EF;void*_tmp3D1=(_tmp8EF.tag=0,((_tmp8EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1564
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8F0[0]=& _tmp8EF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F1="expecting struct or union pointer, found %s",_tag_dyneither(_tmp8F1,sizeof(char),44))),_tag_dyneither(_tmp8F0,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D1;};
# 1515
;_pop_region(r);}
# 1568
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1570
unsigned int _tmp3D3;int _tmp3D4;struct _tuple15 _tmp3D2=Cyc_Evexp_eval_const_uint_exp(index);_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;
# 1571
if(!_tmp3D4){
# 1572
const char*_tmp8F4;void*_tmp8F3;return(_tmp8F3=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8F4="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp8F4,sizeof(char),47))),_tag_dyneither(_tmp8F3,sizeof(void*),0)));}{
# 1574
struct _handler_cons _tmp3D7;_push_handler(& _tmp3D7);{int _tmp3D9=0;if(setjmp(_tmp3D7.handler))_tmp3D9=1;if(!_tmp3D9){
# 1575
{void*_tmp3DA=(*((struct _tuple19*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3D3)).f2;_npop_handler(0);return _tmp3DA;};_pop_handler();}else{void*_tmp3D8=(void*)_exn_thrown;void*_tmp3DC=_tmp3D8;_LL1AC: {struct Cyc_List_Nth_exn_struct*_tmp3DD=(struct Cyc_List_Nth_exn_struct*)_tmp3DC;if(_tmp3DD->tag != Cyc_List_Nth)goto _LL1AE;}_LL1AD: {
# 1577
const char*_tmp8F9;void*_tmp8F8[2];struct Cyc_Int_pa_PrintArg_struct _tmp8F7;struct Cyc_Int_pa_PrintArg_struct _tmp8F6;return(_tmp8F6.tag=1,((_tmp8F6.f1=(unsigned long)
# 1578
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp8F7.tag=1,((_tmp8F7.f1=(unsigned long)((int)_tmp3D3),((_tmp8F8[0]=& _tmp8F7,((_tmp8F8[1]=& _tmp8F6,Cyc_Tcexp_expr_err(te,loc,0,((_tmp8F9="index is %d but tuple has only %d fields",_tag_dyneither(_tmp8F9,sizeof(char),41))),_tag_dyneither(_tmp8F8,sizeof(void*),2)))))))))))));}_LL1AE:;_LL1AF:(void)_throw(_tmp3DC);_LL1AB:;}};};}
# 1582
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1584
struct _RegionHandle _tmp3E2=_new_region("r");struct _RegionHandle*r=& _tmp3E2;_push_region(r);
# 1585
{struct Cyc_Tcenv_Tenv*_tmp3E3=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
# 1586
Cyc_Tcexp_tcExp(_tmp3E3,0,e1);
# 1587
Cyc_Tcexp_tcExp(_tmp3E3,0,e2);{
# 1588
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
# 1589
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
# 1590
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3E3,e2)){
# 1591
const char*_tmp8FD;void*_tmp8FC[1];struct Cyc_String_pa_PrintArg_struct _tmp8FB;void*_tmp3E7=(_tmp8FB.tag=0,((_tmp8FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1592
Cyc_Absynpp_typ2string(t2)),((_tmp8FC[0]=& _tmp8FB,Cyc_Tcexp_expr_err(_tmp3E3,e2->loc,topt,((_tmp8FD="expecting int subscript, found %s",_tag_dyneither(_tmp8FD,sizeof(char),34))),_tag_dyneither(_tmp8FC,sizeof(void*),1)))))));_npop_handler(0);return _tmp3E7;}{
# 1595
void*_tmp3E8=t1;struct Cyc_Absyn_PtrInfo _tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EC;struct Cyc_Absyn_PtrAtts _tmp3ED;void*_tmp3EE;union Cyc_Absyn_Constraint*_tmp3EF;union Cyc_Absyn_Constraint*_tmp3F0;struct Cyc_List_List*_tmp3F2;_LL1B1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E8;if(_tmp3E9->tag != 5)goto _LL1B3;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3EA.elt_typ;_tmp3EC=_tmp3EA.elt_tq;_tmp3ED=_tmp3EA.ptr_atts;_tmp3EE=_tmp3ED.rgn;_tmp3EF=_tmp3ED.bounds;_tmp3F0=_tmp3ED.zero_term;}}_LL1B2:
# 1599
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F0)){
# 1600
int emit_warning=1;
# 1601
{void*_tmp3F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3EF);struct Cyc_Absyn_Exp*_tmp3F5;_LL1B8: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3F4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F3;if(_tmp3F4->tag != 1)goto _LL1BA;else{_tmp3F5=_tmp3F4->f1;}}_LL1B9:
# 1603
 if(Cyc_Tcutil_is_const_exp(_tmp3E3,e2)){
# 1604
unsigned int _tmp3F8;int _tmp3F9;struct _tuple15 _tmp3F7=Cyc_Evexp_eval_const_uint_exp(e2);_tmp3F8=_tmp3F7.f1;_tmp3F9=_tmp3F7.f2;
# 1605
if(_tmp3F9){
# 1606
unsigned int _tmp3FB;int _tmp3FC;struct _tuple15 _tmp3FA=Cyc_Evexp_eval_const_uint_exp(_tmp3F5);_tmp3FB=_tmp3FA.f1;_tmp3FC=_tmp3FA.f2;
# 1607
if(_tmp3FC  && _tmp3FB > _tmp3F8)emit_warning=0;}}
# 1610
goto _LL1B7;_LL1BA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3F6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3F3;if(_tmp3F6->tag != 0)goto _LL1B7;}_LL1BB:
# 1611
 emit_warning=0;goto _LL1B7;_LL1B7:;}
# 1613
if(emit_warning){
# 1614
const char*_tmp900;void*_tmp8FF;(_tmp8FF=0,Cyc_Tcutil_warn(e2->loc,((_tmp900="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp900,sizeof(char),63))),_tag_dyneither(_tmp8FF,sizeof(void*),0)));}}else{
# 1617
if(Cyc_Tcutil_is_const_exp(_tmp3E3,e2)){
# 1618
unsigned int _tmp400;int _tmp401;struct _tuple15 _tmp3FF=Cyc_Evexp_eval_const_uint_exp(e2);_tmp400=_tmp3FF.f1;_tmp401=_tmp3FF.f2;
# 1619
if(_tmp401)
# 1620
Cyc_Tcutil_check_bound(loc,_tmp400,_tmp3EF);}else{
# 1623
if(Cyc_Tcutil_is_bound_one(_tmp3EF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F0)){
# 1624
const char*_tmp903;void*_tmp902;(_tmp902=0,Cyc_Tcutil_warn(e1->loc,((_tmp903="subscript applied to pointer to one object",_tag_dyneither(_tmp903,sizeof(char),43))),_tag_dyneither(_tmp902,sizeof(void*),0)));}
# 1625
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3EF);}}
# 1628
Cyc_Tcenv_check_rgn_accessible(_tmp3E3,loc,_tmp3EE);
# 1629
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3EB),& Cyc_Tcutil_tmk)){
# 1630
const char*_tmp906;void*_tmp905;(_tmp905=0,Cyc_Tcutil_terr(e1->loc,((_tmp906="can't subscript an abstract pointer",_tag_dyneither(_tmp906,sizeof(char),36))),_tag_dyneither(_tmp905,sizeof(void*),0)));}{
# 1631
void*_tmp406=_tmp3EB;_npop_handler(0);return _tmp406;};_LL1B3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E8;if(_tmp3F1->tag != 10)goto _LL1B5;else{_tmp3F2=_tmp3F1->f1;}}_LL1B4: {
# 1632
void*_tmp407=Cyc_Tcexp_ithTupleType(_tmp3E3,loc,_tmp3F2,e2);_npop_handler(0);return _tmp407;}_LL1B5:;_LL1B6: {
# 1633
const char*_tmp90A;void*_tmp909[1];struct Cyc_String_pa_PrintArg_struct _tmp908;void*_tmp40B=(_tmp908.tag=0,((_tmp908.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp909[0]=& _tmp908,Cyc_Tcexp_expr_err(_tmp3E3,loc,topt,((_tmp90A="subscript applied to %s",_tag_dyneither(_tmp90A,sizeof(char),24))),_tag_dyneither(_tmp909,sizeof(void*),1)))))));_npop_handler(0);return _tmp40B;}_LL1B0:;};};}
# 1585
;_pop_region(r);}
# 1639
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
# 1640
int done=0;
# 1641
struct Cyc_List_List*fields=0;
# 1642
if(topt != 0){
# 1643
void*_tmp40C=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp40E;_LL1BD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40C;if(_tmp40D->tag != 10)goto _LL1BF;else{_tmp40E=_tmp40D->f1;}}_LL1BE:
# 1645
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp40E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1648
goto _LL1BC;
# 1650
for(0;es != 0;(es=es->tl,_tmp40E=_tmp40E->tl)){
# 1651
int bogus=0;
# 1652
void*_tmp40F=(*((struct _tuple19*)((struct Cyc_List_List*)_check_null(_tmp40E))->hd)).f2;
# 1653
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp40F,(struct Cyc_Absyn_Exp*)es->hd);
# 1655
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple19*)_tmp40E->hd)).f2,& bogus);{
# 1656
struct _tuple19*_tmp90D;struct Cyc_List_List*_tmp90C;fields=((_tmp90C=_cycalloc(sizeof(*_tmp90C)),((_tmp90C->hd=((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D->f1=(*((struct _tuple19*)_tmp40E->hd)).f1,((_tmp90D->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp90D)))))),((_tmp90C->tl=fields,_tmp90C))))));};}
# 1658
done=1;
# 1659
goto _LL1BC;_LL1BF:;_LL1C0:
# 1660
 goto _LL1BC;_LL1BC:;}
# 1662
if(!done)
# 1663
for(0;es != 0;es=es->tl){
# 1664
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);{
# 1665
struct _tuple19*_tmp910;struct Cyc_List_List*_tmp90F;fields=((_tmp90F=_cycalloc(sizeof(*_tmp90F)),((_tmp90F->hd=((_tmp910=_cycalloc(sizeof(*_tmp910)),((_tmp910->f1=Cyc_Absyn_empty_tqual(0),((_tmp910->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp910)))))),((_tmp90F->tl=fields,_tmp90F))))));};}{
# 1667
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp913;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp912;return(void*)((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp913.tag=10,((_tmp913.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp913)))),_tmp912))));};}
# 1671
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1674
const char*_tmp916;void*_tmp915;return(_tmp915=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp916="tcCompoundLit",_tag_dyneither(_tmp916,sizeof(char),14))),_tag_dyneither(_tmp915,sizeof(void*),0)));}
# 1684 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1687
void*res_t2;
# 1688
struct _RegionHandle _tmp418=_new_region("r");struct _RegionHandle*r=& _tmp418;_push_region(r);{
# 1689
int _tmp419=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
# 1690
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
# 1691
void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
# 1692
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp919;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp918;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp41A=(_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918[0]=((_tmp919.tag=0,((_tmp919.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp419),_tmp919)))),_tmp918)));
# 1693
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp41A,loc);
# 1696
if(zero_term){
# 1697
struct Cyc_Absyn_Exp*_tmp41B=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp419 - 1);
# 1698
if(!Cyc_Tcutil_is_zero(_tmp41B)){
# 1699
const char*_tmp91C;void*_tmp91B;(_tmp91B=0,Cyc_Tcutil_terr(_tmp41B->loc,((_tmp91C="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp91C,sizeof(char),45))),_tag_dyneither(_tmp91B,sizeof(void*),0)));}}
# 1701
sz_exp->topt=(void*)Cyc_Absyn_uint_typ;
# 1702
res_t2=Cyc_Absyn_array_typ(res,
# 1703
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),(struct Cyc_Absyn_Exp*)sz_exp,
# 1704
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1706
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
# 1707
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1709
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1711
const char*_tmp920;void*_tmp91F[1];struct Cyc_String_pa_PrintArg_struct _tmp91E;(_tmp91E.tag=0,((_tmp91E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1712
Cyc_Absynpp_typ2string(res)),((_tmp91F[0]=& _tmp91E,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp920="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp920,sizeof(char),53))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))));}}
# 1715
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
# 1716
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
# 1717
if(ds != 0){
# 1720
void*_tmp423=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp426;_LL1C2: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp424=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp423;if(_tmp424->tag != 1)goto _LL1C4;}_LL1C3:
# 1722
{const char*_tmp923;void*_tmp922;(_tmp922=0,Cyc_Tcutil_terr(loc,((_tmp923="only array index designators are supported",_tag_dyneither(_tmp923,sizeof(char),43))),_tag_dyneither(_tmp922,sizeof(void*),0)));}
# 1723
goto _LL1C1;_LL1C4: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp425=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp423;if(_tmp425->tag != 0)goto _LL1C1;else{_tmp426=_tmp425->f1;}}_LL1C5:
# 1725
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp426);{
# 1726
unsigned int _tmp42A;int _tmp42B;struct _tuple15 _tmp429=Cyc_Evexp_eval_const_uint_exp(_tmp426);_tmp42A=_tmp429.f1;_tmp42B=_tmp429.f2;
# 1727
if(!_tmp42B){
# 1728
const char*_tmp926;void*_tmp925;(_tmp925=0,Cyc_Tcutil_terr(_tmp426->loc,((_tmp926="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp926,sizeof(char),47))),_tag_dyneither(_tmp925,sizeof(void*),0)));}else{
# 1729
if(_tmp42A != offset){
# 1730
const char*_tmp92B;void*_tmp92A[2];struct Cyc_Int_pa_PrintArg_struct _tmp929;struct Cyc_Int_pa_PrintArg_struct _tmp928;(_tmp928.tag=1,((_tmp928.f1=(unsigned long)((int)_tmp42A),((_tmp929.tag=1,((_tmp929.f1=(unsigned long)offset,((_tmp92A[0]=& _tmp929,((_tmp92A[1]=& _tmp928,Cyc_Tcutil_terr(_tmp426->loc,((_tmp92B="expecting index designator %d but found %d",_tag_dyneither(_tmp92B,sizeof(char),43))),_tag_dyneither(_tmp92A,sizeof(void*),2)))))))))))));}}
# 1732
goto _LL1C1;};_LL1C1:;}}}{
# 1736
void*_tmp432=res_t2;_npop_handler(0);return _tmp432;};
# 1688
;_pop_region(r);}
# 1740
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1743
Cyc_Tcexp_tcExp(te,0,bound);
# 1744
{void*_tmp433=Cyc_Tcutil_compress((void*)_check_null(bound->topt));_LL1C7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp434=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp433;if(_tmp434->tag != 19)goto _LL1C9;}_LL1C8:
# 1745
 goto _LL1C6;_LL1C9:;_LL1CA:
# 1747
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
# 1748
const char*_tmp92F;void*_tmp92E[1];struct Cyc_String_pa_PrintArg_struct _tmp92D;(_tmp92D.tag=0,((_tmp92D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1749
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp92E[0]=& _tmp92D,Cyc_Tcutil_terr(bound->loc,((_tmp92F="expecting unsigned int, found %s",_tag_dyneither(_tmp92F,sizeof(char),33))),_tag_dyneither(_tmp92E,sizeof(void*),1)))))));}_LL1C6:;}
# 1752
if(!(vd->tq).real_const){
# 1753
const char*_tmp932;void*_tmp931;(_tmp931=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp932="comprehension index variable is not declared const!",_tag_dyneither(_tmp932,sizeof(char),52))),_tag_dyneither(_tmp931,sizeof(void*),0)));}{
# 1755
struct _RegionHandle _tmp43A=_new_region("r");struct _RegionHandle*r=& _tmp43A;_push_region(r);
# 1756
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
# 1757
if(te2->le != 0){
# 1758
te2=Cyc_Tcenv_new_block(r,loc,te2);
# 1759
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1761
struct Cyc_Tcenv_Tenv*_tmp43B=te2;
# 1762
void**_tmp43C=0;
# 1763
struct Cyc_Absyn_Tqual*_tmp43D=0;
# 1764
union Cyc_Absyn_Constraint**_tmp43E=0;
# 1766
if(topt != 0){
# 1767
void*_tmp43F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp441;struct Cyc_Absyn_ArrayInfo _tmp443;void*_tmp444;struct Cyc_Absyn_Tqual _tmp445;struct Cyc_Absyn_Exp*_tmp446;union Cyc_Absyn_Constraint*_tmp447;_LL1CC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp440=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F;if(_tmp440->tag != 5)goto _LL1CE;else{_tmp441=_tmp440->f1;}}_LL1CD:
# 1769
{void**_tmp933;_tmp43C=((_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933[0]=_tmp441.elt_typ,_tmp933))));}
# 1770
{struct Cyc_Absyn_Tqual*_tmp934;_tmp43D=((_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934[0]=_tmp441.elt_tq,_tmp934))));}
# 1771
{union Cyc_Absyn_Constraint**_tmp935;_tmp43E=((_tmp935=_region_malloc(r,sizeof(*_tmp935)),((_tmp935[0]=(_tmp441.ptr_atts).zero_term,_tmp935))));}
# 1772
goto _LL1CB;_LL1CE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F;if(_tmp442->tag != 8)goto _LL1D0;else{_tmp443=_tmp442->f1;_tmp444=_tmp443.elt_type;_tmp445=_tmp443.tq;_tmp446=_tmp443.num_elts;_tmp447=_tmp443.zero_term;}}_LL1CF:
# 1774
{void**_tmp936;_tmp43C=((_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936[0]=_tmp444,_tmp936))));}
# 1775
{struct Cyc_Absyn_Tqual*_tmp937;_tmp43D=((_tmp937=_region_malloc(r,sizeof(*_tmp937)),((_tmp937[0]=_tmp445,_tmp937))));}
# 1776
{union Cyc_Absyn_Constraint**_tmp938;_tmp43E=((_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=_tmp447,_tmp938))));}
# 1777
goto _LL1CB;_LL1D0:;_LL1D1:
# 1779
 goto _LL1CB;_LL1CB:;}{
# 1782
void*t=Cyc_Tcexp_tcExp(_tmp43B,_tmp43C,body);
# 1783
if(_tmp43B->le == 0){
# 1785
if(!Cyc_Tcutil_is_const_exp(_tmp43B,bound)){
# 1786
const char*_tmp93B;void*_tmp93A;(_tmp93A=0,Cyc_Tcutil_terr(bound->loc,((_tmp93B="bound is not constant",_tag_dyneither(_tmp93B,sizeof(char),22))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}
# 1787
if(!Cyc_Tcutil_is_const_exp(_tmp43B,body)){
# 1788
const char*_tmp93E;void*_tmp93D;(_tmp93D=0,Cyc_Tcutil_terr(bound->loc,((_tmp93E="body is not constant",_tag_dyneither(_tmp93E,sizeof(char),21))),_tag_dyneither(_tmp93D,sizeof(void*),0)));}}
# 1790
if(_tmp43E != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp43E)){
# 1793
struct Cyc_Absyn_Exp*_tmp452=Cyc_Absyn_uint_exp(1,0);_tmp452->topt=(void*)Cyc_Absyn_uint_typ;
# 1794
bound=Cyc_Absyn_add_exp(bound,_tmp452,0);bound->topt=(void*)Cyc_Absyn_uint_typ;
# 1795
*is_zero_term=1;}{
# 1798
void*_tmp453=Cyc_Absyn_array_typ(t,_tmp43D == 0?Cyc_Absyn_empty_tqual(0):*_tmp43D,(struct Cyc_Absyn_Exp*)bound,
# 1799
_tmp43E == 0?Cyc_Absyn_false_conref:*_tmp43E,0);
# 1800
void*_tmp454=_tmp453;_npop_handler(0);return _tmp454;};};};}
# 1756
;_pop_region(r);};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1814 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1818
struct Cyc_Absyn_Aggrdecl**adptr;
# 1819
struct Cyc_Absyn_Aggrdecl*ad;
# 1820
if(*ad_opt != 0){
# 1821
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
# 1822
struct Cyc_Absyn_Aggrdecl**_tmp93F;adptr=((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((_tmp93F[0]=ad,_tmp93F))));};}else{
# 1824
{struct _handler_cons _tmp456;_push_handler(& _tmp456);{int _tmp458=0;if(setjmp(_tmp456.handler))_tmp458=1;if(!_tmp458){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
# 1825
ad=*adptr;
# 1824
;_pop_handler();}else{void*_tmp457=(void*)_exn_thrown;void*_tmp45A=_tmp457;_LL1D3: {struct Cyc_Dict_Absent_exn_struct*_tmp45B=(struct Cyc_Dict_Absent_exn_struct*)_tmp45A;if(_tmp45B->tag != Cyc_Dict_Absent)goto _LL1D5;}_LL1D4:
# 1827
{const char*_tmp943;void*_tmp942[1];struct Cyc_String_pa_PrintArg_struct _tmp941;(_tmp941.tag=0,((_tmp941.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp942[0]=& _tmp941,Cyc_Tcutil_terr(loc,((_tmp943="unbound struct/union name %s",_tag_dyneither(_tmp943,sizeof(char),29))),_tag_dyneither(_tmp942,sizeof(void*),1)))))));}
# 1828
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1D5:;_LL1D6:(void)_throw(_tmp45A);_LL1D2:;}};}
# 1830
*ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;
# 1831
*tn=ad->name;}
# 1833
if(ad->impl == 0){
# 1834
{const char*_tmp94B;void*_tmp94A[1];const char*_tmp949;const char*_tmp948;struct Cyc_String_pa_PrintArg_struct _tmp947;(_tmp947.tag=0,((_tmp947.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp948="struct",_tag_dyneither(_tmp948,sizeof(char),7)):((_tmp949="union",_tag_dyneither(_tmp949,sizeof(char),6)))),((_tmp94A[0]=& _tmp947,Cyc_Tcutil_terr(loc,((_tmp94B="can't construct abstract %s",_tag_dyneither(_tmp94B,sizeof(char),28))),_tag_dyneither(_tmp94A,sizeof(void*),1)))))));}
# 1835
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1837
struct _RegionHandle _tmp464=_new_region("rgn");struct _RegionHandle*rgn=& _tmp464;_push_region(rgn);
# 1839
{struct _tuple12 _tmp94C;struct _tuple12 _tmp465=(_tmp94C.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp94C.f2=rgn,_tmp94C)));
# 1840
struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp465,ad->tvs);
# 1841
struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp465,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
# 1842
struct Cyc_List_List*_tmp468=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp466);
# 1843
struct Cyc_List_List*_tmp469=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp467);
# 1844
struct Cyc_List_List*_tmp46A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp466,_tmp467);
# 1845
void*res_typ;
# 1850
if(topt != 0){
# 1851
void*_tmp46B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_AggrInfo _tmp46D;union Cyc_Absyn_AggrInfoU _tmp46E;struct Cyc_Absyn_Aggrdecl**_tmp46F;struct Cyc_List_List*_tmp470;_LL1D8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp46B;if(_tmp46C->tag != 11)goto _LL1DA;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46D.aggr_info;if((_tmp46E.KnownAggr).tag != 2)goto _LL1DA;_tmp46F=(struct Cyc_Absyn_Aggrdecl**)(_tmp46E.KnownAggr).val;_tmp470=_tmp46D.targs;}}_LL1D9:
# 1853
 if(*_tmp46F == *adptr){
# 1854
{struct Cyc_List_List*_tmp471=_tmp468;for(0;_tmp471 != 0  && _tmp470 != 0;
# 1855
(_tmp471=_tmp471->tl,_tmp470=_tmp470->tl)){
# 1856
Cyc_Tcutil_unify((void*)_tmp471->hd,(void*)_tmp470->hd);}}
# 1858
res_typ=*topt;
# 1859
goto _LL1D7;}
# 1861
goto _LL1DB;_LL1DA:;_LL1DB: {
# 1863
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp952;struct Cyc_Absyn_AggrInfo _tmp951;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp950;res_typ=(void*)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp952.tag=11,((_tmp952.f1=((_tmp951.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp951.targs=_tmp468,_tmp951)))),_tmp952)))),_tmp950))));}_LL1D7:;}else{
# 1866
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp958;struct Cyc_Absyn_AggrInfo _tmp957;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp956;res_typ=(void*)((_tmp956=_cycalloc(sizeof(*_tmp956)),((_tmp956[0]=((_tmp958.tag=11,((_tmp958.f1=((_tmp957.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp957.targs=_tmp468,_tmp957)))),_tmp958)))),_tmp956))));}{
# 1869
struct Cyc_List_List*_tmp478=*ts;
# 1870
struct Cyc_List_List*_tmp479=_tmp469;
# 1871
while(_tmp478 != 0  && _tmp479 != 0){
# 1873
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_ak,1,(void*)_tmp478->hd);
# 1874
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp478->hd);
# 1875
Cyc_Tcutil_unify((void*)_tmp478->hd,(void*)_tmp479->hd);
# 1876
_tmp478=_tmp478->tl;
# 1877
_tmp479=_tmp479->tl;}
# 1879
if(_tmp478 != 0){
# 1880
const char*_tmp95B;void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_terr(loc,((_tmp95B="too many explicit witness types",_tag_dyneither(_tmp95B,sizeof(char),32))),_tag_dyneither(_tmp95A,sizeof(void*),0)));}
# 1882
*ts=_tmp469;{
# 1885
struct Cyc_List_List*fields=
# 1886
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
# 1887
for(0;fields != 0;fields=fields->tl){
# 1888
int bogus=0;
# 1889
struct _tuple20 _tmp47D;struct Cyc_Absyn_Aggrfield*_tmp47E;struct Cyc_Absyn_Exp*_tmp47F;struct _tuple20*_tmp47C=(struct _tuple20*)fields->hd;_tmp47D=*_tmp47C;_tmp47E=_tmp47D.f1;_tmp47F=_tmp47D.f2;{
# 1890
void*_tmp480=Cyc_Tcutil_rsubstitute(rgn,_tmp46A,_tmp47E->type);
# 1891
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp480,_tmp47F);
# 1892
if(!Cyc_Tcutil_coerce_arg(te,_tmp47F,_tmp480,& bogus)){
# 1893
{const char*_tmp967;void*_tmp966[5];struct Cyc_String_pa_PrintArg_struct _tmp965;const char*_tmp964;const char*_tmp963;struct Cyc_String_pa_PrintArg_struct _tmp962;struct Cyc_String_pa_PrintArg_struct _tmp961;struct Cyc_String_pa_PrintArg_struct _tmp960;struct Cyc_String_pa_PrintArg_struct _tmp95F;(_tmp95F.tag=0,((_tmp95F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1896
Cyc_Absynpp_typ2string((void*)_check_null(_tmp47F->topt))),((_tmp960.tag=0,((_tmp960.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1895
Cyc_Absynpp_typ2string(_tmp480)),((_tmp961.tag=0,((_tmp961.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp962.tag=0,((_tmp962.f1=(struct _dyneither_ptr)(
# 1894
ad->kind == Cyc_Absyn_StructA?(_tmp963="struct",_tag_dyneither(_tmp963,sizeof(char),7)):((_tmp964="union",_tag_dyneither(_tmp964,sizeof(char),6)))),((_tmp965.tag=0,((_tmp965.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp47E->name),((_tmp966[0]=& _tmp965,((_tmp966[1]=& _tmp962,((_tmp966[2]=& _tmp961,((_tmp966[3]=& _tmp960,((_tmp966[4]=& _tmp95F,Cyc_Tcutil_terr(_tmp47F->loc,((_tmp967="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp967,sizeof(char),40))),_tag_dyneither(_tmp966,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 1897
Cyc_Tcutil_explain_failure();}};}{
# 1900
struct Cyc_List_List*_tmp48A=0;
# 1901
{struct Cyc_List_List*_tmp48B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp48B != 0;_tmp48B=_tmp48B->tl){
# 1902
struct _tuple0*_tmp96A;struct Cyc_List_List*_tmp969;_tmp48A=((_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969->hd=((_tmp96A=_cycalloc(sizeof(*_tmp96A)),((_tmp96A->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp46A,(*((struct _tuple0*)_tmp48B->hd)).f1),((_tmp96A->f2=
# 1903
Cyc_Tcutil_rsubstitute(rgn,_tmp46A,(*((struct _tuple0*)_tmp48B->hd)).f2),_tmp96A)))))),((_tmp969->tl=_tmp48A,_tmp969))))));}}
# 1905
_tmp48A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp48A);
# 1906
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp48A);{
# 1907
void*_tmp48E=res_typ;_npop_handler(0);return _tmp48E;};};};};}
# 1839
;_pop_region(rgn);};}
# 1911
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 1913
struct _RegionHandle _tmp490=_new_region("rgn");struct _RegionHandle*rgn=& _tmp490;_push_region(rgn);{
# 1914
void*_tmp491=Cyc_Tcutil_compress(ts);enum Cyc_Absyn_AggrKind _tmp493;struct Cyc_List_List*_tmp494;_LL1DD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp492=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp491;if(_tmp492->tag != 12)goto _LL1DF;else{_tmp493=_tmp492->f1;_tmp494=_tmp492->f2;}}_LL1DE:
# 1916
 if(_tmp493 == Cyc_Absyn_UnionA){
# 1917
const char*_tmp96D;void*_tmp96C;(_tmp96C=0,Cyc_Tcutil_terr(loc,((_tmp96D="expecting struct but found union",_tag_dyneither(_tmp96D,sizeof(char),33))),_tag_dyneither(_tmp96C,sizeof(void*),0)));}{
# 1918
struct Cyc_List_List*fields=
# 1919
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp494);
# 1920
for(0;fields != 0;fields=fields->tl){
# 1921
int bogus=0;
# 1922
struct _tuple20 _tmp498;struct Cyc_Absyn_Aggrfield*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct _tuple20*_tmp497=(struct _tuple20*)fields->hd;_tmp498=*_tmp497;_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;
# 1923
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp499->type,_tmp49A);
# 1924
if(!Cyc_Tcutil_coerce_arg(te,_tmp49A,_tmp499->type,& bogus)){
# 1925
{const char*_tmp973;void*_tmp972[3];struct Cyc_String_pa_PrintArg_struct _tmp971;struct Cyc_String_pa_PrintArg_struct _tmp970;struct Cyc_String_pa_PrintArg_struct _tmp96F;(_tmp96F.tag=0,((_tmp96F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1927
Cyc_Absynpp_typ2string((void*)_check_null(_tmp49A->topt))),((_tmp970.tag=0,((_tmp970.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1926
Cyc_Absynpp_typ2string(_tmp499->type)),((_tmp971.tag=0,((_tmp971.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp499->name),((_tmp972[0]=& _tmp971,((_tmp972[1]=& _tmp970,((_tmp972[2]=& _tmp96F,Cyc_Tcutil_terr(_tmp49A->loc,((_tmp973="field %s of struct expects type %s != %s",_tag_dyneither(_tmp973,sizeof(char),41))),_tag_dyneither(_tmp972,sizeof(void*),3)))))))))))))))))));}
# 1928
Cyc_Tcutil_explain_failure();}}
# 1931
goto _LL1DC;};_LL1DF:;_LL1E0: {
# 1932
const char*_tmp976;void*_tmp975;(_tmp975=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp976="tcAnonStruct: wrong type",_tag_dyneither(_tmp976,sizeof(char),25))),_tag_dyneither(_tmp975,sizeof(void*),0)));}_LL1DC:;}{
# 1935
void*_tmp4A2=ts;_npop_handler(0);return _tmp4A2;};
# 1913
;_pop_region(rgn);}
# 1939
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1942
struct _RegionHandle _tmp4A3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4A3;_push_region(rgn);
# 1943
{struct _tuple12 _tmp977;struct _tuple12 _tmp4A4=(_tmp977.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp977.f2=rgn,_tmp977)));
# 1944
struct Cyc_List_List*_tmp4A5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4A4,tud->tvs);
# 1945
struct Cyc_List_List*_tmp4A6=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4A5);
# 1946
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp97D;struct Cyc_Absyn_DatatypeFieldInfo _tmp97C;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp97B;void*res=(void*)(
# 1947
(_tmp97B=_cycalloc(sizeof(*_tmp97B)),((_tmp97B[0]=((_tmp97D.tag=4,((_tmp97D.f1=((_tmp97C.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp97C.targs=_tmp4A6,_tmp97C)))),_tmp97D)))),_tmp97B))));
# 1950
if(topt != 0){
# 1951
void*_tmp4A7=Cyc_Tcutil_compress(*topt);_LL1E2: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4A7;if(_tmp4A8->tag != 4)goto _LL1E4;}_LL1E3:
# 1952
 Cyc_Tcutil_unify(*topt,res);goto _LL1E1;_LL1E4:;_LL1E5:
# 1953
 goto _LL1E1;_LL1E1:;}{
# 1956
struct Cyc_List_List*ts=tuf->typs;
# 1957
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
# 1958
int bogus=0;
# 1959
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
# 1960
void*t=(*((struct _tuple19*)ts->hd)).f2;
# 1961
if(_tmp4A5 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,t);
# 1962
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);
# 1963
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
# 1964
{const char*_tmp985;void*_tmp984[3];struct Cyc_String_pa_PrintArg_struct _tmp983;struct Cyc_String_pa_PrintArg_struct _tmp982;const char*_tmp981;struct Cyc_String_pa_PrintArg_struct _tmp980;(_tmp980.tag=0,((_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 1967
e->topt == 0?(struct _dyneither_ptr)((_tmp981="?",_tag_dyneither(_tmp981,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp982.tag=0,((_tmp982.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1966
Cyc_Absynpp_typ2string(t)),((_tmp983.tag=0,((_tmp983.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp984[0]=& _tmp983,((_tmp984[1]=& _tmp982,((_tmp984[2]=& _tmp980,Cyc_Tcutil_terr(e->loc,((_tmp985="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp985,sizeof(char),82))),_tag_dyneither(_tmp984,sizeof(void*),3)))))))))))))))))));}
# 1968
Cyc_Tcutil_explain_failure();}}
# 1971
if(es != 0){
# 1972
const char*_tmp989;void*_tmp988[1];struct Cyc_String_pa_PrintArg_struct _tmp987;void*_tmp4B2=(_tmp987.tag=0,((_tmp987.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1974
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp988[0]=& _tmp987,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp989="too many arguments for datatype constructor %s",_tag_dyneither(_tmp989,sizeof(char),47))),_tag_dyneither(_tmp988,sizeof(void*),1)))))));_npop_handler(0);return _tmp4B2;}
# 1975
if(ts != 0){
# 1976
const char*_tmp98D;void*_tmp98C[1];struct Cyc_String_pa_PrintArg_struct _tmp98B;void*_tmp4B6=(_tmp98B.tag=0,((_tmp98B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1977
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp98C[0]=& _tmp98B,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp98D="too few arguments for datatype constructor %s",_tag_dyneither(_tmp98D,sizeof(char),46))),_tag_dyneither(_tmp98C,sizeof(void*),1)))))));_npop_handler(0);return _tmp4B6;}{
# 1978
void*_tmp4B7=res;_npop_handler(0);return _tmp4B7;};};}
# 1943
;_pop_region(rgn);}
# 1982
static int Cyc_Tcexp_zeroable_type(void*t){
# 1983
void*_tmp4BC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp4C3;struct Cyc_Absyn_PtrAtts _tmp4C4;union Cyc_Absyn_Constraint*_tmp4C5;struct Cyc_Absyn_ArrayInfo _tmp4C9;void*_tmp4CA;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_AggrInfo _tmp4CF;union Cyc_Absyn_AggrInfoU _tmp4D0;struct Cyc_List_List*_tmp4D1;struct Cyc_List_List*_tmp4D5;_LL1E7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4BC;if(_tmp4BD->tag != 0)goto _LL1E9;}_LL1E8:
# 1984
 return 1;_LL1E9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4BC;if(_tmp4BE->tag != 1)goto _LL1EB;}_LL1EA:
# 1985
 goto _LL1EC;_LL1EB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4BF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4BC;if(_tmp4BF->tag != 2)goto _LL1ED;}_LL1EC:
# 1986
 goto _LL1EE;_LL1ED: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4C0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4BC;if(_tmp4C0->tag != 3)goto _LL1EF;}_LL1EE:
# 1987
 goto _LL1F0;_LL1EF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4BC;if(_tmp4C1->tag != 4)goto _LL1F1;}_LL1F0:
# 1988
 return 0;_LL1F1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4BC;if(_tmp4C2->tag != 5)goto _LL1F3;else{_tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C3.ptr_atts;_tmp4C5=_tmp4C4.nullable;}}_LL1F2:
# 1990
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp4C5);_LL1F3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4BC;if(_tmp4C6->tag != 6)goto _LL1F5;}_LL1F4:
# 1991
 goto _LL1F6;_LL1F5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4C7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4BC;if(_tmp4C7->tag != 7)goto _LL1F7;}_LL1F6:
# 1992
 return 1;_LL1F7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4C8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BC;if(_tmp4C8->tag != 8)goto _LL1F9;else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C9.elt_type;}}_LL1F8:
# 1993
 return Cyc_Tcexp_zeroable_type(_tmp4CA);_LL1F9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4BC;if(_tmp4CB->tag != 9)goto _LL1FB;}_LL1FA:
# 1994
 return 0;_LL1FB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4CC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4BC;if(_tmp4CC->tag != 10)goto _LL1FD;else{_tmp4CD=_tmp4CC->f1;}}_LL1FC:
# 1996
 for(0;(unsigned int)_tmp4CD;_tmp4CD=_tmp4CD->tl){
# 1997
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple19*)_tmp4CD->hd)).f2))return 0;}
# 1998
return 1;_LL1FD: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4CE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4BC;if(_tmp4CE->tag != 11)goto _LL1FF;else{_tmp4CF=_tmp4CE->f1;_tmp4D0=_tmp4CF.aggr_info;_tmp4D1=_tmp4CF.targs;}}_LL1FE: {
# 2000
struct Cyc_Absyn_Aggrdecl*_tmp4E2=Cyc_Absyn_get_known_aggrdecl(_tmp4D0);
# 2001
if(_tmp4E2->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E2->impl))->exist_vars != 0)
# 2002
return 0;{
# 2003
struct _RegionHandle _tmp4E3=_new_region("r");struct _RegionHandle*r=& _tmp4E3;_push_region(r);
# 2004
{struct Cyc_List_List*_tmp4E4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp4E2->tvs,_tmp4D1);
# 2005
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4E2->impl))->fields;for(0;fs != 0;fs=fs->tl){
# 2006
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp4E4,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4E5=0;_npop_handler(0);return _tmp4E5;}}}{
# 2007
int _tmp4E6=1;_npop_handler(0);return _tmp4E6;};}
# 2004
;_pop_region(r);};}_LL1FF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4D2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D2->tag != 13)goto _LL201;}_LL200:
# 2009
 return 1;_LL201: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4D3=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D3->tag != 19)goto _LL203;}_LL202:
# 2010
 return 1;_LL203: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4D4=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D4->tag != 12)goto _LL205;else{_tmp4D5=_tmp4D4->f2;}}_LL204:
# 2012
 for(0;_tmp4D5 != 0;_tmp4D5=_tmp4D5->tl){
# 2013
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp4D5->hd)->type))return 0;}
# 2014
return 1;_LL205: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4D6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D6->tag != 14)goto _LL207;}_LL206:
# 2015
 return 1;_LL207: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4D7=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D7->tag != 17)goto _LL209;}_LL208:
# 2016
 return 0;_LL209: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4D8=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D8->tag != 16)goto _LL20B;}_LL20A:
# 2017
 return 0;_LL20B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4D9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4BC;if(_tmp4D9->tag != 15)goto _LL20D;}_LL20C:
# 2018
 return 0;_LL20D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4DA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4BC;if(_tmp4DA->tag != 26)goto _LL20F;}_LL20E:
# 2020
 goto _LL210;_LL20F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4DB=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4BC;if(_tmp4DB->tag != 18)goto _LL211;}_LL210:
# 2021
 goto _LL212;_LL211: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4DC=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4BC;if(_tmp4DC->tag != 20)goto _LL213;}_LL212:
# 2022
 goto _LL214;_LL213: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4DD=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4BC;if(_tmp4DD->tag != 21)goto _LL215;}_LL214:
# 2023
 goto _LL216;_LL215: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4DE=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4BC;if(_tmp4DE->tag != 22)goto _LL217;}_LL216:
# 2024
 goto _LL218;_LL217: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4BC;if(_tmp4DF->tag != 23)goto _LL219;}_LL218:
# 2025
 goto _LL21A;_LL219: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4BC;if(_tmp4E0->tag != 24)goto _LL21B;}_LL21A:
# 2026
 goto _LL21C;_LL21B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4BC;if(_tmp4E1->tag != 25)goto _LL1E6;}_LL21C: {
# 2027
const char*_tmp991;void*_tmp990[1];struct Cyc_String_pa_PrintArg_struct _tmp98F;(_tmp98F.tag=0,((_tmp98F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp990[0]=& _tmp98F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp991="bad type `%s' in zeroable type",_tag_dyneither(_tmp991,sizeof(char),31))),_tag_dyneither(_tmp990,sizeof(void*),1)))))));}_LL1E6:;}
# 2031
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2033
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2035
if(topt != 0){
# 2036
void*_tmp4EA=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp4EC;void*_tmp4ED;_LL21E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EA;if(_tmp4EB->tag != 5)goto _LL220;else{_tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EC.elt_typ;}}_LL21F:
# 2038
 Cyc_Tcutil_unify(_tmp4ED,t);
# 2039
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2040
goto _LL21D;_LL220:;_LL221:
# 2041
 goto _LL21D;_LL21D:;}{
# 2044
const char*_tmp99A;void*_tmp999[2];const char*_tmp998;const char*_tmp997;struct Cyc_String_pa_PrintArg_struct _tmp996;struct Cyc_String_pa_PrintArg_struct _tmp995;(_tmp995.tag=0,((_tmp995.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2045
Cyc_Absynpp_typ2string(t)),((_tmp996.tag=0,((_tmp996.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp997="calloc",_tag_dyneither(_tmp997,sizeof(char),7))):(struct _dyneither_ptr)((_tmp998="malloc",_tag_dyneither(_tmp998,sizeof(char),7))))),((_tmp999[0]=& _tmp996,((_tmp999[1]=& _tmp995,Cyc_Tcutil_terr(loc,((_tmp99A="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp99A,sizeof(char),60))),_tag_dyneither(_tmp999,sizeof(void*),2)))))))))))));};}
# 2048
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2050
switch((Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual){case Cyc_Absyn_Unique: _LL222:
# 2051
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL223:
# 2052
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2056
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2061
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
# 2062
if(*ropt != 0){
# 2064
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp99D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp99C;void*expected_type=(void*)(
# 2065
(_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C[0]=((_tmp99D.tag=15,((_tmp99D.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp99D)))),_tmp99C))));
# 2066
void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
# 2067
void*_tmp4F4=Cyc_Tcutil_compress(handle_type);void*_tmp4F6;_LL226: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F4;if(_tmp4F5->tag != 15)goto _LL228;else{_tmp4F6=(void*)_tmp4F5->f1;}}_LL227:
# 2069
 rgn=_tmp4F6;
# 2070
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 2071
goto _LL225;_LL228:;_LL229:
# 2073
{const char*_tmp9A1;void*_tmp9A0[1];struct Cyc_String_pa_PrintArg_struct _tmp99F;(_tmp99F.tag=0,((_tmp99F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2074
Cyc_Absynpp_typ2string(handle_type)),((_tmp9A0[0]=& _tmp99F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp9A1="expecting region_t type but found %s",_tag_dyneither(_tmp9A1,sizeof(char),37))),_tag_dyneither(_tmp9A0,sizeof(void*),1)))))));}
# 2075
goto _LL225;_LL225:;}else{
# 2080
if(topt != 0){
# 2081
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
# 2082
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
# 2083
rgn=Cyc_Tcexp_mallocRgn(optrgn);
# 2084
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}
# 2088
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{
# 2097 "tcexp.cyc"
void*elt_type;
# 2098
struct Cyc_Absyn_Exp*num_elts;
# 2099
int one_elt;
# 2100
if(*is_calloc){
# 2101
if(*t == 0){const char*_tmp9A4;void*_tmp9A3;(_tmp9A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9A4="calloc with empty type",_tag_dyneither(_tmp9A4,sizeof(char),23))),_tag_dyneither(_tmp9A3,sizeof(void*),0)));}
# 2102
elt_type=*((void**)_check_null(*t));
# 2103
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
# 2104
Cyc_Tcutil_check_no_qual(loc,elt_type);
# 2105
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
# 2106
num_elts=*e;
# 2107
one_elt=0;}else{
# 2109
void*er=(*e)->r;
# 2110
retry_sizeof: {
# 2111
void*_tmp4FE=er;void*_tmp500;enum Cyc_Absyn_Primop _tmp502;struct Cyc_List_List*_tmp503;struct Cyc_List_List _tmp504;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_List_List*_tmp506;struct Cyc_List_List _tmp507;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_List_List*_tmp509;_LL22B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp4FF->tag != 16)goto _LL22D;else{_tmp500=(void*)_tmp4FF->f1;}}_LL22C:
# 2113
 elt_type=_tmp500;
# 2114
{void**_tmp9A5;*t=(void**)((_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5[0]=elt_type,_tmp9A5))));}
# 2115
num_elts=Cyc_Absyn_uint_exp(1,0);
# 2116
Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,num_elts);
# 2117
one_elt=1;
# 2118
goto _LL22A;_LL22D: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp501=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp501->tag != 2)goto _LL22F;else{_tmp502=_tmp501->f1;if(_tmp502 != Cyc_Absyn_Times)goto _LL22F;_tmp503=_tmp501->f2;if(_tmp503 == 0)goto _LL22F;_tmp504=*_tmp503;_tmp505=(struct Cyc_Absyn_Exp*)_tmp504.hd;_tmp506=_tmp504.tl;if(_tmp506 == 0)goto _LL22F;_tmp507=*_tmp506;_tmp508=(struct Cyc_Absyn_Exp*)_tmp507.hd;_tmp509=_tmp507.tl;if(_tmp509 != 0)goto _LL22F;}}_LL22E:
# 2120
{struct _tuple0 _tmp9A6;struct _tuple0 _tmp50C=(_tmp9A6.f1=_tmp505->r,((_tmp9A6.f2=_tmp508->r,_tmp9A6)));void*_tmp50D;void*_tmp50F;void*_tmp510;void*_tmp512;_LL232: _tmp50D=_tmp50C.f1;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp50D;if(_tmp50E->tag != 16)goto _LL234;else{_tmp50F=(void*)_tmp50E->f1;}};_LL233:
# 2122
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp50F);
# 2123
elt_type=_tmp50F;
# 2124
{void**_tmp9A7;*t=(void**)((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7[0]=elt_type,_tmp9A7))));}
# 2125
num_elts=_tmp508;
# 2126
one_elt=0;
# 2127
goto _LL231;_LL234: _tmp510=_tmp50C.f2;{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp511=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp511->tag != 16)goto _LL236;else{_tmp512=(void*)_tmp511->f1;}};_LL235:
# 2129
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp512);
# 2130
elt_type=_tmp512;
# 2131
{void**_tmp9A8;*t=(void**)((_tmp9A8=_cycalloc(sizeof(*_tmp9A8)),((_tmp9A8[0]=elt_type,_tmp9A8))));}
# 2132
num_elts=_tmp505;
# 2133
one_elt=0;
# 2134
goto _LL231;_LL236:;_LL237:
# 2135
 goto No_sizeof;_LL231:;}
# 2137
goto _LL22A;_LL22F:;_LL230:
# 2139
 No_sizeof: {
# 2142
struct Cyc_Absyn_Exp*_tmp515=*e;
# 2143
{void*_tmp516=_tmp515->r;struct Cyc_Absyn_Exp*_tmp518;_LL239: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp517=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp516;if(_tmp517->tag != 13)goto _LL23B;else{_tmp518=_tmp517->f2;}}_LL23A:
# 2144
 _tmp515=_tmp518;goto _LL238;_LL23B:;_LL23C:
# 2145
 goto _LL238;_LL238:;}
# 2147
{void*_tmp519=Cyc_Tcutil_compress((void*)_check_null(_tmp515->topt));void*_tmp51B;_LL23E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp519;if(_tmp51A->tag != 19)goto _LL240;else{_tmp51B=(void*)_tmp51A->f1;}}_LL23F:
# 2149
{void*_tmp51C=Cyc_Tcutil_compress(_tmp51B);struct Cyc_Absyn_Exp*_tmp51E;_LL243: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp51C;if(_tmp51D->tag != 18)goto _LL245;else{_tmp51E=_tmp51D->f1;}}_LL244:
# 2151
 er=_tmp51E->r;goto retry_sizeof;_LL245:;_LL246:
# 2152
 goto _LL242;_LL242:;}
# 2154
goto _LL23D;_LL240:;_LL241:
# 2155
 goto _LL23D;_LL23D:;}
# 2157
elt_type=Cyc_Absyn_char_typ;
# 2158
{void**_tmp9A9;*t=(void**)((_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9[0]=elt_type,_tmp9A9))));}
# 2159
num_elts=*e;
# 2160
one_elt=0;}
# 2162
goto _LL22A;_LL22A:;}}
# 2166
*is_fat=!one_elt;
# 2169
{void*_tmp520=elt_type;struct Cyc_Absyn_AggrInfo _tmp522;union Cyc_Absyn_AggrInfoU _tmp523;struct Cyc_Absyn_Aggrdecl**_tmp524;struct Cyc_Absyn_Aggrdecl*_tmp525;_LL248: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp520;if(_tmp521->tag != 11)goto _LL24A;else{_tmp522=_tmp521->f1;_tmp523=_tmp522.aggr_info;if((_tmp523.KnownAggr).tag != 2)goto _LL24A;_tmp524=(struct Cyc_Absyn_Aggrdecl**)(_tmp523.KnownAggr).val;_tmp525=*_tmp524;}}_LL249:
# 2171
 if(_tmp525->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp525->impl))->exist_vars != 0){
# 2172
const char*_tmp9AC;void*_tmp9AB;(_tmp9AB=0,Cyc_Tcutil_terr(loc,((_tmp9AC="malloc with existential types not yet implemented",_tag_dyneither(_tmp9AC,sizeof(char),50))),_tag_dyneither(_tmp9AB,sizeof(void*),0)));}
# 2173
goto _LL247;_LL24A:;_LL24B:
# 2174
 goto _LL247;_LL247:;}{
# 2178
void*(*_tmp528)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
# 2179
union Cyc_Absyn_Constraint*_tmp529=Cyc_Absyn_false_conref;
# 2180
if(topt != 0){
# 2181
void*_tmp52A=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp52C;struct Cyc_Absyn_PtrAtts _tmp52D;union Cyc_Absyn_Constraint*_tmp52E;union Cyc_Absyn_Constraint*_tmp52F;union Cyc_Absyn_Constraint*_tmp530;_LL24D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp52B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52A;if(_tmp52B->tag != 5)goto _LL24F;else{_tmp52C=_tmp52B->f1;_tmp52D=_tmp52C.ptr_atts;_tmp52E=_tmp52D.nullable;_tmp52F=_tmp52D.bounds;_tmp530=_tmp52D.zero_term;}}_LL24E:
# 2183
 _tmp529=_tmp530;
# 2184
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp52E))
# 2185
_tmp528=Cyc_Absyn_star_typ;
# 2188
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp530) && !(*is_calloc)){
# 2189
{const char*_tmp9AF;void*_tmp9AE;(_tmp9AE=0,Cyc_Tcutil_warn(loc,((_tmp9AF="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9AF,sizeof(char),55))),_tag_dyneither(_tmp9AE,sizeof(void*),0)));}
# 2190
*is_calloc=1;}
# 2194
{void*_tmp533=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52F);struct Cyc_Absyn_Exp*_tmp536;_LL252: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp534=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp533;if(_tmp534->tag != 0)goto _LL254;}_LL253:
# 2195
 goto _LL251;_LL254:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp535=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp533;if(_tmp535->tag != 1)goto _LL256;else{_tmp536=_tmp535->f1;}}if(!(!one_elt))goto _LL256;_LL255: {
# 2197
int _tmp537=Cyc_Evexp_c_can_eval(num_elts);
# 2198
if(_tmp537  && Cyc_Evexp_same_const_exp(_tmp536,num_elts)){
# 2199
*is_fat=0;
# 2200
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp52F),_tmp529);}
# 2203
{void*_tmp538=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp53A;_LL259: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp539=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp538;if(_tmp539->tag != 19)goto _LL25B;else{_tmp53A=(void*)_tmp539->f1;}}_LL25A: {
# 2205
struct Cyc_Absyn_Exp*_tmp53B=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp53A,0),0,Cyc_Absyn_No_coercion,0);
# 2207
if(Cyc_Evexp_same_const_exp(_tmp53B,_tmp536)){
# 2208
*is_fat=0;
# 2209
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp52F),_tmp529);}
# 2212
goto _LL258;}_LL25B:;_LL25C:
# 2213
 goto _LL258;_LL258:;}
# 2215
goto _LL251;}_LL256:;_LL257:
# 2216
 goto _LL251;_LL251:;}
# 2218
goto _LL24C;_LL24F:;_LL250:
# 2219
 goto _LL24C;_LL24C:;}
# 2221
if(!one_elt)_tmp528=Cyc_Absyn_dyneither_typ;
# 2222
return _tmp528(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp529);};};}
# 2226
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2232
struct _RegionHandle _tmp53C=_new_region("r");struct _RegionHandle*r=& _tmp53C;_push_region(r);{
# 2233
struct Cyc_Tcenv_Tenv*_tmp53D=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
# 2234
Cyc_Tcexp_tcExp(_tmp53D,0,e1);{
# 2235
void*_tmp53E=(void*)_check_null(e1->topt);
# 2236
Cyc_Tcexp_tcExp(_tmp53D,(void**)& _tmp53E,e2);};}{
# 2238
void*t1=(void*)_check_null(e1->topt);
# 2239
void*t2=(void*)_check_null(e2->topt);
# 2241
{void*_tmp53F=Cyc_Tcutil_compress(t1);_LL25E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp53F;if(_tmp540->tag != 8)goto _LL260;}_LL25F:
# 2242
{const char*_tmp9B2;void*_tmp9B1;(_tmp9B1=0,Cyc_Tcutil_terr(loc,((_tmp9B2="cannot assign to an array",_tag_dyneither(_tmp9B2,sizeof(char),26))),_tag_dyneither(_tmp9B1,sizeof(void*),0)));}goto _LL25D;_LL260:;_LL261:
# 2243
 goto _LL25D;_LL25D:;}{
# 2246
int ign_1=0;
# 2247
if(!Cyc_Tcutil_is_pointer_or_boxed(t1,& ign_1)){
# 2248
const char*_tmp9B5;void*_tmp9B4;(_tmp9B4=0,Cyc_Tcutil_terr(loc,((_tmp9B5="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9B5,sizeof(char),58))),_tag_dyneither(_tmp9B4,sizeof(void*),0)));}
# 2251
if(!Cyc_Absyn_is_lvalue(e1)){
# 2252
const char*_tmp9B8;void*_tmp9B7;void*_tmp547=(_tmp9B7=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9B8="swap non-lvalue",_tag_dyneither(_tmp9B8,sizeof(char),16))),_tag_dyneither(_tmp9B7,sizeof(void*),0)));_npop_handler(0);return _tmp547;}
# 2253
if(!Cyc_Absyn_is_lvalue(e2)){
# 2254
const char*_tmp9BB;void*_tmp9BA;void*_tmp54A=(_tmp9BA=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9BB="swap non-lvalue",_tag_dyneither(_tmp9BB,sizeof(char),16))),_tag_dyneither(_tmp9BA,sizeof(void*),0)));_npop_handler(0);return _tmp54A;}{
# 2256
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
# 2257
int b_ign1=0;
# 2258
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
# 2259
const char*_tmp9BE;void*_tmp9BD;void*_tmp54D=(_tmp9BD=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9BE="swap value in zeroterm array",_tag_dyneither(_tmp9BE,sizeof(char),29))),_tag_dyneither(_tmp9BD,sizeof(void*),0)));_npop_handler(0);return _tmp54D;}
# 2260
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
# 2261
const char*_tmp9C1;void*_tmp9C0;void*_tmp550=(_tmp9C0=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9C1="swap value in zeroterm array",_tag_dyneither(_tmp9C1,sizeof(char),29))),_tag_dyneither(_tmp9C0,sizeof(void*),0)));_npop_handler(0);return _tmp550;}
# 2263
Cyc_Tcexp_check_writable(te,e1);
# 2264
Cyc_Tcexp_check_writable(te,e2);
# 2265
if(!Cyc_Tcutil_unify(t1,t2)){
# 2266
const char*_tmp9C6;void*_tmp9C5[2];struct Cyc_String_pa_PrintArg_struct _tmp9C4;struct Cyc_String_pa_PrintArg_struct _tmp9C3;void*_tmp551=(_tmp9C3.tag=0,((_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2267
Cyc_Absynpp_typ2string(t2)),((_tmp9C4.tag=0,((_tmp9C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9C5[0]=& _tmp9C4,((_tmp9C5[1]=& _tmp9C3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9C6="type mismatch: %s != %s",_tag_dyneither(_tmp9C6,sizeof(char),24))),_tag_dyneither(_tmp9C5,sizeof(void*),2)))))))))))));
# 2268
void*_tmp552=_tmp551;_npop_handler(0);return _tmp552;}{
# 2270
void*_tmp557=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp557;};};};};
# 2232
;_pop_region(r);}
# 2274
int Cyc_Tcexp_in_stmt_exp=0;
# 2276
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
# 2277
struct _RegionHandle _tmp558=_new_region("r");struct _RegionHandle*r=& _tmp558;_push_region(r);{
# 2278
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
# 2279
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);
# 2280
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2283
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2285
while(1){
# 2286
void*_tmp559=s->r;struct Cyc_Absyn_Exp*_tmp55B;struct Cyc_Absyn_Stmt*_tmp55D;struct Cyc_Absyn_Stmt*_tmp55E;struct Cyc_Absyn_Decl*_tmp560;struct Cyc_Absyn_Stmt*_tmp561;_LL263: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp55A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp559;if(_tmp55A->tag != 1)goto _LL265;else{_tmp55B=_tmp55A->f1;}}_LL264: {
# 2289
void*_tmp562=(void*)_check_null(_tmp55B->topt);
# 2290
if(!Cyc_Tcutil_unify(_tmp562,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
# 2291
{const char*_tmp9CA;void*_tmp9C9[1];struct Cyc_String_pa_PrintArg_struct _tmp9C8;(_tmp9C8.tag=0,((_tmp9C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2292
Cyc_Absynpp_typ2string(_tmp562)),((_tmp9C9[0]=& _tmp9C8,Cyc_Tcutil_terr(loc,((_tmp9CA="statement expression returns type %s",_tag_dyneither(_tmp9CA,sizeof(char),37))),_tag_dyneither(_tmp9C9,sizeof(void*),1)))))));}
# 2293
Cyc_Tcutil_explain_failure();}{
# 2295
void*_tmp566=_tmp562;_npop_handler(0);return _tmp566;};}_LL265: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp55C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp559;if(_tmp55C->tag != 2)goto _LL267;else{_tmp55D=_tmp55C->f1;_tmp55E=_tmp55C->f2;}}_LL266:
# 2296
 s=_tmp55E;continue;_LL267: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp55F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp559;if(_tmp55F->tag != 12)goto _LL269;else{_tmp560=_tmp55F->f1;_tmp561=_tmp55F->f2;}}_LL268:
# 2297
 s=_tmp561;continue;_LL269:;_LL26A: {
# 2299
const char*_tmp9CD;void*_tmp9CC;void*_tmp569=(_tmp9CC=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9CD="statement expression must end with expression",_tag_dyneither(_tmp9CD,sizeof(char),46))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));_npop_handler(0);return _tmp569;}_LL262:;}
# 2277
;_pop_region(r);}
# 2304
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
# 2305
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));
# 2306
{void*_tmp56A=t;struct Cyc_Absyn_AggrInfo _tmp56C;union Cyc_Absyn_AggrInfoU _tmp56D;struct Cyc_Absyn_Aggrdecl**_tmp56E;struct Cyc_Absyn_Aggrdecl*_tmp56F;_LL26C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp56A;if(_tmp56B->tag != 11)goto _LL26E;else{_tmp56C=_tmp56B->f1;_tmp56D=_tmp56C.aggr_info;if((_tmp56D.KnownAggr).tag != 2)goto _LL26E;_tmp56E=(struct Cyc_Absyn_Aggrdecl**)(_tmp56D.KnownAggr).val;_tmp56F=*_tmp56E;}}_LL26D:
# 2308
 if((_tmp56F->kind == Cyc_Absyn_UnionA  && _tmp56F->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp56F->impl))->tagged)goto _LL26B;
# 2309
goto _LL26F;_LL26E:;_LL26F:
# 2311
{const char*_tmp9D1;void*_tmp9D0[1];struct Cyc_String_pa_PrintArg_struct _tmp9CF;(_tmp9CF.tag=0,((_tmp9CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9D0[0]=& _tmp9CF,Cyc_Tcutil_terr(loc,((_tmp9D1="expecting @tagged union but found %s",_tag_dyneither(_tmp9D1,sizeof(char),37))),_tag_dyneither(_tmp9D0,sizeof(void*),1)))))));}
# 2312
goto _LL26B;_LL26B:;}
# 2314
return Cyc_Absyn_uint_typ;}
# 2318
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2322
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
# 2324
if(*rgn_handle != 0){
# 2326
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9D4;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9D3;void*expected_type=(void*)(
# 2327
(_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=15,((_tmp9D4.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9D4)))),_tmp9D3))));
# 2328
void*handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
# 2329
void*_tmp573=Cyc_Tcutil_compress(handle_type);void*_tmp575;_LL271: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp573;if(_tmp574->tag != 15)goto _LL273;else{_tmp575=(void*)_tmp574->f1;}}_LL272:
# 2331
 rgn=_tmp575;
# 2332
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 2333
goto _LL270;_LL273:;_LL274:
# 2335
{const char*_tmp9D8;void*_tmp9D7[1];struct Cyc_String_pa_PrintArg_struct _tmp9D6;(_tmp9D6.tag=0,((_tmp9D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2336
Cyc_Absynpp_typ2string(handle_type)),((_tmp9D7[0]=& _tmp9D6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmp9D8="expecting region_t type but found %s",_tag_dyneither(_tmp9D8,sizeof(char),37))),_tag_dyneither(_tmp9D7,sizeof(void*),1)))))));}
# 2337
goto _LL270;_LL270:;}else{
# 2342
if(topt != 0){
# 2343
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
# 2344
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
# 2345
rgn=Cyc_Tcexp_mallocRgn(optrgn);
# 2346
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uniquergn_exp;}}}{
# 2350
void*_tmp57B=e1->r;struct Cyc_Core_Opt*_tmp57E;struct Cyc_List_List*_tmp57F;struct Cyc_List_List*_tmp581;union Cyc_Absyn_Cnst _tmp583;struct _dyneither_ptr _tmp584;union Cyc_Absyn_Cnst _tmp586;struct _dyneither_ptr _tmp587;_LL276: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp57C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp57B;if(_tmp57C->tag != 26)goto _LL278;}_LL277: {
# 2353
void*_tmp588=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
# 2354
void*_tmp589=Cyc_Tcutil_compress(_tmp588);struct Cyc_Absyn_ArrayInfo _tmp58B;void*_tmp58C;struct Cyc_Absyn_Tqual _tmp58D;struct Cyc_Absyn_Exp*_tmp58E;union Cyc_Absyn_Constraint*_tmp58F;_LL283: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp589;if(_tmp58A->tag != 8)goto _LL285;else{_tmp58B=_tmp58A->f1;_tmp58C=_tmp58B.elt_type;_tmp58D=_tmp58B.tq;_tmp58E=_tmp58B.num_elts;_tmp58F=_tmp58B.zero_term;}}_LL284: {
# 2356
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp58E);
# 2357
void*b;
# 2358
{void*_tmp590=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp592;_LL288: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp591=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp590;if(_tmp591->tag != 19)goto _LL28A;else{_tmp592=(void*)_tmp591->f1;}}_LL289:
# 2360
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9DB;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9DA;b=(void*)((_tmp9DA=_cycalloc(sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=1,((_tmp9DB.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp592,0),0,Cyc_Absyn_No_coercion,0),_tmp9DB)))),_tmp9DA))));}
# 2361
goto _LL287;_LL28A:;_LL28B:
# 2363
 if(Cyc_Tcutil_is_const_exp(te,bnd)){
# 2364
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp9DE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9DD;b=(void*)((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DE.tag=1,((_tmp9DE.f1=bnd,_tmp9DE)))),_tmp9DD))));}else{
# 2366
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL287:;}{
# 2368
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp9E8;struct Cyc_Absyn_PtrAtts _tmp9E7;struct Cyc_Absyn_PtrInfo _tmp9E6;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E5;void*res_typ=(void*)(
# 2369
(_tmp9E5=_cycalloc(sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E8.tag=5,((_tmp9E8.f1=((_tmp9E6.elt_typ=_tmp58C,((_tmp9E6.elt_tq=_tmp58D,((_tmp9E6.ptr_atts=(
# 2370
(_tmp9E7.rgn=rgn,((_tmp9E7.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp9E7.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp9E7.zero_term=_tmp58F,((_tmp9E7.ptrloc=0,_tmp9E7)))))))))),_tmp9E6)))))),_tmp9E8)))),_tmp9E5))));
# 2372
if(topt != 0){
# 2373
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
# 2374
e->topt=(void*)res_typ;
# 2375
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 2376
res_typ=*topt;}}
# 2379
return res_typ;};}_LL285:;_LL286: {
# 2380
const char*_tmp9EB;void*_tmp9EA;(_tmp9EA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9EB="tcNew: comprehension returned non-array type",_tag_dyneither(_tmp9EB,sizeof(char),45))),_tag_dyneither(_tmp9EA,sizeof(void*),0)));}_LL282:;}_LL278: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57D=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp57B;if(_tmp57D->tag != 34)goto _LL27A;else{_tmp57E=_tmp57D->f1;_tmp57F=_tmp57D->f2;}}_LL279:
# 2383
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp9EE;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9ED;e1->r=(void*)((_tmp9ED=_cycalloc(sizeof(*_tmp9ED)),((_tmp9ED[0]=((_tmp9EE.tag=25,((_tmp9EE.f1=_tmp57F,_tmp9EE)))),_tmp9ED))));}
# 2384
_tmp581=_tmp57F;goto _LL27B;_LL27A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp580=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp57B;if(_tmp580->tag != 25)goto _LL27C;else{_tmp581=_tmp580->f1;}}_LL27B: {
# 2386
void**elt_typ_opt=0;
# 2387
int zero_term=0;
# 2388
if(topt != 0){
# 2389
void*_tmp59F=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp5A1;void*_tmp5A2;void**_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A4;struct Cyc_Absyn_PtrAtts _tmp5A5;union Cyc_Absyn_Constraint*_tmp5A6;_LL28D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59F;if(_tmp5A0->tag != 5)goto _LL28F;else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A1.elt_typ;_tmp5A3=(void**)&(_tmp5A0->f1).elt_typ;_tmp5A4=_tmp5A1.elt_tq;_tmp5A5=_tmp5A1.ptr_atts;_tmp5A6=_tmp5A5.zero_term;}}_LL28E:
# 2392
 elt_typ_opt=(void**)_tmp5A3;
# 2393
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A6);
# 2394
goto _LL28C;_LL28F:;_LL290:
# 2395
 goto _LL28C;_LL28C:;}{
# 2398
void*_tmp5A7=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,_tmp581);
# 2399
e1->topt=(void*)_tmp5A7;{
# 2400
void*res_typ;
# 2401
{void*_tmp5A8=Cyc_Tcutil_compress(_tmp5A7);struct Cyc_Absyn_ArrayInfo _tmp5AA;void*_tmp5AB;struct Cyc_Absyn_Tqual _tmp5AC;struct Cyc_Absyn_Exp*_tmp5AD;union Cyc_Absyn_Constraint*_tmp5AE;_LL292: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A8;if(_tmp5A9->tag != 8)goto _LL294;else{_tmp5AA=_tmp5A9->f1;_tmp5AB=_tmp5AA.elt_type;_tmp5AC=_tmp5AA.tq;_tmp5AD=_tmp5AA.num_elts;_tmp5AE=_tmp5AA.zero_term;}}_LL293:
# 2403
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA03;struct Cyc_Absyn_PtrAtts _tmpA02;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA01;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA00;struct Cyc_Absyn_PtrInfo _tmp9FF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9FE;res_typ=(void*)(
# 2404
(_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmpA03.tag=5,((_tmpA03.f1=((_tmp9FF.elt_typ=_tmp5AB,((_tmp9FF.elt_tq=_tmp5AC,((_tmp9FF.ptr_atts=(
# 2405
(_tmpA02.rgn=rgn,((_tmpA02.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpA02.bounds=
# 2406
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=1,((_tmpA01.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp5AD),_tmpA01)))),_tmpA00))))),((_tmpA02.zero_term=_tmp5AE,((_tmpA02.ptrloc=0,_tmpA02)))))))))),_tmp9FF)))))),_tmpA03)))),_tmp9FE))));}
# 2408
if(topt != 0){
# 2412
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
# 2413
e->topt=(void*)res_typ;
# 2414
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 2415
res_typ=*topt;}}
# 2418
goto _LL291;_LL294:;_LL295: {
# 2419
const char*_tmpA06;void*_tmpA05;(_tmpA05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA06="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmpA06,sizeof(char),50))),_tag_dyneither(_tmpA05,sizeof(void*),0)));}_LL291:;}
# 2421
return res_typ;};};}_LL27C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp582=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp57B;if(_tmp582->tag != 0)goto _LL27E;else{_tmp583=_tmp582->f1;if((_tmp583.String_c).tag != 8)goto _LL27E;_tmp584=(struct _dyneither_ptr)(_tmp583.String_c).val;}}_LL27D: {
# 2426
void*_tmp5B7=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2428
void*_tmp5B8=Cyc_Tcexp_tcExp(te,(void**)& _tmp5B7,e1);
# 2429
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA09;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA08;return Cyc_Absyn_atb_typ(_tmp5B8,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA09.tag=1,((_tmpA09.f1=Cyc_Absyn_uint_exp(1,0),_tmpA09)))),_tmpA08)))),Cyc_Absyn_false_conref);}_LL27E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp585=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp57B;if(_tmp585->tag != 0)goto _LL280;else{_tmp586=_tmp585->f1;if((_tmp586.Wstring_c).tag != 9)goto _LL280;_tmp587=(struct _dyneither_ptr)(_tmp586.Wstring_c).val;}}_LL27F: {
# 2433
void*_tmp5BB=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2435
void*_tmp5BC=Cyc_Tcexp_tcExp(te,(void**)& _tmp5BB,e1);
# 2436
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA0C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA0B;return Cyc_Absyn_atb_typ(_tmp5BC,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA0B=_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B[0]=((_tmpA0C.tag=1,((_tmpA0C.f1=Cyc_Absyn_uint_exp(1,0),_tmpA0C)))),_tmpA0B)))),Cyc_Absyn_false_conref);}_LL280:;_LL281:
# 2442
 RG: {
# 2443
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
# 2444
void**topt2=0;
# 2445
if(topt != 0){
# 2446
void*_tmp5BF=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp5C1;void*_tmp5C2;void**_tmp5C3;struct Cyc_Absyn_Tqual _tmp5C4;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF;if(_tmp5C0->tag != 5)goto _LL299;else{_tmp5C1=_tmp5C0->f1;_tmp5C2=_tmp5C1.elt_typ;_tmp5C3=(void**)&(_tmp5C0->f1).elt_typ;_tmp5C4=_tmp5C1.elt_tq;}}_LL298:
# 2448
 topt2=(void**)_tmp5C3;goto _LL296;_LL299: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5C5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5BF;if(_tmp5C5->tag != 3)goto _LL29B;}_LL29A:
# 2452
 bogus=*topt;
# 2453
topt2=(void**)& bogus;
# 2454
goto _LL296;_LL29B:;_LL29C:
# 2455
 goto _LL296;_LL296:;}{
# 2458
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2459
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA16;struct Cyc_Absyn_PtrAtts _tmpA15;struct Cyc_Absyn_PtrInfo _tmpA14;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA13;void*res_typ=(void*)(
# 2460
(_tmpA13=_cycalloc(sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA16.tag=5,((_tmpA16.f1=((_tmpA14.elt_typ=telt,((_tmpA14.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmpA14.ptr_atts=(
# 2461
(_tmpA15.rgn=rgn,((_tmpA15.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmpA15.bounds=Cyc_Absyn_bounds_one_conref,((_tmpA15.zero_term=Cyc_Absyn_false_conref,((_tmpA15.ptrloc=0,_tmpA15)))))))))),_tmpA14)))))),_tmpA16)))),_tmpA13))));
# 2464
if(topt != 0){
# 2465
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*topt)){
# 2466
e->topt=(void*)res_typ;
# 2467
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
# 2468
res_typ=*topt;}}
# 2471
return res_typ;};}_LL275:;};}
# 2477
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2478
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
# 2479
void*_tmp5CA=t;struct Cyc_Absyn_ArrayInfo _tmp5CC;void*_tmp5CD;struct Cyc_Absyn_Tqual _tmp5CE;struct Cyc_Absyn_Exp*_tmp5CF;union Cyc_Absyn_Constraint*_tmp5D0;_LL29E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CB->tag != 8)goto _LL2A0;else{_tmp5CC=_tmp5CB->f1;_tmp5CD=_tmp5CC.elt_type;_tmp5CE=_tmp5CC.tq;_tmp5CF=_tmp5CC.num_elts;_tmp5D0=_tmp5CC.zero_term;}}_LL29F: {
# 2481
void*_tmp5D2;struct _tuple14 _tmp5D1=Cyc_Tcutil_addressof_props(te,e);_tmp5D2=_tmp5D1.f2;{
# 2482
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA19;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA18;void*_tmp5D3=_tmp5CF == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18[0]=((_tmpA19.tag=1,((_tmpA19.f1=(struct Cyc_Absyn_Exp*)_tmp5CF,_tmpA19)))),_tmpA18))));
# 2483
t=Cyc_Absyn_atb_typ(_tmp5CD,_tmp5D2,_tmp5CE,_tmp5D3,_tmp5D0);
# 2484
e->topt=(void*)t;
# 2485
return t;};}_LL2A0:;_LL2A1:
# 2487
 return t;_LL29D:;}
# 2494
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2495
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2498
struct _RegionHandle _tmp5D6=_new_region("temp");struct _RegionHandle*temp=& _tmp5D6;_push_region(temp);
# 2499
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2504
const char*_tmpA1C;void*_tmpA1B;(_tmpA1B=0,Cyc_Tcutil_terr(e->loc,((_tmpA1C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA1C,sizeof(char),49))),_tag_dyneither(_tmpA1B,sizeof(void*),0)));}{
# 2507
void*_tmp5D9=e->r;union Cyc_Absyn_Cnst _tmp5DD;struct _dyneither_ptr _tmp5DE;union Cyc_Absyn_Cnst _tmp5E0;struct _dyneither_ptr _tmp5E1;_LL2A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5DA=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5D9;if(_tmp5DA->tag != 25)goto _LL2A5;}_LL2A4:
# 2508
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5DB=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5D9;if(_tmp5DB->tag != 26)goto _LL2A7;}_LL2A6:
# 2509
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5DC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D9;if(_tmp5DC->tag != 0)goto _LL2A9;else{_tmp5DD=_tmp5DC->f1;if((_tmp5DD.Wstring_c).tag != 9)goto _LL2A9;_tmp5DE=(struct _dyneither_ptr)(_tmp5DD.Wstring_c).val;}}_LL2A8:
# 2510
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5DF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D9;if(_tmp5DF->tag != 0)goto _LL2AB;else{_tmp5E0=_tmp5DF->f1;if((_tmp5E0.String_c).tag != 8)goto _LL2AB;_tmp5E1=(struct _dyneither_ptr)(_tmp5E0.String_c).val;}}_LL2AA: {
# 2511
void*_tmp5E2=t;_npop_handler(0);return _tmp5E2;}_LL2AB:;_LL2AC:
# 2513
 t=Cyc_Tcutil_compress(t);{
# 2514
void*_tmp5E3=t;struct Cyc_Absyn_ArrayInfo _tmp5E5;void*_tmp5E6;struct Cyc_Absyn_Tqual _tmp5E7;struct Cyc_Absyn_Exp*_tmp5E8;union Cyc_Absyn_Constraint*_tmp5E9;_LL2AE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3;if(_tmp5E4->tag != 8)goto _LL2B0;else{_tmp5E5=_tmp5E4->f1;_tmp5E6=_tmp5E5.elt_type;_tmp5E7=_tmp5E5.tq;_tmp5E8=_tmp5E5.num_elts;_tmp5E9=_tmp5E5.zero_term;}}_LL2AF: {
# 2516
void*_tmp5EB;struct _tuple14 _tmp5EA=Cyc_Tcutil_addressof_props(te,e);_tmp5EB=_tmp5EA.f2;{
# 2517
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA1F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA1E;void*b=_tmp5E8 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)(
# 2518
(_tmpA1E=_cycalloc(sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA1F.tag=1,((_tmpA1F.f1=(struct Cyc_Absyn_Exp*)_tmp5E8,_tmpA1F)))),_tmpA1E))));
# 2519
t=Cyc_Absyn_atb_typ(_tmp5E6,_tmp5EB,_tmp5E7,b,_tmp5E9);
# 2520
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);{
# 2521
void*_tmp5EC=t;_npop_handler(0);return _tmp5EC;};};}_LL2B0:;_LL2B1: {
# 2523
void*_tmp5EF=t;_npop_handler(0);return _tmp5EF;}_LL2AD:;};_LL2A2:;};
# 2498
;_pop_region(temp);}
# 2535 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2536
{void*_tmp5F0=e->r;struct Cyc_Absyn_Exp*_tmp5F2;_LL2B3: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F1=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5F0;if(_tmp5F1->tag != 11)goto _LL2B5;else{_tmp5F2=_tmp5F1->f1;}}_LL2B4:
# 2539
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5F2);
# 2540
_tmp5F2->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5F2->topt),0);
# 2541
e->topt=_tmp5F2->topt;
# 2542
goto _LL2B2;_LL2B5:;_LL2B6:
# 2545
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2546
e->topt=(void*)Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2548
{void*_tmp5F3=Cyc_Tcutil_compress((void*)_check_null(e->topt));struct Cyc_Absyn_PtrInfo _tmp5F5;void*_tmp5F6;struct Cyc_Absyn_Tqual _tmp5F7;struct Cyc_Absyn_PtrAtts _tmp5F8;void*_tmp5F9;union Cyc_Absyn_Constraint*_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;_LL2B8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F3;if(_tmp5F4->tag != 5)goto _LL2BA;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F5.elt_typ;_tmp5F7=_tmp5F5.elt_tq;_tmp5F8=_tmp5F5.ptr_atts;_tmp5F9=_tmp5F8.rgn;_tmp5FA=_tmp5F8.nullable;_tmp5FB=_tmp5F8.bounds;_tmp5FC=_tmp5F8.zero_term;}}_LL2B9:
# 2550
{void*_tmp5FD=Cyc_Tcutil_compress(_tmp5F6);struct Cyc_Absyn_FnInfo _tmp5FF;struct Cyc_List_List*_tmp600;void*_tmp601;struct Cyc_Absyn_Tqual _tmp602;void*_tmp603;struct Cyc_List_List*_tmp604;int _tmp605;struct Cyc_Absyn_VarargInfo*_tmp606;struct Cyc_List_List*_tmp607;struct Cyc_List_List*_tmp608;_LL2BD: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5FD;if(_tmp5FE->tag != 9)goto _LL2BF;else{_tmp5FF=_tmp5FE->f1;_tmp600=_tmp5FF.tvars;_tmp601=_tmp5FF.effect;_tmp602=_tmp5FF.ret_tqual;_tmp603=_tmp5FF.ret_typ;_tmp604=_tmp5FF.args;_tmp605=_tmp5FF.c_varargs;_tmp606=_tmp5FF.cyc_varargs;_tmp607=_tmp5FF.rgn_po;_tmp608=_tmp5FF.attributes;}}_LL2BE:
# 2552
 if(_tmp600 != 0){
# 2553
struct _RegionHandle _tmp609=_new_region("rgn");struct _RegionHandle*rgn=& _tmp609;_push_region(rgn);
# 2554
{struct _tuple12 _tmpA20;struct _tuple12 _tmp60A=(_tmpA20.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA20.f2=rgn,_tmpA20)));
# 2555
struct Cyc_List_List*inst=
# 2556
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp60A,_tmp600);
# 2557
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2558
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA26;struct Cyc_Absyn_FnInfo _tmpA25;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA24;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
# 2559
(_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA26.tag=9,((_tmpA26.f1=((_tmpA25.tvars=0,((_tmpA25.effect=_tmp601,((_tmpA25.ret_tqual=_tmp602,((_tmpA25.ret_typ=_tmp603,((_tmpA25.args=_tmp604,((_tmpA25.c_varargs=_tmp605,((_tmpA25.cyc_varargs=_tmp606,((_tmpA25.rgn_po=_tmp607,((_tmpA25.attributes=_tmp608,_tmpA25)))))))))))))))))),_tmpA26)))),_tmpA24)))));
# 2562
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA30;struct Cyc_Absyn_PtrAtts _tmpA2F;struct Cyc_Absyn_PtrInfo _tmpA2E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60B=(_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=((_tmpA30.tag=5,((_tmpA30.f1=((_tmpA2E.elt_typ=ftyp,((_tmpA2E.elt_tq=_tmp5F7,((_tmpA2E.ptr_atts=((_tmpA2F.rgn=_tmp5F9,((_tmpA2F.nullable=_tmp5FA,((_tmpA2F.bounds=_tmp5FB,((_tmpA2F.zero_term=_tmp5FC,((_tmpA2F.ptrloc=0,_tmpA2F)))))))))),_tmpA2E)))))),_tmpA30)))),_tmpA2D)));
# 2564
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Absyn_copy_exp(e);
# 2565
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA33;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA32;e->r=(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=12,((_tmpA33.f1=_tmp60C,((_tmpA33.f2=ts,_tmpA33)))))),_tmpA32))));}
# 2566
e->topt=(void*)_tmp60B;}
# 2554
;_pop_region(rgn);}
# 2569
goto _LL2BC;_LL2BF:;_LL2C0:
# 2570
 goto _LL2BC;_LL2BC:;}
# 2572
goto _LL2B7;_LL2BA:;_LL2BB:
# 2573
 goto _LL2B7;_LL2B7:;}
# 2575
goto _LL2B2;_LL2B2:;}
# 2577
return(void*)_check_null(e->topt);}
# 2585
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2587
struct _RegionHandle _tmp617=_new_region("r");struct _RegionHandle*r=& _tmp617;_push_region(r);
# 2588
{struct Cyc_List_List*_tmp618=0;
# 2590
{void*_tmp619=fn_exp->r;struct Cyc_List_List*_tmp61B;_LL2C2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp61A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp619;if(_tmp61A->tag != 9)goto _LL2C4;else{_tmp61B=_tmp61A->f2;}}_LL2C3:
# 2592
{void*_tmp61C=e->r;struct Cyc_List_List*_tmp61E;_LL2C7: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp61D=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61C;if(_tmp61D->tag != 9)goto _LL2C9;else{_tmp61E=_tmp61D->f2;}}_LL2C8: {
# 2594
struct Cyc_List_List*_tmp61F=alias_arg_exps;
# 2595
int _tmp620=0;
# 2596
while(_tmp61F != 0){
# 2597
while(_tmp620 != (int)_tmp61F->hd){
# 2598
if(_tmp61B == 0){
# 2599
struct Cyc_Int_pa_PrintArg_struct _tmpA3D;struct Cyc_Int_pa_PrintArg_struct _tmpA3C;void*_tmpA3B[2];const char*_tmpA3A;void*_tmpA39;(_tmpA39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA3D.tag=1,((_tmpA3D.f1=(unsigned long)((int)_tmp61F->hd),((_tmpA3C.tag=1,((_tmpA3C.f1=(unsigned long)_tmp620,((_tmpA3B[0]=& _tmpA3C,((_tmpA3B[1]=& _tmpA3D,Cyc_aprintf(((_tmpA3A="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA3A,sizeof(char),36))),_tag_dyneither(_tmpA3B,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA39,sizeof(void*),0)));}
# 2601
++ _tmp620;
# 2602
_tmp61B=_tmp61B->tl;
# 2603
_tmp61E=((struct Cyc_List_List*)_check_null(_tmp61E))->tl;}{
# 2606
struct Cyc_Absyn_Decl*_tmp627;struct Cyc_Absyn_Exp*_tmp628;struct _tuple11 _tmp626=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp61B))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp61E))->hd)->topt)));_tmp627=_tmp626.f1;_tmp628=_tmp626.f2;
# 2607
_tmp61B->hd=(void*)_tmp628;
# 2608
{struct Cyc_List_List*_tmpA3E;_tmp618=((_tmpA3E=_region_malloc(r,sizeof(*_tmpA3E)),((_tmpA3E->hd=_tmp627,((_tmpA3E->tl=_tmp618,_tmpA3E))))));}
# 2609
_tmp61F=_tmp61F->tl;};}
# 2611
goto _LL2C6;}_LL2C9:;_LL2CA: {
# 2612
const char*_tmpA41;void*_tmpA40;(_tmpA40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA41="not a function call!",_tag_dyneither(_tmpA41,sizeof(char),21))),_tag_dyneither(_tmpA40,sizeof(void*),0)));}_LL2C6:;}
# 2614
goto _LL2C1;_LL2C4:;_LL2C5: {
# 2615
const char*_tmpA44;void*_tmpA43;(_tmpA43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA44="not a function call!",_tag_dyneither(_tmpA44,sizeof(char),21))),_tag_dyneither(_tmpA43,sizeof(void*),0)));}_LL2C1:;}
# 2619
while(_tmp618 != 0){
# 2620
struct Cyc_Absyn_Decl*_tmp62E=(struct Cyc_Absyn_Decl*)_tmp618->hd;
# 2621
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp62E,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
# 2622
_tmp618=_tmp618->tl;}
# 2626
e->topt=0;
# 2627
e->r=fn_exp->r;}
# 2588
;_pop_region(r);}
# 2632
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
# 2633
unsigned int loc=e->loc;
# 2634
void*t;
# 2636
{void*_tmp62F=e->r;struct Cyc_Absyn_Exp*_tmp631;struct Cyc_Absyn_Exp*_tmp633;struct Cyc_List_List*_tmp634;struct Cyc_Absyn_VarargCallInfo*_tmp635;struct Cyc_Absyn_VarargCallInfo**_tmp636;int _tmp637;int*_tmp638;struct Cyc_Core_Opt*_tmp63A;struct Cyc_List_List*_tmp63B;union Cyc_Absyn_Cnst _tmp63D;union Cyc_Absyn_Cnst*_tmp63E;struct _tuple1*_tmp640;void*_tmp641;void**_tmp642;enum Cyc_Absyn_Primop _tmp644;struct Cyc_List_List*_tmp645;struct Cyc_Absyn_Exp*_tmp647;enum Cyc_Absyn_Incrementor _tmp648;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Core_Opt*_tmp64B;struct Cyc_Absyn_Exp*_tmp64C;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Absyn_Exp*_tmp650;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_List_List*_tmp65C;struct Cyc_Absyn_VarargCallInfo*_tmp65D;struct Cyc_Absyn_VarargCallInfo**_tmp65E;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Exp*_tmp662;struct Cyc_List_List*_tmp663;void*_tmp665;struct Cyc_Absyn_Exp*_tmp666;enum Cyc_Absyn_Coercion _tmp667;enum Cyc_Absyn_Coercion*_tmp668;struct Cyc_Absyn_Exp*_tmp66A;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp**_tmp66D;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp670;void*_tmp672;void*_tmp674;void*_tmp675;struct Cyc_Absyn_Exp*_tmp677;struct Cyc_Absyn_Exp*_tmp679;struct _dyneither_ptr*_tmp67A;int _tmp67B;int*_tmp67C;int _tmp67D;int*_tmp67E;struct Cyc_Absyn_Exp*_tmp680;struct _dyneither_ptr*_tmp681;int _tmp682;int*_tmp683;int _tmp684;int*_tmp685;struct Cyc_Absyn_Exp*_tmp687;struct Cyc_Absyn_Exp*_tmp688;struct Cyc_List_List*_tmp68A;struct _tuple9*_tmp68C;struct Cyc_List_List*_tmp68D;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Stmt*_tmp691;struct Cyc_Absyn_Vardecl*_tmp693;struct Cyc_Absyn_Exp*_tmp694;struct Cyc_Absyn_Exp*_tmp695;int _tmp696;int*_tmp697;struct _tuple1*_tmp699;struct _tuple1**_tmp69A;struct Cyc_List_List*_tmp69B;struct Cyc_List_List**_tmp69C;struct Cyc_List_List*_tmp69D;struct Cyc_Absyn_Aggrdecl*_tmp69E;struct Cyc_Absyn_Aggrdecl**_tmp69F;void*_tmp6A1;struct Cyc_List_List*_tmp6A2;struct Cyc_List_List*_tmp6A4;struct Cyc_Absyn_Datatypedecl*_tmp6A5;struct Cyc_Absyn_Datatypefield*_tmp6A6;struct _tuple1*_tmp6A8;struct _tuple1**_tmp6A9;struct Cyc_Absyn_Enumdecl*_tmp6AA;struct Cyc_Absyn_Enumfield*_tmp6AB;struct _tuple1*_tmp6AD;struct _tuple1**_tmp6AE;void*_tmp6AF;struct Cyc_Absyn_Enumfield*_tmp6B0;struct Cyc_Absyn_MallocInfo _tmp6B2;int _tmp6B3;int*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B5;struct Cyc_Absyn_Exp**_tmp6B6;void**_tmp6B7;void***_tmp6B8;struct Cyc_Absyn_Exp*_tmp6B9;struct Cyc_Absyn_Exp**_tmp6BA;int _tmp6BB;int*_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BE;struct Cyc_Absyn_Exp*_tmp6BF;struct Cyc_Absyn_Exp*_tmp6C1;struct _dyneither_ptr*_tmp6C2;void*_tmp6C4;_LL2CC: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp630=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp630->tag != 11)goto _LL2CE;else{_tmp631=_tmp630->f1;}}_LL2CD:
# 2641
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp631);
# 2642
return;_LL2CE:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp632=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp632->tag != 9)goto _LL2D0;else{_tmp633=_tmp632->f1;_tmp634=_tmp632->f2;_tmp635=_tmp632->f3;_tmp636=(struct Cyc_Absyn_VarargCallInfo**)& _tmp632->f3;_tmp637=_tmp632->f4;_tmp638=(int*)& _tmp632->f4;}}if(!(!(*_tmp638)))goto _LL2D0;_LL2CF:
# 2647
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp633,_tmp634)){
# 2648
*_tmp638=1;
# 2649
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2651
if(topt != 0)e->topt=(void*)*topt;else{
# 2652
e->topt=(void*)Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2654
return;_LL2D0: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp639=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp639->tag != 34)goto _LL2D2;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp639->f2;}}_LL2D1:
# 2658
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp63B);
# 2659
Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2660
return;_LL2D2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp63C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp63C->tag != 0)goto _LL2D4;else{_tmp63D=_tmp63C->f1;_tmp63E=(union Cyc_Absyn_Cnst*)& _tmp63C->f1;}}_LL2D3:
# 2663
 t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)_tmp63E,e);goto _LL2CB;_LL2D4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp63F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp63F->tag != 1)goto _LL2D6;else{_tmp640=_tmp63F->f1;_tmp641=(void**)& _tmp63F->f2;_tmp642=(void**)((void**)& _tmp63F->f2);}}_LL2D5:
# 2665
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp640,(void**)_tmp642);goto _LL2CB;_LL2D6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp643=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp643->tag != 2)goto _LL2D8;else{_tmp644=_tmp643->f1;_tmp645=_tmp643->f2;}}_LL2D7:
# 2667
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp644,_tmp645);goto _LL2CB;_LL2D8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp646=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp646->tag != 4)goto _LL2DA;else{_tmp647=_tmp646->f1;_tmp648=_tmp646->f2;}}_LL2D9:
# 2669
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp647,_tmp648);goto _LL2CB;_LL2DA: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp649=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp649->tag != 3)goto _LL2DC;else{_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;_tmp64C=_tmp649->f3;}}_LL2DB:
# 2671
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp64A,_tmp64B,_tmp64C);goto _LL2CB;_LL2DC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp64D->tag != 5)goto _LL2DE;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64D->f2;_tmp650=_tmp64D->f3;}}_LL2DD:
# 2673
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp64E,_tmp64F,_tmp650);goto _LL2CB;_LL2DE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp651=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp651->tag != 6)goto _LL2E0;else{_tmp652=_tmp651->f1;_tmp653=_tmp651->f2;}}_LL2DF:
# 2675
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp652,_tmp653);goto _LL2CB;_LL2E0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp654=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp654->tag != 7)goto _LL2E2;else{_tmp655=_tmp654->f1;_tmp656=_tmp654->f2;}}_LL2E1:
# 2677
 t=Cyc_Tcexp_tcOr(te,loc,_tmp655,_tmp656);goto _LL2CB;_LL2E2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp657=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp657->tag != 8)goto _LL2E4;else{_tmp658=_tmp657->f1;_tmp659=_tmp657->f2;}}_LL2E3:
# 2679
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp658,_tmp659);goto _LL2CB;_LL2E4: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp65A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp65A->tag != 9)goto _LL2E6;else{_tmp65B=_tmp65A->f1;_tmp65C=_tmp65A->f2;_tmp65D=_tmp65A->f3;_tmp65E=(struct Cyc_Absyn_VarargCallInfo**)& _tmp65A->f3;}}_LL2E5: {
# 2685
struct Cyc_List_List*alias_arg_exps=0;
# 2686
int ok=1;
# 2687
struct Cyc_Absyn_Exp*fn_exp;
# 2688
{struct _handler_cons _tmp6C6;_push_handler(& _tmp6C6);{int _tmp6C8=0;if(setjmp(_tmp6C6.handler))_tmp6C8=1;if(!_tmp6C8){
# 2689
fn_exp=Cyc_Tcutil_deep_copy_exp(e);;_pop_handler();}else{void*_tmp6C7=(void*)_exn_thrown;void*_tmp6CA=_tmp6C7;struct _dyneither_ptr _tmp6CC;_LL31D: {struct Cyc_Core_Failure_exn_struct*_tmp6CB=(struct Cyc_Core_Failure_exn_struct*)_tmp6CA;if(_tmp6CB->tag != Cyc_Core_Failure)goto _LL31F;else{_tmp6CC=_tmp6CB->f1;}}_LL31E:
# 2692
 ok=0;
# 2693
fn_exp=e;
# 2694
goto _LL31C;_LL31F:;_LL320:(void)_throw(_tmp6CA);_LL31C:;}};}
# 2696
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp65B,_tmp65C,_tmp65E,& alias_arg_exps);
# 2697
if(alias_arg_exps != 0  && ok){
# 2698
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
# 2699
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
# 2700
Cyc_Tcexp_tcExpNoInst(te,topt,e);
# 2701
return;}
# 2703
goto _LL2CB;}_LL2E6: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp65F=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp65F->tag != 10)goto _LL2E8;else{_tmp660=_tmp65F->f1;}}_LL2E7:
# 2705
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp660);goto _LL2CB;_LL2E8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp661=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp661->tag != 12)goto _LL2EA;else{_tmp662=_tmp661->f1;_tmp663=_tmp661->f2;}}_LL2E9:
# 2707
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp662,_tmp663);goto _LL2CB;_LL2EA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp664->tag != 13)goto _LL2EC;else{_tmp665=(void*)_tmp664->f1;_tmp666=_tmp664->f2;_tmp667=_tmp664->f4;_tmp668=(enum Cyc_Absyn_Coercion*)& _tmp664->f4;}}_LL2EB:
# 2709
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp665,_tmp666,(enum Cyc_Absyn_Coercion*)_tmp668);goto _LL2CB;_LL2EC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp669=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp669->tag != 14)goto _LL2EE;else{_tmp66A=_tmp669->f1;}}_LL2ED:
# 2711
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp66A);goto _LL2CB;_LL2EE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp66B=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp66B->tag != 15)goto _LL2F0;else{_tmp66C=_tmp66B->f1;_tmp66D=(struct Cyc_Absyn_Exp**)& _tmp66B->f1;_tmp66E=_tmp66B->f2;}}_LL2EF:
# 2713
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp66D,e,_tmp66E);goto _LL2CB;_LL2F0: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp66F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp66F->tag != 17)goto _LL2F2;else{_tmp670=_tmp66F->f1;}}_LL2F1: {
# 2715
void*_tmp6CD=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp670);
# 2716
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6CD);goto _LL2CB;}_LL2F2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp671->tag != 16)goto _LL2F4;else{_tmp672=(void*)_tmp671->f1;}}_LL2F3:
# 2718
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp672);goto _LL2CB;_LL2F4: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp673->tag != 18)goto _LL2F6;else{_tmp674=(void*)_tmp673->f1;_tmp675=(void*)_tmp673->f2;}}_LL2F5:
# 2720
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp674,_tmp675);goto _LL2CB;_LL2F6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp676=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp676->tag != 19)goto _LL2F8;else{_tmp677=_tmp676->f1;}}_LL2F7:
# 2722
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp677);goto _LL2CB;_LL2F8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp678=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp678->tag != 20)goto _LL2FA;else{_tmp679=_tmp678->f1;_tmp67A=_tmp678->f2;_tmp67B=_tmp678->f3;_tmp67C=(int*)& _tmp678->f3;_tmp67D=_tmp678->f4;_tmp67E=(int*)& _tmp678->f4;}}_LL2F9:
# 2724
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp679,_tmp67A,_tmp67C,_tmp67E);goto _LL2CB;_LL2FA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp67F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp67F->tag != 21)goto _LL2FC;else{_tmp680=_tmp67F->f1;_tmp681=_tmp67F->f2;_tmp682=_tmp67F->f3;_tmp683=(int*)& _tmp67F->f3;_tmp684=_tmp67F->f4;_tmp685=(int*)& _tmp67F->f4;}}_LL2FB:
# 2726
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp680,_tmp681,_tmp683,_tmp685);goto _LL2CB;_LL2FC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp686=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp686->tag != 22)goto _LL2FE;else{_tmp687=_tmp686->f1;_tmp688=_tmp686->f2;}}_LL2FD:
# 2728
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp687,_tmp688);goto _LL2CB;_LL2FE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp689=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp689->tag != 23)goto _LL300;else{_tmp68A=_tmp689->f1;}}_LL2FF:
# 2730
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp68A);goto _LL2CB;_LL300: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68B=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp68B->tag != 24)goto _LL302;else{_tmp68C=_tmp68B->f1;_tmp68D=_tmp68B->f2;}}_LL301:
# 2732
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp68C,_tmp68D);goto _LL2CB;_LL302: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp68E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp68E->tag != 25)goto _LL304;else{_tmp68F=_tmp68E->f1;}}_LL303: {
# 2736
void**elt_topt=0;
# 2737
struct Cyc_Absyn_Tqual*elt_tqopt=0;
# 2738
int zero_term=0;
# 2739
if(topt != 0){
# 2740
void*_tmp6CE=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp6D0;void*_tmp6D1;void**_tmp6D2;struct Cyc_Absyn_Tqual _tmp6D3;struct Cyc_Absyn_Tqual*_tmp6D4;union Cyc_Absyn_Constraint*_tmp6D5;_LL322: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6CF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6CE;if(_tmp6CF->tag != 8)goto _LL324;else{_tmp6D0=_tmp6CF->f1;_tmp6D1=_tmp6D0.elt_type;_tmp6D2=(void**)&(_tmp6CF->f1).elt_type;_tmp6D3=_tmp6D0.tq;_tmp6D4=(struct Cyc_Absyn_Tqual*)&(_tmp6CF->f1).tq;_tmp6D5=_tmp6D0.zero_term;}}_LL323:
# 2742
 elt_topt=(void**)_tmp6D2;
# 2743
elt_tqopt=(struct Cyc_Absyn_Tqual*)_tmp6D4;
# 2744
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6D5);
# 2745
goto _LL321;_LL324:;_LL325:
# 2746
 goto _LL321;_LL321:;}
# 2749
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp68F);goto _LL2CB;}_LL304: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp690=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp690->tag != 35)goto _LL306;else{_tmp691=_tmp690->f1;}}_LL305:
# 2751
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp691);goto _LL2CB;_LL306: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp692=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp692->tag != 26)goto _LL308;else{_tmp693=_tmp692->f1;_tmp694=_tmp692->f2;_tmp695=_tmp692->f3;_tmp696=_tmp692->f4;_tmp697=(int*)& _tmp692->f4;}}_LL307:
# 2753
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp693,_tmp694,_tmp695,_tmp697);goto _LL2CB;_LL308: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp698=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp698->tag != 27)goto _LL30A;else{_tmp699=_tmp698->f1;_tmp69A=(struct _tuple1**)& _tmp698->f1;_tmp69B=_tmp698->f2;_tmp69C=(struct Cyc_List_List**)& _tmp698->f2;_tmp69D=_tmp698->f3;_tmp69E=_tmp698->f4;_tmp69F=(struct Cyc_Absyn_Aggrdecl**)& _tmp698->f4;}}_LL309:
# 2755
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp69A,_tmp69C,_tmp69D,_tmp69F);goto _LL2CB;_LL30A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6A0=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6A0->tag != 28)goto _LL30C;else{_tmp6A1=(void*)_tmp6A0->f1;_tmp6A2=_tmp6A0->f2;}}_LL30B:
# 2757
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp6A1,_tmp6A2);goto _LL2CB;_LL30C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6A3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6A3->tag != 29)goto _LL30E;else{_tmp6A4=_tmp6A3->f1;_tmp6A5=_tmp6A3->f2;_tmp6A6=_tmp6A3->f3;}}_LL30D:
# 2759
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp6A4,_tmp6A5,_tmp6A6);goto _LL2CB;_LL30E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6A7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6A7->tag != 30)goto _LL310;else{_tmp6A8=_tmp6A7->f1;_tmp6A9=(struct _tuple1**)& _tmp6A7->f1;_tmp6AA=_tmp6A7->f2;_tmp6AB=_tmp6A7->f3;}}_LL30F:
# 2761
*_tmp6A9=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6AB))->name;
# 2762
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA47;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA46;t=(void*)((_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46[0]=((_tmpA47.tag=13,((_tmpA47.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp6AA))->name,((_tmpA47.f2=_tmp6AA,_tmpA47)))))),_tmpA46))));}goto _LL2CB;_LL310: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6AC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6AC->tag != 31)goto _LL312;else{_tmp6AD=_tmp6AC->f1;_tmp6AE=(struct _tuple1**)& _tmp6AC->f1;_tmp6AF=(void*)_tmp6AC->f2;_tmp6B0=_tmp6AC->f3;}}_LL311:
# 2764
*_tmp6AE=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6B0))->name;
# 2765
t=_tmp6AF;goto _LL2CB;_LL312: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp6B1=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6B1->tag != 32)goto _LL314;else{_tmp6B2=_tmp6B1->f1;_tmp6B3=_tmp6B2.is_calloc;_tmp6B4=(int*)&(_tmp6B1->f1).is_calloc;_tmp6B5=_tmp6B2.rgn;_tmp6B6=(struct Cyc_Absyn_Exp**)&(_tmp6B1->f1).rgn;_tmp6B7=_tmp6B2.elt_type;_tmp6B8=(void***)&(_tmp6B1->f1).elt_type;_tmp6B9=_tmp6B2.num_elts;_tmp6BA=(struct Cyc_Absyn_Exp**)&(_tmp6B1->f1).num_elts;_tmp6BB=_tmp6B2.fat_result;_tmp6BC=(int*)&(_tmp6B1->f1).fat_result;}}_LL313:
# 2767
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp6B6,_tmp6B8,_tmp6BA,_tmp6B4,_tmp6BC);goto _LL2CB;_LL314: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6BD=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6BD->tag != 33)goto _LL316;else{_tmp6BE=_tmp6BD->f1;_tmp6BF=_tmp6BD->f2;}}_LL315:
# 2769
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp6BE,_tmp6BF);goto _LL2CB;_LL316: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp6C0=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6C0->tag != 36)goto _LL318;else{_tmp6C1=_tmp6C0->f1;_tmp6C2=_tmp6C0->f2;}}_LL317:
# 2771
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp6C1,_tmp6C2);goto _LL2CB;_LL318: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6C3=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6C3->tag != 37)goto _LL31A;else{_tmp6C4=(void*)_tmp6C3->f1;}}_LL319:
# 2773
 if(!te->allow_valueof){
# 2774
const char*_tmpA4A;void*_tmpA49;(_tmpA49=0,Cyc_Tcutil_terr(e->loc,((_tmpA4A="valueof(-) can only occur within types",_tag_dyneither(_tmpA4A,sizeof(char),39))),_tag_dyneither(_tmpA49,sizeof(void*),0)));}
# 2781
t=Cyc_Absyn_sint_typ;
# 2782
goto _LL2CB;_LL31A: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6C5=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp62F;if(_tmp6C5->tag != 38)goto _LL2CB;}_LL31B:
# 2784
{const char*_tmpA4D;void*_tmpA4C;(_tmpA4C=0,Cyc_Tcutil_terr(e->loc,((_tmpA4D="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA4D,sizeof(char),50))),_tag_dyneither(_tmpA4C,sizeof(void*),0)));}
# 2785
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2CB:;}
# 2787
e->topt=(void*)t;}
