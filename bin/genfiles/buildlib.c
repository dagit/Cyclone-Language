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
static _INLINE void 
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn(x,i,__FILE__,__LINE__)
static _INLINE void 
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
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
  _check_dyneither_subscript(a,s,i,__FILE__,__LINE__)
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
# 26 "/home/jgm/cyclone/include/cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;
# 38
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;
struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};
struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*
f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{
int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 167 "/home/jgm/cyclone/include/cycboot.h"
int remove(const char*);
# 224 "/home/jgm/cyclone/include/cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;
struct _dyneither_ptr f1;};
# 300 "/home/jgm/cyclone/include/cycboot.h"
int isspace(int);
# 310
int toupper(int);
# 318
int system(const char*);
# 321
int mkdir(const char*pathname,unsigned short mode);
# 324
int close(int);
# 325
int chdir(const char*);
# 326
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{
char*tag;struct _dyneither_ptr f1;};
# 160 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
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
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};
# 57 "/home/jgm/cyclone/include/lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;
struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr
lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct
_dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
# 85
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
# 86
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
# 87
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};
# 39 "/home/jgm/cyclone/include/list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "/home/jgm/cyclone/include/list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;
};
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 160
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 305
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 29 "/home/jgm/cyclone/include/iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 30
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "/home/jgm/cyclone/include/set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "/home/jgm/cyclone/include/set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*
s);
# 38 "/home/jgm/cyclone/include/string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "/home/jgm/cyclone/include/string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "/home/jgm/cyclone/include/string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct Cyc_Hashtable_Table;
# 35 "/home/jgm/cyclone/include/hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*
hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 30 "/home/jgm/cyclone/include/filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct
_dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "/home/jgm/cyclone/include/lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "/home/jgm/cyclone/include/position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{
int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{
int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct
_dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};extern char Cyc_Arg_Bad[4];
struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[
6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(
struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;
void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct
Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "/home/jgm/cyclone/include/arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),
struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "/home/jgm/cyclone/include/buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "/home/jgm/cyclone/include/buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);
# 80 "/home/jgm/cyclone/src/absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
# 81
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int
tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 94
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public
 = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz
 = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
 = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct
Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None
 = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{
int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{
int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{
int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};
struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{
int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
# 281
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{
struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct
Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;
unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;
void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int
tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;
void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{
int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{
int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct
Cyc_List_List*f5;};
# 444 "/home/jgm/cyclone/src/absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct
Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{
int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type
f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;
int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{
int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};
struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts
f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int
tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{
int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 507
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};
struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;
struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};
struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;
struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct
_union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;
long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct
_tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct
_tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct
_union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct
_union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct
_union_Cnst_Wstring_c Wstring_c;};
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 
2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{
int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 
1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst
f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;
};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;
void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*
f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct
_tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};
struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;
};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{
int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct
_dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*
annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*
f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int
tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;
struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int
tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{
int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;
int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{
int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;
unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;
struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;
};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual
ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;
struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual
tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;
struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind
kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};
struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;
struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;
};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*
f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{
int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{
void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{
int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{
char*tag;};
# 1096 "/home/jgm/cyclone/src/absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct _tuple10{
enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1157
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "/home/jgm/cyclone/include/pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int
expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int
rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;
int print_using_stmts;int print_externC_stmts;int print_full_evars;int
print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*
curr_namespace;};
# 60 "/home/jgm/cyclone/src/absynpp.h"
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 32 "/home/jgm/cyclone/src/parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct _tuple11{
struct _tuple11*tl;};struct Cyc_FlatList{struct _tuple11*tl;};
# 36
typedef struct _tuple11*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;
int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int
Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct
_tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator
f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _tuple13*tl;struct _tuple12 hd;};
# 53
typedef struct _tuple13*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,
Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{
enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier
type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{
struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct
_union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{
int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;
};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct
_union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple14{int f1;struct
_dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple14 val;};
struct _tuple15{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple15*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple16{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple17*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec val;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY21{int tag;
struct _tuple13*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class*val;
};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Type_specifier val;};struct
_union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};struct
_union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY29{int tag;struct Cyc_Declarator val;};struct _union_YYSTYPE_YY30{
int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;
};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct
_union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct
_union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual
f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{
int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct
_union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct
_union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int
tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};
struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*
val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct
_union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct
_union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;
struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7
YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct
_union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12
YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct
_union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17
YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct
_union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22
YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct
_union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27
YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct
_union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32
YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct
_union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37
YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct
_union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42
YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct
_union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47
YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct
_union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52
YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct
_union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};
# 75 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Dict_T;
# 46 "/home/jgm/cyclone/include/dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{
char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "/home/jgm/cyclone/src/rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
# 43
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
# 44
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);
# 45
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
# 46
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*
eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{
void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "/home/jgm/cyclone/src/tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
# 41
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*
f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 51
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{
int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};
# 85
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 114 "/home/jgm/cyclone/src/tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 34 "/home/jgm/cyclone/src/tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);
# 81 "/home/jgm/cyclone/src/buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 89
extern char*Ccomp;
# 91
static int Cyc_do_setjmp=0;
# 93
struct Cyc___cycFILE*Cyc_log_file=0;
# 94
struct Cyc___cycFILE*Cyc_cstubs_file=0;
# 95
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 97
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 100
if(Cyc_log_file == 0){
# 101
{const char*_tmp407;void*_tmp406;(_tmp406=0,Cyc_fprintf(Cyc_stderr,((_tmp407="Internal error: log file is NULL\n",
_tag_dyneither(_tmp407,sizeof(char),34))),_tag_dyneither(_tmp406,sizeof(void*),0)));}
# 102
exit(1);}{
# 104
int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
# 105
Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
# 106
return _tmp2;};}
# 109
static struct _dyneither_ptr*Cyc_current_source=0;
# 110
static struct Cyc_List_List*Cyc_current_args=0;
# 111
static struct Cyc_Set_Set**Cyc_current_targets=0;
# 112
static void Cyc_add_target(struct _dyneither_ptr*sptr){
# 113
struct Cyc_Set_Set**_tmp408;Cyc_current_targets=((_tmp408=_cycalloc(sizeof(*
_tmp408)),((_tmp408[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr),_tmp408))));}struct _tuple20{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 116
typedef struct _tuple20*Cyc_line_t;
# 117
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*);
# 118
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
# 119
int Cyc_args(struct Cyc_Lexing_lexbuf*);
# 120
int Cyc_token(struct Cyc_Lexing_lexbuf*);
# 121
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 123
struct Cyc___cycFILE*Cyc_slurp_out=0;
# 124
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
# 125
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
# 126
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
# 127
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
# 128
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple21{struct
_dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 130
typedef struct _tuple21*Cyc_suck_line_t;
# 131
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
# 132
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
# 133
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
# 134
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct
_tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 138
typedef struct _tuple22*Cyc_ifdefined_t;struct _tuple23{struct _dyneither_ptr f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*
f5;struct Cyc_List_List*f6;};
# 140
typedef struct _tuple23*Cyc_spec_t;
# 142
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*);
# 143
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
# 144
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
# 145
int Cyc_block(struct Cyc_Lexing_lexbuf*);
# 146
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
# 147
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
# 148
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
# 149
struct Cyc_List_List*Cyc_snarfed_symbols=0;
# 150
struct Cyc_List_List*Cyc_current_symbols=0;
# 151
struct Cyc_List_List*Cyc_current_cstubs=0;
# 152
struct Cyc_List_List*Cyc_current_cycstubs=0;
# 153
struct Cyc_List_List*Cyc_current_hstubs=0;
# 154
struct Cyc_List_List*Cyc_current_omit_symbols=0;
# 155
struct Cyc_List_List*Cyc_current_cpp=0;
# 156
struct Cyc_Buffer_t*Cyc_specbuf=0;
# 157
int Cyc_braces_to_match=0;
# 158
int Cyc_parens_to_match=0;
# 160
int Cyc_numdef=0;
# 162
static struct Cyc_List_List*Cyc_cppargs=0;
# 165
const int Cyc_lex_base[425]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,
715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,
29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,74,100,91,107,
94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,525,176,210,228,
214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,684,- 8,720,- 11,
432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,244,254,0,13,4,
2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,1051,1056,261,314,
242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,300,313,321,326,- 15,
309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,422,421,415,436,433,
449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,449,482,489,490,4,6,
21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,53,627,644,- 13,647,
594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,659,623,624,639,
645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,714,770,797,798,
746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,772,767,769,772,
773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,875,1370,893,894,
892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,1150,- 2,945,- 4,974,
998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,2433,994,1057,- 36,- 42,
- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,1467,1081,1091,1563,
2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,1157,1195,1206,1198,
1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,1112,1117,2128,1119,
1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,- 18,2239,1192,- 24,- 41,
- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,1171,1185,1255};
# 166
const int Cyc_lex_backtrk[425]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,
13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,
6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,
48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,
7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,
1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
# 167
const int Cyc_lex_default[425]={- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,
31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,
0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,0,- 1,0,- 1,- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,
- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,
- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
# 168
const int Cyc_lex_trans[3481]={0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,
19,45,45,45,45,45,22,45,0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,
224,22,415,415,415,415,415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,31,217,218,221,415,130,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,413,413,413,413,
413,413,413,413,413,413,121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,
60,413,61,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,
69,70,129,34,34,34,34,34,34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,
325,326,104,327,328,329,330,331,331,331,331,331,331,331,331,331,332,77,333,334,
335,104,19,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
307,338,37,308,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,
309,88,95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,192,193,26,35,35,
35,35,35,35,35,35,178,172,163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,
78,73,150,78,151,152,153,154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,
159,143,144,145,160,146,161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,
113,113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,- 1,169,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,
175,97,97,104,104,97,176,104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,
80,21,97,182,104,183,184,185,186,112,187,188,189,190,191,271,194,105,195,80,196,
112,112,91,197,112,198,199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,
106,21,112,200,201,202,206,207,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,208,209,210,211,106,212,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,
92,219,220,92,222,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,
225,- 1,226,- 1,227,228,99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,
35,35,35,35,35,99,235,236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,
243,240,50,241,244,245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,
137,137,137,137,137,137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,
259,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,102,100,261,262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,
33,269,270,272,273,288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,
282,36,284,285,286,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,
291,44,292,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,
44,411,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,78,78,410,408,78,- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,
351,383,346,341,78,343,344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,
79,79,79,79,79,79,79,79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,
80,294,155,80,287,287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,
278,80,270,223,275,275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,
79,79,79,79,79,275,100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,
294,419,292,316,316,277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,
292,0,316,0,0,316,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,
375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,0,0,0,0,90,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,91,92,92,300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,
0,0,0,92,0,300,412,0,412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,
412,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,
378,0,0,0,0,357,0,357,0,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,
358,358,358,358,358,358,358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,
362,362,362,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,
108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,108,
108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,
109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,
0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,
0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,
28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,127,
127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,
0,0,0,0,0,0,0,31,0,0,- 1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,
405,405,405,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,
127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,
136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,
136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,
0,0,0,138,312,312,312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,
136,136,136,136,136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,
314,314,314,314,314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,
314,314,314,314,314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,
314,0,0,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,
314,314,314,314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,
353,353,353,353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,
0,0,0,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,
356,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,359,359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,
352,93,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,
358,358,0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,
359,359,359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,
362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,
93,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,
0,0,0,0,373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,
0,0,354,0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,
378,378,378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,
368,368,368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,
360,93,0,0,0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,
368,368,368,368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,
370,0,0,390,390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,
369,0,0,0,0,0,0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,
395,0,0,0,0,0,0,0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,
401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,402,0,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,
0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,
413,413,413,413,413,413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
0,0,0,0,413,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,
415,415,415,415,415,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
# 169
const int Cyc_lex_check[3481]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,
131,133,308,311,41,41,45,45,41,417,45,- 1,- 1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,
10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,204,10,38,213,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,216,217,220,1,129,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,
30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,
81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,
83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,
118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,
27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,
65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,163,
164,73,142,165,166,167,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,
168,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,88,88,66,169,88,170,171,172,173,174,96,96,104,104,96,175,104,
105,105,176,178,105,179,88,180,13,13,80,80,13,73,80,16,96,181,104,182,183,184,
185,105,186,187,188,189,190,192,193,13,194,80,195,112,112,4,196,112,197,198,5,6,
8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,199,200,201,205,206,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,207,208,209,210,
13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,218,219,92,221,
14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,222,92,224,103,225,110,
226,227,99,99,228,14,99,229,230,231,232,32,32,32,32,32,32,128,128,128,128,128,
128,128,128,99,234,235,236,238,111,237,130,15,15,237,239,15,101,101,240,241,101,
242,237,14,237,243,244,245,14,14,246,247,248,249,15,14,250,251,252,101,134,134,
134,134,134,134,134,134,253,254,15,15,15,15,15,15,15,15,15,15,237,255,256,257,92,
258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,267,33,33,33,33,33,33,33,33,33,
33,268,269,271,272,276,277,278,33,33,33,33,33,33,137,137,137,137,137,137,137,137,
279,280,281,282,283,284,285,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,294,295,296,15,297,298,299,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,
302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,329,273,312,312,312,312,312,
312,312,312,332,326,334,337,78,335,335,323,139,139,273,344,139,140,140,324,327,
140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,327,327,377,386,140,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,333,333,333,
338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,79,79,139,388,79,287,287,140,389,287,391,318,139,318,318,392,393,140,
355,356,365,366,140,79,394,395,275,275,287,396,275,293,293,338,318,293,287,397,
398,79,79,79,79,79,79,79,79,79,79,275,355,356,365,366,293,369,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,399,406,416,370,79,
369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,89,89,275,370,89,292,292,293,418,292,309,309,275,419,309,420,421,293,371,422,
372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,309,- 1,309,292,- 1,- 1,89,89,89,89,
89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,374,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,300,300,90,
- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,315,315,- 1,- 1,- 1,90,- 1,300,412,
- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,90,90,90,90,90,90,90,412,- 1,- 1,
- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,328,328,328,328,328,328,328,328,
- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,
- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,
98,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,360,- 1,360,- 1,98,360,360,
360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,
106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,-
1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,
108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,
109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,
126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,
132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,390,- 1,- 1,
126,126,126,126,126,126,390,390,390,390,390,390,390,390,- 1,123,127,127,127,127,
127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,
135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,
- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,136,136,136,136,136,136,
136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,405,405,405,405,405,- 1,136,
136,136,136,136,136,- 1,313,313,313,313,313,313,313,313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,361,361,361,361,361,361,361,- 1,
- 1,314,314,314,314,314,314,314,314,314,314,310,313,313,313,313,313,313,314,314,
314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,
- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,
330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,336,336,336,336,336,
336,336,336,336,336,336,336,336,336,336,336,336,336,336,342,342,342,342,342,342,
342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,342,- 1,
342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,
342,342,342,342,342,342,352,352,352,352,352,352,352,352,352,352,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,353,353,353,353,353,353,353,353,
353,358,358,358,358,358,358,358,358,358,358,- 1,353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,
352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,
353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,- 1,359,353,358,- 1,- 1,362,362,362,
362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,-
1,359,362,363,- 1,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,- 1,- 1,- 1,- 1,- 1,364,363,364,364,364,
364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,-
1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,378,378,378,378,378,378,378,378,
378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,- 1,- 1,- 1,- 1,- 1,378,364,367,367,
367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,367,367,367,367,367,378,
378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,368,368,368,368,368,368,368,368,
368,- 1,367,367,367,367,367,367,368,368,368,368,368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,
387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,387,387,387,387,- 1,368,368,368,368,
368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,
- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,400,400,400,400,400,400,400,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,
400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,
400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,
401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,
413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,413,413,413,413,413,413,413,
413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,415,- 1,-
1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,415,415,415,415,415,415,
415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
# 170
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 172
int state;int base;int backtrk;
# 173
int c;
# 174
state=start_state;
# 176
if(state >= 0){
# 177
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
# 178
lbuf->lex_last_action=- 1;}else{
# 180
state=(- state)- 1;}
# 182
while(1){
# 183
base=Cyc_lex_base[_check_known_subscript_notnull(425,state)];
# 184
if(base < 0)return(- base)- 1;
# 185
backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];
# 186
if(backtrk >= 0){
# 187
lbuf->lex_last_pos=lbuf->lex_curr_pos;
# 188
lbuf->lex_last_action=backtrk;}
# 190
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
# 191
if(!lbuf->lex_eof_reached)
# 192
return(- state)- 1;else{
# 194
c=256;}}else{
# 196
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos
++));
# 197
if(c == - 1)c=256;}
# 199
if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]== state)
# 200
state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
# 202
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}
# 203
if(state < 0){
# 204
lbuf->lex_curr_pos=lbuf->lex_last_pos;
# 205
if(lbuf->lex_last_action == - 1){
# 206
struct Cyc_Lexing_Error_exn_struct _tmp40E;const char*_tmp40D;struct Cyc_Lexing_Error_exn_struct*
_tmp40C;(int)_throw((void*)((_tmp40C=_cycalloc(sizeof(*_tmp40C)),((_tmp40C[0]=((
_tmp40E.tag=Cyc_Lexing_Error,((_tmp40E.f1=((_tmp40D="empty token",_tag_dyneither(
_tmp40D,sizeof(char),12))),_tmp40E)))),_tmp40C)))));}else{
# 208
return lbuf->lex_last_action;}}else{
# 211
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 215
struct _tuple20*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 216
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 217
switch(lexstate){case 0: _LL0:
# 174 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_macroname(lexbuf);
# 175
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(
Cyc_current_args))->tl){
# 176
struct Cyc_Set_Set**_tmp40F;Cyc_current_targets=((_tmp40F=_cycalloc(sizeof(*
_tmp40F)),((_tmp40F[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),
_tmp40F))));}{
# 179
struct _tuple20*_tmp410;return(_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp410->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp410)))));};case 1: _LL1:
# 182 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL2:
# 184
 return 0;default: _LL3:
# 185
(lexbuf->refill_buff)(lexbuf);
# 186
return Cyc_line_rec(lexbuf,lexstate);}{
# 188
struct Cyc_Lexing_Error_exn_struct _tmp416;const char*_tmp415;struct Cyc_Lexing_Error_exn_struct*
_tmp414;(int)_throw((void*)((_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414[0]=((
_tmp416.tag=Cyc_Lexing_Error,((_tmp416.f1=((_tmp415="some action didn't return!",
_tag_dyneither(_tmp415,sizeof(char),27))),_tmp416)))),_tmp414)))));};}
# 190
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(
lexbuf,0);}
# 191
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 192
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 193
switch(lexstate){case 0: _LL5:
# 188 "/home/jgm/cyclone/src/buildlib.cyl"
{struct _dyneither_ptr*_tmp417;Cyc_current_source=((_tmp417=_cycalloc(sizeof(*
_tmp417)),((_tmp417[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
# 189
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp417))));}
# 190
Cyc_current_args=0;
# 191
{struct Cyc_Set_Set**_tmp418;Cyc_current_targets=((_tmp418=_cycalloc(sizeof(*
_tmp418)),((_tmp418[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp418))));}
# 192
Cyc_token(lexbuf);
# 193
return 0;case 1: _LL6:
# 196
{struct _dyneither_ptr*_tmp419;Cyc_current_source=((_tmp419=_cycalloc(sizeof(*
_tmp419)),((_tmp419[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
# 197
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp419))));}
# 198
Cyc_current_args=0;
# 199
{struct Cyc_Set_Set**_tmp41A;Cyc_current_targets=((_tmp41A=_cycalloc(sizeof(*
_tmp41A)),((_tmp41A[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp41A))));}
# 200
Cyc_args(lexbuf);
# 201
return 0;case 2: _LL7:
# 204
{struct _dyneither_ptr*_tmp41B;Cyc_current_source=((_tmp41B=_cycalloc(sizeof(*
_tmp41B)),((_tmp41B[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp41B))));}
# 205
Cyc_current_args=0;
# 206
{struct Cyc_Set_Set**_tmp41C;Cyc_current_targets=((_tmp41C=_cycalloc(sizeof(*
_tmp41C)),((_tmp41C[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp41C))));}
# 207
Cyc_token(lexbuf);
# 208
return 0;default: _LL8:
# 210
(lexbuf->refill_buff)(lexbuf);
# 211
return Cyc_macroname_rec(lexbuf,lexstate);}{
# 213
struct Cyc_Lexing_Error_exn_struct _tmp422;const char*_tmp421;struct Cyc_Lexing_Error_exn_struct*
_tmp420;(int)_throw((void*)((_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=((
_tmp422.tag=Cyc_Lexing_Error,((_tmp422.f1=((_tmp421="some action didn't return!",
_tag_dyneither(_tmp421,sizeof(char),27))),_tmp422)))),_tmp420)))));};}
# 215
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,
1);}
# 216
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 217
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 218
switch(lexstate){case 0: _LLA: {
# 213 "/home/jgm/cyclone/src/buildlib.cyl"
struct _dyneither_ptr*_tmp423;struct _dyneither_ptr*_tmp15=(_tmp423=_cycalloc(
sizeof(*_tmp423)),((_tmp423[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
# 214
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp423)));
# 215
{struct Cyc_List_List*_tmp424;Cyc_current_args=((_tmp424=_cycalloc(sizeof(*
_tmp424)),((_tmp424->hd=_tmp15,((_tmp424->tl=Cyc_current_args,_tmp424))))));}
# 216
return Cyc_args(lexbuf);}case 1: _LLB: {
# 219
struct _dyneither_ptr*_tmp425;struct _dyneither_ptr*_tmp18=(_tmp425=_cycalloc(
sizeof(*_tmp425)),((_tmp425[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
# 220
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp425)));
# 221
{struct Cyc_List_List*_tmp426;Cyc_current_args=((_tmp426=_cycalloc(sizeof(*
_tmp426)),((_tmp426->hd=_tmp18,((_tmp426->tl=Cyc_current_args,_tmp426))))));}
# 222
return Cyc_args(lexbuf);}case 2: _LLC: {
# 225
struct _dyneither_ptr*_tmp427;struct _dyneither_ptr*_tmp1B=(_tmp427=_cycalloc(
sizeof(*_tmp427)),((_tmp427[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
# 226
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp427)));
# 227
{struct Cyc_List_List*_tmp428;Cyc_current_args=((_tmp428=_cycalloc(sizeof(*
_tmp428)),((_tmp428->hd=_tmp1B,((_tmp428->tl=Cyc_current_args,_tmp428))))));}
# 228
return Cyc_token(lexbuf);}default: _LLD:
# 230
(lexbuf->refill_buff)(lexbuf);
# 231
return Cyc_args_rec(lexbuf,lexstate);}{
# 233
struct Cyc_Lexing_Error_exn_struct _tmp42E;const char*_tmp42D;struct Cyc_Lexing_Error_exn_struct*
_tmp42C;(int)_throw((void*)((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=((
_tmp42E.tag=Cyc_Lexing_Error,((_tmp42E.f1=((_tmp42D="some action didn't return!",
_tag_dyneither(_tmp42D,sizeof(char),27))),_tmp42E)))),_tmp42C)))));};}
# 235
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
# 236
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 237
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 238
switch(lexstate){case 0: _LLF:
# 234 "/home/jgm/cyclone/src/buildlib.cyl"
{struct _dyneither_ptr*_tmp42F;Cyc_add_target(((_tmp42F=_cycalloc(sizeof(*
_tmp42F)),((_tmp42F[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp42F)))));}
return Cyc_token(lexbuf);case 1: _LL10:
# 237 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 2: _LL11:
# 240 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL12:
# 243 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL13:
# 246 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL14:
# 248
 return Cyc_token(lexbuf);case 6: _LL15:
# 250
 return Cyc_token(lexbuf);case 7: _LL16:
# 252
 return Cyc_token(lexbuf);case 8: _LL17:
# 255 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL18:
# 258 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL19:
# 261 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL1A:
# 263
 return Cyc_token(lexbuf);case 12: _LL1B:
# 265
 return Cyc_token(lexbuf);case 13: _LL1C:
# 267
 return Cyc_token(lexbuf);case 14: _LL1D:
# 269
 return Cyc_token(lexbuf);case 15: _LL1E:
# 271
 return Cyc_token(lexbuf);case 16: _LL1F:
# 273
 return Cyc_token(lexbuf);case 17: _LL20:
# 275
 return Cyc_token(lexbuf);case 18: _LL21:
# 277
 return Cyc_token(lexbuf);case 19: _LL22:
# 279
 return Cyc_token(lexbuf);case 20: _LL23:
# 281
 return Cyc_token(lexbuf);case 21: _LL24:
# 283
 return Cyc_token(lexbuf);case 22: _LL25:
# 285
 return Cyc_token(lexbuf);case 23: _LL26:
# 287
 return Cyc_token(lexbuf);case 24: _LL27:
# 290 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LL28:
# 292
 return Cyc_token(lexbuf);case 26: _LL29:
# 294
 return Cyc_token(lexbuf);case 27: _LL2A:
# 296
 return Cyc_token(lexbuf);case 28: _LL2B:
# 298
 return Cyc_token(lexbuf);case 29: _LL2C:
# 300
 return Cyc_token(lexbuf);case 30: _LL2D:
# 302
 return Cyc_token(lexbuf);case 31: _LL2E:
# 304
 return Cyc_token(lexbuf);case 32: _LL2F:
# 306
 return Cyc_token(lexbuf);case 33: _LL30:
# 308
 return Cyc_token(lexbuf);case 34: _LL31:
# 310
 return Cyc_token(lexbuf);case 35: _LL32:
# 312
 return Cyc_token(lexbuf);case 36: _LL33:
# 314
 return Cyc_token(lexbuf);case 37: _LL34:
# 316
 return Cyc_token(lexbuf);case 38: _LL35:
# 318
 return Cyc_token(lexbuf);case 39: _LL36:
# 320
 return Cyc_token(lexbuf);case 40: _LL37:
# 322
 return Cyc_token(lexbuf);case 41: _LL38:
# 324
 return Cyc_token(lexbuf);case 42: _LL39:
# 326
 return Cyc_token(lexbuf);case 43: _LL3A:
# 328
 return Cyc_token(lexbuf);case 44: _LL3B:
# 330
 return Cyc_token(lexbuf);case 45: _LL3C:
# 332
 return Cyc_token(lexbuf);case 46: _LL3D:
# 334
 return Cyc_token(lexbuf);case 47: _LL3E:
# 336
 return Cyc_token(lexbuf);case 48: _LL3F:
# 339 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL40:
# 340
(lexbuf->refill_buff)(lexbuf);
# 341
return Cyc_token_rec(lexbuf,lexstate);}{
# 343
struct Cyc_Lexing_Error_exn_struct _tmp435;const char*_tmp434;struct Cyc_Lexing_Error_exn_struct*
_tmp433;(int)_throw((void*)((_tmp433=_cycalloc(sizeof(*_tmp433)),((_tmp433[0]=((
_tmp435.tag=Cyc_Lexing_Error,((_tmp435.f1=((_tmp434="some action didn't return!",
_tag_dyneither(_tmp434,sizeof(char),27))),_tmp435)))),_tmp433)))));};}
# 345
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
# 346
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 347
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 348
switch(lexstate){case 0: _LL42:
# 344 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LL43:
# 345 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 2: _LL44:
# 346 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LL45:
# 347 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LL46:
# 350 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LL47:
# 353 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LL48:
# 355
 return Cyc_string(lexbuf);case 7: _LL49:
# 356 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 8: _LL4A:
# 357 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 9: _LL4B:
# 358 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL4C:
# 359
(lexbuf->refill_buff)(lexbuf);
# 360
return Cyc_string_rec(lexbuf,lexstate);}{
# 362
struct Cyc_Lexing_Error_exn_struct _tmp43B;const char*_tmp43A;struct Cyc_Lexing_Error_exn_struct*
_tmp439;(int)_throw((void*)((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439[0]=((
_tmp43B.tag=Cyc_Lexing_Error,((_tmp43B.f1=((_tmp43A="some action didn't return!",
_tag_dyneither(_tmp43A,sizeof(char),27))),_tmp43B)))),_tmp439)))));};}
# 364
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
# 365
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 366
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 367
switch(lexstate){case 0: _LL4E:
# 367 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 1: _LL4F:
# 369
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
# 370
while(Cyc_slurp_string(lexbuf)){;}
# 371
return 1;case 2: _LL50:
# 376 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));
# 377
{const char*_tmp43E;void*_tmp43D;(_tmp43D=0,Cyc_log(((_tmp43E="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp43E,sizeof(char),44))),_tag_dyneither(_tmp43D,sizeof(void*),0)));}
# 378
return 1;case 3: _LL51:
# 382 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));
# 383
{const char*_tmp441;void*_tmp440;(_tmp440=0,Cyc_log(((_tmp441="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp441,sizeof(char),44))),_tag_dyneither(_tmp440,sizeof(void*),0)));}
# 384
return 1;case 4: _LL52:
# 388 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));
# 389
{const char*_tmp444;void*_tmp443;(_tmp443=0,Cyc_log(((_tmp444="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp444,sizeof(char),44))),_tag_dyneither(_tmp443,sizeof(void*),0)));}
# 390
return 1;case 5: _LL53:
# 394 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));
# 395
{const char*_tmp447;void*_tmp446;(_tmp446=0,Cyc_log(((_tmp447="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp447,sizeof(char),44))),_tag_dyneither(_tmp446,sizeof(void*),0)));}
# 396
return 1;case 6: _LL54:
# 398
 Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
# 399
{const char*_tmp44A;void*_tmp449;(_tmp449=0,Cyc_log(((_tmp44A="Warning: use of region sidestepped\n",
_tag_dyneither(_tmp44A,sizeof(char),36))),_tag_dyneither(_tmp449,sizeof(void*),0)));}
# 400
return 1;case 7: _LL55:
# 402
{const char*_tmp44D;void*_tmp44C;(_tmp44C=0,Cyc_log(((_tmp44D="Warning: use of __extension__ deleted\n",
_tag_dyneither(_tmp44D,sizeof(char),39))),_tag_dyneither(_tmp44C,sizeof(void*),0)));}
# 403
return 1;case 8: _LL56:
# 407 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp450;void*_tmp44F;(_tmp44F=0,Cyc_log(((_tmp450="Warning: use of mode HI deleted\n",
_tag_dyneither(_tmp450,sizeof(char),33))),_tag_dyneither(_tmp44F,sizeof(void*),0)));}
# 408
return 1;case 9: _LL57:
# 410
{const char*_tmp453;void*_tmp452;(_tmp452=0,Cyc_log(((_tmp453="Warning: use of mode SI deleted\n",
_tag_dyneither(_tmp453,sizeof(char),33))),_tag_dyneither(_tmp452,sizeof(void*),0)));}
# 411
return 1;case 10: _LL58:
# 413
{const char*_tmp456;void*_tmp455;(_tmp455=0,Cyc_log(((_tmp456="Warning: use of mode QI deleted\n",
_tag_dyneither(_tmp456,sizeof(char),33))),_tag_dyneither(_tmp455,sizeof(void*),0)));}
# 414
return 1;case 11: _LL59:
# 416
{const char*_tmp459;void*_tmp458;(_tmp458=0,Cyc_log(((_tmp459="Warning: use of mode DI deleted\n",
_tag_dyneither(_tmp459,sizeof(char),33))),_tag_dyneither(_tmp458,sizeof(void*),0)));}
# 417
return 1;case 12: _LL5A:
# 419
{const char*_tmp45C;void*_tmp45B;(_tmp45B=0,Cyc_log(((_tmp45C="Warning: use of mode DI deleted\n",
_tag_dyneither(_tmp45C,sizeof(char),33))),_tag_dyneither(_tmp45B,sizeof(void*),0)));}
# 420
return 1;case 13: _LL5B:
# 422
{const char*_tmp45F;void*_tmp45E;(_tmp45E=0,Cyc_log(((_tmp45F="Warning: use of mode word deleted\n",
_tag_dyneither(_tmp45F,sizeof(char),35))),_tag_dyneither(_tmp45E,sizeof(void*),0)));}
# 423
return 1;case 14: _LL5C:
# 425
 Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 15: _LL5D:
# 427
 Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 16: _LL5E:
# 429
 Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 17: _LL5F:
# 431
 Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 18: _LL60:
# 436 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 19: _LL61:
# 438
 return 1;case 20: _LL62:
# 440
 Cyc_parens_to_match=1;
# 441
while(Cyc_asmtok(lexbuf)){;}
# 442
Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
# 443
{const char*_tmp462;void*_tmp461;(_tmp461=0,Cyc_log(((_tmp462="Warning: replacing use of __asm__ with 0\n",
_tag_dyneither(_tmp462,sizeof(char),42))),_tag_dyneither(_tmp461,sizeof(void*),0)));}
# 444
return 1;case 21: _LL63:
# 446
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL64:
# 447
(lexbuf->refill_buff)(lexbuf);
# 448
return Cyc_slurp_rec(lexbuf,lexstate);}{
# 450
struct Cyc_Lexing_Error_exn_struct _tmp468;const char*_tmp467;struct Cyc_Lexing_Error_exn_struct*
_tmp466;(int)_throw((void*)((_tmp466=_cycalloc(sizeof(*_tmp466)),((_tmp466[0]=((
_tmp468.tag=Cyc_Lexing_Error,((_tmp468.f1=((_tmp467="some action didn't return!",
_tag_dyneither(_tmp467,sizeof(char),27))),_tmp468)))),_tmp466)))));};}
# 452
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
# 453
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 454
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 455
switch(lexstate){case 0: _LL66:
# 450 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 1: _LL67:
# 452
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;
case 2: _LL68:
# 454
{const char*_tmp46B;void*_tmp46A;(_tmp46A=0,Cyc_log(((_tmp46B="Warning: unclosed string\n",
_tag_dyneither(_tmp46B,sizeof(char),26))),_tag_dyneither(_tmp46A,sizeof(void*),0)));}
# 455
{const char*_tmp46F;void*_tmp46E[1];struct Cyc_String_pa_PrintArg_struct _tmp46D;(
_tmp46D.tag=0,((_tmp46D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp46E[0]=& _tmp46D,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp46F="%s",_tag_dyneither(_tmp46F,sizeof(char),3))),_tag_dyneither(_tmp46E,
sizeof(void*),1)))))));}return 1;case 3: _LL69:
# 457
{const char*_tmp473;void*_tmp472[1];struct Cyc_String_pa_PrintArg_struct _tmp471;(
_tmp471.tag=0,((_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp472[0]=& _tmp471,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp473="%s",_tag_dyneither(_tmp473,sizeof(char),3))),_tag_dyneither(_tmp472,
sizeof(void*),1)))))));}return 1;case 4: _LL6A:
# 459
{const char*_tmp477;void*_tmp476[1];struct Cyc_String_pa_PrintArg_struct _tmp475;(
_tmp475.tag=0,((_tmp475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp476[0]=& _tmp475,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp477="%s",_tag_dyneither(_tmp477,sizeof(char),3))),_tag_dyneither(_tmp476,
sizeof(void*),1)))))));}return 1;case 5: _LL6B:
# 461
{const char*_tmp47B;void*_tmp47A[1];struct Cyc_String_pa_PrintArg_struct _tmp479;(
_tmp479.tag=0,((_tmp479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp47A[0]=& _tmp479,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp47B="%s",_tag_dyneither(_tmp47B,sizeof(char),3))),_tag_dyneither(_tmp47A,
sizeof(void*),1)))))));}return 1;case 6: _LL6C:
# 463
{const char*_tmp47F;void*_tmp47E[1];struct Cyc_String_pa_PrintArg_struct _tmp47D;(
_tmp47D.tag=0,((_tmp47D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp47E[0]=& _tmp47D,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp47F="%s",_tag_dyneither(_tmp47F,sizeof(char),3))),_tag_dyneither(_tmp47E,
sizeof(void*),1)))))));}return 1;case 7: _LL6D:
# 465
{const char*_tmp483;void*_tmp482[1];struct Cyc_String_pa_PrintArg_struct _tmp481;(
_tmp481.tag=0,((_tmp481.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp482[0]=& _tmp481,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp483="%s",_tag_dyneither(_tmp483,sizeof(char),3))),_tag_dyneither(_tmp482,
sizeof(void*),1)))))));}return 1;case 8: _LL6E:
# 467
{const char*_tmp487;void*_tmp486[1];struct Cyc_String_pa_PrintArg_struct _tmp485;(
_tmp485.tag=0,((_tmp485.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp486[0]=& _tmp485,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp487="%s",_tag_dyneither(_tmp487,sizeof(char),3))),_tag_dyneither(_tmp486,
sizeof(void*),1)))))));}return 1;default: _LL6F:
# 468
(lexbuf->refill_buff)(lexbuf);
# 469
return Cyc_slurp_string_rec(lexbuf,lexstate);}{
# 471
struct Cyc_Lexing_Error_exn_struct _tmp48D;const char*_tmp48C;struct Cyc_Lexing_Error_exn_struct*
_tmp48B;(int)_throw((void*)((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((_tmp48B[0]=((
_tmp48D.tag=Cyc_Lexing_Error,((_tmp48D.f1=((_tmp48C="some action didn't return!",
_tag_dyneither(_tmp48C,sizeof(char),27))),_tmp48D)))),_tmp48B)))));};}
# 473
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}
# 474
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 475
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 476
switch(lexstate){case 0: _LL71:
# 477 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 1: _LL72:
# 479
 if(Cyc_parens_to_match == 1)return 0;
# 480
-- Cyc_parens_to_match;
# 481
return 1;case 2: _LL73:
# 483
 ++ Cyc_parens_to_match;
# 484
return 1;case 3: _LL74:
# 486
 while(Cyc_asm_string(lexbuf)){;}
# 487
return 1;case 4: _LL75:
# 489
 while(Cyc_asm_comment(lexbuf)){;}
# 490
return 1;case 5: _LL76:
# 492
 return 1;case 6: _LL77:
# 494
 return 1;default: _LL78:
# 495
(lexbuf->refill_buff)(lexbuf);
# 496
return Cyc_asmtok_rec(lexbuf,lexstate);}{
# 498
struct Cyc_Lexing_Error_exn_struct _tmp493;const char*_tmp492;struct Cyc_Lexing_Error_exn_struct*
_tmp491;(int)_throw((void*)((_tmp491=_cycalloc(sizeof(*_tmp491)),((_tmp491[0]=((
_tmp493.tag=Cyc_Lexing_Error,((_tmp493.f1=((_tmp492="some action didn't return!",
_tag_dyneither(_tmp492,sizeof(char),27))),_tmp493)))),_tmp491)))));};}
# 500
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
# 501
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 502
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 503
switch(lexstate){case 0: _LL7A:
# 498 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp496;void*_tmp495;(_tmp495=0,Cyc_log(((_tmp496="Warning: unclosed string\n",
_tag_dyneither(_tmp496,sizeof(char),26))),_tag_dyneither(_tmp495,sizeof(void*),0)));}
return 0;case 1: _LL7B:
# 500
 return 0;case 2: _LL7C:
# 502
{const char*_tmp499;void*_tmp498;(_tmp498=0,Cyc_log(((_tmp499="Warning: unclosed string\n",
_tag_dyneither(_tmp499,sizeof(char),26))),_tag_dyneither(_tmp498,sizeof(void*),0)));}
return 1;case 3: _LL7D:
# 504
 return 1;case 4: _LL7E:
# 506
 return 1;case 5: _LL7F:
# 508
 return 1;case 6: _LL80:
# 510
 return 1;case 7: _LL81:
# 512
 return 1;case 8: _LL82:
# 514
 return 1;default: _LL83:
# 515
(lexbuf->refill_buff)(lexbuf);
# 516
return Cyc_asm_string_rec(lexbuf,lexstate);}{
# 518
struct Cyc_Lexing_Error_exn_struct _tmp49F;const char*_tmp49E;struct Cyc_Lexing_Error_exn_struct*
_tmp49D;(int)_throw((void*)((_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((
_tmp49F.tag=Cyc_Lexing_Error,((_tmp49F.f1=((_tmp49E="some action didn't return!",
_tag_dyneither(_tmp49E,sizeof(char),27))),_tmp49F)))),_tmp49D)))));};}
# 520
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}
# 521
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 522
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 523
switch(lexstate){case 0: _LL85:
# 518 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp4A2;void*_tmp4A1;(_tmp4A1=0,Cyc_log(((_tmp4A2="Warning: unclosed comment\n",
_tag_dyneither(_tmp4A2,sizeof(char),27))),_tag_dyneither(_tmp4A1,sizeof(void*),0)));}
return 0;case 1: _LL86:
# 520
 return 0;case 2: _LL87:
# 522
 return 1;default: _LL88:
# 523
(lexbuf->refill_buff)(lexbuf);
# 524
return Cyc_asm_comment_rec(lexbuf,lexstate);}{
# 526
struct Cyc_Lexing_Error_exn_struct _tmp4A8;const char*_tmp4A7;struct Cyc_Lexing_Error_exn_struct*
_tmp4A6;(int)_throw((void*)((_tmp4A6=_cycalloc(sizeof(*_tmp4A6)),((_tmp4A6[0]=((
_tmp4A8.tag=Cyc_Lexing_Error,((_tmp4A8.f1=((_tmp4A7="some action didn't return!",
_tag_dyneither(_tmp4A7,sizeof(char),27))),_tmp4A8)))),_tmp4A6)))));};}
# 528
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}
# 529
struct _tuple21*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 530
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 531
switch(lexstate){case 0: _LL8A:
# 530 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp4A9;Cyc_current_line=((_tmp4A9="#define ",_tag_dyneither(_tmp4A9,
sizeof(char),9)));}
# 531
Cyc_suck_macroname(lexbuf);{
# 532
struct _tuple21*_tmp4AA;return(_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA->f1=
Cyc_current_line,((_tmp4AA->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source),
_tmp4AA)))));};case 1: _LL8B:
# 534
 return Cyc_suck_line(lexbuf);case 2: _LL8C:
# 536
 return 0;default: _LL8D:
# 537
(lexbuf->refill_buff)(lexbuf);
# 538
return Cyc_suck_line_rec(lexbuf,lexstate);}{
# 540
struct Cyc_Lexing_Error_exn_struct _tmp4B0;const char*_tmp4AF;struct Cyc_Lexing_Error_exn_struct*
_tmp4AE;(int)_throw((void*)((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE[0]=((
_tmp4B0.tag=Cyc_Lexing_Error,((_tmp4B0.f1=((_tmp4AF="some action didn't return!",
_tag_dyneither(_tmp4AF,sizeof(char),27))),_tmp4B0)))),_tmp4AE)))));};}
# 542
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}
# 543
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 544
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 545
switch(lexstate){case 0: _LL8F:
# 540 "/home/jgm/cyclone/src/buildlib.cyl"
{struct _dyneither_ptr*_tmp4B1;Cyc_current_source=((_tmp4B1=_cycalloc(sizeof(*
_tmp4B1)),((_tmp4B1[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp4B1))));}
# 541
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
# 542
return Cyc_suck_restofline(lexbuf);default: _LL90:
# 544
(lexbuf->refill_buff)(lexbuf);
# 545
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{
# 547
struct Cyc_Lexing_Error_exn_struct _tmp4B7;const char*_tmp4B6;struct Cyc_Lexing_Error_exn_struct*
_tmp4B5;(int)_throw((void*)((_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5[0]=((
_tmp4B7.tag=Cyc_Lexing_Error,((_tmp4B7.f1=((_tmp4B6="some action didn't return!",
_tag_dyneither(_tmp4B6,sizeof(char),27))),_tmp4B7)))),_tmp4B5)))));};}
# 549
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}
# 550
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 551
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 552
switch(lexstate){case 0: _LL92:
# 547 "/home/jgm/cyclone/src/buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL93:
# 548
(lexbuf->refill_buff)(lexbuf);
# 549
return Cyc_suck_restofline_rec(lexbuf,lexstate);}{
# 551
struct Cyc_Lexing_Error_exn_struct _tmp4BD;const char*_tmp4BC;struct Cyc_Lexing_Error_exn_struct*
_tmp4BB;(int)_throw((void*)((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB[0]=((
_tmp4BD.tag=Cyc_Lexing_Error,((_tmp4BD.f1=((_tmp4BC="some action didn't return!",
_tag_dyneither(_tmp4BC,sizeof(char),27))),_tmp4BD)))),_tmp4BB)))));};}
# 553
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}
# 554
struct _tuple23*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 555
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 556
switch(lexstate){case 0: _LL95:
# 554 "/home/jgm/cyclone/src/buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL96:
# 556
 Cyc_current_headerfile=(struct _dyneither_ptr)
# 557
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
# 558
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));
# 559
Cyc_current_symbols=0;
# 560
Cyc_current_omit_symbols=0;
# 561
Cyc_current_cstubs=0;
# 562
Cyc_current_cycstubs=0;
# 563
Cyc_current_hstubs=0;
# 564
while(Cyc_commands(lexbuf)){;}
# 565
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_hstubs);
# 566
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cstubs);
# 567
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cycstubs);{
# 568
struct _tuple23*_tmp4BE;return(_tmp4BE=_cycalloc(sizeof(*_tmp4BE)),((_tmp4BE->f1=
Cyc_current_headerfile,((_tmp4BE->f2=Cyc_current_symbols,((_tmp4BE->f3=Cyc_current_omit_symbols,((
_tmp4BE->f4=Cyc_current_hstubs,((_tmp4BE->f5=Cyc_current_cstubs,((_tmp4BE->f6=
Cyc_current_cycstubs,_tmp4BE)))))))))))));};case 2: _LL97:
# 576
 return Cyc_spec(lexbuf);case 3: _LL98:
# 578
 return 0;case 4: _LL99:
# 580
{const char*_tmp4C2;void*_tmp4C1[1];struct Cyc_Int_pa_PrintArg_struct _tmp4C0;(
_tmp4C0.tag=1,((_tmp4C0.f1=(unsigned long)((int)
# 582
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4C1[0]=& _tmp4C0,Cyc_fprintf(Cyc_stderr,((
_tmp4C2="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp4C2,sizeof(char),67))),_tag_dyneither(_tmp4C1,sizeof(void*),1)))))));}
# 583
return 0;default: _LL9A:
# 584
(lexbuf->refill_buff)(lexbuf);
# 585
return Cyc_spec_rec(lexbuf,lexstate);}{
# 587
struct Cyc_Lexing_Error_exn_struct _tmp4C8;const char*_tmp4C7;struct Cyc_Lexing_Error_exn_struct*
_tmp4C6;(int)_throw((void*)((_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6[0]=((
_tmp4C8.tag=Cyc_Lexing_Error,((_tmp4C8.f1=((_tmp4C7="some action didn't return!",
_tag_dyneither(_tmp4C7,sizeof(char),27))),_tmp4C8)))),_tmp4C6)))));};}
# 589
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(
lexbuf,13);}
# 590
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 591
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 592
switch(lexstate){case 0: _LL9C:
# 587 "/home/jgm/cyclone/src/buildlib.cyl"
 return 0;case 1: _LL9D:
# 589
 return 0;case 2: _LL9E:
# 591
 Cyc_snarfed_symbols=0;
# 592
while(Cyc_snarfsymbols(lexbuf)){;}
# 593
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
# 594
return 1;case 3: _LL9F:
# 596
 Cyc_snarfed_symbols=0;
# 597
while(Cyc_snarfsymbols(lexbuf)){;}
# 598
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
# 599
return 1;case 4: _LLA0:
# 601
 Cyc_braces_to_match=1;
# 602
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 603
while(Cyc_block(lexbuf)){;}{
# 604
struct _tuple22*_tmp4C9;struct _tuple22*x=(_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((
_tmp4C9->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4C9->f2=(struct
_dyneither_ptr)
# 605
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4C9)))));
# 606
{struct Cyc_List_List*_tmp4CA;Cyc_current_hstubs=((_tmp4CA=_cycalloc(sizeof(*
_tmp4CA)),((_tmp4CA->hd=x,((_tmp4CA->tl=Cyc_current_hstubs,_tmp4CA))))));}
# 607
return 1;};case 5: _LLA1: {
# 609
struct _dyneither_ptr _tmp83=Cyc_Lexing_lexeme(lexbuf);
# 610
{const char*_tmp4CB;_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),(int)Cyc_strlen(((
_tmp4CB="hstub",_tag_dyneither(_tmp4CB,sizeof(char),6)))));}
# 611
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp83,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp83,sizeof(char),1);}{
# 612
struct _dyneither_ptr t=_tmp83;
# 613
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
# 614
struct _dyneither_ptr _tmp85=Cyc_substring((struct _dyneither_ptr)_tmp83,0,(
unsigned long)((t.curr - _tmp83.curr)/ sizeof(char)));
# 615
Cyc_braces_to_match=1;
# 616
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 617
while(Cyc_block(lexbuf)){;}{
# 618
struct _tuple22*_tmp4CC;struct _tuple22*x=(_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((
_tmp4CC->f1=(struct _dyneither_ptr)_tmp85,((_tmp4CC->f2=(struct _dyneither_ptr)
# 619
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4CC)))));
# 620
{struct Cyc_List_List*_tmp4CD;Cyc_current_hstubs=((_tmp4CD=_cycalloc(sizeof(*
_tmp4CD)),((_tmp4CD->hd=x,((_tmp4CD->tl=Cyc_current_hstubs,_tmp4CD))))));}
# 621
return 1;};};};}case 6: _LLA2:
# 623
 Cyc_braces_to_match=1;
# 624
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 625
while(Cyc_block(lexbuf)){;}{
# 626
struct _tuple22*_tmp4CE;struct _tuple22*x=(_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((
_tmp4CE->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4CE->f2=(struct
_dyneither_ptr)
# 627
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4CE)))));
# 628
{struct Cyc_List_List*_tmp4CF;Cyc_current_cstubs=((_tmp4CF=_cycalloc(sizeof(*
_tmp4CF)),((_tmp4CF->hd=x,((_tmp4CF->tl=Cyc_current_cstubs,_tmp4CF))))));}
# 629
return 1;};case 7: _LLA3: {
# 631
struct _dyneither_ptr _tmp8A=Cyc_Lexing_lexeme(lexbuf);
# 632
{const char*_tmp4D0;_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),(int)Cyc_strlen(((
_tmp4D0="cstub",_tag_dyneither(_tmp4D0,sizeof(char),6)))));}
# 633
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp8A,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp8A,sizeof(char),1);}{
# 634
struct _dyneither_ptr t=_tmp8A;
# 635
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
# 636
struct _dyneither_ptr _tmp8C=Cyc_substring((struct _dyneither_ptr)_tmp8A,0,(
unsigned long)((t.curr - _tmp8A.curr)/ sizeof(char)));
# 637
Cyc_braces_to_match=1;
# 638
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 639
while(Cyc_block(lexbuf)){;}{
# 640
struct _tuple22*_tmp4D1;struct _tuple22*x=(_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((
_tmp4D1->f1=(struct _dyneither_ptr)_tmp8C,((_tmp4D1->f2=(struct _dyneither_ptr)
# 641
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4D1)))));
# 642
{struct Cyc_List_List*_tmp4D2;Cyc_current_cstubs=((_tmp4D2=_cycalloc(sizeof(*
_tmp4D2)),((_tmp4D2->hd=x,((_tmp4D2->tl=Cyc_current_cstubs,_tmp4D2))))));}
# 643
return 1;};};};}case 8: _LLA4:
# 645
 Cyc_braces_to_match=1;
# 646
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 647
while(Cyc_block(lexbuf)){;}{
# 648
struct _tuple22*_tmp4D3;struct _tuple22*x=(_tmp4D3=_cycalloc(sizeof(*_tmp4D3)),((
_tmp4D3->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp4D3->f2=(struct
_dyneither_ptr)
# 649
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4D3)))));
# 650
{struct Cyc_List_List*_tmp4D4;Cyc_current_cycstubs=((_tmp4D4=_cycalloc(sizeof(*
_tmp4D4)),((_tmp4D4->hd=x,((_tmp4D4->tl=Cyc_current_cycstubs,_tmp4D4))))));}
# 651
return 1;};case 9: _LLA5: {
# 653
struct _dyneither_ptr _tmp91=Cyc_Lexing_lexeme(lexbuf);
# 654
{const char*_tmp4D5;_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),(int)Cyc_strlen(((
_tmp4D5="cycstub",_tag_dyneither(_tmp4D5,sizeof(char),8)))));}
# 655
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp91,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp91,sizeof(char),1);}{
# 656
struct _dyneither_ptr t=_tmp91;
# 657
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
# 658
struct _dyneither_ptr _tmp93=Cyc_substring((struct _dyneither_ptr)_tmp91,0,(
unsigned long)((t.curr - _tmp91.curr)/ sizeof(char)));
# 659
Cyc_braces_to_match=1;
# 660
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 661
while(Cyc_block(lexbuf)){;}{
# 662
struct _tuple22*_tmp4D6;struct _tuple22*x=(_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((
_tmp4D6->f1=(struct _dyneither_ptr)_tmp93,((_tmp4D6->f2=(struct _dyneither_ptr)
# 663
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp4D6)))));
# 664
{struct Cyc_List_List*_tmp4D7;Cyc_current_cycstubs=((_tmp4D7=_cycalloc(sizeof(*
_tmp4D7)),((_tmp4D7->hd=x,((_tmp4D7->tl=Cyc_current_cycstubs,_tmp4D7))))));}
# 665
return 1;};};};}case 10: _LLA6:
# 667
 Cyc_braces_to_match=1;
# 668
Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
# 669
while(Cyc_block(lexbuf)){;}{
# 670
struct _dyneither_ptr*_tmp4D8;struct _dyneither_ptr*x=(_tmp4D8=_cycalloc(sizeof(*
_tmp4D8)),((_tmp4D8[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp4D8)));
# 671
{struct Cyc_List_List*_tmp4D9;Cyc_current_cpp=((_tmp4D9=_cycalloc(sizeof(*
_tmp4D9)),((_tmp4D9->hd=x,((_tmp4D9->tl=Cyc_current_cpp,_tmp4D9))))));}
# 672
return 1;};case 11: _LLA7:
# 674
 return 1;case 12: _LLA8:
# 676
 return 1;case 13: _LLA9:
# 678
{const char*_tmp4DD;void*_tmp4DC[1];struct Cyc_Int_pa_PrintArg_struct _tmp4DB;(
_tmp4DB.tag=1,((_tmp4DB.f1=(unsigned long)((int)
# 680
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4DC[0]=& _tmp4DB,Cyc_fprintf(Cyc_stderr,((
_tmp4DD="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp4DD,sizeof(char),58))),_tag_dyneither(_tmp4DC,sizeof(void*),1)))))));}
# 681
return 0;default: _LLAA:
# 682
(lexbuf->refill_buff)(lexbuf);
# 683
return Cyc_commands_rec(lexbuf,lexstate);}{
# 685
struct Cyc_Lexing_Error_exn_struct _tmp4E3;const char*_tmp4E2;struct Cyc_Lexing_Error_exn_struct*
_tmp4E1;(int)_throw((void*)((_tmp4E1=_cycalloc(sizeof(*_tmp4E1)),((_tmp4E1[0]=((
_tmp4E3.tag=Cyc_Lexing_Error,((_tmp4E3.f1=((_tmp4E2="some action didn't return!",
_tag_dyneither(_tmp4E2,sizeof(char),27))),_tmp4E3)))),_tmp4E1)))));};}
# 687
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,
14);}
# 688
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 689
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 690
switch(lexstate){case 0: _LLAC:
# 690 "/home/jgm/cyclone/src/buildlib.cyl"
{struct _dyneither_ptr*_tmp4E6;struct Cyc_List_List*_tmp4E5;Cyc_snarfed_symbols=((
_tmp4E5=_cycalloc(sizeof(*_tmp4E5)),((_tmp4E5->hd=((_tmp4E6=_cycalloc(sizeof(*
_tmp4E6)),((_tmp4E6[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp4E6)))),((
_tmp4E5->tl=Cyc_snarfed_symbols,_tmp4E5))))));}
# 691
return 1;case 1: _LLAD:
# 693
 return 1;case 2: _LLAE:
# 695
 return 0;case 3: _LLAF:
# 697
{const char*_tmp4E9;void*_tmp4E8;(_tmp4E8=0,Cyc_fprintf(Cyc_stderr,((_tmp4E9="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp4E9,sizeof(char),44))),_tag_dyneither(_tmp4E8,sizeof(void*),0)));}
# 699
return 0;case 4: _LLB0:
# 701
{const char*_tmp4ED;void*_tmp4EC[1];struct Cyc_Int_pa_PrintArg_struct _tmp4EB;(
_tmp4EB.tag=1,((_tmp4EB.f1=(unsigned long)((int)
# 703
Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp4EC[0]=& _tmp4EB,Cyc_fprintf(Cyc_stderr,((
_tmp4ED="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp4ED,sizeof(char),57))),_tag_dyneither(_tmp4EC,sizeof(void*),1)))))));}
# 704
return 0;default: _LLB1:
# 705
(lexbuf->refill_buff)(lexbuf);
# 706
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}{
# 708
struct Cyc_Lexing_Error_exn_struct _tmp4F3;const char*_tmp4F2;struct Cyc_Lexing_Error_exn_struct*
_tmp4F1;(int)_throw((void*)((_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=((
_tmp4F3.tag=Cyc_Lexing_Error,((_tmp4F3.f1=((_tmp4F2="some action didn't return!",
_tag_dyneither(_tmp4F2,sizeof(char),27))),_tmp4F3)))),_tmp4F1)))));};}
# 710
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(
lexbuf,15);}
# 711
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 712
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 713
switch(lexstate){case 0: _LLB3:
# 714 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp4F6;void*_tmp4F5;(_tmp4F5=0,Cyc_log(((_tmp4F6="Warning: unclosed brace\n",
_tag_dyneither(_tmp4F6,sizeof(char),25))),_tag_dyneither(_tmp4F5,sizeof(void*),0)));}
return 0;case 1: _LLB4:
# 716
 if(Cyc_braces_to_match == 1)return 0;
# 717
-- Cyc_braces_to_match;
# 718
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
# 719
return 1;case 2: _LLB5:
# 721
 ++ Cyc_braces_to_match;
# 722
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
# 723
return 1;case 3: _LLB6:
# 725
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
# 726
while(Cyc_block_string(lexbuf)){;}
# 727
return 1;case 4: _LLB7:
# 729
{const char*_tmp4F7;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((
_tmp4F7="/*",_tag_dyneither(_tmp4F7,sizeof(char),3))));}
# 730
while(Cyc_block_comment(lexbuf)){;}
# 731
return 1;case 5: _LLB8:
# 733
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 734
return 1;case 6: _LLB9:
# 736
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(
lexbuf,0));
# 737
return 1;default: _LLBA:
# 738
(lexbuf->refill_buff)(lexbuf);
# 739
return Cyc_block_rec(lexbuf,lexstate);}{
# 741
struct Cyc_Lexing_Error_exn_struct _tmp4FD;const char*_tmp4FC;struct Cyc_Lexing_Error_exn_struct*
_tmp4FB;(int)_throw((void*)((_tmp4FB=_cycalloc(sizeof(*_tmp4FB)),((_tmp4FB[0]=((
_tmp4FD.tag=Cyc_Lexing_Error,((_tmp4FD.f1=((_tmp4FC="some action didn't return!",
_tag_dyneither(_tmp4FC,sizeof(char),27))),_tmp4FD)))),_tmp4FB)))));};}
# 743
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
# 744
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 745
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 746
switch(lexstate){case 0: _LLBC:
# 741 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_log(((_tmp500="Warning: unclosed string\n",
_tag_dyneither(_tmp500,sizeof(char),26))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}
return 0;case 1: _LLBD:
# 743
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;
case 2: _LLBE:
# 745
{const char*_tmp503;void*_tmp502;(_tmp502=0,Cyc_log(((_tmp503="Warning: unclosed string\n",
_tag_dyneither(_tmp503,sizeof(char),26))),_tag_dyneither(_tmp502,sizeof(void*),0)));}
# 746
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 747
return 1;case 3: _LLBF:
# 749
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 750
return 1;case 4: _LLC0:
# 752
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 753
return 1;case 5: _LLC1:
# 755
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 756
return 1;case 6: _LLC2:
# 758
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 759
return 1;case 7: _LLC3:
# 761
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 762
return 1;case 8: _LLC4:
# 764
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 765
return 1;default: _LLC5:
# 766
(lexbuf->refill_buff)(lexbuf);
# 767
return Cyc_block_string_rec(lexbuf,lexstate);}{
# 769
struct Cyc_Lexing_Error_exn_struct _tmp509;const char*_tmp508;struct Cyc_Lexing_Error_exn_struct*
_tmp507;(int)_throw((void*)((_tmp507=_cycalloc(sizeof(*_tmp507)),((_tmp507[0]=((
_tmp509.tag=Cyc_Lexing_Error,((_tmp509.f1=((_tmp508="some action didn't return!",
_tag_dyneither(_tmp508,sizeof(char),27))),_tmp509)))),_tmp507)))));};}
# 771
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}
# 772
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
# 773
lexstate=Cyc_lex_engine(lexstate,lexbuf);
# 774
switch(lexstate){case 0: _LLC7:
# 769 "/home/jgm/cyclone/src/buildlib.cyl"
{const char*_tmp50C;void*_tmp50B;(_tmp50B=0,Cyc_log(((_tmp50C="Warning: unclosed comment\n",
_tag_dyneither(_tmp50C,sizeof(char),27))),_tag_dyneither(_tmp50B,sizeof(void*),0)));}
return 0;case 1: _LLC8:
# 771
{const char*_tmp50D;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((
_tmp50D="*/",_tag_dyneither(_tmp50D,sizeof(char),3))));}return 0;case 2: _LLC9:
# 773
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
# 774
return 1;default: _LLCA:
# 775
(lexbuf->refill_buff)(lexbuf);
# 776
return Cyc_block_comment_rec(lexbuf,lexstate);}{
# 778
struct Cyc_Lexing_Error_exn_struct _tmp513;const char*_tmp512;struct Cyc_Lexing_Error_exn_struct*
_tmp511;(int)_throw((void*)((_tmp511=_cycalloc(sizeof(*_tmp511)),((_tmp511[0]=((
_tmp513.tag=Cyc_Lexing_Error,((_tmp513.f1=((_tmp512="some action didn't return!",
_tag_dyneither(_tmp512,sizeof(char),27))),_tmp513)))),_tmp511)))));};}
# 780
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}
# 783 "/home/jgm/cyclone/src/buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 786
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple24{struct
Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 787
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
# 788
void*_tmpBB=((struct Cyc_Absyn_Exp*)_check_null(e))->r;struct _tuple0*_tmpBD;
struct _tuple0 _tmpBE;struct _dyneither_ptr*_tmpBF;struct Cyc_List_List*_tmpC1;
struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC6;
struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;
struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpD0;
struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;
struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;
struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDE;
struct Cyc_List_List*_tmpDF;void*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_MallocInfo
_tmpE4;int _tmpE5;struct Cyc_Absyn_Exp*_tmpE6;void**_tmpE7;struct Cyc_Absyn_Exp*
_tmpE8;struct Cyc_Absyn_Exp*_tmpEA;void*_tmpEC;void*_tmpEE;struct Cyc_Absyn_Exp*
_tmpF0;struct _dyneither_ptr*_tmpF1;struct Cyc_Absyn_Exp*_tmpF3;struct
_dyneither_ptr*_tmpF4;void*_tmpF6;void*_tmpF7;struct Cyc_List_List*_tmpFA;_LLCD: {
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpBC->tag != 1)goto _LLCF;else{_tmpBD=_tmpBC->f1;_tmpBE=*_tmpBD;_tmpBF=
_tmpBE.f2;}}_LLCE:
# 790
 Cyc_add_target(_tmpBF);
# 791
return;_LLCF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC0=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpC0->tag != 2)goto _LLD1;else{_tmpC1=_tmpC0->f2;}}_LLD0:
# 793
 for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
# 794
Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpC1->hd),dep);}
# 796
return;_LLD1: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC2=(struct
Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC2->tag != 22)goto _LLD3;
else{_tmpC3=_tmpC2->f1;_tmpC4=_tmpC2->f2;}}_LLD2:
# 798
 _tmpC6=_tmpC3;_tmpC7=_tmpC4;goto _LLD4;_LLD3: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*
_tmpC5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC5->tag != 8)
goto _LLD5;else{_tmpC6=_tmpC5->f1;_tmpC7=_tmpC5->f2;}}_LLD4:
# 800
 _tmpC9=_tmpC6;_tmpCA=_tmpC7;goto _LLD6;_LLD5: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*
_tmpC8=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpC8->tag != 
3)goto _LLD7;else{_tmpC9=_tmpC8->f1;_tmpCA=_tmpC8->f3;}}_LLD6:
# 802
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC9,dep);
# 803
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCA,dep);
# 804
return;_LLD7: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpCB->tag != 19)goto _LLD9;else{_tmpCC=_tmpCB->f1;}}_LLD8:
# 806
 _tmpCE=_tmpCC;goto _LLDA;_LLD9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*
_tmpCD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpCD->tag
!= 17)goto _LLDB;else{_tmpCE=_tmpCD->f1;}}_LLDA:
# 808
 _tmpD0=_tmpCE;goto _LLDC;_LLDB: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*
_tmpCF=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpCF->tag != 
14)goto _LLDD;else{_tmpD0=_tmpCF->f1;}}_LLDC:
# 810
 _tmpD2=_tmpD0;goto _LLDE;_LLDD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*
_tmpD1=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpD1->tag
!= 4)goto _LLDF;else{_tmpD2=_tmpD1->f1;}}_LLDE:
# 812
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD2,dep);
# 813
return;_LLDF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD3=(
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpD3->tag != 5)
goto _LLE1;else{_tmpD4=_tmpD3->f1;_tmpD5=_tmpD3->f2;_tmpD6=_tmpD3->f3;}}_LLE0:
# 815
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD4,dep);
# 816
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD5,dep);
# 817
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD6,dep);
# 818
return;_LLE1: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpD7->tag != 6)goto _LLE3;else{_tmpD8=_tmpD7->f1;_tmpD9=_tmpD7->f2;}}
_LLE2:
# 819
 _tmpDB=_tmpD8;_tmpDC=_tmpD9;goto _LLE4;_LLE3: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*
_tmpDA=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpDA->tag != 7)
goto _LLE5;else{_tmpDB=_tmpDA->f1;_tmpDC=_tmpDA->f2;}}_LLE4:
# 821
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDB,dep);
# 822
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDC,dep);
# 823
return;_LLE5: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDD=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpDD->tag != 9)goto _LLE7;else{_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;}}
_LLE6:
# 825
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDE,dep);
# 826
for(0;_tmpDF != 0;_tmpDF=_tmpDF->tl){
# 827
Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpDF->hd),dep);}
# 829
return;_LLE7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpE0->tag != 13)goto _LLE9;else{_tmpE1=(void*)_tmpE0->f1;_tmpE2=_tmpE0->f2;}}
_LLE8:
# 831
 Cyc_scan_type(_tmpE1,dep);
# 832
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE2,dep);
# 833
return;_LLE9: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpE3->tag != 32)goto _LLEB;else{_tmpE4=_tmpE3->f1;_tmpE5=_tmpE4.is_calloc;
_tmpE6=_tmpE4.rgn;_tmpE7=_tmpE4.elt_type;_tmpE8=_tmpE4.num_elts;}}_LLEA:
# 835
 if(_tmpE6 != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpE6),
dep);
# 836
if(_tmpE7 != 0)Cyc_scan_type(*_tmpE7,dep);
# 837
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE8,dep);
# 838
return;_LLEB: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE9=(struct
Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpE9->tag != 36)goto _LLED;
else{_tmpEA=_tmpE9->f1;}}_LLEC:
# 840
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpEA,dep);return;_LLED: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*
_tmpEB=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpEB->tag != 
37)goto _LLEF;else{_tmpEC=(void*)_tmpEB->f1;}}_LLEE:
# 841
 _tmpEE=_tmpEC;goto _LLF0;_LLEF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*
_tmpED=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpED->tag
!= 16)goto _LLF1;else{_tmpEE=(void*)_tmpED->f1;}}_LLF0:
# 843
 Cyc_scan_type(_tmpEE,dep);
# 844
return;_LLF1: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEF=(struct
Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpEF->tag != 20)goto
_LLF3;else{_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;}}_LLF2:
# 846
 _tmpF3=_tmpF0;_tmpF4=_tmpF1;goto _LLF4;_LLF3: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*
_tmpF2=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF2->tag
!= 21)goto _LLF5;else{_tmpF3=_tmpF2->f1;_tmpF4=_tmpF2->f2;}}_LLF4:
# 848
 Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpF3,dep);
# 849
Cyc_add_target(_tmpF4);
# 850
return;_LLF5: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpF5=(struct
Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF5->tag != 18)goto _LLF7;
else{_tmpF6=(void*)_tmpF5->f1;_tmpF7=(void*)_tmpF5->f2;}}_LLF6:
# 852
 Cyc_scan_type(_tmpF6,dep);
# 853
{void*_tmp10B=_tmpF7;struct _dyneither_ptr*_tmp10D;_LL11C: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*
_tmp10C=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp10B;if(
_tmp10C->tag != 0)goto _LL11E;else{_tmp10D=_tmp10C->f1;}}_LL11D:
# 854
 Cyc_add_target(_tmp10D);goto _LL11B;_LL11E: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*
_tmp10E=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp10B;if(
_tmp10E->tag != 1)goto _LL11B;}_LL11F:
# 855
 goto _LL11B;_LL11B:;}
# 857
return;_LLF7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpF8->tag != 0)goto _LLF9;}_LLF8:
# 859
 return;_LLF9: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF9=(
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpF9->tag != 34)
goto _LLFB;else{_tmpFA=_tmpF9->f2;}}_LLFA:
# 861
 for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
# 862
struct _tuple24 _tmp110;struct Cyc_Absyn_Exp*_tmp111;struct _tuple24*_tmp10F=(
struct _tuple24*)_tmpFA->hd;_tmp110=*_tmp10F;_tmp111=_tmp110.f2;
# 863
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp111,dep);}
# 865
return;_LLFB: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpFB->tag != 38)goto _LLFD;}_LLFC:
# 866
 return;_LLFD: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmpFC=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)
_tmpBB;if(_tmpFC->tag != 33)goto _LLFF;}_LLFE:
# 868
{const char*_tmp516;void*_tmp515;(_tmp515=0,Cyc_fprintf(Cyc_stderr,((_tmp516="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp516,sizeof(char),26))),_tag_dyneither(_tmp515,sizeof(void*),0)));}
# 869
exit(1);return;_LLFF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpFD=(
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFD->tag != 35)goto
_LL101;}_LL100:
# 871
{const char*_tmp519;void*_tmp518;(_tmp518=0,Cyc_fprintf(Cyc_stderr,((_tmp519="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp519,sizeof(char),26))),_tag_dyneither(_tmp518,sizeof(void*),0)));}
# 872
exit(1);return;_LL101: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmpFE=(
struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFE->tag != 10)goto
_LL103;}_LL102:
# 874
{const char*_tmp51C;void*_tmp51B;(_tmp51B=0,Cyc_fprintf(Cyc_stderr,((_tmp51C="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp51C,sizeof(char),27))),_tag_dyneither(_tmp51B,sizeof(void*),0)));}
# 875
exit(1);return;_LL103: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*
_tmpFF=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmpFF->tag
!= 11)goto _LL105;}_LL104:
# 877
{const char*_tmp51F;void*_tmp51E;(_tmp51E=0,Cyc_fprintf(Cyc_stderr,((_tmp51F="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp51F,sizeof(char),35))),_tag_dyneither(_tmp51E,sizeof(void*),0)));}
# 878
exit(1);return;_LL105: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*
_tmp100=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp100->tag
!= 12)goto _LL107;}_LL106:
# 880
{const char*_tmp522;void*_tmp521;(_tmp521=0,Cyc_fprintf(Cyc_stderr,((_tmp522="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp522,sizeof(char),33))),_tag_dyneither(_tmp521,sizeof(void*),0)));}
# 881
exit(1);return;_LL107: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp101=(
struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp101->tag != 15)goto
_LL109;}_LL108:
# 883
{const char*_tmp525;void*_tmp524;(_tmp524=0,Cyc_fprintf(Cyc_stderr,((_tmp525="Error: unexpected New_e\n",
_tag_dyneither(_tmp525,sizeof(char),25))),_tag_dyneither(_tmp524,sizeof(void*),0)));}
# 884
exit(1);return;_LL109: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp102=(
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp102->tag != 23)goto
_LL10B;}_LL10A:
# 886
{const char*_tmp528;void*_tmp527;(_tmp527=0,Cyc_fprintf(Cyc_stderr,((_tmp528="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp528,sizeof(char),27))),_tag_dyneither(_tmp527,sizeof(void*),0)));}
# 887
exit(1);return;_LL10B: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*
_tmp103=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp103->tag
!= 24)goto _LL10D;}_LL10C:
# 889
{const char*_tmp52B;void*_tmp52A;(_tmp52A=0,Cyc_fprintf(Cyc_stderr,((_tmp52B="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp52B,sizeof(char),33))),_tag_dyneither(_tmp52A,sizeof(void*),0)));}
# 890
exit(1);return;_LL10D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp104=(
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp104->tag != 25)goto
_LL10F;}_LL10E:
# 892
{const char*_tmp52E;void*_tmp52D;(_tmp52D=0,Cyc_fprintf(Cyc_stderr,((_tmp52E="Error: unexpected Array_e\n",
_tag_dyneither(_tmp52E,sizeof(char),27))),_tag_dyneither(_tmp52D,sizeof(void*),0)));}
# 893
exit(1);return;_LL10F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*
_tmp105=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp105->tag
!= 26)goto _LL111;}_LL110:
# 895
{const char*_tmp531;void*_tmp530;(_tmp530=0,Cyc_fprintf(Cyc_stderr,((_tmp531="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp531,sizeof(char),35))),_tag_dyneither(_tmp530,sizeof(void*),0)));}
# 896
exit(1);return;_LL111: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*
_tmp106=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp106->tag
!= 27)goto _LL113;}_LL112:
# 898
{const char*_tmp534;void*_tmp533;(_tmp533=0,Cyc_fprintf(Cyc_stderr,((_tmp534="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp534,sizeof(char),31))),_tag_dyneither(_tmp533,sizeof(void*),0)));}
# 899
exit(1);return;_LL113: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*
_tmp107=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp107->tag
!= 28)goto _LL115;}_LL114:
# 901
{const char*_tmp537;void*_tmp536;(_tmp536=0,Cyc_fprintf(Cyc_stderr,((_tmp537="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp537,sizeof(char),32))),_tag_dyneither(_tmp536,sizeof(void*),0)));}
# 902
exit(1);return;_LL115: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp108=(
struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp108->tag != 29)
goto _LL117;}_LL116:
# 904
{const char*_tmp53A;void*_tmp539;(_tmp539=0,Cyc_fprintf(Cyc_stderr,((_tmp53A="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp53A,sizeof(char),30))),_tag_dyneither(_tmp539,sizeof(void*),0)));}
# 905
exit(1);return;_LL117: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp109=(
struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp109->tag != 30)goto
_LL119;}_LL118:
# 907
{const char*_tmp53D;void*_tmp53C;(_tmp53C=0,Cyc_fprintf(Cyc_stderr,((_tmp53D="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp53D,sizeof(char),26))),_tag_dyneither(_tmp53C,sizeof(void*),0)));}
# 908
exit(1);return;_LL119: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp10A=(
struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBB;if(_tmp10A->tag != 31)
goto _LLCC;}_LL11A:
# 910
{const char*_tmp540;void*_tmp53F;(_tmp53F=0,Cyc_fprintf(Cyc_stderr,((_tmp540="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp540,sizeof(char),30))),_tag_dyneither(_tmp53F,sizeof(void*),0)));}
# 911
exit(1);return;_LLCC:;}
# 915
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
# 916
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
# 917
return;}
# 920
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
# 921
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
# 922
void*_tmp130=t;struct Cyc_Absyn_PtrInfo _tmp135;struct Cyc_Absyn_ArrayInfo _tmp137;
void*_tmp138;struct Cyc_Absyn_Exp*_tmp139;union Cyc_Absyn_Constraint*_tmp13A;
struct Cyc_Absyn_FnInfo _tmp13C;struct Cyc_List_List*_tmp13E;struct Cyc_Absyn_AggrInfo
_tmp141;union Cyc_Absyn_AggrInfoU _tmp142;struct _tuple0*_tmp144;struct _tuple0
_tmp145;struct _dyneither_ptr*_tmp146;struct _tuple0*_tmp148;struct _tuple0 _tmp149;
struct _dyneither_ptr*_tmp14A;struct Cyc_Absyn_TypeDecl*_tmp14C;struct Cyc_Absyn_TypeDecl
_tmp14D;void*_tmp14E;struct Cyc_Absyn_Aggrdecl*_tmp150;struct Cyc_Absyn_TypeDecl*
_tmp152;struct Cyc_Absyn_TypeDecl _tmp153;void*_tmp154;struct Cyc_Absyn_Enumdecl*
_tmp156;struct Cyc_Absyn_TypeDecl*_tmp158;struct Cyc_Absyn_TypeDecl _tmp159;void*
_tmp15A;struct Cyc_Absyn_Datatypedecl*_tmp15C;_LL121: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*
_tmp131=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp130;if(_tmp131->tag != 0)
goto _LL123;}_LL122:
# 923
 goto _LL124;_LL123: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp132=(struct
Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp130;if(_tmp132->tag != 6)goto _LL125;}
_LL124:
# 924
 goto _LL126;_LL125: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp133=(struct
Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp130;if(_tmp133->tag != 7)goto _LL127;}
_LL126:
# 926
 return;_LL127: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp134=(struct
Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130;if(_tmp134->tag != 5)goto _LL129;
else{_tmp135=_tmp134->f1;}}_LL128:
# 929
 Cyc_scan_type(_tmp135.elt_typ,dep);
# 930
return;_LL129: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp136=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)
_tmp130;if(_tmp136->tag != 8)goto _LL12B;else{_tmp137=_tmp136->f1;_tmp138=_tmp137.elt_type;
_tmp139=_tmp137.num_elts;_tmp13A=_tmp137.zero_term;}}_LL12A:
# 932
 Cyc_scan_type(_tmp138,dep);
# 933
Cyc_scan_exp_opt(_tmp139,dep);
# 934
return;_LL12B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)
_tmp130;if(_tmp13B->tag != 9)goto _LL12D;else{_tmp13C=_tmp13B->f1;}}_LL12C:
# 936
 Cyc_scan_type(_tmp13C.ret_typ,dep);
# 937
{struct Cyc_List_List*_tmp16C=_tmp13C.args;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
# 938
struct _tuple8 _tmp16E;void*_tmp16F;struct _tuple8*_tmp16D=(struct _tuple8*)_tmp16C->hd;
_tmp16E=*_tmp16D;_tmp16F=_tmp16E.f3;
# 939
Cyc_scan_type(_tmp16F,dep);}}
# 941
if(_tmp13C.cyc_varargs != 0)
# 942
Cyc_scan_type((_tmp13C.cyc_varargs)->type,dep);
# 943
return;_LL12D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp13D=(struct
Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp130;if(_tmp13D->tag != 12)goto
_LL12F;else{_tmp13E=_tmp13D->f2;}}_LL12E:
# 945
 for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){
# 946
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp13E->hd)->type,dep);
# 947
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp13E->hd)->width,dep);}
# 949
return;_LL12F: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp13F=(struct
Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp130;if(_tmp13F->tag != 14)goto
_LL131;}_LL130:
# 951
 return;_LL131: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)
_tmp130;if(_tmp140->tag != 11)goto _LL133;else{_tmp141=_tmp140->f1;_tmp142=_tmp141.aggr_info;}}
_LL132: {
# 953
struct _tuple0*_tmp171;struct _tuple0 _tmp172;struct _dyneither_ptr*_tmp173;struct
_tuple10 _tmp170=Cyc_Absyn_aggr_kinded_name(_tmp142);_tmp171=_tmp170.f2;_tmp172=*
_tmp171;_tmp173=_tmp172.f2;
# 954
_tmp146=_tmp173;goto _LL134;}_LL133: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*
_tmp143=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp130;if(_tmp143->tag != 
13)goto _LL135;else{_tmp144=_tmp143->f1;_tmp145=*_tmp144;_tmp146=_tmp145.f2;}}
_LL134:
# 956
 _tmp14A=_tmp146;goto _LL136;_LL135: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*
_tmp147=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp130;if(_tmp147->tag
!= 17)goto _LL137;else{_tmp148=_tmp147->f1;_tmp149=*_tmp148;_tmp14A=_tmp149.f2;}}
_LL136:
# 958
 Cyc_add_target(_tmp14A);
# 959
return;_LL137: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp14B=(struct
Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp130;if(_tmp14B->tag != 26)goto
_LL139;else{_tmp14C=_tmp14B->f1;_tmp14D=*_tmp14C;_tmp14E=_tmp14D.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*
_tmp14F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp14E;if(_tmp14F->tag
!= 0)goto _LL139;else{_tmp150=_tmp14F->f1;}};}}_LL138:
# 962
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp543;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*
_tmp542;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp542=_cycalloc(sizeof(*
_tmp542)),((_tmp542[0]=((_tmp543.tag=6,((_tmp543.f1=_tmp150,_tmp543)))),_tmp542)))),
0),dep);}{
# 963
struct _tuple0 _tmp177;struct _dyneither_ptr*_tmp178;struct _tuple0*_tmp176=_tmp150->name;
_tmp177=*_tmp176;_tmp178=_tmp177.f2;
# 964
Cyc_add_target(_tmp178);
# 965
return;};_LL139: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp151=(
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp130;if(_tmp151->tag != 26)
goto _LL13B;else{_tmp152=_tmp151->f1;_tmp153=*_tmp152;_tmp154=_tmp153.r;{struct
Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp155=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)
_tmp154;if(_tmp155->tag != 1)goto _LL13B;else{_tmp156=_tmp155->f1;}};}}_LL13A:
# 968
{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp546;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*
_tmp545;Cyc_scan_decl(Cyc_Absyn_new_decl((void*)((_tmp545=_cycalloc(sizeof(*
_tmp545)),((_tmp545[0]=((_tmp546.tag=8,((_tmp546.f1=_tmp156,_tmp546)))),_tmp545)))),
0),dep);}{
# 969
struct _tuple0 _tmp17C;struct _dyneither_ptr*_tmp17D;struct _tuple0*_tmp17B=_tmp156->name;
_tmp17C=*_tmp17B;_tmp17D=_tmp17C.f2;
# 970
Cyc_add_target(_tmp17D);
# 971
return;};_LL13B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp157=(
struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp130;if(_tmp157->tag != 26)
goto _LL13D;else{_tmp158=_tmp157->f1;_tmp159=*_tmp158;_tmp15A=_tmp159.r;{struct
Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp15B=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)
_tmp15A;if(_tmp15B->tag != 2)goto _LL13D;else{_tmp15C=_tmp15B->f1;}};}}_LL13C:
# 974
{const char*_tmp549;void*_tmp548;(_tmp548=0,Cyc_fprintf(Cyc_stderr,((_tmp549="Error: unexpected Datatype declaration\n",
_tag_dyneither(_tmp549,sizeof(char),40))),_tag_dyneither(_tmp548,sizeof(void*),0)));}
# 975
exit(1);return;_LL13D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15D=(struct
Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp130;if(_tmp15D->tag != 1)goto _LL13F;}_LL13E:
# 977
{const char*_tmp54C;void*_tmp54B;(_tmp54B=0,Cyc_fprintf(Cyc_stderr,((_tmp54C="Error: unexpected Evar\n",
_tag_dyneither(_tmp54C,sizeof(char),24))),_tag_dyneither(_tmp54B,sizeof(void*),0)));}
# 978
exit(1);return;_LL13F: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp15E=(
struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp130;if(_tmp15E->tag != 2)goto
_LL141;}_LL140:
# 980
{const char*_tmp54F;void*_tmp54E;(_tmp54E=0,Cyc_fprintf(Cyc_stderr,((_tmp54F="Error: unexpected VarType\n",
_tag_dyneither(_tmp54F,sizeof(char),27))),_tag_dyneither(_tmp54E,sizeof(void*),0)));}
# 981
exit(1);return;_LL141: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp15F=(
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp130;if(_tmp15F->tag != 3)goto
_LL143;}_LL142:
# 983
{const char*_tmp552;void*_tmp551;(_tmp551=0,Cyc_fprintf(Cyc_stderr,((_tmp552="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp552,sizeof(char),32))),_tag_dyneither(_tmp551,sizeof(void*),0)));}
# 984
exit(1);return;_LL143: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*
_tmp160=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp130;if(_tmp160->tag
!= 4)goto _LL145;}_LL144:
# 986
{const char*_tmp555;void*_tmp554;(_tmp554=0,Cyc_fprintf(Cyc_stderr,((_tmp555="Error: unexpected DatatypeFieldType\n",
_tag_dyneither(_tmp555,sizeof(char),37))),_tag_dyneither(_tmp554,sizeof(void*),0)));}
# 987
exit(1);return;_LL145: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp161=(
struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp130;if(_tmp161->tag != 10)goto
_LL147;}_LL146:
# 989
{const char*_tmp558;void*_tmp557;(_tmp557=0,Cyc_fprintf(Cyc_stderr,((_tmp558="Error: unexpected TupleType\n",
_tag_dyneither(_tmp558,sizeof(char),29))),_tag_dyneither(_tmp557,sizeof(void*),0)));}
# 990
exit(1);return;_LL147: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp162=(
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp130;if(_tmp162->tag != 15)
goto _LL149;}_LL148:
# 992
{const char*_tmp55B;void*_tmp55A;(_tmp55A=0,Cyc_fprintf(Cyc_stderr,((_tmp55B="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp55B,sizeof(char),33))),_tag_dyneither(_tmp55A,sizeof(void*),0)));}
# 993
exit(1);return;_LL149: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp163=(
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp130;if(_tmp163->tag != 16)goto
_LL14B;}_LL14A:
# 995
{const char*_tmp55E;void*_tmp55D;(_tmp55D=0,Cyc_fprintf(Cyc_stderr,((_tmp55E="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp55E,sizeof(char),30))),_tag_dyneither(_tmp55D,sizeof(void*),0)));}
# 996
exit(1);return;_LL14B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp164=(
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp130;if(_tmp164->tag != 20)goto
_LL14D;}_LL14C:
# 998
{const char*_tmp561;void*_tmp560;(_tmp560=0,Cyc_fprintf(Cyc_stderr,((_tmp561="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp561,sizeof(char),27))),_tag_dyneither(_tmp560,sizeof(void*),0)));}
# 999
exit(1);return;_LL14D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp165=(
struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp130;if(_tmp165->tag != 21)goto
_LL14F;}_LL14E:
# 1001
{const char*_tmp564;void*_tmp563;(_tmp563=0,Cyc_fprintf(Cyc_stderr,((_tmp564="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp564,sizeof(char),29))),_tag_dyneither(_tmp563,sizeof(void*),0)));}
# 1002
exit(1);return;_LL14F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp166=(
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp130;if(_tmp166->tag != 22)goto
_LL151;}_LL150:
# 1004
{const char*_tmp567;void*_tmp566;(_tmp566=0,Cyc_fprintf(Cyc_stderr,((_tmp567="Error: unexpected RefCntRgn\n",
_tag_dyneither(_tmp567,sizeof(char),29))),_tag_dyneither(_tmp566,sizeof(void*),0)));}
# 1005
exit(1);return;_LL151: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp167=(
struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp130;if(_tmp167->tag != 23)goto
_LL153;}_LL152:
# 1007
{const char*_tmp56A;void*_tmp569;(_tmp569=0,Cyc_fprintf(Cyc_stderr,((_tmp56A="Error: unexpected AccessEff\n",
_tag_dyneither(_tmp56A,sizeof(char),29))),_tag_dyneither(_tmp569,sizeof(void*),0)));}
# 1008
exit(1);return;_LL153: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp168=(
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp130;if(_tmp168->tag != 24)goto
_LL155;}_LL154:
# 1010
{const char*_tmp56D;void*_tmp56C;(_tmp56C=0,Cyc_fprintf(Cyc_stderr,((_tmp56D="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp56D,sizeof(char),27))),_tag_dyneither(_tmp56C,sizeof(void*),0)));}
# 1011
exit(1);return;_LL155: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp169=(
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp130;if(_tmp169->tag != 25)goto
_LL157;}_LL156:
# 1013
{const char*_tmp570;void*_tmp56F;(_tmp56F=0,Cyc_fprintf(Cyc_stderr,((_tmp570="Error: unexpected RgnsEff\n",
_tag_dyneither(_tmp570,sizeof(char),27))),_tag_dyneither(_tmp56F,sizeof(void*),0)));}
# 1014
exit(1);return;_LL157: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp16A=(
struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp130;if(_tmp16A->tag != 19)goto
_LL159;}_LL158:
# 1016
{const char*_tmp573;void*_tmp572;(_tmp572=0,Cyc_fprintf(Cyc_stderr,((_tmp573="Error: unexpected tag_t\n",
_tag_dyneither(_tmp573,sizeof(char),25))),_tag_dyneither(_tmp572,sizeof(void*),0)));}
# 1017
exit(1);return;_LL159: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp16B=(
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp130;if(_tmp16B->tag != 18)goto
_LL120;}_LL15A:
# 1019
{const char*_tmp576;void*_tmp575;(_tmp575=0,Cyc_fprintf(Cyc_stderr,((_tmp576="Error: unexpected valueof_t\n",
_tag_dyneither(_tmp576,sizeof(char),29))),_tag_dyneither(_tmp575,sizeof(void*),0)));}
# 1020
exit(1);return;_LL120:;}
# 1024
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
# 1025
struct Cyc_Set_Set**_tmp19E=Cyc_current_targets;
# 1026
struct _dyneither_ptr*_tmp19F=Cyc_current_source;
# 1027
{struct Cyc_Set_Set**_tmp577;Cyc_current_targets=((_tmp577=_cycalloc(sizeof(*
_tmp577)),((_tmp577[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp577))));}
# 1028
{void*_tmp1A1=d->r;struct Cyc_Absyn_Vardecl*_tmp1A3;struct Cyc_Absyn_Fndecl*
_tmp1A5;struct Cyc_Absyn_Aggrdecl*_tmp1A7;struct Cyc_Absyn_Enumdecl*_tmp1A9;struct
Cyc_Absyn_Typedefdecl*_tmp1AB;_LL15C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp1A2=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1A2->tag != 
0)goto _LL15E;else{_tmp1A3=_tmp1A2->f1;}}_LL15D: {
# 1030
struct _tuple0 _tmp1B8;struct _dyneither_ptr*_tmp1B9;struct _tuple0*_tmp1B7=_tmp1A3->name;
_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.f2;
# 1031
Cyc_current_source=(struct _dyneither_ptr*)_tmp1B9;
# 1032
Cyc_scan_type(_tmp1A3->type,dep);
# 1033
Cyc_scan_exp_opt(_tmp1A3->initializer,dep);
# 1034
goto _LL15B;}_LL15E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp1A4=(struct
Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1A4->tag != 1)goto _LL160;
else{_tmp1A5=_tmp1A4->f1;}}_LL15F: {
# 1036
struct _tuple0 _tmp1BB;struct _dyneither_ptr*_tmp1BC;struct _tuple0*_tmp1BA=_tmp1A5->name;
_tmp1BB=*_tmp1BA;_tmp1BC=_tmp1BB.f2;
# 1037
Cyc_current_source=(struct _dyneither_ptr*)_tmp1BC;
# 1038
Cyc_scan_type(_tmp1A5->ret_type,dep);
# 1039
{struct Cyc_List_List*_tmp1BD=_tmp1A5->args;for(0;_tmp1BD != 0;_tmp1BD=_tmp1BD->tl){
# 1040
struct _tuple8 _tmp1BF;void*_tmp1C0;struct _tuple8*_tmp1BE=(struct _tuple8*)_tmp1BD->hd;
_tmp1BF=*_tmp1BE;_tmp1C0=_tmp1BF.f3;
# 1041
Cyc_scan_type(_tmp1C0,dep);}}
# 1043
if(_tmp1A5->cyc_varargs != 0)
# 1044
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp1A5->cyc_varargs))->type,
dep);
# 1045
if(_tmp1A5->is_inline){
# 1046
const char*_tmp57A;void*_tmp579;(_tmp579=0,Cyc_fprintf(Cyc_stderr,((_tmp57A="Warning: ignoring inline function\n",
_tag_dyneither(_tmp57A,sizeof(char),35))),_tag_dyneither(_tmp579,sizeof(void*),0)));}
# 1048
goto _LL15B;}_LL160: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1A6=(
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1A6->tag != 6)goto
_LL162;else{_tmp1A7=_tmp1A6->f1;}}_LL161: {
# 1050
struct _tuple0 _tmp1C4;struct _dyneither_ptr*_tmp1C5;struct _tuple0*_tmp1C3=_tmp1A7->name;
_tmp1C4=*_tmp1C3;_tmp1C5=_tmp1C4.f2;
# 1051
Cyc_current_source=(struct _dyneither_ptr*)_tmp1C5;
# 1052
if((unsigned int)_tmp1A7->impl){
# 1053
{struct Cyc_List_List*_tmp1C6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp1A7->impl))->fields;for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
# 1054
struct Cyc_Absyn_Aggrfield*_tmp1C7=(struct Cyc_Absyn_Aggrfield*)_tmp1C6->hd;
# 1055
Cyc_scan_type(_tmp1C7->type,dep);
# 1056
Cyc_scan_exp_opt(_tmp1C7->width,dep);}}{
# 1060
struct Cyc_List_List*_tmp1C8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A7->impl))->fields;
for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){;}};}
# 1064
goto _LL15B;}_LL162: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1A8=(
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1A8->tag != 8)goto
_LL164;else{_tmp1A9=_tmp1A8->f1;}}_LL163: {
# 1066
struct _tuple0 _tmp1CA;struct _dyneither_ptr*_tmp1CB;struct _tuple0*_tmp1C9=_tmp1A9->name;
_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.f2;
# 1067
Cyc_current_source=(struct _dyneither_ptr*)_tmp1CB;
# 1068
if((unsigned int)_tmp1A9->fields){
# 1069
{struct Cyc_List_List*_tmp1CC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A9->fields))->v;for(0;_tmp1CC != 0;_tmp1CC=_tmp1CC->tl){
# 1070
struct Cyc_Absyn_Enumfield*_tmp1CD=(struct Cyc_Absyn_Enumfield*)_tmp1CC->hd;
# 1071
Cyc_scan_exp_opt(_tmp1CD->tag,dep);}}{
# 1075
struct Cyc_List_List*_tmp1CE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1A9->fields))->v;for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){;}};}
# 1079
goto _LL15B;}_LL164: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp1AA=(
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1AA->tag != 9)
goto _LL166;else{_tmp1AB=_tmp1AA->f1;}}_LL165: {
# 1081
struct _tuple0 _tmp1D0;struct _dyneither_ptr*_tmp1D1;struct _tuple0*_tmp1CF=_tmp1AB->name;
_tmp1D0=*_tmp1CF;_tmp1D1=_tmp1D0.f2;
# 1082
Cyc_current_source=(struct _dyneither_ptr*)_tmp1D1;
# 1083
if((unsigned int)_tmp1AB->defn)
# 1084
Cyc_scan_type((void*)_check_null(_tmp1AB->defn),dep);
# 1085
goto _LL15B;}_LL166: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp1AC=(
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1AC->tag != 4)goto
_LL168;}_LL167:
# 1087
{const char*_tmp57D;void*_tmp57C;(_tmp57C=0,Cyc_fprintf(Cyc_stderr,((_tmp57D="Error: unexpected region declaration",
_tag_dyneither(_tmp57D,sizeof(char),37))),_tag_dyneither(_tmp57C,sizeof(void*),0)));}
# 1088
exit(1);_LL168: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp1AD=(struct
Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1AD->tag != 5)goto _LL16A;}
_LL169:
# 1090
{const char*_tmp580;void*_tmp57F;(_tmp57F=0,Cyc_fprintf(Cyc_stderr,((_tmp580="Error: unexpected alias declaration",
_tag_dyneither(_tmp580,sizeof(char),36))),_tag_dyneither(_tmp57F,sizeof(void*),0)));}
# 1091
exit(1);_LL16A: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp1AE=(struct
Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1AE->tag != 14)goto
_LL16C;}_LL16B:
# 1093
{const char*_tmp583;void*_tmp582;(_tmp582=0,Cyc_fprintf(Cyc_stderr,((_tmp583="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp583,sizeof(char),38))),_tag_dyneither(_tmp582,sizeof(void*),0)));}
# 1094
exit(1);_LL16C: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp1AF=(
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1AF->tag != 15)
goto _LL16E;}_LL16D:
# 1096
{const char*_tmp586;void*_tmp585;(_tmp585=0,Cyc_fprintf(Cyc_stderr,((_tmp586="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp586,sizeof(char),39))),_tag_dyneither(_tmp585,sizeof(void*),0)));}
# 1097
exit(1);_LL16E: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp1B0=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)
_tmp1A1;if(_tmp1B0->tag != 2)goto _LL170;}_LL16F:
# 1099
{const char*_tmp589;void*_tmp588;(_tmp588=0,Cyc_fprintf(Cyc_stderr,((_tmp589="Error: unexpected let declaration\n",
_tag_dyneither(_tmp589,sizeof(char),35))),_tag_dyneither(_tmp588,sizeof(void*),0)));}
# 1100
exit(1);_LL170: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1B1=(
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1B1->tag != 7)
goto _LL172;}_LL171:
# 1102
{const char*_tmp58C;void*_tmp58B;(_tmp58B=0,Cyc_fprintf(Cyc_stderr,((_tmp58C="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp58C,sizeof(char),40))),_tag_dyneither(_tmp58B,sizeof(void*),0)));}
# 1103
exit(1);_LL172: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp1B2=(struct
Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1B2->tag != 3)goto _LL174;}
_LL173:
# 1105
{const char*_tmp58F;void*_tmp58E;(_tmp58E=0,Cyc_fprintf(Cyc_stderr,((_tmp58F="Error: unexpected let declaration\n",
_tag_dyneither(_tmp58F,sizeof(char),35))),_tag_dyneither(_tmp58E,sizeof(void*),0)));}
# 1106
exit(1);_LL174: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp1B3=(
struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1B3->tag != 10)
goto _LL176;}_LL175:
# 1108
{const char*_tmp592;void*_tmp591;(_tmp591=0,Cyc_fprintf(Cyc_stderr,((_tmp592="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp592,sizeof(char),41))),_tag_dyneither(_tmp591,sizeof(void*),0)));}
# 1109
exit(1);_LL176: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp1B4=(struct
Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1B4->tag != 11)goto _LL178;}
_LL177:
# 1111
{const char*_tmp595;void*_tmp594;(_tmp594=0,Cyc_fprintf(Cyc_stderr,((_tmp595="Error: unexpected using declaration\n",
_tag_dyneither(_tmp595,sizeof(char),37))),_tag_dyneither(_tmp594,sizeof(void*),0)));}
# 1112
exit(1);_LL178: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp1B5=(
struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1B5->tag != 12)
goto _LL17A;}_LL179:
# 1114
{const char*_tmp598;void*_tmp597;(_tmp597=0,Cyc_fprintf(Cyc_stderr,((_tmp598="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp598,sizeof(char),42))),_tag_dyneither(_tmp597,sizeof(void*),0)));}
# 1115
exit(1);_LL17A: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp1B6=(
struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1A1;if(_tmp1B6->tag
!= 13)goto _LL15B;}_LL17B:
# 1117
{const char*_tmp59B;void*_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B="Error: unexpected extern \"C include\" declaration\n",
_tag_dyneither(_tmp59B,sizeof(char),50))),_tag_dyneither(_tmp59A,sizeof(void*),0)));}
# 1118
exit(1);_LL15B:;}{
# 1125
struct Cyc_Set_Set*old;
# 1126
struct _dyneither_ptr*_tmp1E8=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
# 1127
{struct _handler_cons _tmp1E9;_push_handler(& _tmp1E9);{int _tmp1EB=0;if(setjmp(
_tmp1E9.handler))_tmp1EB=1;if(!_tmp1EB){
# 1128
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*
key))Cyc_Hashtable_lookup)(dep,_tmp1E8);;_pop_handler();}else{void*_tmp1EA=(void*)
_exn_thrown;void*_tmp1ED=_tmp1EA;_LL17D: {struct Cyc_Core_Not_found_exn_struct*
_tmp1EE=(struct Cyc_Core_Not_found_exn_struct*)_tmp1ED;if(_tmp1EE->tag != Cyc_Core_Not_found)
goto _LL17F;}_LL17E:
# 1130
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17C;_LL17F:;_LL180:(void)
_throw(_tmp1ED);_LL17C:;}};}{
# 1132
struct Cyc_Set_Set*_tmp1EF=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
old);
# 1133
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*
val))Cyc_Hashtable_insert)(dep,_tmp1E8,_tmp1EF);
# 1135
Cyc_current_targets=_tmp19E;
# 1136
Cyc_current_source=_tmp19F;};};}
# 1139
struct Cyc_Hashtable_Table*Cyc_new_deps(){
# 1140
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1143
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
# 1144
struct _handler_cons _tmp1F0;_push_handler(& _tmp1F0);{int _tmp1F2=0;if(setjmp(
_tmp1F0.handler))_tmp1F2=1;if(!_tmp1F2){{struct Cyc_Set_Set*_tmp1F3=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp1F3;};_pop_handler();}else{void*_tmp1F1=(void*)
_exn_thrown;void*_tmp1F5=_tmp1F1;_LL182: {struct Cyc_Core_Not_found_exn_struct*
_tmp1F6=(struct Cyc_Core_Not_found_exn_struct*)_tmp1F5;if(_tmp1F6->tag != Cyc_Core_Not_found)
goto _LL184;}_LL183:
# 1146
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL184:;_LL185:(void)_throw(
_tmp1F5);_LL181:;}};}
# 1150
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*
t){
# 1161 "/home/jgm/cyclone/src/buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
# 1162
struct Cyc_Set_Set*curr;
# 1163
for(curr=emptyset;init != 0;init=init->tl){
# 1164
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(
curr,(struct _dyneither_ptr*)init->hd);}{
# 1165
struct Cyc_Set_Set*_tmp1F7=curr;
# 1167
struct _dyneither_ptr*_tmp1F8=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"",sizeof(char),1);
# 1168
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1F7)> 0){
# 1169
struct Cyc_Set_Set*_tmp1F9=emptyset;
# 1170
struct Cyc_Iter_Iter _tmp1FA=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1F7);
# 1171
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(
_tmp1FA,& _tmp1F8)){
# 1172
_tmp1F9=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp1F9,Cyc_find(t,_tmp1F8));}
# 1173
_tmp1F7=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(
_tmp1F9,curr);
# 1174
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp1F7);}
# 1176
return curr;};}
# 1179
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH
 = 3};
# 1180
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
# 1181
static int Cyc_gathering(){
# 1182
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1185
static struct Cyc___cycFILE*Cyc_script_file=0;
# 1186
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1189
if(Cyc_script_file == 0){
# 1190
{const char*_tmp59E;void*_tmp59D;(_tmp59D=0,Cyc_fprintf(Cyc_stderr,((_tmp59E="Internal error: script file is NULL\n",
_tag_dyneither(_tmp59E,sizeof(char),37))),_tag_dyneither(_tmp59D,sizeof(void*),0)));}
# 1191
exit(1);}
# 1193
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1196
int Cyc_force_directory(struct _dyneither_ptr d){
# 1197
if(Cyc_mode == Cyc_GATHERSCRIPT){
# 1198
const char*_tmp5A3;void*_tmp5A2[2];struct Cyc_String_pa_PrintArg_struct _tmp5A1;
struct Cyc_String_pa_PrintArg_struct _tmp5A0;(_tmp5A0.tag=0,((_tmp5A0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)d),((_tmp5A1.tag=0,((_tmp5A1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)d),((_tmp5A2[0]=& _tmp5A1,((_tmp5A2[1]=&
_tmp5A0,Cyc_prscript(((_tmp5A3="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp5A3,sizeof(char),36))),_tag_dyneither(_tmp5A2,sizeof(void*),2)))))))))))));}
else{
# 1203
unsigned short _tmp5A4[0];int _tmp202=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp5A4,sizeof(
unsigned short),0));
# 1204
if(_tmp202 == - 1){
# 1205
if(mkdir((const char*)_untag_dyneither_ptr(d,sizeof(char),1),448)== - 1){
# 1206
{const char*_tmp5A8;void*_tmp5A7[1];struct Cyc_String_pa_PrintArg_struct _tmp5A6;(
_tmp5A6.tag=0,((_tmp5A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp5A7[0]=& _tmp5A6,Cyc_fprintf(Cyc_stderr,((_tmp5A8="Error: could not create directory %s\n",
_tag_dyneither(_tmp5A8,sizeof(char),38))),_tag_dyneither(_tmp5A7,sizeof(void*),1)))))));}
# 1207
return 1;}}else{
# 1210
close(_tmp202);}}
# 1212
return 0;}
# 1215
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1219
struct _dyneither_ptr _tmp207=Cyc_strdup((struct _dyneither_ptr)file);
# 1221
struct Cyc_List_List*_tmp208=0;
# 1222
while(1){
# 1223
_tmp207=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp207);
# 1224
if(Cyc_strlen((struct _dyneither_ptr)_tmp207)== 0)break;{
# 1225
struct _dyneither_ptr*_tmp5AB;struct Cyc_List_List*_tmp5AA;_tmp208=((_tmp5AA=
_cycalloc(sizeof(*_tmp5AA)),((_tmp5AA->hd=((_tmp5AB=_cycalloc(sizeof(*_tmp5AB)),((
_tmp5AB[0]=(struct _dyneither_ptr)_tmp207,_tmp5AB)))),((_tmp5AA->tl=_tmp208,
_tmp5AA))))));};}
# 1228
for(0;_tmp208 != 0;_tmp208=_tmp208->tl){
# 1229
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp208->hd)))return 1;}
# 1231
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{
char*tag;struct _dyneither_ptr f1;};
# 1238
static int Cyc_is_other_special(char c){
# 1239
switch(c){case '\\': _LL186:
# 1240
 goto _LL187;case '"': _LL187:
# 1241
 goto _LL188;case ';': _LL188:
# 1242
 goto _LL189;case '&': _LL189:
# 1243
 goto _LL18A;case '(': _LL18A:
# 1244
 goto _LL18B;case ')': _LL18B:
# 1245
 goto _LL18C;case '|': _LL18C:
# 1246
 goto _LL18D;case '^': _LL18D:
# 1247
 goto _LL18E;case '<': _LL18E:
# 1248
 goto _LL18F;case '>': _LL18F:
# 1249
 goto _LL190;case '\n': _LL190:
# 1253
 goto _LL191;case '\t': _LL191:
# 1254
 return 1;default: _LL192:
# 1255
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);
static void _tmp5B4(unsigned int*_tmp5B3,unsigned int*_tmp5B2,char**_tmp5B0){for(*
_tmp5B3=0;*_tmp5B3 < *_tmp5B2;(*_tmp5B3)++){(*_tmp5B0)[*_tmp5B3]='\000';}}
# 1259
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
# 1260
unsigned long _tmp20B=Cyc_strlen((struct _dyneither_ptr)s);
# 1263
int _tmp20C=0;
# 1264
int _tmp20D=0;
# 1265
{int i=0;for(0;i < _tmp20B;++ i){
# 1266
char _tmp20E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
# 1267
if(_tmp20E == '\'')++ _tmp20C;else{
# 1268
if(Cyc_is_other_special(_tmp20E))++ _tmp20D;}}}
# 1272
if(_tmp20C == 0  && _tmp20D == 0)
# 1273
return s;
# 1276
if(_tmp20C == 0){
# 1277
struct _dyneither_ptr*_tmp5AE;struct _dyneither_ptr*_tmp5AD[3];return(struct
_dyneither_ptr)Cyc_strconcat_l(((_tmp5AD[2]=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp5AD[1]=((_tmp5AE=
_cycalloc(sizeof(*_tmp5AE)),((_tmp5AE[0]=(struct _dyneither_ptr)s,_tmp5AE)))),((
_tmp5AD[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",
sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5AD,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 1280
unsigned long _tmp213=(_tmp20B + _tmp20C)+ _tmp20D;
# 1281
unsigned int _tmp5B3;unsigned int _tmp5B2;struct _dyneither_ptr _tmp5B1;char*
_tmp5B0;unsigned int _tmp5AF;struct _dyneither_ptr s2=(_tmp5AF=_tmp213 + 1,((_tmp5B0=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp5AF + 1)),((_tmp5B1=
_tag_dyneither(_tmp5B0,sizeof(char),_tmp5AF + 1),((((_tmp5B2=_tmp5AF,((_tmp5B4(&
_tmp5B3,& _tmp5B2,& _tmp5B0),_tmp5B0[_tmp5B2]=(char)0)))),_tmp5B1)))))));
# 1282
int _tmp214=0;
# 1283
int _tmp215=0;
# 1284
for(0;_tmp214 < _tmp20B;++ _tmp214){
# 1285
char _tmp216=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp214));
# 1286
if(_tmp216 == '\''  || Cyc_is_other_special(_tmp216)){
# 1287
char _tmp5B7;char _tmp5B6;struct _dyneither_ptr _tmp5B5;(_tmp5B5=
_dyneither_ptr_plus(s2,sizeof(char),_tmp215 ++),((_tmp5B6=*((char*)
_check_dyneither_subscript(_tmp5B5,sizeof(char),0)),((_tmp5B7='\\',((
_get_dyneither_size(_tmp5B5,sizeof(char))== 1  && (_tmp5B6 == '\000'  && _tmp5B7 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5B5.curr)=_tmp5B7)))))));}{
# 1288
char _tmp5BA;char _tmp5B9;struct _dyneither_ptr _tmp5B8;(_tmp5B8=
_dyneither_ptr_plus(s2,sizeof(char),_tmp215 ++),((_tmp5B9=*((char*)
_check_dyneither_subscript(_tmp5B8,sizeof(char),0)),((_tmp5BA=_tmp216,((
_get_dyneither_size(_tmp5B8,sizeof(char))== 1  && (_tmp5B9 == '\000'  && _tmp5BA != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5B8.curr)=_tmp5BA)))))));};}
# 1290
return(struct _dyneither_ptr)s2;};}
# 1292
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
# 1293
struct _dyneither_ptr*_tmp5BB;return(_tmp5BB=_cycalloc(sizeof(*_tmp5BB)),((
_tmp5BB[0]=Cyc_sh_escape_string(*sp),_tmp5BB)));}struct _tuple25{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};
# 1297
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct
Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,
struct Cyc_List_List*cycstubs){
# 1303
struct Cyc___cycFILE*maybe;
# 1304
struct Cyc___cycFILE*in_file;
# 1305
struct Cyc___cycFILE*out_file;
# 1306
int errorcode=0;
# 1308
{const char*_tmp5C1;void*_tmp5C0[1];const char*_tmp5BF;struct Cyc_String_pa_PrintArg_struct
_tmp5BE;(_tmp5BE.tag=0,((_tmp5BE.f1=(struct _dyneither_ptr)(
# 1309
(_tmp5BF=filename,_tag_dyneither(_tmp5BF,sizeof(char),_get_zero_arr_size_char((
void*)_tmp5BF,1)))),((_tmp5C0[0]=& _tmp5BE,Cyc_fprintf(Cyc_stderr,((_tmp5C1="********************************* %s...\n",
_tag_dyneither(_tmp5C1,sizeof(char),41))),_tag_dyneither(_tmp5C0,sizeof(void*),1)))))));}
# 1311
if(!Cyc_gathering()){const char*_tmp5C7;void*_tmp5C6[1];const char*_tmp5C5;struct
Cyc_String_pa_PrintArg_struct _tmp5C4;(_tmp5C4.tag=0,((_tmp5C4.f1=(struct
_dyneither_ptr)((_tmp5C5=filename,_tag_dyneither(_tmp5C5,sizeof(char),
_get_zero_arr_size_char((void*)_tmp5C5,1)))),((_tmp5C6[0]=& _tmp5C4,Cyc_log(((
_tmp5C7="\n%s:\n",_tag_dyneither(_tmp5C7,sizeof(char),6))),_tag_dyneither(
_tmp5C6,sizeof(void*),1)))))));}{
# 1323 "/home/jgm/cyclone/src/buildlib.cyl"
const char*_tmp5C8;struct _dyneither_ptr _tmp22A=Cyc_Filename_basename(((_tmp5C8=
filename,_tag_dyneither(_tmp5C8,sizeof(char),_get_zero_arr_size_char((void*)
_tmp5C8,1)))));
# 1324
const char*_tmp5C9;struct _dyneither_ptr _tmp22B=Cyc_Filename_dirname(((_tmp5C9=
filename,_tag_dyneither(_tmp5C9,sizeof(char),_get_zero_arr_size_char((void*)
_tmp5C9,1)))));
# 1325
struct _dyneither_ptr _tmp22C=Cyc_Filename_chop_extension((struct _dyneither_ptr)
_tmp22A);
# 1326
const char*_tmp5CA;const char*_tmp22D=(const char*)_untag_dyneither_ptr(Cyc_strconcat((
struct _dyneither_ptr)_tmp22C,((_tmp5CA=".iA",_tag_dyneither(_tmp5CA,sizeof(char),
4)))),sizeof(char),1);
# 1327
const char*_tmp5D2;void*_tmp5D1[1];struct Cyc_String_pa_PrintArg_struct _tmp5D0;
const char*_tmp5CE;void*_tmp5CD[1];struct Cyc_String_pa_PrintArg_struct _tmp5CC;
const char*_tmp22E=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp22B,
sizeof(char))== 0?(_tmp5D0.tag=0,((_tmp5D0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp22C),((_tmp5D1[0]=& _tmp5D0,Cyc_aprintf(((_tmp5D2="%s.iB",
_tag_dyneither(_tmp5D2,sizeof(char),6))),_tag_dyneither(_tmp5D1,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp22B,(struct _dyneither_ptr)((
_tmp5CC.tag=0,((_tmp5CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22C),((
_tmp5CD[0]=& _tmp5CC,Cyc_aprintf(((_tmp5CE="%s.iB",_tag_dyneither(_tmp5CE,sizeof(
char),6))),_tag_dyneither(_tmp5CD,sizeof(void*),1))))))))),sizeof(char),1);
# 1331
const char*_tmp5DA;void*_tmp5D9[1];struct Cyc_String_pa_PrintArg_struct _tmp5D8;
const char*_tmp5D6;void*_tmp5D5[1];struct Cyc_String_pa_PrintArg_struct _tmp5D4;
const char*_tmp22F=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp22B,
sizeof(char))== 0?(_tmp5D8.tag=0,((_tmp5D8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp22C),((_tmp5D9[0]=& _tmp5D8,Cyc_aprintf(((_tmp5DA="%s.iC",
_tag_dyneither(_tmp5DA,sizeof(char),6))),_tag_dyneither(_tmp5D9,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp22B,(struct _dyneither_ptr)((
_tmp5D4.tag=0,((_tmp5D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22C),((
_tmp5D5[0]=& _tmp5D4,Cyc_aprintf(((_tmp5D6="%s.iC",_tag_dyneither(_tmp5D6,sizeof(
char),6))),_tag_dyneither(_tmp5D5,sizeof(void*),1))))))))),sizeof(char),1);
# 1335
const char*_tmp5E2;void*_tmp5E1[1];struct Cyc_String_pa_PrintArg_struct _tmp5E0;
const char*_tmp5DE;void*_tmp5DD[1];struct Cyc_String_pa_PrintArg_struct _tmp5DC;
const char*_tmp230=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp22B,
sizeof(char))== 0?(_tmp5E0.tag=0,((_tmp5E0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp22C),((_tmp5E1[0]=& _tmp5E0,Cyc_aprintf(((_tmp5E2="%s.iD",
_tag_dyneither(_tmp5E2,sizeof(char),6))),_tag_dyneither(_tmp5E1,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp22B,(struct _dyneither_ptr)((
_tmp5DC.tag=0,((_tmp5DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp22C),((
_tmp5DD[0]=& _tmp5DC,Cyc_aprintf(((_tmp5DE="%s.iD",_tag_dyneither(_tmp5DE,sizeof(
char),6))),_tag_dyneither(_tmp5DD,sizeof(void*),1))))))))),sizeof(char),1);
# 1340
{struct _handler_cons _tmp231;_push_handler(& _tmp231);{int _tmp233=0;if(setjmp(
_tmp231.handler))_tmp233=1;if(!_tmp233){
# 1343
{const char*_tmp5E3;if(Cyc_force_directory_prefixes(((_tmp5E3=filename,
_tag_dyneither(_tmp5E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E3,1)))))){
# 1344
int _tmp235=1;_npop_handler(0);return _tmp235;}}
# 1348
if(Cyc_mode != Cyc_FINISH){
# 1349
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);
# 1350
if(Cyc_mode == Cyc_GATHERSCRIPT){
# 1351
{const char*_tmp5E9;void*_tmp5E8[1];const char*_tmp5E7;struct Cyc_String_pa_PrintArg_struct
_tmp5E6;(_tmp5E6.tag=0,((_tmp5E6.f1=(struct _dyneither_ptr)((_tmp5E7=_tmp22D,
_tag_dyneither(_tmp5E7,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E7,1)))),((
_tmp5E8[0]=& _tmp5E6,Cyc_prscript(((_tmp5E9="cat >%s <<XXX\n",_tag_dyneither(
_tmp5E9,sizeof(char),15))),_tag_dyneither(_tmp5E8,sizeof(void*),1)))))));}
# 1352
{struct Cyc_List_List*_tmp23A=Cyc_current_cpp;for(0;_tmp23A != 0;_tmp23A=_tmp23A->tl){
# 1353
const char*_tmp5ED;void*_tmp5EC[1];struct Cyc_String_pa_PrintArg_struct _tmp5EB;(
_tmp5EB.tag=0,((_tmp5EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp23A->hd)),((_tmp5EC[0]=& _tmp5EB,Cyc_prscript(((_tmp5ED="%s",
_tag_dyneither(_tmp5ED,sizeof(char),3))),_tag_dyneither(_tmp5EC,sizeof(void*),1)))))));}}
# 1354
{const char*_tmp5F3;void*_tmp5F2[1];const char*_tmp5F1;struct Cyc_String_pa_PrintArg_struct
_tmp5F0;(_tmp5F0.tag=0,((_tmp5F0.f1=(struct _dyneither_ptr)((_tmp5F1=filename,
_tag_dyneither(_tmp5F1,sizeof(char),_get_zero_arr_size_char((void*)_tmp5F1,1)))),((
_tmp5F2[0]=& _tmp5F0,Cyc_prscript(((_tmp5F3="#include <%s>\n",_tag_dyneither(
_tmp5F3,sizeof(char),15))),_tag_dyneither(_tmp5F2,sizeof(void*),1)))))));}
# 1355
{const char*_tmp5F6;void*_tmp5F5;(_tmp5F5=0,Cyc_prscript(((_tmp5F6="XXX\n",
_tag_dyneither(_tmp5F6,sizeof(char),5))),_tag_dyneither(_tmp5F5,sizeof(void*),0)));}
# 1356
{const char*_tmp5FF;void*_tmp5FE[2];const char*_tmp5FD;struct Cyc_String_pa_PrintArg_struct
_tmp5FC;const char*_tmp5FB;struct Cyc_String_pa_PrintArg_struct _tmp5FA;(_tmp5FA.tag=
0,((_tmp5FA.f1=(struct _dyneither_ptr)((_tmp5FB=_tmp22D,_tag_dyneither(_tmp5FB,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5FB,1)))),((_tmp5FC.tag=0,((
_tmp5FC.f1=(struct _dyneither_ptr)((_tmp5FD=_tmp22E,_tag_dyneither(_tmp5FD,
sizeof(char),_get_zero_arr_size_char((void*)_tmp5FD,1)))),((_tmp5FE[0]=& _tmp5FC,((
_tmp5FE[1]=& _tmp5FA,Cyc_prscript(((_tmp5FF="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp5FF,sizeof(char),32))),_tag_dyneither(_tmp5FE,sizeof(void*),2)))))))))))));}
# 1357
{const char*_tmp608;void*_tmp607[2];const char*_tmp606;struct Cyc_String_pa_PrintArg_struct
_tmp605;const char*_tmp604;struct Cyc_String_pa_PrintArg_struct _tmp603;(_tmp603.tag=
0,((_tmp603.f1=(struct _dyneither_ptr)((_tmp604=_tmp22D,_tag_dyneither(_tmp604,
sizeof(char),_get_zero_arr_size_char((void*)_tmp604,1)))),((_tmp605.tag=0,((
_tmp605.f1=(struct _dyneither_ptr)((_tmp606=_tmp22F,_tag_dyneither(_tmp606,
sizeof(char),_get_zero_arr_size_char((void*)_tmp606,1)))),((_tmp607[0]=& _tmp605,((
_tmp607[1]=& _tmp603,Cyc_prscript(((_tmp608="$GCC -E     -o %s -x c %s;\n",
_tag_dyneither(_tmp608,sizeof(char),28))),_tag_dyneither(_tmp607,sizeof(void*),2)))))))))))));}{
# 1358
const char*_tmp60E;void*_tmp60D[1];const char*_tmp60C;struct Cyc_String_pa_PrintArg_struct
_tmp60B;(_tmp60B.tag=0,((_tmp60B.f1=(struct _dyneither_ptr)((_tmp60C=_tmp22D,
_tag_dyneither(_tmp60C,sizeof(char),_get_zero_arr_size_char((void*)_tmp60C,1)))),((
_tmp60D[0]=& _tmp60B,Cyc_prscript(((_tmp60E="rm %s\n",_tag_dyneither(_tmp60E,
sizeof(char),7))),_tag_dyneither(_tmp60D,sizeof(void*),1)))))));};}else{
# 1361
maybe=Cyc_fopen(_tmp22D,(const char*)"w");
# 1362
if(!((unsigned int)maybe)){
# 1363
{const char*_tmp614;void*_tmp613[1];const char*_tmp612;struct Cyc_String_pa_PrintArg_struct
_tmp611;(_tmp611.tag=0,((_tmp611.f1=(struct _dyneither_ptr)((_tmp612=_tmp22D,
_tag_dyneither(_tmp612,sizeof(char),_get_zero_arr_size_char((void*)_tmp612,1)))),((
_tmp613[0]=& _tmp611,Cyc_fprintf(Cyc_stderr,((_tmp614="Error: could not create file %s\n",
_tag_dyneither(_tmp614,sizeof(char),33))),_tag_dyneither(_tmp613,sizeof(void*),1)))))));}{
# 1364
int _tmp258=1;_npop_handler(0);return _tmp258;};}
# 1366
out_file=(struct Cyc___cycFILE*)maybe;
# 1367
{struct Cyc_List_List*_tmp259=Cyc_current_cpp;for(0;_tmp259 != 0;_tmp259=_tmp259->tl){
# 1368
Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp259->hd),
sizeof(char),1),out_file);}}
# 1369
{const char*_tmp61A;void*_tmp619[1];const char*_tmp618;struct Cyc_String_pa_PrintArg_struct
_tmp617;(_tmp617.tag=0,((_tmp617.f1=(struct _dyneither_ptr)((_tmp618=filename,
_tag_dyneither(_tmp618,sizeof(char),_get_zero_arr_size_char((void*)_tmp618,1)))),((
_tmp619[0]=& _tmp617,Cyc_fprintf(out_file,((_tmp61A="#include <%s>\n",
_tag_dyneither(_tmp61A,sizeof(char),15))),_tag_dyneither(_tmp619,sizeof(void*),1)))))));}
# 1370
Cyc_fclose(out_file);{
# 1371
struct _dyneither_ptr _tmp25E=Cstring_to_string(Ccomp);
# 1372
const char*_tmp621;struct _dyneither_ptr*_tmp620;const char*_tmp61F;struct Cyc_List_List*
_tmp61E;struct _dyneither_ptr _tmp25F=
# 1373
Cyc_str_sepstr(((_tmp61E=_cycalloc(sizeof(*_tmp61E)),((_tmp61E->hd=((_tmp620=
_cycalloc(sizeof(*_tmp620)),((_tmp620[0]=(struct _dyneither_ptr)((_tmp61F="",
_tag_dyneither(_tmp61F,sizeof(char),1))),_tmp620)))),((_tmp61E->tl=
# 1374
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp61E)))))),(
# 1375
(_tmp621=" ",_tag_dyneither(_tmp621,sizeof(char),2))));
# 1376
const char*_tmp62C;void*_tmp62B[4];struct Cyc_String_pa_PrintArg_struct _tmp62A;
const char*_tmp629;struct Cyc_String_pa_PrintArg_struct _tmp628;struct Cyc_String_pa_PrintArg_struct
_tmp627;const char*_tmp626;struct Cyc_String_pa_PrintArg_struct _tmp625;char*cmd=(
char*)_untag_dyneither_ptr(((_tmp625.tag=0,((_tmp625.f1=(struct _dyneither_ptr)((
_tmp626=_tmp22D,_tag_dyneither(_tmp626,sizeof(char),_get_zero_arr_size_char((
void*)_tmp626,1)))),((_tmp627.tag=0,((_tmp627.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp25F),((_tmp628.tag=0,((_tmp628.f1=(struct _dyneither_ptr)((
_tmp629=_tmp22E,_tag_dyneither(_tmp629,sizeof(char),_get_zero_arr_size_char((
void*)_tmp629,1)))),((_tmp62A.tag=0,((_tmp62A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp25E),((_tmp62B[0]=& _tmp62A,((_tmp62B[1]=& _tmp628,((_tmp62B[2]=&
_tmp627,((_tmp62B[3]=& _tmp625,Cyc_aprintf(((_tmp62C="%s -E -dM -o %s -x c %s %s",
_tag_dyneither(_tmp62C,sizeof(char),27))),_tag_dyneither(_tmp62B,sizeof(void*),4)))))))))))))))))))))))))),
sizeof(char),1);
# 1378
if(!system((const char*)cmd)){
# 1381
{const char*_tmp637;void*_tmp636[4];struct Cyc_String_pa_PrintArg_struct _tmp635;
const char*_tmp634;struct Cyc_String_pa_PrintArg_struct _tmp633;struct Cyc_String_pa_PrintArg_struct
_tmp632;const char*_tmp631;struct Cyc_String_pa_PrintArg_struct _tmp630;cmd=(char*)
_untag_dyneither_ptr(((_tmp630.tag=0,((_tmp630.f1=(struct _dyneither_ptr)((
_tmp631=_tmp22D,_tag_dyneither(_tmp631,sizeof(char),_get_zero_arr_size_char((
void*)_tmp631,1)))),((_tmp632.tag=0,((_tmp632.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp25F),((_tmp633.tag=0,((_tmp633.f1=(struct _dyneither_ptr)((
_tmp634=_tmp22F,_tag_dyneither(_tmp634,sizeof(char),_get_zero_arr_size_char((
void*)_tmp634,1)))),((_tmp635.tag=0,((_tmp635.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp25E),((_tmp636[0]=& _tmp635,((_tmp636[1]=& _tmp633,((_tmp636[2]=&
_tmp632,((_tmp636[3]=& _tmp630,Cyc_aprintf(((_tmp637="%s -E -o %s -x c %s %s",
_tag_dyneither(_tmp637,sizeof(char),23))),_tag_dyneither(_tmp636,sizeof(void*),4)))))))))))))))))))))))))),
sizeof(char),1);}
# 1383
system((const char*)cmd);}
# 1385
remove(_tmp22D);};}}
# 1388
if(Cyc_gathering()){int _tmp274=0;_npop_handler(0);return _tmp274;}{
# 1391
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
# 1392
maybe=Cyc_fopen(_tmp22E,(const char*)"r");
# 1393
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp648;const char*
_tmp647;void*_tmp646[1];const char*_tmp645;struct Cyc_String_pa_PrintArg_struct
_tmp644;struct Cyc_NO_SUPPORT_exn_struct*_tmp643;(int)_throw((void*)((_tmp643=
_cycalloc(sizeof(*_tmp643)),((_tmp643[0]=((_tmp648.tag=Cyc_NO_SUPPORT,((_tmp648.f1=(
struct _dyneither_ptr)((_tmp644.tag=0,((_tmp644.f1=(struct _dyneither_ptr)((
_tmp645=_tmp22E,_tag_dyneither(_tmp645,sizeof(char),_get_zero_arr_size_char((
void*)_tmp645,1)))),((_tmp646[0]=& _tmp644,Cyc_aprintf(((_tmp647="can't open macrosfile %s",
_tag_dyneither(_tmp647,sizeof(char),25))),_tag_dyneither(_tmp646,sizeof(void*),1)))))))),
_tmp648)))),_tmp643)))));}
# 1396
in_file=(struct Cyc___cycFILE*)maybe;{
# 1397
struct Cyc_Lexing_lexbuf*_tmp27B=Cyc_Lexing_from_file(in_file);
# 1398
struct _tuple20*entry;
# 1399
while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(
_tmp27B))!= 0){
# 1400
struct _tuple20 _tmp27D;struct _dyneither_ptr*_tmp27E;struct Cyc_Set_Set*_tmp27F;
struct _tuple20*_tmp27C=(struct _tuple20*)_check_null(entry);_tmp27D=*_tmp27C;
_tmp27E=_tmp27D.f1;_tmp27F=_tmp27D.f2;
# 1401
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*
val))Cyc_Hashtable_insert)(t,_tmp27E,_tmp27F);}
# 1405
Cyc_fclose(in_file);
# 1408
maybe=Cyc_fopen(_tmp22F,(const char*)"r");
# 1409
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp659;const char*
_tmp658;void*_tmp657[1];const char*_tmp656;struct Cyc_String_pa_PrintArg_struct
_tmp655;struct Cyc_NO_SUPPORT_exn_struct*_tmp654;(int)_throw((void*)((_tmp654=
_cycalloc(sizeof(*_tmp654)),((_tmp654[0]=((_tmp659.tag=Cyc_NO_SUPPORT,((_tmp659.f1=(
struct _dyneither_ptr)((_tmp655.tag=0,((_tmp655.f1=(struct _dyneither_ptr)((
_tmp656=_tmp22F,_tag_dyneither(_tmp656,sizeof(char),_get_zero_arr_size_char((
void*)_tmp656,1)))),((_tmp657[0]=& _tmp655,Cyc_aprintf(((_tmp658="can't open declsfile %s",
_tag_dyneither(_tmp658,sizeof(char),24))),_tag_dyneither(_tmp657,sizeof(void*),1)))))))),
_tmp659)))),_tmp654)))));}
# 1412
in_file=(struct Cyc___cycFILE*)maybe;
# 1413
_tmp27B=Cyc_Lexing_from_file(in_file);
# 1414
Cyc_slurp_out=Cyc_fopen(_tmp230,(const char*)"w");
# 1415
if(!((unsigned int)Cyc_slurp_out)){int _tmp286=1;_npop_handler(0);return _tmp286;}
# 1416
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp27B)){;}
# 1417
Cyc_fclose(in_file);
# 1418
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
# 1419
if(Cyc_mode != Cyc_FINISH)remove(_tmp22F);
# 1421
maybe=Cyc_fopen(_tmp230,(const char*)"r");
# 1422
if(!((unsigned int)maybe)){int _tmp287=1;_npop_handler(0);return _tmp287;}
# 1423
in_file=(struct Cyc___cycFILE*)maybe;
# 1424
{const char*_tmp65A;Cyc_Position_reset_position(((_tmp65A=_tmp230,_tag_dyneither(
_tmp65A,sizeof(char),_get_zero_arr_size_char((void*)_tmp65A,1)))));}
# 1425
Cyc_Lex_lex_init(0);{
# 1426
struct Cyc_List_List*_tmp289=Cyc_Parse_parse_file(in_file);
# 1427
Cyc_Lex_lex_init(0);
# 1428
Cyc_fclose(in_file);
# 1429
remove(_tmp230);
# 1431
{struct Cyc_List_List*_tmp28A=_tmp289;for(0;_tmp28A != 0;_tmp28A=_tmp28A->tl){
# 1432
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp28A->hd,t);}}{
# 1435
struct Cyc_Set_Set*_tmp28B=Cyc_reachable(start_symbols,t);
# 1438
struct Cyc_List_List*_tmp28C=0;
# 1439
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
# 1440
{struct Cyc_List_List*_tmp28D=_tmp289;for(0;_tmp28D != 0;_tmp28D=_tmp28D->tl){
# 1441
struct Cyc_Absyn_Decl*_tmp28E=(struct Cyc_Absyn_Decl*)_tmp28D->hd;
# 1442
struct _dyneither_ptr*name;
# 1443
{void*_tmp28F=_tmp28E->r;struct Cyc_Absyn_Vardecl*_tmp291;struct Cyc_Absyn_Fndecl*
_tmp293;struct Cyc_Absyn_Aggrdecl*_tmp295;struct Cyc_Absyn_Enumdecl*_tmp297;struct
Cyc_Absyn_Typedefdecl*_tmp299;_LL195: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp290=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp290->tag != 
0)goto _LL197;else{_tmp291=_tmp290->f1;}}_LL196: {
# 1447
struct _tuple0 _tmp2A6;struct _dyneither_ptr*_tmp2A7;struct _tuple0*_tmp2A5=_tmp291->name;
_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A6.f2;
# 1448
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2A7);
# 1449
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,
_tmp2A7))name=0;else{
# 1450
name=(struct _dyneither_ptr*)_tmp2A7;}
# 1451
goto _LL194;}_LL197: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp292=(struct
Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp292->tag != 1)goto _LL199;
else{_tmp293=_tmp292->f1;}}_LL198: {
# 1453
struct _tuple0 _tmp2A9;struct _dyneither_ptr*_tmp2AA;struct _tuple0*_tmp2A8=_tmp293->name;
_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.f2;
# 1454
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2AA);
# 1455
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct
Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,
_tmp2AA))name=0;else{
# 1456
name=(struct _dyneither_ptr*)_tmp2AA;}
# 1457
goto _LL194;}_LL199: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp294=(
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp294->tag != 6)goto
_LL19B;else{_tmp295=_tmp294->f1;}}_LL19A: {
# 1459
struct _tuple0 _tmp2AC;struct _dyneither_ptr*_tmp2AD;struct _tuple0*_tmp2AB=_tmp295->name;
_tmp2AC=*_tmp2AB;_tmp2AD=_tmp2AC.f2;
# 1460
name=(struct _dyneither_ptr*)_tmp2AD;
# 1461
goto _LL194;}_LL19B: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp296=(
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp296->tag != 8)goto
_LL19D;else{_tmp297=_tmp296->f1;}}_LL19C: {
# 1463
struct _tuple0 _tmp2AF;struct _dyneither_ptr*_tmp2B0;struct _tuple0*_tmp2AE=_tmp297->name;
_tmp2AF=*_tmp2AE;_tmp2B0=_tmp2AF.f2;
# 1464
name=(struct _dyneither_ptr*)_tmp2B0;
# 1467
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp28B,(struct _dyneither_ptr*)name)){
# 1468
struct Cyc_List_List*_tmp65B;_tmp28C=((_tmp65B=_cycalloc(sizeof(*_tmp65B)),((
_tmp65B->hd=_tmp28E,((_tmp65B->tl=_tmp28C,_tmp65B))))));}else{
# 1470
if((unsigned int)_tmp297->fields){
# 1471
struct Cyc_List_List*_tmp2B2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp297->fields))->v;for(0;_tmp2B2 != 0;_tmp2B2=_tmp2B2->tl){
# 1472
struct Cyc_Absyn_Enumfield*_tmp2B3=(struct Cyc_Absyn_Enumfield*)_tmp2B2->hd;
# 1473
struct _tuple0 _tmp2B5;struct _dyneither_ptr*_tmp2B6;struct _tuple0*_tmp2B4=_tmp2B3->name;
_tmp2B5=*_tmp2B4;_tmp2B6=_tmp2B5.f2;
# 1474
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp28B,_tmp2B6)){
# 1475
{struct Cyc_List_List*_tmp65C;_tmp28C=((_tmp65C=_cycalloc(sizeof(*_tmp65C)),((
_tmp65C->hd=_tmp28E,((_tmp65C->tl=_tmp28C,_tmp65C))))));}
# 1476
break;}}}}
# 1480
name=0;
# 1481
goto _LL194;}_LL19D: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp298=(
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp298->tag != 9)
goto _LL19F;else{_tmp299=_tmp298->f1;}}_LL19E: {
# 1483
struct _tuple0 _tmp2B9;struct _dyneither_ptr*_tmp2BA;struct _tuple0*_tmp2B8=_tmp299->name;
_tmp2B9=*_tmp2B8;_tmp2BA=_tmp2B9.f2;
# 1484
name=(struct _dyneither_ptr*)_tmp2BA;
# 1485
goto _LL194;}_LL19F: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp29A=(
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp29A->tag != 14)
goto _LL1A1;}_LL1A0:
# 1486
 goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp29B=(
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp29B->tag != 15)
goto _LL1A3;}_LL1A2:
# 1487
 goto _LL1A4;_LL1A3: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp29C=(struct
Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp29C->tag != 2)goto _LL1A5;}
_LL1A4:
# 1488
 goto _LL1A6;_LL1A5: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp29D=(
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp29D->tag != 7)
goto _LL1A7;}_LL1A6:
# 1489
 goto _LL1A8;_LL1A7: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp29E=(
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp29E->tag != 3)goto
_LL1A9;}_LL1A8:
# 1490
 goto _LL1AA;_LL1A9: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp29F=(
struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp29F->tag != 10)
goto _LL1AB;}_LL1AA:
# 1491
 goto _LL1AC;_LL1AB: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp2A0=(
struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp2A0->tag != 11)goto
_LL1AD;}_LL1AC:
# 1492
 goto _LL1AE;_LL1AD: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp2A1=(
struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp2A1->tag != 12)
goto _LL1AF;}_LL1AE:
# 1493
 goto _LL1B0;_LL1AF: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*
_tmp2A2=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp28F;if(
_tmp2A2->tag != 13)goto _LL1B1;}_LL1B0:
# 1494
 goto _LL1B2;_LL1B1: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp2A3=(
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp2A3->tag != 4)goto
_LL1B3;}_LL1B2:
# 1495
 goto _LL1B4;_LL1B3: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp2A4=(
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp28F;if(_tmp2A4->tag != 5)goto
_LL194;}_LL1B4:
# 1497
 name=0;
# 1498
goto _LL194;_LL194:;}
# 1500
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp28B,(struct _dyneither_ptr*)name)){
# 1501
struct Cyc_List_List*_tmp65D;_tmp28C=((_tmp65D=_cycalloc(sizeof(*_tmp65D)),((
_tmp65D->hd=_tmp28E,((_tmp65D->tl=_tmp28C,_tmp65D))))));}}}
# 1505
if(!Cyc_do_setjmp){
# 1506
maybe=Cyc_fopen(filename,(const char*)"w");
# 1507
if(!((unsigned int)maybe)){int _tmp2BC=1;_npop_handler(0);return _tmp2BC;}
# 1508
out_file=(struct Cyc___cycFILE*)maybe;}else{
# 1509
out_file=Cyc_stdout;}{
# 1510
const char*_tmp663;void*_tmp662[1];const char*_tmp661;struct Cyc_String_pa_PrintArg_struct
_tmp660;struct _dyneither_ptr ifdefmacro=(_tmp660.tag=0,((_tmp660.f1=(struct
_dyneither_ptr)((_tmp661=filename,_tag_dyneither(_tmp661,sizeof(char),
_get_zero_arr_size_char((void*)_tmp661,1)))),((_tmp662[0]=& _tmp660,Cyc_aprintf(((
_tmp663="_%s_",_tag_dyneither(_tmp663,sizeof(char),5))),_tag_dyneither(_tmp662,
sizeof(void*),1)))))));
# 1511
{int _tmp2BD=0;for(0;_tmp2BD < _get_dyneither_size(ifdefmacro,sizeof(char));++
_tmp2BD){
# 1512
if(((char*)ifdefmacro.curr)[_tmp2BD]== '.'  || ((char*)ifdefmacro.curr)[_tmp2BD]
== '/'){
# 1513
char _tmp666;char _tmp665;struct _dyneither_ptr _tmp664;(_tmp664=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2BD),((_tmp665=*((char*)
_check_dyneither_subscript(_tmp664,sizeof(char),0)),((_tmp666='_',((
_get_dyneither_size(_tmp664,sizeof(char))== 1  && (_tmp665 == '\000'  && _tmp666 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp664.curr)=_tmp666)))))));}else{
# 1514
if(((char*)ifdefmacro.curr)[_tmp2BD]!= '_'  && ((char*)ifdefmacro.curr)[_tmp2BD]
!= '/'){
# 1515
char _tmp669;char _tmp668;struct _dyneither_ptr _tmp667;(_tmp667=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2BD),((_tmp668=*((char*)
_check_dyneither_subscript(_tmp667,sizeof(char),0)),((_tmp669=(char)toupper((int)((
char*)ifdefmacro.curr)[_tmp2BD]),((_get_dyneither_size(_tmp667,sizeof(char))== 1
 && (_tmp668 == '\000'  && _tmp669 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp667.curr)=_tmp669)))))));}}}}
# 1517
{const char*_tmp66E;void*_tmp66D[2];struct Cyc_String_pa_PrintArg_struct _tmp66C;
struct Cyc_String_pa_PrintArg_struct _tmp66B;(_tmp66B.tag=0,((_tmp66B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp66C.tag=0,((_tmp66C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp66D[0]=& _tmp66C,((
_tmp66D[1]=& _tmp66B,Cyc_fprintf(out_file,((_tmp66E="#ifndef %s\n#define %s\n",
_tag_dyneither(_tmp66E,sizeof(char),23))),_tag_dyneither(_tmp66D,sizeof(void*),2)))))))))))));}{
# 1524
struct Cyc_List_List*_tmp2C8=0;
# 1525
struct Cyc_List_List*_tmp2C9=0;
# 1526
{struct Cyc_List_List*_tmp2CA=_tmp28C;for(0;_tmp2CA != 0;_tmp2CA=_tmp2CA->tl){
# 1527
struct Cyc_Absyn_Decl*_tmp2CB=(struct Cyc_Absyn_Decl*)_tmp2CA->hd;
# 1528
int _tmp2CC=0;
# 1529
struct _dyneither_ptr*name;
# 1530
{void*_tmp2CD=_tmp2CB->r;struct Cyc_Absyn_Vardecl*_tmp2CF;struct Cyc_Absyn_Fndecl*
_tmp2D1;struct Cyc_Absyn_Aggrdecl*_tmp2D3;struct Cyc_Absyn_Enumdecl*_tmp2D5;struct
Cyc_Absyn_Typedefdecl*_tmp2D7;_LL1B6: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*
_tmp2CE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2CE->tag != 
0)goto _LL1B8;else{_tmp2CF=_tmp2CE->f1;}}_LL1B7: {
# 1532
struct _tuple0 _tmp2E4;struct _dyneither_ptr*_tmp2E5;struct _tuple0*_tmp2E3=_tmp2CF->name;
_tmp2E4=*_tmp2E3;_tmp2E5=_tmp2E4.f2;
# 1533
name=(struct _dyneither_ptr*)_tmp2E5;
# 1534
goto _LL1B5;}_LL1B8: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp2D0=(struct
Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2D0->tag != 1)goto _LL1BA;
else{_tmp2D1=_tmp2D0->f1;}}_LL1B9: {
# 1536
struct _tuple0 _tmp2E7;struct _dyneither_ptr*_tmp2E8;struct _tuple0*_tmp2E6=_tmp2D1->name;
_tmp2E7=*_tmp2E6;_tmp2E8=_tmp2E7.f2;
# 1537
name=(struct _dyneither_ptr*)_tmp2E8;
# 1538
goto _LL1B5;}_LL1BA: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp2D2=(
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2D2->tag != 6)goto
_LL1BC;else{_tmp2D3=_tmp2D2->f1;}}_LL1BB: {
# 1540
struct _tuple0 _tmp2EA;struct _dyneither_ptr*_tmp2EB;struct _tuple0*_tmp2E9=_tmp2D3->name;
_tmp2EA=*_tmp2E9;_tmp2EB=_tmp2EA.f2;
# 1541
name=(struct _dyneither_ptr*)_tmp2EB;
# 1542
goto _LL1B5;}_LL1BC: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp2D4=(
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2D4->tag != 8)goto
_LL1BE;else{_tmp2D5=_tmp2D4->f1;}}_LL1BD: {
# 1544
struct _tuple0 _tmp2ED;struct _dyneither_ptr*_tmp2EE;struct _tuple0*_tmp2EC=_tmp2D5->name;
_tmp2ED=*_tmp2EC;_tmp2EE=_tmp2ED.f2;
# 1545
name=(struct _dyneither_ptr*)_tmp2EE;
# 1546
goto _LL1B5;}_LL1BE: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp2D6=(
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2D6->tag != 9)
goto _LL1C0;else{_tmp2D7=_tmp2D6->f1;}}_LL1BF: {
# 1548
struct _tuple0 _tmp2F0;struct _dyneither_ptr*_tmp2F1;struct _tuple0*_tmp2EF=_tmp2D7->name;
_tmp2F0=*_tmp2EF;_tmp2F1=_tmp2F0.f2;
# 1549
name=(struct _dyneither_ptr*)_tmp2F1;
# 1550
goto _LL1B5;}_LL1C0: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp2D8=(
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2D8->tag != 4)goto
_LL1C2;}_LL1C1:
# 1551
 goto _LL1C3;_LL1C2: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp2D9=(
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2D9->tag != 5)goto
_LL1C4;}_LL1C3:
# 1552
 goto _LL1C5;_LL1C4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp2DA=(
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2DA->tag != 14)
goto _LL1C6;}_LL1C5:
# 1553
 goto _LL1C7;_LL1C6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp2DB=(
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2DB->tag != 15)
goto _LL1C8;}_LL1C7:
# 1554
 goto _LL1C9;_LL1C8: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp2DC=(struct
Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2DC->tag != 2)goto _LL1CA;}
_LL1C9:
# 1555
 goto _LL1CB;_LL1CA: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2DD=(
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2DD->tag != 7)
goto _LL1CC;}_LL1CB:
# 1556
 goto _LL1CD;_LL1CC: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp2DE=(
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2DE->tag != 3)goto
_LL1CE;}_LL1CD:
# 1557
 goto _LL1CF;_LL1CE: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp2DF=(
struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2DF->tag != 10)
goto _LL1D0;}_LL1CF:
# 1558
 goto _LL1D1;_LL1D0: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp2E0=(
struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2E0->tag != 11)goto
_LL1D2;}_LL1D1:
# 1559
 goto _LL1D3;_LL1D2: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp2E1=(
struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2CD;if(_tmp2E1->tag != 12)
goto _LL1D4;}_LL1D3:
# 1560
 goto _LL1D5;_LL1D4: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*
_tmp2E2=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2CD;if(
_tmp2E2->tag != 13)goto _LL1B5;}_LL1D5:
# 1562
 name=0;
# 1563
goto _LL1B5;_LL1B5:;}
# 1565
if(!((unsigned int)name) && !_tmp2CC)continue;
# 1566
{struct Cyc_List_List*_tmp66F;_tmp2C8=((_tmp66F=_cycalloc(sizeof(*_tmp66F)),((
_tmp66F->hd=_tmp2CB,((_tmp66F->tl=_tmp2C8,_tmp66F))))));}{
# 1567
struct Cyc_List_List*_tmp670;_tmp2C9=((_tmp670=_cycalloc(sizeof(*_tmp670)),((
_tmp670->hd=name,((_tmp670->tl=_tmp2C9,_tmp670))))));};}}
# 1571
{struct _handler_cons _tmp2F4;_push_handler(& _tmp2F4);{int _tmp2F6=0;if(setjmp(
_tmp2F4.handler))_tmp2F6=1;if(!_tmp2F6){
# 1572
{struct _RegionHandle _tmp2F7=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=&
_tmp2F7;_push_region(tc_rgn);
# 1573
{struct Cyc_Tcenv_Tenv*_tmp2F8=Cyc_Tcenv_tc_init(tc_rgn);
# 1574
Cyc_Tc_tc(tc_rgn,_tmp2F8,1,_tmp2C8);}
# 1573
;_pop_region(tc_rgn);}
# 1572
;_pop_handler();}else{void*_tmp2F5=(void*)_exn_thrown;void*_tmp2FA=_tmp2F5;
_LL1D7:;_LL1D8:
# 1578
{struct Cyc_NO_SUPPORT_exn_struct _tmp676;const char*_tmp675;struct Cyc_NO_SUPPORT_exn_struct*
_tmp674;(int)_throw((void*)((_tmp674=_cycalloc(sizeof(*_tmp674)),((_tmp674[0]=((
_tmp676.tag=Cyc_NO_SUPPORT,((_tmp676.f1=((_tmp675="can't typecheck acquired declarations",
_tag_dyneither(_tmp675,sizeof(char),38))),_tmp676)))),_tmp674)))));}
# 1579
goto _LL1D6;_LL1D9:;_LL1DA:(void)_throw(_tmp2FA);_LL1D6:;}};}
# 1583
{struct Cyc_List_List*_tmp300;struct Cyc_List_List*_tmp301;struct _tuple25 _tmp677;
struct _tuple25 _tmp2FF=(_tmp677.f1=_tmp2C8,((_tmp677.f2=_tmp2C9,_tmp677)));
_tmp300=_tmp2FF.f1;_tmp301=_tmp2FF.f2;for(0;
# 1584
_tmp300 != 0  && _tmp301 != 0;(_tmp300=_tmp300->tl,_tmp301=_tmp301->tl)){
# 1585
struct Cyc_Absyn_Decl*_tmp302=(struct Cyc_Absyn_Decl*)_tmp300->hd;
# 1586
struct _dyneither_ptr*_tmp303=(struct _dyneither_ptr*)_tmp301->hd;
# 1587
int _tmp304=0;
# 1588
if(!((unsigned int)_tmp303))
# 1589
_tmp304=1;
# 1591
if((unsigned int)_tmp303){
# 1592
{const char*_tmp67B;void*_tmp67A[1];struct Cyc_String_pa_PrintArg_struct _tmp679;
ifdefmacro=((_tmp679.tag=0,((_tmp679.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp303),((_tmp67A[0]=& _tmp679,Cyc_aprintf(((_tmp67B="_%s_def_",
_tag_dyneither(_tmp67B,sizeof(char),9))),_tag_dyneither(_tmp67A,sizeof(void*),1))))))));}
# 1593
{const char*_tmp67F;void*_tmp67E[1];struct Cyc_String_pa_PrintArg_struct _tmp67D;(
_tmp67D.tag=0,((_tmp67D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp67E[0]=& _tmp67D,Cyc_fprintf(out_file,((_tmp67F="#ifndef %s\n",
_tag_dyneither(_tmp67F,sizeof(char),12))),_tag_dyneither(_tmp67E,sizeof(void*),1)))))));}
# 1594
{const char*_tmp683;void*_tmp682[1];struct Cyc_String_pa_PrintArg_struct _tmp681;(
_tmp681.tag=0,((_tmp681.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp682[0]=& _tmp681,Cyc_fprintf(out_file,((_tmp683="#define %s\n",
_tag_dyneither(_tmp683,sizeof(char),12))),_tag_dyneither(_tmp682,sizeof(void*),1)))))));}
# 1596
{struct Cyc_Absyn_Decl*_tmp684[1];Cyc_Absynpp_decllist2file(((_tmp684[0]=_tmp302,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp684,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}{
# 1597
const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(out_file,((_tmp687="#endif\n",
_tag_dyneither(_tmp687,sizeof(char),8))),_tag_dyneither(_tmp686,sizeof(void*),0)));};}
else{
# 1601
struct Cyc_Absyn_Decl*_tmp688[1];Cyc_Absynpp_decllist2file(((_tmp688[0]=_tmp302,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp688,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}
# 1606
maybe=Cyc_fopen(_tmp22E,(const char*)"r");
# 1607
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_exn_struct _tmp699;const char*
_tmp698;void*_tmp697[1];const char*_tmp696;struct Cyc_String_pa_PrintArg_struct
_tmp695;struct Cyc_NO_SUPPORT_exn_struct*_tmp694;(int)_throw((void*)((_tmp694=
_cycalloc(sizeof(*_tmp694)),((_tmp694[0]=((_tmp699.tag=Cyc_NO_SUPPORT,((_tmp699.f1=(
struct _dyneither_ptr)((_tmp695.tag=0,((_tmp695.f1=(struct _dyneither_ptr)((
_tmp696=_tmp22E,_tag_dyneither(_tmp696,sizeof(char),_get_zero_arr_size_char((
void*)_tmp696,1)))),((_tmp697[0]=& _tmp695,Cyc_aprintf(((_tmp698="can't open macrosfile %s",
_tag_dyneither(_tmp698,sizeof(char),25))),_tag_dyneither(_tmp697,sizeof(void*),1)))))))),
_tmp699)))),_tmp694)))));}
# 1609
in_file=(struct Cyc___cycFILE*)maybe;
# 1610
_tmp27B=Cyc_Lexing_from_file(in_file);{
# 1611
struct _tuple21*entry2;
# 1612
while((entry2=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(
_tmp27B))!= 0){
# 1613
struct _tuple21 _tmp319;struct _dyneither_ptr _tmp31A;struct _dyneither_ptr*_tmp31B;
struct _tuple21*_tmp318=(struct _tuple21*)_check_null(entry2);_tmp319=*_tmp318;
_tmp31A=_tmp319.f1;_tmp31B=_tmp319.f2;
# 1614
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp28B,_tmp31B)){
# 1615
{const char*_tmp69D;void*_tmp69C[1];struct Cyc_String_pa_PrintArg_struct _tmp69B;(
_tmp69B.tag=0,((_tmp69B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp31B),((
_tmp69C[0]=& _tmp69B,Cyc_fprintf(out_file,((_tmp69D="#ifndef %s\n",_tag_dyneither(
_tmp69D,sizeof(char),12))),_tag_dyneither(_tmp69C,sizeof(void*),1)))))));}
# 1616
{const char*_tmp6A1;void*_tmp6A0[1];struct Cyc_String_pa_PrintArg_struct _tmp69F;(
_tmp69F.tag=0,((_tmp69F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp31A),((
_tmp6A0[0]=& _tmp69F,Cyc_fprintf(out_file,((_tmp6A1="%s\n",_tag_dyneither(_tmp6A1,
sizeof(char),4))),_tag_dyneither(_tmp6A0,sizeof(void*),1)))))));}{
# 1617
const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(out_file,((_tmp6A4="#endif\n",
_tag_dyneither(_tmp6A4,sizeof(char),8))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));};}}
# 1620
Cyc_fclose(in_file);
# 1621
if(Cyc_mode != Cyc_FINISH)remove(_tmp22E);
# 1623
if(hstubs != 0){
# 1624
struct Cyc_List_List*_tmp324=hstubs;for(0;_tmp324 != 0;_tmp324=_tmp324->tl){
# 1625
struct _tuple22 _tmp326;struct _dyneither_ptr _tmp327;struct _dyneither_ptr _tmp328;
struct _tuple22*_tmp325=(struct _tuple22*)_tmp324->hd;_tmp326=*_tmp325;_tmp327=
_tmp326.f1;_tmp328=_tmp326.f2;{
# 1626
struct _dyneither_ptr*_tmp6A5;if((char*)_tmp328.curr != (char*)(_tag_dyneither(0,
0,0)).curr  && (
# 1627
(char*)_tmp327.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp6A5=_cycalloc(
sizeof(*_tmp6A5)),((_tmp6A5[0]=_tmp327,_tmp6A5)))))))
# 1629
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp328,sizeof(char),1),out_file);
else{
# 1631
const char*_tmp6A9;void*_tmp6A8[1];struct Cyc_String_pa_PrintArg_struct _tmp6A7;(
_tmp6A7.tag=0,((_tmp6A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp327),((
_tmp6A8[0]=& _tmp6A7,Cyc_log(((_tmp6A9="%s is not supported on this platform\n",
_tag_dyneither(_tmp6A9,sizeof(char),38))),_tag_dyneither(_tmp6A8,sizeof(void*),1)))))));}};}}
# 1634
{const char*_tmp6AC;void*_tmp6AB;(_tmp6AB=0,Cyc_fprintf(out_file,((_tmp6AC="#endif\n",
_tag_dyneither(_tmp6AC,sizeof(char),8))),_tag_dyneither(_tmp6AB,sizeof(void*),0)));}
# 1635
if(Cyc_do_setjmp){int _tmp32F=0;_npop_handler(0);return _tmp32F;}else{
# 1636
Cyc_fclose(out_file);}
# 1639
if(cstubs != 0){
# 1640
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
# 1641
struct Cyc_List_List*_tmp330=cstubs;for(0;_tmp330 != 0;_tmp330=_tmp330->tl){
# 1642
struct _tuple22 _tmp332;struct _dyneither_ptr _tmp333;struct _dyneither_ptr _tmp334;
struct _tuple22*_tmp331=(struct _tuple22*)_tmp330->hd;_tmp332=*_tmp331;_tmp333=
_tmp332.f1;_tmp334=_tmp332.f2;{
# 1643
struct _dyneither_ptr*_tmp6AD;if((char*)_tmp334.curr != (char*)(_tag_dyneither(0,
0,0)).curr  && (
# 1644
(char*)_tmp333.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp6AD=_cycalloc(
sizeof(*_tmp6AD)),((_tmp6AD[0]=_tmp333,_tmp6AD)))))))
# 1645
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp334,sizeof(char),1),out_file);};}};}
# 1650
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1652
{const char*_tmp6B3;void*_tmp6B2[1];const char*_tmp6B1;struct Cyc_String_pa_PrintArg_struct
_tmp6B0;(_tmp6B0.tag=0,((_tmp6B0.f1=(struct _dyneither_ptr)((_tmp6B1=filename,
_tag_dyneither(_tmp6B1,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B1,1)))),((
_tmp6B2[0]=& _tmp6B0,Cyc_fprintf(out_file,((_tmp6B3="#include <%s>\n\n",
_tag_dyneither(_tmp6B3,sizeof(char),16))),_tag_dyneither(_tmp6B2,sizeof(void*),1)))))));}
# 1653
if(cycstubs != 0){
# 1654
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1655
{struct Cyc_List_List*_tmp33A=cycstubs;for(0;_tmp33A != 0;_tmp33A=_tmp33A->tl){
# 1656
struct _tuple22 _tmp33C;struct _dyneither_ptr _tmp33D;struct _dyneither_ptr _tmp33E;
struct _tuple22*_tmp33B=(struct _tuple22*)_tmp33A->hd;_tmp33C=*_tmp33B;_tmp33D=
_tmp33C.f1;_tmp33E=_tmp33C.f2;{
# 1657
struct _dyneither_ptr*_tmp6B4;if((char*)_tmp33E.curr != (char*)(_tag_dyneither(0,
0,0)).curr  && (
# 1658
(char*)_tmp33D.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp6B4=_cycalloc(
sizeof(*_tmp6B4)),((_tmp6B4[0]=_tmp33D,_tmp6B4)))))))
# 1659
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp33E,sizeof(char),1),out_file);};}}{
# 1661
const char*_tmp6B7;void*_tmp6B6;(_tmp6B6=0,Cyc_fprintf(out_file,((_tmp6B7="\n",
_tag_dyneither(_tmp6B7,sizeof(char),2))),_tag_dyneither(_tmp6B6,sizeof(void*),0)));};}{
# 1664
int _tmp342=0;_npop_handler(0);return _tmp342;};};};};};};};};
# 1343
;_pop_handler();}else{void*_tmp232=(void*)_exn_thrown;void*_tmp348=_tmp232;
struct _dyneither_ptr _tmp34A;struct _dyneither_ptr _tmp34D;struct _dyneither_ptr
_tmp34F;struct _dyneither_ptr _tmp352;_LL1DC: {struct Cyc_Core_Impossible_exn_struct*
_tmp349=(struct Cyc_Core_Impossible_exn_struct*)_tmp348;if(_tmp349->tag != Cyc_Core_Impossible)
goto _LL1DE;else{_tmp34A=_tmp349->f1;}}_LL1DD:
# 1668
{const char*_tmp6BB;void*_tmp6BA[1];struct Cyc_String_pa_PrintArg_struct _tmp6B9;(
_tmp6B9.tag=0,((_tmp6B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34A),((
_tmp6BA[0]=& _tmp6B9,Cyc_fprintf(Cyc_stderr,((_tmp6BB="Got Core::Impossible(%s)\n",
_tag_dyneither(_tmp6BB,sizeof(char),26))),_tag_dyneither(_tmp6BA,sizeof(void*),1)))))));}
goto _LL1DB;_LL1DE: {struct Cyc_Dict_Absent_exn_struct*_tmp34B=(struct Cyc_Dict_Absent_exn_struct*)
_tmp348;if(_tmp34B->tag != Cyc_Dict_Absent)goto _LL1E0;}_LL1DF:
# 1670
{const char*_tmp6BE;void*_tmp6BD;(_tmp6BD=0,Cyc_fprintf(Cyc_stderr,((_tmp6BE="Got Dict::Absent\n",
_tag_dyneither(_tmp6BE,sizeof(char),18))),_tag_dyneither(_tmp6BD,sizeof(void*),0)));}
goto _LL1DB;_LL1E0: {struct Cyc_Core_Failure_exn_struct*_tmp34C=(struct Cyc_Core_Failure_exn_struct*)
_tmp348;if(_tmp34C->tag != Cyc_Core_Failure)goto _LL1E2;else{_tmp34D=_tmp34C->f1;}}
_LL1E1:
# 1672
{const char*_tmp6C2;void*_tmp6C1[1];struct Cyc_String_pa_PrintArg_struct _tmp6C0;(
_tmp6C0.tag=0,((_tmp6C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34D),((
_tmp6C1[0]=& _tmp6C0,Cyc_fprintf(Cyc_stderr,((_tmp6C2="Got Core::Failure(%s)\n",
_tag_dyneither(_tmp6C2,sizeof(char),23))),_tag_dyneither(_tmp6C1,sizeof(void*),1)))))));}
goto _LL1DB;_LL1E2: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp34E=(struct
Cyc_Core_Invalid_argument_exn_struct*)_tmp348;if(_tmp34E->tag != Cyc_Core_Invalid_argument)
goto _LL1E4;else{_tmp34F=_tmp34E->f1;}}_LL1E3:
# 1674
{const char*_tmp6C6;void*_tmp6C5[1];struct Cyc_String_pa_PrintArg_struct _tmp6C4;(
_tmp6C4.tag=0,((_tmp6C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34F),((
_tmp6C5[0]=& _tmp6C4,Cyc_fprintf(Cyc_stderr,((_tmp6C6="Got Invalid_argument(%s)\n",
_tag_dyneither(_tmp6C6,sizeof(char),26))),_tag_dyneither(_tmp6C5,sizeof(void*),1)))))));}
goto _LL1DB;_LL1E4: {struct Cyc_Core_Not_found_exn_struct*_tmp350=(struct Cyc_Core_Not_found_exn_struct*)
_tmp348;if(_tmp350->tag != Cyc_Core_Not_found)goto _LL1E6;}_LL1E5:
# 1676
{const char*_tmp6C9;void*_tmp6C8;(_tmp6C8=0,Cyc_fprintf(Cyc_stderr,((_tmp6C9="Got Not_found\n",
_tag_dyneither(_tmp6C9,sizeof(char),15))),_tag_dyneither(_tmp6C8,sizeof(void*),0)));}
goto _LL1DB;_LL1E6: {struct Cyc_NO_SUPPORT_exn_struct*_tmp351=(struct Cyc_NO_SUPPORT_exn_struct*)
_tmp348;if(_tmp351->tag != Cyc_NO_SUPPORT)goto _LL1E8;else{_tmp352=_tmp351->f1;}}
_LL1E7:
# 1678
{const char*_tmp6CD;void*_tmp6CC[1];struct Cyc_String_pa_PrintArg_struct _tmp6CB;(
_tmp6CB.tag=0,((_tmp6CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp352),((
_tmp6CC[0]=& _tmp6CB,Cyc_fprintf(Cyc_stderr,((_tmp6CD="No support because %s\n",
_tag_dyneither(_tmp6CD,sizeof(char),23))),_tag_dyneither(_tmp6CC,sizeof(void*),1)))))));}
goto _LL1DB;_LL1E8:;_LL1E9:
# 1680
{const char*_tmp6D0;void*_tmp6CF;(_tmp6CF=0,Cyc_fprintf(Cyc_stderr,((_tmp6D0="Got unknown exception\n",
_tag_dyneither(_tmp6D0,sizeof(char),23))),_tag_dyneither(_tmp6CF,sizeof(void*),0)));}
goto _LL1DB;_LL1EA:;_LL1EB:(void)_throw(_tmp348);_LL1DB:;}};}
# 1685
maybe=Cyc_fopen(filename,(const char*)"w");
# 1686
if(!((unsigned int)maybe)){
# 1687
{const char*_tmp6D6;void*_tmp6D5[1];const char*_tmp6D4;struct Cyc_String_pa_PrintArg_struct
_tmp6D3;(_tmp6D3.tag=0,((_tmp6D3.f1=(struct _dyneither_ptr)((_tmp6D4=filename,
_tag_dyneither(_tmp6D4,sizeof(char),_get_zero_arr_size_char((void*)_tmp6D4,1)))),((
_tmp6D5[0]=& _tmp6D3,Cyc_fprintf(Cyc_stderr,((_tmp6D6="Error: could not create file %s\n",
_tag_dyneither(_tmp6D6,sizeof(char),33))),_tag_dyneither(_tmp6D5,sizeof(void*),1)))))));}
# 1688
return 1;}
# 1690
out_file=(struct Cyc___cycFILE*)maybe;
# 1691
{const char*_tmp6DC;void*_tmp6DB[1];const char*_tmp6DA;struct Cyc_String_pa_PrintArg_struct
_tmp6D9;(_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)(
# 1693
(_tmp6DA=filename,_tag_dyneither(_tmp6DA,sizeof(char),_get_zero_arr_size_char((
void*)_tmp6DA,1)))),((_tmp6DB[0]=& _tmp6D9,Cyc_fprintf(out_file,((_tmp6DC="#error -- %s is not supported on this platform\n",
_tag_dyneither(_tmp6DC,sizeof(char),48))),_tag_dyneither(_tmp6DB,sizeof(void*),1)))))));}
# 1694
Cyc_fclose(out_file);
# 1695
{const char*_tmp6E2;void*_tmp6E1[1];const char*_tmp6E0;struct Cyc_String_pa_PrintArg_struct
_tmp6DF;(_tmp6DF.tag=0,((_tmp6DF.f1=(struct _dyneither_ptr)(
# 1696
(_tmp6E0=filename,_tag_dyneither(_tmp6E0,sizeof(char),_get_zero_arr_size_char((
void*)_tmp6E0,1)))),((_tmp6E1[0]=& _tmp6DF,Cyc_fprintf(Cyc_stderr,((_tmp6E2="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp6E2,sizeof(char),52))),_tag_dyneither(_tmp6E1,sizeof(void*),1)))))));}
# 1697
{const char*_tmp6E5;void*_tmp6E4;(_tmp6E4=0,Cyc_log(((_tmp6E5="Not supported on this platform\n",
_tag_dyneither(_tmp6E5,sizeof(char),32))),_tag_dyneither(_tmp6E4,sizeof(void*),0)));}
# 1700
remove(_tmp22E);
# 1701
remove(_tmp22F);
# 1702
remove(_tmp230);
# 1704
return 0;};}int Cyc_process_specfile(const char*file,const char*dir);static void
_tmp6F0(unsigned int*_tmp6EF,unsigned int*_tmp6EE,char**_tmp6ED){for(*_tmp6EF=0;*
_tmp6EF < *_tmp6EE;(*_tmp6EF)++){(*_tmp6ED)[*_tmp6EF]='\000';}}
# 1708
int Cyc_process_specfile(const char*file,const char*dir){
# 1709
struct Cyc___cycFILE*_tmp388=Cyc_fopen(file,(const char*)"r");
# 1710
if(!((unsigned int)_tmp388)){
# 1711
{const char*_tmp6EB;void*_tmp6EA[1];const char*_tmp6E9;struct Cyc_String_pa_PrintArg_struct
_tmp6E8;(_tmp6E8.tag=0,((_tmp6E8.f1=(struct _dyneither_ptr)((_tmp6E9=file,
_tag_dyneither(_tmp6E9,sizeof(char),_get_zero_arr_size_char((void*)_tmp6E9,1)))),((
_tmp6EA[0]=& _tmp6E8,Cyc_fprintf(Cyc_stderr,((_tmp6EB="Error: could not open %s\n",
_tag_dyneither(_tmp6EB,sizeof(char),26))),_tag_dyneither(_tmp6EA,sizeof(void*),1)))))));}
# 1712
return 1;}{
# 1714
struct Cyc___cycFILE*_tmp38D=(struct Cyc___cycFILE*)_tmp388;
# 1718
char*_tmp6F5;unsigned int _tmp6F4;char*_tmp6F3;unsigned int _tmp6F2;unsigned int
_tmp6F1;struct _dyneither_ptr buf=(_tmp6F5=((_tmp6F4=(unsigned int)1024,((_tmp6F3=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6F4 + 1)),((((_tmp6F2=
_tmp6F4,((_tmp6F0(& _tmp6F1,& _tmp6F2,& _tmp6F3),_tmp6F3[_tmp6F2]=(char)0)))),
_tmp6F3)))))),_tag_dyneither(_tmp6F5,sizeof(char),_get_zero_arr_size_char((void*)
_tmp6F5,(unsigned int)1024 + 1)));
# 1719
struct _dyneither_ptr _tmp38E=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
# 1720
if(Cyc_mode != Cyc_GATHERSCRIPT){
# 1721
if(chdir(dir)){
# 1722
{const char*_tmp6FB;void*_tmp6FA[1];const char*_tmp6F9;struct Cyc_String_pa_PrintArg_struct
_tmp6F8;(_tmp6F8.tag=0,((_tmp6F8.f1=(struct _dyneither_ptr)((_tmp6F9=dir,
_tag_dyneither(_tmp6F9,sizeof(char),_get_zero_arr_size_char((void*)_tmp6F9,1)))),((
_tmp6FA[0]=& _tmp6F8,Cyc_fprintf(Cyc_stderr,((_tmp6FB="Error: can't change directory to %s\n",
_tag_dyneither(_tmp6FB,sizeof(char),37))),_tag_dyneither(_tmp6FA,sizeof(void*),1)))))));}
# 1723
return 1;}}
# 1726
if(Cyc_mode == Cyc_GATHER){
# 1728
struct _dyneither_ptr _tmp393=Cstring_to_string(Ccomp);
# 1729
const char*_tmp6FF;void*_tmp6FE[1];struct Cyc_String_pa_PrintArg_struct _tmp6FD;
system((const char*)_untag_dyneither_ptr(((_tmp6FD.tag=0,((_tmp6FD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp393),((_tmp6FE[0]=& _tmp6FD,Cyc_aprintf(((
_tmp6FF="echo | %s -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp6FF,sizeof(
char),36))),_tag_dyneither(_tmp6FE,sizeof(void*),1)))))))),sizeof(char),1));}{
# 1732
struct Cyc_Lexing_lexbuf*_tmp397=Cyc_Lexing_from_file(_tmp38D);
# 1733
struct _tuple23*entry;
# 1734
while((entry=((struct _tuple23*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(
_tmp397))!= 0){
# 1735
struct _tuple23 _tmp399;struct _dyneither_ptr _tmp39A;struct Cyc_List_List*_tmp39B;
struct Cyc_List_List*_tmp39C;struct Cyc_List_List*_tmp39D;struct Cyc_List_List*
_tmp39E;struct Cyc_List_List*_tmp39F;struct _tuple23*_tmp398=(struct _tuple23*)
_check_null(entry);_tmp399=*_tmp398;_tmp39A=_tmp399.f1;_tmp39B=_tmp399.f2;
_tmp39C=_tmp399.f3;_tmp39D=_tmp399.f4;_tmp39E=_tmp399.f5;_tmp39F=_tmp399.f6;
# 1737
if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp39A,sizeof(char),1),
_tmp39B,_tmp39C,_tmp39D,_tmp39E,_tmp39F))
# 1739
return 1;}
# 1741
Cyc_fclose(_tmp38D);
# 1743
if(Cyc_mode != Cyc_GATHERSCRIPT){
# 1744
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp38E,sizeof(char),1)))){
# 1745
{const char*_tmp703;void*_tmp702[1];struct Cyc_String_pa_PrintArg_struct _tmp701;(
_tmp701.tag=0,((_tmp701.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38E),((
_tmp702[0]=& _tmp701,Cyc_fprintf(Cyc_stderr,((_tmp703="Error: could not change directory to %s\n",
_tag_dyneither(_tmp703,sizeof(char),41))),_tag_dyneither(_tmp702,sizeof(void*),1)))))));}
# 1746
return 1;}}
# 1749
return 0;};};}int Cyc_process_setjmp(const char*dir);static void _tmp708(
unsigned int*_tmp707,unsigned int*_tmp706,char**_tmp705){for(*_tmp707=0;*_tmp707
< *_tmp706;(*_tmp707)++){(*_tmp705)[*_tmp707]='\000';}}
# 1753
int Cyc_process_setjmp(const char*dir){
# 1756
char*_tmp70D;unsigned int _tmp70C;char*_tmp70B;unsigned int _tmp70A;unsigned int
_tmp709;struct _dyneither_ptr buf=(_tmp70D=((_tmp70C=(unsigned int)1024,((_tmp70B=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp70C + 1)),((((_tmp70A=
_tmp70C,((_tmp708(& _tmp709,& _tmp70A,& _tmp70B),_tmp70B[_tmp70A]=(char)0)))),
_tmp70B)))))),_tag_dyneither(_tmp70D,sizeof(char),_get_zero_arr_size_char((void*)
_tmp70D,(unsigned int)1024 + 1)));
# 1757
struct _dyneither_ptr _tmp3A7=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
# 1758
if(chdir(dir)){
# 1759
{const char*_tmp713;void*_tmp712[1];const char*_tmp711;struct Cyc_String_pa_PrintArg_struct
_tmp710;(_tmp710.tag=0,((_tmp710.f1=(struct _dyneither_ptr)((_tmp711=dir,
_tag_dyneither(_tmp711,sizeof(char),_get_zero_arr_size_char((void*)_tmp711,1)))),((
_tmp712[0]=& _tmp710,Cyc_fprintf(Cyc_stderr,((_tmp713="Error: can't change directory to %s\n",
_tag_dyneither(_tmp713,sizeof(char),37))),_tag_dyneither(_tmp712,sizeof(void*),1)))))));}
# 1760
return 1;}
# 1762
{struct _tuple22*_tmp71D;const char*_tmp71C;const char*_tmp71B;struct _tuple22*
_tmp71A[1];struct _dyneither_ptr*_tmp714[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp714[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp714,sizeof(struct _dyneither_ptr*),1)))),0,(
# 1763
(_tmp71A[0]=((_tmp71D=_cycalloc(sizeof(*_tmp71D)),((_tmp71D->f1=((_tmp71C="setjmp",
_tag_dyneither(_tmp71C,sizeof(char),7))),((_tmp71D->f2=((_tmp71B="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp71B,sizeof(char),29))),_tmp71D)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp71A,sizeof(struct
_tuple22*),1)))),0,0))
# 1765
return 1;}
# 1766
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp3A7,sizeof(char),1)))){
# 1767
{const char*_tmp721;void*_tmp720[1];struct Cyc_String_pa_PrintArg_struct _tmp71F;(
_tmp71F.tag=0,((_tmp71F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A7),((
_tmp720[0]=& _tmp71F,Cyc_fprintf(Cyc_stderr,((_tmp721="Error: could not change directory to %s\n",
_tag_dyneither(_tmp721,sizeof(char),41))),_tag_dyneither(_tmp720,sizeof(void*),1)))))));}
# 1768
return 1;}
# 1770
return 0;}static char _tmp3B9[13]="BUILDLIB.OUT";
# 1774
static struct _dyneither_ptr Cyc_output_dir={_tmp3B9,_tmp3B9,_tmp3B9 + 13};
# 1775
static void Cyc_set_output_dir(struct _dyneither_ptr s){
# 1776
Cyc_output_dir=s;}
# 1778
static struct Cyc_List_List*Cyc_spec_files=0;
# 1779
static void Cyc_add_spec_file(struct _dyneither_ptr s){
# 1780
struct Cyc_List_List*_tmp722;Cyc_spec_files=((_tmp722=_cycalloc(sizeof(*_tmp722)),((
_tmp722->hd=(const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp722->tl=Cyc_spec_files,
_tmp722))))));}
# 1782
static void Cyc_set_GATHER(){
# 1783
Cyc_mode=Cyc_GATHER;}
# 1785
static void Cyc_set_GATHERSCRIPT(){
# 1786
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1788
static void Cyc_set_FINISH(){
# 1789
Cyc_mode=Cyc_FINISH;}
# 1791
static void Cyc_add_cpparg(struct _dyneither_ptr s){
# 1792
struct _dyneither_ptr*_tmp725;struct Cyc_List_List*_tmp724;Cyc_cppargs=((_tmp724=
_cycalloc(sizeof(*_tmp724)),((_tmp724->hd=((_tmp725=_cycalloc(sizeof(*_tmp725)),((
_tmp725[0]=s,_tmp725)))),((_tmp724->tl=Cyc_cppargs,_tmp724))))));}
# 1794
static int Cyc_badparse=0;
# 1795
static void Cyc_unsupported_option(struct _dyneither_ptr s){
# 1796
{const char*_tmp729;void*_tmp728[1];struct Cyc_String_pa_PrintArg_struct _tmp727;(
_tmp727.tag=0,((_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp728[0]=& _tmp727,Cyc_fprintf(Cyc_stderr,((_tmp729="Unsupported option %s\n",
_tag_dyneither(_tmp729,sizeof(char),23))),_tag_dyneither(_tmp728,sizeof(void*),1)))))));}
# 1797
Cyc_badparse=1;}
# 1803
void GC_blacklist_warn_clear();struct _tuple26{struct _dyneither_ptr f1;int f2;
struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 1804
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1805
GC_blacklist_warn_clear();{
# 1807
struct _tuple26*_tmp796;const char*_tmp795;const char*_tmp794;struct Cyc_Arg_String_spec_Arg_Spec_struct
_tmp793;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp792;const char*_tmp791;
struct _tuple26*_tmp790;const char*_tmp78F;const char*_tmp78E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct
_tmp78D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp78C;const char*_tmp78B;struct
_tuple26*_tmp78A;const char*_tmp789;const char*_tmp788;struct Cyc_Arg_Unit_spec_Arg_Spec_struct
_tmp787;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp786;const char*_tmp785;struct
_tuple26*_tmp784;const char*_tmp783;const char*_tmp782;struct Cyc_Arg_Unit_spec_Arg_Spec_struct
_tmp781;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp780;const char*_tmp77F;struct
_tuple26*_tmp77E;const char*_tmp77D;const char*_tmp77C;struct Cyc_Arg_Set_spec_Arg_Spec_struct
_tmp77B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp77A;const char*_tmp779;struct
_tuple26*_tmp778;const char*_tmp777;const char*_tmp776;struct Cyc_Arg_Flag_spec_Arg_Spec_struct
_tmp775;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp774;const char*_tmp773;struct
_tuple26*_tmp772[6];struct Cyc_List_List*options=
# 1808
(_tmp772[5]=(
# 1826
(_tmp778=_cycalloc(sizeof(*_tmp778)),((_tmp778->f1=((_tmp777="-",_tag_dyneither(
_tmp777,sizeof(char),2))),((_tmp778->f2=1,((_tmp778->f3=((_tmp776="",
_tag_dyneither(_tmp776,sizeof(char),1))),((_tmp778->f4=(void*)(
# 1827
(_tmp774=_cycalloc(sizeof(*_tmp774)),((_tmp774[0]=((_tmp775.tag=1,((_tmp775.f1=
Cyc_add_cpparg,_tmp775)))),_tmp774)))),((_tmp778->f5=(
# 1828
(_tmp773="",_tag_dyneither(_tmp773,sizeof(char),1))),_tmp778)))))))))))),((
_tmp772[4]=(
# 1820
(_tmp77E=_cycalloc(sizeof(*_tmp77E)),((_tmp77E->f1=((_tmp77D="-setjmp",
_tag_dyneither(_tmp77D,sizeof(char),8))),((_tmp77E->f2=0,((_tmp77E->f3=((_tmp77C="",
_tag_dyneither(_tmp77C,sizeof(char),1))),((_tmp77E->f4=(void*)(
# 1821
(_tmp77A=_cycalloc(sizeof(*_tmp77A)),((_tmp77A[0]=((_tmp77B.tag=3,((_tmp77B.f1=&
Cyc_do_setjmp,_tmp77B)))),_tmp77A)))),((_tmp77E->f5=(
# 1825
(_tmp779="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp779,sizeof(char),186))),_tmp77E)))))))))))),((_tmp772[3]=(
# 1817
(_tmp784=_cycalloc(sizeof(*_tmp784)),((_tmp784->f1=((_tmp783="-finish",
_tag_dyneither(_tmp783,sizeof(char),8))),((_tmp784->f2=0,((_tmp784->f3=((_tmp782="",
_tag_dyneither(_tmp782,sizeof(char),1))),((_tmp784->f4=(void*)(
# 1818
(_tmp780=_cycalloc(sizeof(*_tmp780)),((_tmp780[0]=((_tmp781.tag=0,((_tmp781.f1=
Cyc_set_FINISH,_tmp781)))),_tmp780)))),((_tmp784->f5=(
# 1819
(_tmp77F="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp77F,sizeof(char),57))),_tmp784)))))))))))),((_tmp772[2]=(
# 1814
(_tmp78A=_cycalloc(sizeof(*_tmp78A)),((_tmp78A->f1=((_tmp789="-gatherscript",
_tag_dyneither(_tmp789,sizeof(char),14))),((_tmp78A->f2=0,((_tmp78A->f3=((
_tmp788="",_tag_dyneither(_tmp788,sizeof(char),1))),((_tmp78A->f4=(void*)(
# 1815
(_tmp786=_cycalloc(sizeof(*_tmp786)),((_tmp786[0]=((_tmp787.tag=0,((_tmp787.f1=
Cyc_set_GATHERSCRIPT,_tmp787)))),_tmp786)))),((_tmp78A->f5=(
# 1816
(_tmp785="Produce a script to gather C library info",_tag_dyneither(_tmp785,
sizeof(char),42))),_tmp78A)))))))))))),((_tmp772[1]=(
# 1811
(_tmp790=_cycalloc(sizeof(*_tmp790)),((_tmp790->f1=((_tmp78F="-gather",
_tag_dyneither(_tmp78F,sizeof(char),8))),((_tmp790->f2=0,((_tmp790->f3=((_tmp78E="",
_tag_dyneither(_tmp78E,sizeof(char),1))),((_tmp790->f4=(void*)(
# 1812
(_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C[0]=((_tmp78D.tag=0,((_tmp78D.f1=
Cyc_set_GATHER,_tmp78D)))),_tmp78C)))),((_tmp790->f5=(
# 1813
(_tmp78B="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp78B,sizeof(char),56))),_tmp790)))))))))))),((_tmp772[0]=(
# 1808
(_tmp796=_cycalloc(sizeof(*_tmp796)),((_tmp796->f1=((_tmp795="-d",
_tag_dyneither(_tmp795,sizeof(char),3))),((_tmp796->f2=0,((_tmp796->f3=((_tmp794=" <file>",
_tag_dyneither(_tmp794,sizeof(char),8))),((_tmp796->f4=(void*)(
# 1809
(_tmp792=_cycalloc(sizeof(*_tmp792)),((_tmp792[0]=((_tmp793.tag=5,((_tmp793.f1=
Cyc_set_output_dir,_tmp793)))),_tmp792)))),((_tmp796->f5=(
# 1810
(_tmp791="Set the output directory to <file>",_tag_dyneither(_tmp791,sizeof(
char),35))),_tmp796)))))))))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp772,sizeof(struct _tuple26*),6)))))))))))));
# 1830
{const char*_tmp797;Cyc_Arg_parse(options,Cyc_add_spec_file,((_tmp797="Options:",
_tag_dyneither(_tmp797,sizeof(char),9))),argv);}
# 1831
if((((Cyc_badparse  || 
# 1832
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
# 1833
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
# 1834
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
# 1835
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
# 1836
{const char*_tmp798;Cyc_Arg_usage(options,(
# 1838
(_tmp798="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp798,sizeof(char),59))));}
# 1839
return 1;}
# 1842
if(Cyc_mode == Cyc_GATHERSCRIPT){
# 1843
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");
# 1844
if(!((unsigned int)Cyc_script_file)){
# 1845
{const char*_tmp79B;void*_tmp79A;(_tmp79A=0,Cyc_fprintf(Cyc_stderr,((_tmp79B="Could not create file BUILDLIB.sh\n",
_tag_dyneither(_tmp79B,sizeof(char),35))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}
# 1846
exit(1);}
# 1848
{const char*_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_prscript(((_tmp79E="#!/bin/sh\n",
_tag_dyneither(_tmp79E,sizeof(char),11))),_tag_dyneither(_tmp79D,sizeof(void*),0)));}{
# 1849
const char*_tmp7A1;void*_tmp7A0;(_tmp7A0=0,Cyc_prscript(((_tmp7A1="GCC=\"gcc\"\n",
_tag_dyneither(_tmp7A1,sizeof(char),11))),_tag_dyneither(_tmp7A0,sizeof(void*),0)));};}
# 1853
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
# 1854
{const char*_tmp7A5;void*_tmp7A4[1];struct Cyc_String_pa_PrintArg_struct _tmp7A3;(
_tmp7A3.tag=0,((_tmp7A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp7A4[0]=& _tmp7A3,Cyc_fprintf(Cyc_stderr,((_tmp7A5="Error: could not create directory %s\n",
_tag_dyneither(_tmp7A5,sizeof(char),38))),_tag_dyneither(_tmp7A4,sizeof(void*),1)))))));}
# 1855
return 1;}
# 1858
if(Cyc_mode == Cyc_GATHERSCRIPT){
# 1859
{const char*_tmp7A9;void*_tmp7A8[1];struct Cyc_String_pa_PrintArg_struct _tmp7A7;(
_tmp7A7.tag=0,((_tmp7A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp7A8[0]=& _tmp7A7,Cyc_prscript(((_tmp7A9="cd %s\n",_tag_dyneither(_tmp7A9,
sizeof(char),7))),_tag_dyneither(_tmp7A8,sizeof(void*),1)))))));}{
# 1860
const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,Cyc_prscript(((_tmp7AC="echo | $GCC -E -dM - -o INITMACROS.h\n",
_tag_dyneither(_tmp7AC,sizeof(char),38))),_tag_dyneither(_tmp7AB,sizeof(void*),0)));};}
# 1863
if(!Cyc_gathering()){
# 1866
{const char*_tmp7AD;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(
Cyc_output_dir,((_tmp7AD="BUILDLIB.LOG",_tag_dyneither(_tmp7AD,sizeof(char),13)))),
sizeof(char),1),(const char*)"w");}
# 1867
if(!((unsigned int)Cyc_log_file)){
# 1868
{const char*_tmp7B1;void*_tmp7B0[1];struct Cyc_String_pa_PrintArg_struct _tmp7AF;(
_tmp7AF.tag=0,((_tmp7AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp7B0[0]=& _tmp7AF,Cyc_fprintf(Cyc_stderr,((_tmp7B1="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp7B1,sizeof(char),50))),_tag_dyneither(_tmp7B0,sizeof(void*),1)))))));}
# 1869
return 1;}
# 1872
if(!Cyc_do_setjmp){
# 1874
{const char*_tmp7B2;Cyc_cstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(
Cyc_Filename_concat(Cyc_output_dir,((_tmp7B2="cstubs.c",_tag_dyneither(_tmp7B2,
sizeof(char),9)))),sizeof(char),1),(const char*)"w");}
# 1875
if(!((unsigned int)Cyc_cstubs_file)){
# 1876
{const char*_tmp7B6;void*_tmp7B5[1];struct Cyc_String_pa_PrintArg_struct _tmp7B4;(
_tmp7B4.tag=0,((_tmp7B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp7B5[0]=& _tmp7B4,Cyc_fprintf(Cyc_stderr,((_tmp7B6="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp7B6,sizeof(char),50))),_tag_dyneither(_tmp7B5,sizeof(void*),1)))))));}
# 1877
return 1;}
# 1881
{const char*_tmp7B7;Cyc_cycstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(
Cyc_Filename_concat(Cyc_output_dir,((_tmp7B7="cycstubs.cyc",_tag_dyneither(
_tmp7B7,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}
# 1882
if(!((unsigned int)Cyc_cycstubs_file)){
# 1883
{const char*_tmp7BB;void*_tmp7BA[1];struct Cyc_String_pa_PrintArg_struct _tmp7B9;(
_tmp7B9.tag=0,((_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp7BA[0]=& _tmp7B9,Cyc_fprintf(Cyc_stderr,((_tmp7BB="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp7BB,sizeof(char),52))),_tag_dyneither(_tmp7BA,sizeof(void*),1)))))));}
# 1886
return 1;}{
# 1888
const char*_tmp7BE;void*_tmp7BD;(_tmp7BD=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp7BE="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp7BE,sizeof(char),32))),_tag_dyneither(_tmp7BD,sizeof(void*),0)));};}}{
# 1895
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
# 1896
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
# 1897
return 1;else{
# 1901
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
# 1902
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){
# 1903
{const char*_tmp7C1;void*_tmp7C0;(_tmp7C0=0,Cyc_fprintf(Cyc_stderr,((_tmp7C1="FATAL ERROR -- QUIT!\n",
_tag_dyneither(_tmp7C1,sizeof(char),22))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));}
# 1904
exit(1);}}}
# 1909
if(Cyc_mode == Cyc_GATHERSCRIPT)
# 1910
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 1912
if(!Cyc_gathering()){
# 1913
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
# 1914
if(!Cyc_do_setjmp){
# 1915
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
# 1916
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 1920
return 0;};};}
