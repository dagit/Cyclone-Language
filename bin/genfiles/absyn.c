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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
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
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 38
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 338
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 431
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 432
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 481
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
# 482
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
# 483
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 484
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
# 485
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
# 486
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 487
extern struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val;
# 488
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
# 489
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
# 490
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
# 491
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
# 492
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
# 493
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
# 494
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
# 495
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
# 496
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
# 497
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 524
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 525
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 529
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 530
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
# 531
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 686 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
# 721
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 748
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 883
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
# 884
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 898
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 905
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 906
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 907
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 910
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 913
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 918
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
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
# 933
void*Cyc_Absyn_compress_kb(void*);
# 934
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 940
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 941
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 943
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 945
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 947
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 949
extern void*Cyc_Absyn_empty_effect;
# 951
extern struct _tuple0*Cyc_Absyn_exn_name;
# 952
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 957
void*Cyc_Absyn_exn_typ();
# 959
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
# 960
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
# 962
void*Cyc_Absyn_string_typ(void*rgn);
# 963
void*Cyc_Absyn_const_string_typ(void*rgn);
# 965
void*Cyc_Absyn_file_typ();
# 967
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 968
extern void*Cyc_Absyn_bounds_one;
# 970
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 973
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 976
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 979
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
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
# 993
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);
# 995
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
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
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1066
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1067
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
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
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1077
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1087
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1093
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1094
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1098
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
# 1101
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
# 1102
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1103
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1104
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1111
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1114
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1117
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1120
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1125
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1131
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1141
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1143
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1146
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1150
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1154
int Cyc_Absyn_fntype_att(void*a);
# 1156
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1158
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1160
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1162
int Cyc_Absyn_is_union_type(void*);
# 1164
int Cyc_Absyn_is_nontagged_union_type(void*);
# 1166
int Cyc_Absyn_is_aggr_type(void*t);
# 1172
extern int Cyc_Absyn_porting_c_code;
# 1174
extern int Cyc_Absyn_no_regions;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 134
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 33 "absyn.cyc"
int Cyc_Cyclone_tovc_r=0;
# 35
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 44
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
# 45
for(0;ss1 != 0;ss1=ss1->tl){
# 46
if(ss2 == 0)return 1;{
# 47
int i=Cyc_strptrcmp((struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);
# 48
if(i != 0)return i;
# 49
ss2=ss2->tl;};}
# 51
if(ss2 != 0)return - 1;
# 52
return 0;}
# 54
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
# 55
if((int)vs1 == (int)vs2)return 0;
# 56
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 58
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
# 59
if(q1 == q2)return 0;{
# 60
struct _tuple0 _tmp1;union Cyc_Absyn_Nmspace _tmp2;struct _dyneither_ptr*_tmp3;struct _tuple0*_tmp0=q1;_tmp1=*_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;{
# 61
struct _tuple0 _tmp5;union Cyc_Absyn_Nmspace _tmp6;struct _dyneither_ptr*_tmp7;struct _tuple0*_tmp4=q2;_tmp5=*_tmp4;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{
# 62
int i=Cyc_strptrcmp(_tmp3,_tmp7);
# 63
if(i != 0)return i;{
# 64
struct _tuple12 _tmp246;struct _tuple12 _tmp9=(_tmp246.f1=_tmp2,((_tmp246.f2=_tmp6,_tmp246)));union Cyc_Absyn_Nmspace _tmpA;int _tmpB;union Cyc_Absyn_Nmspace _tmpC;int _tmpD;union Cyc_Absyn_Nmspace _tmpE;struct Cyc_List_List*_tmpF;union Cyc_Absyn_Nmspace _tmp10;struct Cyc_List_List*_tmp11;union Cyc_Absyn_Nmspace _tmp12;struct Cyc_List_List*_tmp13;union Cyc_Absyn_Nmspace _tmp14;struct Cyc_List_List*_tmp15;union Cyc_Absyn_Nmspace _tmp16;struct Cyc_List_List*_tmp17;union Cyc_Absyn_Nmspace _tmp18;struct Cyc_List_List*_tmp19;union Cyc_Absyn_Nmspace _tmp1A;int _tmp1B;union Cyc_Absyn_Nmspace _tmp1C;int _tmp1D;union Cyc_Absyn_Nmspace _tmp1E;struct Cyc_List_List*_tmp1F;union Cyc_Absyn_Nmspace _tmp20;struct Cyc_List_List*_tmp21;union Cyc_Absyn_Nmspace _tmp22;struct Cyc_List_List*_tmp23;union Cyc_Absyn_Nmspace _tmp24;struct Cyc_List_List*_tmp25;_LL1: _tmpA=_tmp9.f1;if((_tmpA.Loc_n).tag != 4)goto _LL3;_tmpB=(int)(_tmpA.Loc_n).val;_tmpC=_tmp9.f2;if((_tmpC.Loc_n).tag != 4)goto _LL3;_tmpD=(int)(_tmpC.Loc_n).val;_LL2:
# 65
 return 0;_LL3: _tmpE=_tmp9.f1;if((_tmpE.Rel_n).tag != 1)goto _LL5;_tmpF=(struct Cyc_List_List*)(_tmpE.Rel_n).val;_tmp10=_tmp9.f2;if((_tmp10.Rel_n).tag != 1)goto _LL5;_tmp11=(struct Cyc_List_List*)(_tmp10.Rel_n).val;_LL4:
# 66
 return Cyc_Absyn_strlist_cmp(_tmpF,_tmp11);_LL5: _tmp12=_tmp9.f1;if((_tmp12.Abs_n).tag != 2)goto _LL7;_tmp13=(struct Cyc_List_List*)(_tmp12.Abs_n).val;_tmp14=_tmp9.f2;if((_tmp14.Abs_n).tag != 2)goto _LL7;_tmp15=(struct Cyc_List_List*)(_tmp14.Abs_n).val;_LL6:
# 67
 return Cyc_Absyn_strlist_cmp(_tmp13,_tmp15);_LL7: _tmp16=_tmp9.f1;if((_tmp16.C_n).tag != 3)goto _LL9;_tmp17=(struct Cyc_List_List*)(_tmp16.C_n).val;_tmp18=_tmp9.f2;if((_tmp18.C_n).tag != 3)goto _LL9;_tmp19=(struct Cyc_List_List*)(_tmp18.C_n).val;_LL8:
# 68
 return Cyc_Absyn_strlist_cmp(_tmp17,_tmp19);_LL9: _tmp1A=_tmp9.f1;if((_tmp1A.Loc_n).tag != 4)goto _LLB;_tmp1B=(int)(_tmp1A.Loc_n).val;_LLA:
# 70
 return - 1;_LLB: _tmp1C=_tmp9.f2;if((_tmp1C.Loc_n).tag != 4)goto _LLD;_tmp1D=(int)(_tmp1C.Loc_n).val;_LLC:
# 71
 return 1;_LLD: _tmp1E=_tmp9.f1;if((_tmp1E.Rel_n).tag != 1)goto _LLF;_tmp1F=(struct Cyc_List_List*)(_tmp1E.Rel_n).val;_LLE:
# 72
 return - 1;_LLF: _tmp20=_tmp9.f2;if((_tmp20.Rel_n).tag != 1)goto _LL11;_tmp21=(struct Cyc_List_List*)(_tmp20.Rel_n).val;_LL10:
# 73
 return 1;_LL11: _tmp22=_tmp9.f1;if((_tmp22.Abs_n).tag != 2)goto _LL13;_tmp23=(struct Cyc_List_List*)(_tmp22.Abs_n).val;_LL12:
# 74
 return - 1;_LL13: _tmp24=_tmp9.f2;if((_tmp24.Abs_n).tag != 2)goto _LL0;_tmp25=(struct Cyc_List_List*)(_tmp24.Abs_n).val;_LL14:
# 75
 return 1;_LL0:;};};};};}
# 79
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
# 80
int i=Cyc_strptrcmp(tv1->name,tv2->name);
# 81
if(i != 0)return i;
# 82
return tv1->identity - tv2->identity;}
# 85
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
# 86
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
# 87
if(C_scope){
# 88
union Cyc_Absyn_Nmspace _tmp247;return((_tmp247.C_n).val=x,(((_tmp247.C_n).tag=3,_tmp247)));}else{
# 90
union Cyc_Absyn_Nmspace _tmp248;return((_tmp248.Abs_n).val=x,(((_tmp248.Abs_n).tag=2,_tmp248)));}}
# 92
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp249;return((_tmp249.Rel_n).val=x,(((_tmp249.Rel_n).tag=1,_tmp249)));}
# 93
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 95
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
# 96
union Cyc_Absyn_Nmspace _tmp29=(*qv).f1;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp2B;int _tmp2C;_LL16: if((_tmp29.Rel_n).tag != 1)goto _LL18;_tmp2A=(struct Cyc_List_List*)(_tmp29.Rel_n).val;if(_tmp2A != 0)goto _LL18;_LL17:
# 97
 goto _LL19;_LL18: if((_tmp29.Abs_n).tag != 2)goto _LL1A;_tmp2B=(struct Cyc_List_List*)(_tmp29.Abs_n).val;if(_tmp2B != 0)goto _LL1A;_LL19:
# 98
 goto _LL1B;_LL1A: if((_tmp29.Loc_n).tag != 4)goto _LL1C;_tmp2C=(int)(_tmp29.Loc_n).val;_LL1B:
# 99
 return 0;_LL1C:;_LL1D:
# 100
 return 1;_LL15:;}
# 104
static int Cyc_Absyn_new_type_counter=0;
# 106
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
# 107
struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp24C;struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp24B;return(void*)((_tmp24B=_cycalloc(sizeof(*_tmp24B)),((_tmp24B[0]=((_tmp24C.tag=1,((_tmp24C.f1=k,((_tmp24C.f2=(void*)0,((_tmp24C.f3=Cyc_Absyn_new_type_counter ++,((_tmp24C.f4=env,_tmp24C)))))))))),_tmp24B))));}
# 109
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
# 110
return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_tmko,tenv);}
# 113
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
# 114
struct Cyc_Absyn_Tqual _tmp24D;return(_tmp24D.print_const=x.print_const  || y.print_const,((_tmp24D.q_volatile=
# 115
x.q_volatile  || y.q_volatile,((_tmp24D.q_restrict=
# 116
x.q_restrict  || y.q_restrict,((_tmp24D.real_const=
# 117
x.real_const  || y.real_const,((_tmp24D.loc=
# 118
Cyc_Position_segment_join(x.loc,y.loc),_tmp24D)))))))));}
# 121
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){struct Cyc_Absyn_Tqual _tmp24E;return(_tmp24E.print_const=0,((_tmp24E.q_volatile=0,((_tmp24E.q_restrict=0,((_tmp24E.real_const=0,((_tmp24E.loc=loc,_tmp24E)))))))));}
# 122
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){struct Cyc_Absyn_Tqual _tmp24F;return(_tmp24F.print_const=1,((_tmp24F.q_volatile=0,((_tmp24F.q_restrict=0,((_tmp24F.real_const=1,((_tmp24F.loc=loc,_tmp24F)))))))));}
# 124
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 128
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Absyn_one_b_raw={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 130
struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 131
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
# 132
static struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct Cyc_Absyn_one_bt={1,& Cyc_Absyn_exp_unsigned_one_v};
# 133
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
# 135
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
# 136
union Cyc_Absyn_DatatypeInfoU _tmp250;return((_tmp250.UnknownDatatype).val=udi,(((_tmp250.UnknownDatatype).tag=1,_tmp250)));}
# 138
extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
# 139
union Cyc_Absyn_DatatypeInfoU _tmp251;return((_tmp251.KnownDatatype).val=d,(((_tmp251.KnownDatatype).tag=2,_tmp251)));}
# 141
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
# 142
union Cyc_Absyn_DatatypeFieldInfoU _tmp252;return((_tmp252.UnknownDatatypefield).val=s,(((_tmp252.UnknownDatatypefield).tag=1,_tmp252)));}
# 144
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
# 145
struct _tuple1 _tmp255;union Cyc_Absyn_DatatypeFieldInfoU _tmp254;return((_tmp254.KnownDatatypefield).val=((_tmp255.f1=dd,((_tmp255.f2=df,_tmp255)))),(((_tmp254.KnownDatatypefield).tag=2,_tmp254)));}
# 147
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
# 148
struct _tuple2 _tmp258;union Cyc_Absyn_AggrInfoU _tmp257;return((_tmp257.UnknownAggr).val=((_tmp258.f1=ak,((_tmp258.f2=n,((_tmp258.f3=tagged,_tmp258)))))),(((_tmp257.UnknownAggr).tag=1,_tmp257)));}
# 150
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
# 151
union Cyc_Absyn_AggrInfoU _tmp259;return((_tmp259.KnownAggr).val=ad,(((_tmp259.KnownAggr).tag=2,_tmp259)));}
# 154
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*_tmp25A;return(_tmp25A=_cycalloc(sizeof(*_tmp25A)),(((_tmp25A->Eq_constr).val=(void*)x,(((_tmp25A->Eq_constr).tag=1,_tmp25A)))));}
# 155
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){union Cyc_Absyn_Constraint*_tmp25B;return(_tmp25B=_cycalloc_atomic(sizeof(*_tmp25B)),(((_tmp25B->No_constr).val=0,(((_tmp25B->No_constr).tag=3,_tmp25B)))));}
# 161
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0};
# 163
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
# 164
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
# 165
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;
# 166
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
# 170
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x){
# 171
union Cyc_Absyn_Constraint*_tmp40=x;union Cyc_Absyn_Constraint _tmp41;int _tmp42;union Cyc_Absyn_Constraint _tmp43;void*_tmp44;union Cyc_Absyn_Constraint _tmp45;union Cyc_Absyn_Constraint*_tmp46;_LL1F: _tmp41=*_tmp40;if((_tmp41.No_constr).tag != 3)goto _LL21;_tmp42=(int)(_tmp41.No_constr).val;_LL20:
# 172
 goto _LL22;_LL21: _tmp43=*_tmp40;if((_tmp43.Eq_constr).tag != 1)goto _LL23;_tmp44=(void*)(_tmp43.Eq_constr).val;_LL22:
# 173
 return x;_LL23: _tmp45=*_tmp40;if((_tmp45.Forward_constr).tag != 2)goto _LL1E;_tmp46=(union Cyc_Absyn_Constraint*)(_tmp45.Forward_constr).val;_LL24: {
# 175
union Cyc_Absyn_Constraint*_tmp47=Cyc_Absyn_compress_conref(_tmp46);
# 176
{struct _union_Constraint_Forward_constr*_tmp25C;(_tmp25C=& x->Forward_constr,((_tmp25C->tag=2,_tmp25C->val=_tmp47)));}
# 177
return _tmp47;}_LL1E:;}
# 181
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
# 182
union Cyc_Absyn_Constraint*_tmp49=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint _tmp4A;void*_tmp4B;_LL26: _tmp4A=*_tmp49;if((_tmp4A.Eq_constr).tag != 1)goto _LL28;_tmp4B=(void*)(_tmp4A.Eq_constr).val;_LL27:
# 183
 return _tmp4B;_LL28:;_LL29: {
# 184
const char*_tmp25F;void*_tmp25E;(_tmp25E=0,Cyc_Tcutil_impos(((_tmp25F="conref_val",_tag_dyneither(_tmp25F,sizeof(char),11))),_tag_dyneither(_tmp25E,sizeof(void*),0)));}_LL25:;}
# 188
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){
# 189
union Cyc_Absyn_Constraint*_tmp4E=x;union Cyc_Absyn_Constraint _tmp4F;void*_tmp50;union Cyc_Absyn_Constraint _tmp51;int _tmp52;_LL2B: _tmp4F=*_tmp4E;if((_tmp4F.Eq_constr).tag != 1)goto _LL2D;_tmp50=(void*)(_tmp4F.Eq_constr).val;_LL2C:
# 190
 return _tmp50;_LL2D: _tmp51=*_tmp4E;if((_tmp51.No_constr).tag != 3)goto _LL2F;_tmp52=(int)(_tmp51.No_constr).val;_LL2E:
# 191
 return y;_LL2F:;_LL30:
# 192
 return Cyc_Absyn_conref_def(y,Cyc_Absyn_compress_conref(x));_LL2A:;}
# 196
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){
# 197
x=Cyc_Absyn_compress_conref(x);{
# 198
union Cyc_Absyn_Constraint*_tmp53=x;union Cyc_Absyn_Constraint _tmp54;void*_tmp55;_LL32: _tmp54=*_tmp53;if((_tmp54.Eq_constr).tag != 1)goto _LL34;_tmp55=(void*)(_tmp54.Eq_constr).val;_LL33:
# 199
 return _tmp55;_LL34:;_LL35:
# 200
{struct _union_Constraint_Eq_constr*_tmp260;(_tmp260=& x->Eq_constr,((_tmp260->tag=1,_tmp260->val=y)));}return y;_LL31:;};}
# 204
void*Cyc_Absyn_compress_kb(void*k){
# 205
void*_tmp57=k;struct Cyc_Core_Opt*_tmp5A;struct Cyc_Core_Opt*_tmp5C;struct Cyc_Core_Opt*_tmp5E;struct Cyc_Core_Opt _tmp5F;void*_tmp60;void**_tmp61;struct Cyc_Core_Opt*_tmp63;struct Cyc_Core_Opt _tmp64;void*_tmp65;void**_tmp66;_LL37: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp58=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp58->tag != 0)goto _LL39;}_LL38:
# 206
 goto _LL3A;_LL39: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp59=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp59->tag != 1)goto _LL3B;else{_tmp5A=_tmp59->f1;if(_tmp5A != 0)goto _LL3B;}}_LL3A:
# 207
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp5B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp5B->tag != 2)goto _LL3D;else{_tmp5C=_tmp5B->f1;if(_tmp5C != 0)goto _LL3D;}}_LL3C:
# 208
 return k;_LL3D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp5D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp5D->tag != 1)goto _LL3F;else{_tmp5E=_tmp5D->f1;if(_tmp5E == 0)goto _LL3F;_tmp5F=*_tmp5E;_tmp60=(void*)_tmp5F.v;_tmp61=(void**)&(*_tmp5D->f1).v;}}_LL3E:
# 209
 _tmp66=_tmp61;goto _LL40;_LL3F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp62=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57;if(_tmp62->tag != 2)goto _LL36;else{_tmp63=_tmp62->f1;if(_tmp63 == 0)goto _LL36;_tmp64=*_tmp63;_tmp65=(void*)_tmp64.v;_tmp66=(void**)&(*_tmp62->f1).v;}}_LL40:
# 211
*_tmp66=Cyc_Absyn_compress_kb(*_tmp66);
# 212
return*_tmp66;_LL36:;}
# 216
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
# 217
void*_tmp67=Cyc_Absyn_compress_kb(kb);struct Cyc_Absyn_Kind*_tmp69;struct Cyc_Core_Opt*_tmp6B;struct Cyc_Core_Opt**_tmp6C;struct Cyc_Core_Opt*_tmp6E;struct Cyc_Core_Opt**_tmp6F;struct Cyc_Absyn_Kind*_tmp70;_LL42: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp68=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp67;if(_tmp68->tag != 0)goto _LL44;else{_tmp69=_tmp68->f1;}}_LL43:
# 218
 return _tmp69;_LL44: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp6A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp67;if(_tmp6A->tag != 1)goto _LL46;else{_tmp6B=_tmp6A->f1;_tmp6C=(struct Cyc_Core_Opt**)& _tmp6A->f1;}}_LL45:
# 219
 _tmp6F=_tmp6C;_tmp70=& Cyc_Tcutil_bk;goto _LL47;_LL46: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp67;if(_tmp6D->tag != 2)goto _LL41;else{_tmp6E=_tmp6D->f1;_tmp6F=(struct Cyc_Core_Opt**)& _tmp6D->f1;_tmp70=_tmp6D->f2;}}_LL47:
# 221
{struct Cyc_Core_Opt*_tmp261;*_tmp6F=((_tmp261=_cycalloc(sizeof(*_tmp261)),((_tmp261->v=Cyc_Tcutil_kind_to_bound(_tmp70),_tmp261))));}
# 222
return _tmp70;_LL41:;}
# 227
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={20};
# 228
struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={21};
# 229
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={22};
# 230
struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0};
# 233
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_char_tt={6,Cyc_Absyn_None,Cyc_Absyn_Char_sz};
# 234
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uchar_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};
# 235
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ushort_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};
# 236
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uint_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};
# 237
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};
# 238
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulonglong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};
# 239
void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
# 240
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
# 241
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;
# 242
void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
# 243
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
# 244
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;
# 246
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_schar_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};
# 247
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sshort_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};
# 248
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sint_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};
# 249
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};
# 250
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slonglong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};
# 251
void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
# 252
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
# 253
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;
# 254
void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
# 255
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
# 257
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nshort_tt={6,Cyc_Absyn_None,Cyc_Absyn_Short_sz};
# 258
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nint_tt={6,Cyc_Absyn_None,Cyc_Absyn_Int_sz};
# 259
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlong_tt={6,Cyc_Absyn_None,Cyc_Absyn_Long_sz};
# 260
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlonglong_tt={6,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
# 261
void*Cyc_Absyn_nshort_typ=(void*)& Cyc_Absyn_nshort_tt;
# 262
void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
# 263
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;
# 264
void*Cyc_Absyn_nlonglong_typ=(void*)& Cyc_Absyn_nlonglong_tt;
# 266
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
# 267
switch(sn){case Cyc_Absyn_Signed: _LL48:
# 269
 switch(sz){case Cyc_Absyn_Char_sz: _LL4A:
# 270
 return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LL4B:
# 271
 return Cyc_Absyn_sshort_typ;case Cyc_Absyn_Int_sz: _LL4C:
# 272
 return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LL4D:
# 273
 return Cyc_Absyn_slong_typ;case Cyc_Absyn_LongLong_sz: _LL4E:
# 274
 return Cyc_Absyn_slonglong_typ;}case Cyc_Absyn_Unsigned: _LL49:
# 277
 switch(sz){case Cyc_Absyn_Char_sz: _LL51:
# 278
 return Cyc_Absyn_uchar_typ;case Cyc_Absyn_Short_sz: _LL52:
# 279
 return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz: _LL53:
# 280
 return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL54:
# 281
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_LongLong_sz: _LL55:
# 282
 return Cyc_Absyn_ulonglong_typ;}case Cyc_Absyn_None: _LL50:
# 285
 switch(sz){case Cyc_Absyn_Char_sz: _LL58:
# 286
 return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz: _LL59:
# 287
 return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL5A:
# 288
 return Cyc_Absyn_nint_typ;case Cyc_Absyn_Long_sz: _LL5B:
# 289
 return Cyc_Absyn_nlong_typ;case Cyc_Absyn_LongLong_sz: _LL5C:
# 290
 return Cyc_Absyn_nlonglong_typ;}}}
# 295
void*Cyc_Absyn_float_typ(int i){
# 296
static struct Cyc_Absyn_FloatType_Absyn_Type_struct fl={7,0};
# 297
static struct Cyc_Absyn_FloatType_Absyn_Type_struct db={7,1};
# 298
static struct Cyc_Absyn_FloatType_Absyn_Type_struct ldb={7,2};
# 299
if(i == 0)return(void*)& fl;
# 300
if(i == 1)return(void*)& db;
# 301
return(void*)& ldb;}
# 306
extern int Wchar_t_unsigned;
# 307
extern int Sizeof_wchar_t;
# 309
void*Cyc_Absyn_wchar_typ(){
# 310
switch(Sizeof_wchar_t){case 1: _LL5E:
# 320 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;case 2: _LL5F:
# 321
 return Wchar_t_unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;default: _LL60:
# 324
 return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}}static char _tmp88[4]="exn";
# 328
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp88,_tmp88,_tmp88 + 4};
# 329
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
# 330
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp89[15]="Null_Exception";
# 340 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp89,_tmp89,_tmp89 + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp8A[13]="Array_bounds";
# 341
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp8A,_tmp8A,_tmp8A + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp8B[16]="Match_Exception";
# 342
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp8B,_tmp8B,_tmp8B + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp8C[10]="Bad_alloc";
# 343
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp8C,_tmp8C,_tmp8C + 10};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
# 346
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
# 347
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l0};
# 348
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
# 349
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l2};
# 351
static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};
# 352
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)& Cyc_Absyn_exn_ol,1};
# 354
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
# 357
static struct Cyc_Absyn_DatatypeType_Absyn_Type_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={2,& Cyc_Absyn_exn_tud}},0}};
# 359
void*Cyc_Absyn_exn_typ(){
# 360
static struct Cyc_Core_Opt*exn_type_val=0;
# 361
if(exn_type_val == 0){
# 362
struct Cyc_Core_Opt*_tmp262;exn_type_val=((_tmp262=_cycalloc(sizeof(*_tmp262)),((_tmp262->v=Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,
# 363
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp262))));}
# 365
return(void*)exn_type_val->v;}static char _tmp8F[9]="PrintArg";
# 369
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp8F,_tmp8F,_tmp8F + 9};static char _tmp90[9]="ScanfArg";
# 370
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp90,_tmp90,_tmp90 + 9};
# 371
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 373
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 375
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
# 376
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp91[14]="unique_region";
# 383
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp91,_tmp91,_tmp91 + 14};static char _tmp92[5]="Core";
# 384
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp92,_tmp92,_tmp92 + 5};
# 385
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)((struct _dyneither_ptr*)& Cyc_Absyn_Core_str),(struct Cyc_List_List*)0};
# 387
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,(struct Cyc_List_List*)& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 389
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 391
static struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct Cyc_Absyn_uniquergn_typ={15,(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};
# 394
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,{0,0,0,0,0},(void*)& Cyc_Absyn_uniquergn_typ,0,0,0,0};
# 397
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1,& Cyc_Absyn_uniquergn_var_d};
# 399
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1,& Cyc_Absyn_uniquergn_qvar_p,(void*)((void*)& Cyc_Absyn_uniquergn_bind_p)};
# 402
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergn_typ,(void*)& Cyc_Absyn_uniquergn_exp_r,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 405
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 408
static void**Cyc_Absyn_string_t_opt=0;
# 409
void*Cyc_Absyn_string_typ(void*rgn){
# 410
void*_tmp96=Cyc_Tcutil_compress(rgn);_LL63: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp96;if(_tmp97->tag != 20)goto _LL65;}_LL64:
# 412
 if(Cyc_Absyn_string_t_opt == 0){
# 413
void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 415
void**_tmp263;Cyc_Absyn_string_t_opt=((_tmp263=_cycalloc(sizeof(*_tmp263)),((_tmp263[0]=t,_tmp263))));}
# 417
return*((void**)_check_null(Cyc_Absyn_string_t_opt));_LL65:;_LL66:
# 419
 return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL62:;}
# 422
static void**Cyc_Absyn_const_string_t_opt=0;
# 423
void*Cyc_Absyn_const_string_typ(void*rgn){
# 424
void*_tmp99=Cyc_Tcutil_compress(rgn);_LL68: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp99;if(_tmp9A->tag != 20)goto _LL6A;}_LL69:
# 426
 if(Cyc_Absyn_const_string_t_opt == 0){
# 427
void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 429
void**_tmp264;Cyc_Absyn_const_string_t_opt=((_tmp264=_cycalloc(sizeof(*_tmp264)),((_tmp264[0]=t,_tmp264))));}
# 431
return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));_LL6A:;_LL6B:
# 433
 return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL67:;}
# 438
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 440
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp26E;struct Cyc_Absyn_PtrAtts _tmp26D;struct Cyc_Absyn_PtrInfo _tmp26C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26B;return(void*)((_tmp26B=_cycalloc(sizeof(*_tmp26B)),((_tmp26B[0]=((_tmp26E.tag=5,((_tmp26E.f1=((_tmp26C.elt_typ=t,((_tmp26C.elt_tq=tq,((_tmp26C.ptr_atts=(
# 441
(_tmp26D.rgn=r,((_tmp26D.nullable=Cyc_Absyn_true_conref,((_tmp26D.bounds=
# 442
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp26D.zero_term=zeroterm,((_tmp26D.ptrloc=0,_tmp26D)))))))))),_tmp26C)))))),_tmp26E)))),_tmp26B))));}
# 446
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 448
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp278;struct Cyc_Absyn_PtrAtts _tmp277;struct Cyc_Absyn_PtrInfo _tmp276;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp275;return(void*)((_tmp275=_cycalloc(sizeof(*_tmp275)),((_tmp275[0]=((_tmp278.tag=5,((_tmp278.f1=((_tmp276.elt_typ=t,((_tmp276.elt_tq=tq,((_tmp276.ptr_atts=(
# 449
(_tmp277.rgn=r,((_tmp277.nullable=Cyc_Absyn_false_conref,((_tmp277.bounds=
# 450
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),((_tmp277.zero_term=zeroterm,((_tmp277.ptrloc=0,_tmp277)))))))))),_tmp276)))))),_tmp278)))),_tmp275))));}
# 455
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
# 456
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp282;struct Cyc_Absyn_PtrAtts _tmp281;struct Cyc_Absyn_PtrInfo _tmp280;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp27F;return(void*)((_tmp27F=_cycalloc(sizeof(*_tmp27F)),((_tmp27F[0]=((_tmp282.tag=5,((_tmp282.f1=((_tmp280.elt_typ=t,((_tmp280.elt_tq=tq,((_tmp280.ptr_atts=(
# 457
(_tmp281.rgn=r,((_tmp281.nullable=Cyc_Absyn_true_conref,((_tmp281.bounds=Cyc_Absyn_bounds_one_conref,((_tmp281.zero_term=zeroterm,((_tmp281.ptrloc=0,_tmp281)))))))))),_tmp280)))))),_tmp282)))),_tmp27F))));}
# 462
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
# 463
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp28C;struct Cyc_Absyn_PtrAtts _tmp28B;struct Cyc_Absyn_PtrInfo _tmp28A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp289;return(void*)((_tmp289=_cycalloc(sizeof(*_tmp289)),((_tmp289[0]=((_tmp28C.tag=5,((_tmp28C.f1=((_tmp28A.elt_typ=t,((_tmp28A.elt_tq=tq,((_tmp28A.ptr_atts=(
# 464
(_tmp28B.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((_tmp28B.nullable=Cyc_Absyn_true_conref,((_tmp28B.bounds=Cyc_Absyn_bounds_one_conref,((_tmp28B.zero_term=Cyc_Absyn_false_conref,((_tmp28B.ptrloc=0,_tmp28B)))))))))),_tmp28A)))))),_tmp28C)))),_tmp289))));}
# 470
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
# 471
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp296;struct Cyc_Absyn_PtrAtts _tmp295;struct Cyc_Absyn_PtrInfo _tmp294;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp293;return(void*)((_tmp293=_cycalloc(sizeof(*_tmp293)),((_tmp293[0]=((_tmp296.tag=5,((_tmp296.f1=((_tmp294.elt_typ=t,((_tmp294.elt_tq=tq,((_tmp294.ptr_atts=(
# 472
(_tmp295.rgn=r,((_tmp295.nullable=Cyc_Absyn_false_conref,((_tmp295.bounds=Cyc_Absyn_bounds_one_conref,((_tmp295.zero_term=zeroterm,((_tmp295.ptrloc=0,_tmp295)))))))))),_tmp294)))))),_tmp296)))),_tmp293))));}
# 477
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
# 478
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2A0;struct Cyc_Absyn_PtrAtts _tmp29F;struct Cyc_Absyn_PtrInfo _tmp29E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp29D;return(void*)((_tmp29D=_cycalloc(sizeof(*_tmp29D)),((_tmp29D[0]=((_tmp2A0.tag=5,((_tmp2A0.f1=((_tmp29E.elt_typ=t,((_tmp29E.elt_tq=tq,((_tmp29E.ptr_atts=(
# 479
(_tmp29F.rgn=r,((_tmp29F.nullable=Cyc_Absyn_true_conref,((_tmp29F.bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp29F.zero_term=zeroterm,((_tmp29F.ptrloc=0,_tmp29F)))))))))),_tmp29E)))))),_tmp2A0)))),_tmp29D))));}
# 485
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc){
# 487
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp2A6;struct Cyc_Absyn_ArrayInfo _tmp2A5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2A4;return(void*)((_tmp2A4=_cycalloc(sizeof(*_tmp2A4)),((_tmp2A4[0]=((_tmp2A6.tag=8,((_tmp2A6.f1=((_tmp2A5.elt_type=elt_type,((_tmp2A5.tq=tq,((_tmp2A5.num_elts=num_elts,((_tmp2A5.zero_term=zero_term,((_tmp2A5.zt_loc=ztloc,_tmp2A5)))))))))),_tmp2A6)))),_tmp2A4))));}static char _tmpBE[8]="__sFILE";
# 495
void*Cyc_Absyn_file_typ(){
# 496
static void**file_t_opt=0;
# 497
static struct _dyneither_ptr sf_str={_tmpBE,_tmpBE,_tmpBE + 8};
# 498
static struct _dyneither_ptr*sf=& sf_str;
# 500
if(file_t_opt == 0){
# 501
struct _tuple0*_tmp2A7;struct _tuple0*file_t_name=(_tmp2A7=_cycalloc(sizeof(*_tmp2A7)),((_tmp2A7->f1=Cyc_Absyn_Abs_n(0,0),((_tmp2A7->f2=sf,_tmp2A7)))));
# 502
struct Cyc_Absyn_Aggrdecl*_tmp2A8;struct Cyc_Absyn_Aggrdecl*sd=(_tmp2A8=_cycalloc(sizeof(*_tmp2A8)),((_tmp2A8->kind=Cyc_Absyn_StructA,((_tmp2A8->sc=Cyc_Absyn_Abstract,((_tmp2A8->name=file_t_name,((_tmp2A8->tvs=0,((_tmp2A8->impl=0,((_tmp2A8->attributes=0,_tmp2A8)))))))))))));
# 503
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp2B2;struct Cyc_Absyn_Aggrdecl**_tmp2B1;struct Cyc_Absyn_AggrInfo _tmp2B0;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2AF;void*file_struct_typ=(void*)((_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF[0]=((_tmp2B2.tag=11,((_tmp2B2.f1=((_tmp2B0.aggr_info=Cyc_Absyn_KnownAggr(((_tmp2B1=_cycalloc(sizeof(*_tmp2B1)),((_tmp2B1[0]=sd,_tmp2B1))))),((_tmp2B0.targs=0,_tmp2B0)))),_tmp2B2)))),_tmp2AF))));
# 504
void**_tmp2B3;file_t_opt=((_tmp2B3=_cycalloc(sizeof(*_tmp2B3)),((_tmp2B3[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp2B3))));}
# 507
return*file_t_opt;}
# 510
void*Cyc_Absyn_void_star_typ(){
# 511
static void**void_star_t_opt=0;
# 513
if(void_star_t_opt == 0){
# 514
void**_tmp2B4;void_star_t_opt=((_tmp2B4=_cycalloc(sizeof(*_tmp2B4)),((_tmp2B4[0]=Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,
# 515
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp2B4))));}
# 516
return*void_star_t_opt;}
# 518
static struct Cyc_Absyn_JoinEff_Absyn_Type_struct Cyc_Absyn_empty_eff={24,0};
# 519
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
# 525
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
# 526
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp2BE;struct _tuple0*_tmp2BD;struct Cyc_Absyn_AggrInfo _tmp2BC;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2BB;return(void*)((_tmp2BB=_cycalloc(sizeof(*_tmp2BB)),((_tmp2BB[0]=((_tmp2BE.tag=11,((_tmp2BE.f1=((_tmp2BC.aggr_info=Cyc_Absyn_UnknownAggr(k,((_tmp2BD=_cycalloc(sizeof(*_tmp2BD)),((_tmp2BD->f1=Cyc_Absyn_rel_ns_null,((_tmp2BD->f2=name,_tmp2BD)))))),0),((_tmp2BC.targs=0,_tmp2BC)))),_tmp2BE)))),_tmp2BB))));}
# 529
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
# 530
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}
# 532
void*Cyc_Absyn_strctq(struct _tuple0*name){
# 533
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp2C4;struct Cyc_Absyn_AggrInfo _tmp2C3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2C2;return(void*)((_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C4.tag=11,((_tmp2C4.f1=((_tmp2C3.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),((_tmp2C3.targs=0,_tmp2C3)))),_tmp2C4)))),_tmp2C2))));}
# 535
void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
# 536
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp2CA;struct Cyc_Absyn_AggrInfo _tmp2C9;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2C8;return(void*)((_tmp2C8=_cycalloc(sizeof(*_tmp2C8)),((_tmp2C8[0]=((_tmp2CA.tag=11,((_tmp2CA.f1=((_tmp2C9.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((_tmp2C9.targs=0,_tmp2C9)))),_tmp2CA)))),_tmp2C8))));}
# 539
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
# 540
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct _tuple3 _tmp2CD;union Cyc_Absyn_Cnst _tmp2CC;return((_tmp2CC.Char_c).val=((_tmp2CD.f1=sn,((_tmp2CD.f2=c,_tmp2CD)))),(((_tmp2CC.Char_c).tag=2,_tmp2CC)));}
# 541
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2CE;return((_tmp2CE.Wchar_c).val=s,(((_tmp2CE.Wchar_c).tag=3,_tmp2CE)));}
# 542
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){struct _tuple4 _tmp2D1;union Cyc_Absyn_Cnst _tmp2D0;return((_tmp2D0.Short_c).val=((_tmp2D1.f1=sn,((_tmp2D1.f2=s,_tmp2D1)))),(((_tmp2D0.Short_c).tag=4,_tmp2D0)));}
# 543
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){struct _tuple5 _tmp2D4;union Cyc_Absyn_Cnst _tmp2D3;return((_tmp2D3.Int_c).val=((_tmp2D4.f1=sn,((_tmp2D4.f2=i,_tmp2D4)))),(((_tmp2D3.Int_c).tag=5,_tmp2D3)));}
# 544
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){struct _tuple6 _tmp2D7;union Cyc_Absyn_Cnst _tmp2D6;return((_tmp2D6.LongLong_c).val=((_tmp2D7.f1=sn,((_tmp2D7.f2=l,_tmp2D7)))),(((_tmp2D6.LongLong_c).tag=6,_tmp2D6)));}
# 545
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){struct _tuple7 _tmp2DA;union Cyc_Absyn_Cnst _tmp2D9;return((_tmp2D9.Float_c).val=((_tmp2DA.f1=s,((_tmp2DA.f2=i,_tmp2DA)))),(((_tmp2D9.Float_c).tag=7,_tmp2D9)));}
# 546
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2DB;return((_tmp2DB.String_c).val=s,(((_tmp2DB.String_c).tag=8,_tmp2DB)));}
# 547
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp2DC;return((_tmp2DC.Wstring_c).val=s,(((_tmp2DC.Wstring_c).tag=9,_tmp2DC)));}
# 550
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
# 551
struct Cyc_Absyn_Exp*_tmp2DD;return(_tmp2DD=_cycalloc(sizeof(*_tmp2DD)),((_tmp2DD->topt=0,((_tmp2DD->r=r,((_tmp2DD->loc=loc,((_tmp2DD->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp2DD)))))))));}
# 553
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 554
struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp2E0;struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp2DF;return Cyc_Absyn_new_exp((void*)((_tmp2DF=_cycalloc(sizeof(*_tmp2DF)),((_tmp2DF[0]=((_tmp2E0.tag=15,((_tmp2E0.f1=rgn_handle,((_tmp2E0.f2=e,_tmp2E0)))))),_tmp2DF)))),loc);}
# 556
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
# 557
struct Cyc_Absyn_Exp*_tmp2E1;return(_tmp2E1=_cycalloc(sizeof(*_tmp2E1)),((_tmp2E1[0]=*e,_tmp2E1)));}
# 559
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
# 560
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp2E4;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp2E3;return Cyc_Absyn_new_exp((void*)((_tmp2E3=_cycalloc(sizeof(*_tmp2E3)),((_tmp2E3[0]=((_tmp2E4.tag=0,((_tmp2E4.f1=c,_tmp2E4)))),_tmp2E3)))),loc);}
# 562
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
# 563
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0,{.Null_c={1,0}}};
# 564
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 566
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(s,i),seg);}
# 567
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
# 568
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 569
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 570
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
# 571
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
# 572
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 574
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
# 575
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 576
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 577
if(i == 0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
# 578
if(i == 1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
# 579
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 581
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
# 582
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
# 583
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
# 584
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
# 585
return Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(Cyc_Absyn_None,c),loc);}
# 587
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
# 588
return Cyc_Absyn_const_exp(Cyc_Absyn_Wchar_c(s),loc);}
# 590
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(f,i),loc);}
# 591
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),loc);}
# 592
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Wstring_c(s),loc);}
# 594
struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val={0};
# 597
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
# 598
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2E7;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E6;return Cyc_Absyn_new_exp((void*)((_tmp2E6=_cycalloc(sizeof(*_tmp2E6)),((_tmp2E6[0]=((_tmp2E7.tag=1,((_tmp2E7.f1=q,((_tmp2E7.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp2E7)))))),_tmp2E6)))),loc);}
# 600
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,unsigned int loc){
# 601
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2EA;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2E9;return Cyc_Absyn_new_exp((void*)((_tmp2E9=_cycalloc(sizeof(*_tmp2E9)),((_tmp2E9[0]=((_tmp2EA.tag=1,((_tmp2EA.f1=q,((_tmp2EA.f2=(void*)b,_tmp2EA)))))),_tmp2E9)))),loc);}
# 604
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
# 605
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2ED;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2EC;return Cyc_Absyn_new_exp((void*)((_tmp2EC=_cycalloc(sizeof(*_tmp2EC)),((_tmp2EC[0]=((_tmp2ED.tag=1,((_tmp2ED.f1=q,((_tmp2ED.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val),_tmp2ED)))))),_tmp2EC)))),loc);}
# 607
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
# 608
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp2F0;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2EF;return Cyc_Absyn_new_exp((void*)((_tmp2EF=_cycalloc(sizeof(*_tmp2EF)),((_tmp2EF[0]=((_tmp2F0.tag=2,((_tmp2F0.f1=p,((_tmp2F0.f2=es,_tmp2F0)))))),_tmp2EF)))),loc);}
# 610
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 611
struct Cyc_List_List*_tmp2F1;return Cyc_Absyn_primop_exp(p,((_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1->hd=e,((_tmp2F1->tl=0,_tmp2F1)))))),loc);}
# 613
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 614
struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F3;return Cyc_Absyn_primop_exp(p,((_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3->hd=e1,((_tmp2F3->tl=((_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4->hd=e2,((_tmp2F4->tl=0,_tmp2F4)))))),_tmp2F3)))))),loc);}
# 616
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 617
struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmp2F7;struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2F6;return Cyc_Absyn_new_exp((void*)((_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((_tmp2F6[0]=((_tmp2F7.tag=33,((_tmp2F7.f1=e1,((_tmp2F7.f2=e2,_tmp2F7)))))),_tmp2F6)))),loc);}
# 619
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
# 620
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
# 621
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
# 622
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
# 623
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
# 624
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
# 625
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
# 626
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
# 627
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 630
struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp2FA;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp2F9;return Cyc_Absyn_new_exp((void*)((_tmp2F9=_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9[0]=((_tmp2FA.tag=3,((_tmp2FA.f1=e1,((_tmp2FA.f2=popt,((_tmp2FA.f3=e2,_tmp2FA)))))))),_tmp2F9)))),loc);}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 633
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 635
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
# 636
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp2FD;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2FC;return Cyc_Absyn_new_exp((void*)((_tmp2FC=_cycalloc(sizeof(*_tmp2FC)),((_tmp2FC[0]=((_tmp2FD.tag=4,((_tmp2FD.f1=e,((_tmp2FD.f2=i,_tmp2FD)))))),_tmp2FC)))),loc);}
# 638
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
# 639
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
# 640
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
# 641
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
# 642
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
# 643
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp300;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp2FF;return Cyc_Absyn_new_exp((void*)((_tmp2FF=_cycalloc(sizeof(*_tmp2FF)),((_tmp2FF[0]=((_tmp300.tag=5,((_tmp300.f1=e1,((_tmp300.f2=e2,((_tmp300.f3=e3,_tmp300)))))))),_tmp2FF)))),loc);}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 646
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp303;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp302;return Cyc_Absyn_new_exp((void*)((_tmp302=_cycalloc(sizeof(*_tmp302)),((_tmp302[0]=((_tmp303.tag=6,((_tmp303.f1=e1,((_tmp303.f2=e2,_tmp303)))))),_tmp302)))),loc);}
# 648
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 649
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp306;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp305;return Cyc_Absyn_new_exp((void*)((_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305[0]=((_tmp306.tag=7,((_tmp306.f1=e1,((_tmp306.f2=e2,_tmp306)))))),_tmp305)))),loc);}
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 652
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp309;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp308;return Cyc_Absyn_new_exp((void*)((_tmp308=_cycalloc(sizeof(*_tmp308)),((_tmp308[0]=((_tmp309.tag=8,((_tmp309.f1=e1,((_tmp309.f2=e2,_tmp309)))))),_tmp308)))),loc);}
# 654
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
# 655
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp30C;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp30B;return Cyc_Absyn_new_exp((void*)((_tmp30B=_cycalloc(sizeof(*_tmp30B)),((_tmp30B[0]=((_tmp30C.tag=9,((_tmp30C.f1=e,((_tmp30C.f2=es,((_tmp30C.f3=0,((_tmp30C.f4=0,_tmp30C)))))))))),_tmp30B)))),loc);}
# 657
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
# 658
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp30F;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp30E;return Cyc_Absyn_new_exp((void*)((_tmp30E=_cycalloc(sizeof(*_tmp30E)),((_tmp30E[0]=((_tmp30F.tag=9,((_tmp30F.f1=e,((_tmp30F.f2=es,((_tmp30F.f3=0,((_tmp30F.f4=1,_tmp30F)))))))))),_tmp30E)))),loc);}
# 660
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
# 661
struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmp312;struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp311;return Cyc_Absyn_new_exp((void*)((_tmp311=_cycalloc(sizeof(*_tmp311)),((_tmp311[0]=((_tmp312.tag=11,((_tmp312.f1=e,_tmp312)))),_tmp311)))),loc);}
# 663
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
# 664
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp315;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp314;return Cyc_Absyn_new_exp((void*)((_tmp314=_cycalloc(sizeof(*_tmp314)),((_tmp314[0]=((_tmp315.tag=12,((_tmp315.f1=e,((_tmp315.f2=ts,_tmp315)))))),_tmp314)))),loc);}
# 666
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
# 667
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp318;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp317;return Cyc_Absyn_new_exp((void*)((_tmp317=_cycalloc(sizeof(*_tmp317)),((_tmp317[0]=((_tmp318.tag=13,((_tmp318.f1=(void*)t,((_tmp318.f2=e,((_tmp318.f3=user_cast,((_tmp318.f4=c,_tmp318)))))))))),_tmp317)))),loc);}
# 669
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp31B;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp31A;return Cyc_Absyn_new_exp((void*)((_tmp31A=_cycalloc(sizeof(*_tmp31A)),((_tmp31A[0]=((_tmp31B.tag=10,((_tmp31B.f1=e,_tmp31B)))),_tmp31A)))),loc);}
# 670
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmp31E;struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp31D;return Cyc_Absyn_new_exp((void*)((_tmp31D=_cycalloc(sizeof(*_tmp31D)),((_tmp31D[0]=((_tmp31E.tag=14,((_tmp31E.f1=e,_tmp31E)))),_tmp31D)))),loc);}
# 671
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int loc){
# 672
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp321;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp320;return Cyc_Absyn_new_exp((void*)((_tmp320=_cycalloc(sizeof(*_tmp320)),((_tmp320[0]=((_tmp321.tag=16,((_tmp321.f1=(void*)t,_tmp321)))),_tmp320)))),loc);}
# 674
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
# 675
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp324;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp323;return Cyc_Absyn_new_exp((void*)((_tmp323=_cycalloc(sizeof(*_tmp323)),((_tmp323[0]=((_tmp324.tag=17,((_tmp324.f1=e,_tmp324)))),_tmp323)))),loc);}
# 677
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,void*of,unsigned int loc){
# 678
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp327;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp326;return Cyc_Absyn_new_exp((void*)((_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326[0]=((_tmp327.tag=18,((_tmp327.f1=(void*)t,((_tmp327.f2=(void*)of,_tmp327)))))),_tmp326)))),loc);}
# 680
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp32A;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp329;return Cyc_Absyn_new_exp((void*)((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=((_tmp32A.tag=19,((_tmp32A.f1=e,_tmp32A)))),_tmp329)))),loc);}
# 681
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
# 682
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp32D;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp32C;return Cyc_Absyn_new_exp((void*)((_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C[0]=((_tmp32D.tag=20,((_tmp32D.f1=e,((_tmp32D.f2=n,((_tmp32D.f3=0,((_tmp32D.f4=0,_tmp32D)))))))))),_tmp32C)))),loc);}
# 684
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
# 685
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp330;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp32F;return Cyc_Absyn_new_exp((void*)((_tmp32F=_cycalloc(sizeof(*_tmp32F)),((_tmp32F[0]=((_tmp330.tag=21,((_tmp330.f1=e,((_tmp330.f2=n,((_tmp330.f3=0,((_tmp330.f4=0,_tmp330)))))))))),_tmp32F)))),loc);}
# 687
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 688
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp333;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp332;return Cyc_Absyn_new_exp((void*)((_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332[0]=((_tmp333.tag=22,((_tmp333.f1=e1,((_tmp333.f2=e2,_tmp333)))))),_tmp332)))),loc);}
# 690
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
# 691
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmp336;struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp335;return Cyc_Absyn_new_exp((void*)((_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=((_tmp336.tag=23,((_tmp336.f1=es,_tmp336)))),_tmp335)))),loc);}
# 693
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp339;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp338;return Cyc_Absyn_new_exp((void*)((_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338[0]=((_tmp339.tag=35,((_tmp339.f1=s,_tmp339)))),_tmp338)))),loc);}
# 695
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
# 696
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 699
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
# 700
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp33C;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp33B;return Cyc_Absyn_new_exp((void*)((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((_tmp33B[0]=((_tmp33C.tag=37,((_tmp33C.f1=(void*)t,_tmp33C)))),_tmp33B)))),loc);}
# 702
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
# 703
struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmp33F;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp33E;return Cyc_Absyn_new_exp((void*)((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E[0]=((_tmp33F.tag=38,((_tmp33F.f1=volatile_kw,((_tmp33F.f2=body,_tmp33F)))))),_tmp33E)))),loc);}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 706
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 708
struct Cyc_List_List*dles=0;
# 709
for(0;es != 0;es=es->tl){
# 710
struct _tuple13*_tmp342;struct Cyc_List_List*_tmp341;dles=((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341->hd=((_tmp342=_cycalloc(sizeof(*_tmp342)),((_tmp342->f1=0,((_tmp342->f2=(struct Cyc_Absyn_Exp*)es->hd,_tmp342)))))),((_tmp341->tl=dles,_tmp341))))));}
# 711
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{
# 712
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp345;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp344;return Cyc_Absyn_new_exp((void*)((_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=((_tmp345.tag=25,((_tmp345.f1=dles,_tmp345)))),_tmp344)))),loc);};}
# 714
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 717
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp348;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp347;return Cyc_Absyn_new_exp((void*)((_tmp347=_cycalloc(sizeof(*_tmp347)),((_tmp347[0]=((_tmp348.tag=34,((_tmp348.f1=n,((_tmp348.f2=dles,_tmp348)))))),_tmp347)))),loc);}
# 720
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
# 721
struct Cyc_Absyn_Stmt*_tmp349;return(_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349->r=s,((_tmp349->loc=loc,((_tmp349->non_local_preds=0,((_tmp349->try_depth=0,((_tmp349->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp349)))))))))));}
# 724
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
# 725
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
# 726
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp34C;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp34B;return Cyc_Absyn_new_stmt((void*)((_tmp34B=_cycalloc(sizeof(*_tmp34B)),((_tmp34B[0]=((_tmp34C.tag=1,((_tmp34C.f1=e,_tmp34C)))),_tmp34B)))),loc);}
# 727
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
# 728
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
# 729
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
# 730
return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}
# 732
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
# 733
struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmp34F;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp34E;return Cyc_Absyn_new_stmt((void*)((_tmp34E=_cycalloc(sizeof(*_tmp34E)),((_tmp34E[0]=((_tmp34F.tag=3,((_tmp34F.f1=e,_tmp34F)))),_tmp34E)))),loc);}
# 735
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
# 736
struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmp352;struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp351;return Cyc_Absyn_new_stmt((void*)((_tmp351=_cycalloc(sizeof(*_tmp351)),((_tmp351[0]=((_tmp352.tag=4,((_tmp352.f1=e,((_tmp352.f2=s1,((_tmp352.f3=s2,_tmp352)))))))),_tmp351)))),loc);}
# 738
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
# 739
struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmp358;struct _tuple9 _tmp357;struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp356;return Cyc_Absyn_new_stmt((void*)((_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356[0]=((_tmp358.tag=5,((_tmp358.f1=((_tmp357.f1=e,((_tmp357.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp357)))),((_tmp358.f2=s,_tmp358)))))),_tmp356)))),loc);}
# 741
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct _tmp35B;struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp35A;return Cyc_Absyn_new_stmt((void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp35B.tag=6,((_tmp35B.f1=0,_tmp35B)))),_tmp35A)))),loc);}
# 742
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct _tmp35E;struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp35D;return Cyc_Absyn_new_stmt((void*)((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D[0]=((_tmp35E.tag=7,((_tmp35E.f1=0,_tmp35E)))),_tmp35D)))),loc);}
# 743
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
# 744
struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmp367;struct _tuple9 _tmp366;struct _tuple9 _tmp365;struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp364;return Cyc_Absyn_new_stmt((void*)((_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364[0]=((_tmp367.tag=9,((_tmp367.f1=e1,((_tmp367.f2=((_tmp366.f1=e2,((_tmp366.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp366)))),((_tmp367.f3=(
# 745
(_tmp365.f1=e3,((_tmp365.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp365)))),((_tmp367.f4=s,_tmp367)))))))))),_tmp364)))),loc);}
# 748
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
# 749
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp36A;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp369;return Cyc_Absyn_new_stmt((void*)((_tmp369=_cycalloc(sizeof(*_tmp369)),((_tmp369[0]=((_tmp36A.tag=10,((_tmp36A.f1=e,((_tmp36A.f2=scs,_tmp36A)))))),_tmp369)))),loc);}struct _tuple14{void*f1;void*f2;};
# 751
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
# 752
struct _tuple14 _tmp36B;struct _tuple14 _tmp13D=(_tmp36B.f1=s1->r,((_tmp36B.f2=s2->r,_tmp36B)));void*_tmp13E;void*_tmp140;_LL6D: _tmp13E=_tmp13D.f1;{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp13F=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp13E;if(_tmp13F->tag != 0)goto _LL6F;};_LL6E:
# 753
 return s2;_LL6F: _tmp140=_tmp13D.f2;{struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp141=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp140;if(_tmp141->tag != 0)goto _LL71;};_LL70:
# 754
 return s1;_LL71:;_LL72: {
# 755
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp36E;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp36D;return Cyc_Absyn_new_stmt((void*)((_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D[0]=((_tmp36E.tag=2,((_tmp36E.f1=s1,((_tmp36E.f2=s2,_tmp36E)))))),_tmp36D)))),loc);}_LL6C:;}
# 758
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
# 759
struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp371;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp370;return Cyc_Absyn_new_stmt((void*)((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370[0]=((_tmp371.tag=11,((_tmp371.f1=el,((_tmp371.f2=0,_tmp371)))))),_tmp370)))),loc);}
# 761
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
# 762
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp374;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp373;return Cyc_Absyn_new_stmt((void*)((_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373[0]=((_tmp374.tag=12,((_tmp374.f1=d,((_tmp374.f2=s,_tmp374)))))),_tmp373)))),loc);}
# 764
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
# 765
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp377;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp376;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((void*)((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp377.tag=0,((_tmp377.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp377)))),_tmp376)))),loc);
# 766
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp37A;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp379;return Cyc_Absyn_new_stmt((void*)((_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379[0]=((_tmp37A.tag=12,((_tmp37A.f1=d,((_tmp37A.f2=s,_tmp37A)))))),_tmp379)))),loc);}
# 768
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
# 769
struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp37D;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp37C;return Cyc_Absyn_new_stmt((void*)((_tmp37C=_cycalloc(sizeof(*_tmp37C)),((_tmp37C[0]=((_tmp37D.tag=13,((_tmp37D.f1=v,((_tmp37D.f2=s,_tmp37D)))))),_tmp37C)))),loc);}
# 771
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 772
struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp383;struct _tuple9 _tmp382;struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp381;return Cyc_Absyn_new_stmt((void*)((_tmp381=_cycalloc(sizeof(*_tmp381)),((_tmp381[0]=((_tmp383.tag=14,((_tmp383.f1=s,((_tmp383.f2=((_tmp382.f1=e,((_tmp382.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp382)))),_tmp383)))))),_tmp381)))),loc);}
# 774
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
# 775
struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp386;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp385;return Cyc_Absyn_new_stmt((void*)((_tmp385=_cycalloc(sizeof(*_tmp385)),((_tmp385[0]=((_tmp386.tag=15,((_tmp386.f1=s,((_tmp386.f2=scs,_tmp386)))))),_tmp385)))),loc);}
# 777
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
# 778
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp389;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp388;return Cyc_Absyn_new_stmt((void*)((_tmp388=_cycalloc(sizeof(*_tmp388)),((_tmp388[0]=((_tmp389.tag=8,((_tmp389.f1=lab,((_tmp389.f2=0,_tmp389)))))),_tmp388)))),loc);}
# 780
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
# 781
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}
# 784
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){struct Cyc_Absyn_Pat*_tmp38A;return(_tmp38A=_cycalloc(sizeof(*_tmp38A)),((_tmp38A->r=p,((_tmp38A->topt=0,((_tmp38A->loc=s,_tmp38A)))))));}
# 785
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp38D;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp38C;return Cyc_Absyn_new_pat((void*)((_tmp38C=_cycalloc(sizeof(*_tmp38C)),((_tmp38C[0]=((_tmp38D.tag=16,((_tmp38D.f1=e,_tmp38D)))),_tmp38C)))),e->loc);}
# 786
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
# 787
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={8};
# 790
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){struct Cyc_Absyn_Decl*_tmp38E;return(_tmp38E=_cycalloc(sizeof(*_tmp38E)),((_tmp38E->r=r,((_tmp38E->loc=loc,_tmp38E)))));}
# 791
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
# 792
struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp391;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp390;return Cyc_Absyn_new_decl((void*)((_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390[0]=((_tmp391.tag=2,((_tmp391.f1=p,((_tmp391.f2=0,((_tmp391.f3=e,_tmp391)))))))),_tmp390)))),loc);}
# 794
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
# 795
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp394;struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp393;return Cyc_Absyn_new_decl((void*)((_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393[0]=((_tmp394.tag=3,((_tmp394.f1=vds,_tmp394)))),_tmp393)))),loc);}
# 797
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*eo,unsigned int loc){
# 798
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp397;struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp396;return Cyc_Absyn_new_decl((void*)((_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396[0]=((_tmp397.tag=4,((_tmp397.f1=tv,((_tmp397.f2=vd,((_tmp397.f3=resetable,((_tmp397.f4=eo,_tmp397)))))))))),_tmp396)))),loc);}
# 800
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,unsigned int loc){
# 801
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct _tmp39A;struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp399;return Cyc_Absyn_new_decl((void*)((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((_tmp39A.tag=5,((_tmp39A.f1=e,((_tmp39A.f2=tv,((_tmp39A.f3=vd,_tmp39A)))))))),_tmp399)))),loc);}
# 804
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
# 805
struct Cyc_Absyn_Vardecl*_tmp39B;return(_tmp39B=_cycalloc(sizeof(*_tmp39B)),((_tmp39B->sc=Cyc_Absyn_Public,((_tmp39B->name=x,((_tmp39B->tq=Cyc_Absyn_empty_tqual(0),((_tmp39B->type=t,((_tmp39B->initializer=init,((_tmp39B->rgn=0,((_tmp39B->attributes=0,((_tmp39B->escapes=0,_tmp39B)))))))))))))))));}
# 809
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
# 810
struct Cyc_Absyn_Vardecl*_tmp39C;return(_tmp39C=_cycalloc(sizeof(*_tmp39C)),((_tmp39C->sc=Cyc_Absyn_Static,((_tmp39C->name=x,((_tmp39C->tq=Cyc_Absyn_empty_tqual(0),((_tmp39C->type=t,((_tmp39C->initializer=init,((_tmp39C->rgn=0,((_tmp39C->attributes=0,((_tmp39C->escapes=0,_tmp39C)))))))))))))))));}
# 814
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 818
struct Cyc_Absyn_AggrdeclImpl*_tmp39D;return(_tmp39D=_cycalloc(sizeof(*_tmp39D)),((_tmp39D->exist_vars=exists,((_tmp39D->rgn_po=po,((_tmp39D->fields=fs,((_tmp39D->tagged=tagged,_tmp39D)))))))));}
# 821
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 824
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp3A3;struct Cyc_Absyn_Aggrdecl*_tmp3A2;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp3A1;return Cyc_Absyn_new_decl((void*)((_tmp3A1=_cycalloc(sizeof(*_tmp3A1)),((_tmp3A1[0]=((_tmp3A3.tag=6,((_tmp3A3.f1=((_tmp3A2=_cycalloc(sizeof(*_tmp3A2)),((_tmp3A2->kind=k,((_tmp3A2->sc=s,((_tmp3A2->name=n,((_tmp3A2->tvs=ts,((_tmp3A2->impl=i,((_tmp3A2->attributes=atts,_tmp3A2)))))))))))))),_tmp3A3)))),_tmp3A1)))),loc);}
# 828
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 831
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp3AD;struct Cyc_Absyn_Aggrdecl*_tmp3AC;struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp3AB;struct Cyc_Absyn_TypeDecl*_tmp3AA;return(_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->r=(void*)((_tmp3AD=_cycalloc(sizeof(*_tmp3AD)),((_tmp3AD[0]=((_tmp3AB.tag=0,((_tmp3AB.f1=((_tmp3AC=_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC->kind=k,((_tmp3AC->sc=s,((_tmp3AC->name=n,((_tmp3AC->tvs=ts,((_tmp3AC->impl=i,((_tmp3AC->attributes=atts,_tmp3AC)))))))))))))),_tmp3AB)))),_tmp3AD)))),((_tmp3AA->loc=loc,_tmp3AA)))));}
# 835
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 838
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 840
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 843
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 845
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 848
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp3B3;struct Cyc_Absyn_Datatypedecl*_tmp3B2;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp3B1;return Cyc_Absyn_new_decl((void*)((_tmp3B1=_cycalloc(sizeof(*_tmp3B1)),((_tmp3B1[0]=((_tmp3B3.tag=7,((_tmp3B3.f1=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->sc=s,((_tmp3B2->name=n,((_tmp3B2->tvs=ts,((_tmp3B2->fields=fs,((_tmp3B2->is_extensible=is_extensible,_tmp3B2)))))))))))),_tmp3B3)))),_tmp3B1)))),loc);}
# 851
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 854
struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp3BD;struct Cyc_Absyn_Datatypedecl*_tmp3BC;struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp3BB;struct Cyc_Absyn_TypeDecl*_tmp3BA;return(_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA->r=(void*)((_tmp3BD=_cycalloc(sizeof(*_tmp3BD)),((_tmp3BD[0]=((_tmp3BB.tag=2,((_tmp3BB.f1=((_tmp3BC=_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->sc=s,((_tmp3BC->name=n,((_tmp3BC->tvs=ts,((_tmp3BC->fields=fs,((_tmp3BC->is_extensible=is_extensible,_tmp3BC)))))))))))),_tmp3BB)))),_tmp3BD)))),((_tmp3BA->loc=loc,_tmp3BA)))));}
# 866 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
# 867
void*_tmp174=(*a).f3;
# 868
void*_tmp175=Cyc_Absyn_pointer_expand(_tmp174,1);
# 869
if(_tmp174 != _tmp175)
# 870
(*a).f3=_tmp175;}
# 873
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){
# 880
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);{
# 881
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp3C3;struct Cyc_Absyn_FnInfo _tmp3C2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3C1;return(void*)((_tmp3C1=_cycalloc(sizeof(*_tmp3C1)),((_tmp3C1[0]=((_tmp3C3.tag=9,((_tmp3C3.f1=((_tmp3C2.tvars=tvs,((_tmp3C2.ret_tqual=ret_tqual,((_tmp3C2.ret_typ=
# 883
Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp3C2.effect=eff_typ,((_tmp3C2.args=args,((_tmp3C2.c_varargs=c_varargs,((_tmp3C2.cyc_varargs=cyc_varargs,((_tmp3C2.rgn_po=rgn_po,((_tmp3C2.attributes=atts,_tmp3C2)))))))))))))))))),_tmp3C3)))),_tmp3C1))));};}
# 893
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
# 894
void*_tmp179=Cyc_Tcutil_compress(t);_LL74: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179;if(_tmp17A->tag != 9)goto _LL76;}_LL75: {
# 895
struct Cyc_Core_Opt*_tmp3C4;return Cyc_Absyn_at_typ(t,
# 896
fresh_evar?
# 897
Cyc_Absyn_new_evar(((_tmp3C4=_cycalloc(sizeof(*_tmp3C4)),((_tmp3C4->v=& Cyc_Tcutil_rk,_tmp3C4)))),0):(void*)& Cyc_Absyn_HeapRgn_val,
# 899
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}_LL76:;_LL77:
# 900
 return t;_LL73:;}
# 915 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
# 916
void*_tmp17C=e->r;void*_tmp17E;void*_tmp181;struct Cyc_Absyn_Vardecl*_tmp183;void*_tmp185;struct Cyc_Absyn_Vardecl*_tmp187;struct Cyc_Absyn_Exp*_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp191;_LL79: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp17D->tag != 1)goto _LL7B;else{_tmp17E=(void*)_tmp17D->f2;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp17F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp17E;if(_tmp17F->tag != 2)goto _LL7B;};}}_LL7A:
# 918
 return 0;_LL7B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp180=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp180->tag != 1)goto _LL7D;else{_tmp181=(void*)_tmp180->f2;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp182=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp181;if(_tmp182->tag != 1)goto _LL7D;else{_tmp183=_tmp182->f1;}};}}_LL7C:
# 919
 _tmp187=_tmp183;goto _LL7E;_LL7D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp184->tag != 1)goto _LL7F;else{_tmp185=(void*)_tmp184->f2;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp186=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp185;if(_tmp186->tag != 4)goto _LL7F;else{_tmp187=_tmp186->f1;}};}}_LL7E: {
# 921
void*_tmp192=Cyc_Tcutil_compress(_tmp187->type);_LL90: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp192;if(_tmp193->tag != 8)goto _LL92;}_LL91:
# 922
 return 0;_LL92:;_LL93:
# 923
 return 1;_LL8F:;}_LL7F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp188->tag != 1)goto _LL81;}_LL80:
# 925
 goto _LL82;_LL81: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp189->tag != 21)goto _LL83;}_LL82:
# 926
 goto _LL84;_LL83: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18A->tag != 19)goto _LL85;}_LL84:
# 927
 goto _LL86;_LL85: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18B->tag != 22)goto _LL87;}_LL86:
# 928
 return 1;_LL87: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18C->tag != 20)goto _LL89;else{_tmp18D=_tmp18C->f1;}}_LL88:
# 929
 return Cyc_Absyn_is_lvalue(_tmp18D);_LL89: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp18E->tag != 12)goto _LL8B;else{_tmp18F=_tmp18E->f1;}}_LL8A:
# 930
 return Cyc_Absyn_is_lvalue(_tmp18F);_LL8B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp17C;if(_tmp190->tag != 11)goto _LL8D;else{_tmp191=_tmp190->f1;}}_LL8C:
# 931
 return Cyc_Absyn_is_lvalue(_tmp191);_LL8D:;_LL8E:
# 932
 return 0;_LL78:;}
# 936
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
# 937
{struct Cyc_List_List*_tmp194=fields;for(0;_tmp194 != 0;_tmp194=_tmp194->tl){
# 938
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp194->hd)->name,v)== 0)
# 939
return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp194->hd);}}
# 940
return 0;}
# 942
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
# 943
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 946
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
# 947
for(0;i != 0;-- i){
# 948
if(ts == 0)
# 949
return 0;
# 950
ts=ts->tl;}
# 952
if(ts == 0)
# 953
return 0;
# 954
return(struct _tuple10*)((struct _tuple10*)ts->hd);}
# 957
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={1};
# 958
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2};
# 959
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3};
# 960
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4};
# 961
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5};
# 962
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7};
# 963
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9};
# 964
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10};
# 965
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11};
# 966
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12};
# 967
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={13};
# 968
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={14};
# 969
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={15};
# 970
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={16};
# 971
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={17};
# 972
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={18};
# 973
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={23};
# 975
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
# 976
void*_tmp1A6=a;int _tmp1A8;int _tmp1AF;struct _dyneither_ptr _tmp1B2;enum Cyc_Absyn_Format_Type _tmp1BE;int _tmp1BF;int _tmp1C0;enum Cyc_Absyn_Format_Type _tmp1C2;int _tmp1C3;int _tmp1C4;int _tmp1C6;int _tmp1C8;int _tmp1CA;struct _dyneither_ptr _tmp1CD;_LL95: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp1A7=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1A7->tag != 0)goto _LL97;else{_tmp1A8=_tmp1A7->f1;}}_LL96: {
# 977
const char*_tmp3C8;void*_tmp3C7[1];struct Cyc_Int_pa_PrintArg_struct _tmp3C6;return(struct _dyneither_ptr)((_tmp3C6.tag=1,((_tmp3C6.f1=(unsigned long)_tmp1A8,((_tmp3C7[0]=& _tmp3C6,Cyc_aprintf(((_tmp3C8="regparm(%d)",_tag_dyneither(_tmp3C8,sizeof(char),12))),_tag_dyneither(_tmp3C7,sizeof(void*),1))))))));}_LL97: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp1A9=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1A9->tag != 1)goto _LL99;}_LL98: {
# 978
const char*_tmp3C9;return(_tmp3C9="stdcall",_tag_dyneither(_tmp3C9,sizeof(char),8));}_LL99: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp1AA=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AA->tag != 2)goto _LL9B;}_LL9A: {
# 979
const char*_tmp3CA;return(_tmp3CA="cdecl",_tag_dyneither(_tmp3CA,sizeof(char),6));}_LL9B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp1AB=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AB->tag != 3)goto _LL9D;}_LL9C: {
# 980
const char*_tmp3CB;return(_tmp3CB="fastcall",_tag_dyneither(_tmp3CB,sizeof(char),9));}_LL9D: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1AC=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AC->tag != 4)goto _LL9F;}_LL9E: {
# 981
const char*_tmp3CC;return(_tmp3CC="noreturn",_tag_dyneither(_tmp3CC,sizeof(char),9));}_LL9F: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1AD=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AD->tag != 5)goto _LLA1;}_LLA0: {
# 982
const char*_tmp3CD;return(_tmp3CD="const",_tag_dyneither(_tmp3CD,sizeof(char),6));}_LLA1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp1AE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1AE->tag != 6)goto _LLA3;else{_tmp1AF=_tmp1AE->f1;}}_LLA2:
# 984
 if(_tmp1AF == - 1){const char*_tmp3CE;return(_tmp3CE="aligned",_tag_dyneither(_tmp3CE,sizeof(char),8));}else{
# 986
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLC9: {
# 987
const char*_tmp3D2;void*_tmp3D1[1];struct Cyc_Int_pa_PrintArg_struct _tmp3D0;return(struct _dyneither_ptr)((_tmp3D0.tag=1,((_tmp3D0.f1=(unsigned long)_tmp1AF,((_tmp3D1[0]=& _tmp3D0,Cyc_aprintf(((_tmp3D2="aligned(%d)",_tag_dyneither(_tmp3D2,sizeof(char),12))),_tag_dyneither(_tmp3D1,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c: _LLCA: {
# 988
const char*_tmp3D6;void*_tmp3D5[1];struct Cyc_Int_pa_PrintArg_struct _tmp3D4;return(struct _dyneither_ptr)((_tmp3D4.tag=1,((_tmp3D4.f1=(unsigned long)_tmp1AF,((_tmp3D5[0]=& _tmp3D4,Cyc_aprintf(((_tmp3D6="align(%d)",_tag_dyneither(_tmp3D6,sizeof(char),10))),_tag_dyneither(_tmp3D5,sizeof(void*),1))))))));}}}_LLA3: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp1B0=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B0->tag != 7)goto _LLA5;}_LLA4: {
# 990
const char*_tmp3D7;return(_tmp3D7="packed",_tag_dyneither(_tmp3D7,sizeof(char),7));}_LLA5: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp1B1=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B1->tag != 8)goto _LLA7;else{_tmp1B2=_tmp1B1->f1;}}_LLA6: {
# 991
const char*_tmp3DB;void*_tmp3DA[1];struct Cyc_String_pa_PrintArg_struct _tmp3D9;return(struct _dyneither_ptr)((_tmp3D9.tag=0,((_tmp3D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1B2),((_tmp3DA[0]=& _tmp3D9,Cyc_aprintf(((_tmp3DB="section(\"%s\")",_tag_dyneither(_tmp3DB,sizeof(char),14))),_tag_dyneither(_tmp3DA,sizeof(void*),1))))))));}_LLA7: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp1B3=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B3->tag != 9)goto _LLA9;}_LLA8: {
# 992
const char*_tmp3DC;return(_tmp3DC="nocommon",_tag_dyneither(_tmp3DC,sizeof(char),9));}_LLA9: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp1B4=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B4->tag != 10)goto _LLAB;}_LLAA: {
# 993
const char*_tmp3DD;return(_tmp3DD="shared",_tag_dyneither(_tmp3DD,sizeof(char),7));}_LLAB: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp1B5=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B5->tag != 11)goto _LLAD;}_LLAC: {
# 994
const char*_tmp3DE;return(_tmp3DE="unused",_tag_dyneither(_tmp3DE,sizeof(char),7));}_LLAD: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp1B6=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B6->tag != 12)goto _LLAF;}_LLAE: {
# 995
const char*_tmp3DF;return(_tmp3DF="weak",_tag_dyneither(_tmp3DF,sizeof(char),5));}_LLAF: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp1B7=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B7->tag != 13)goto _LLB1;}_LLB0: {
# 996
const char*_tmp3E0;return(_tmp3E0="dllimport",_tag_dyneither(_tmp3E0,sizeof(char),10));}_LLB1: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp1B8=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B8->tag != 14)goto _LLB3;}_LLB2: {
# 997
const char*_tmp3E1;return(_tmp3E1="dllexport",_tag_dyneither(_tmp3E1,sizeof(char),10));}_LLB3: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp1B9=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1B9->tag != 15)goto _LLB5;}_LLB4: {
# 998
const char*_tmp3E2;return(_tmp3E2="no_instrument_function",_tag_dyneither(_tmp3E2,sizeof(char),23));}_LLB5: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp1BA=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1BA->tag != 16)goto _LLB7;}_LLB6: {
# 999
const char*_tmp3E3;return(_tmp3E3="constructor",_tag_dyneither(_tmp3E3,sizeof(char),12));}_LLB7: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp1BB=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1BB->tag != 17)goto _LLB9;}_LLB8: {
# 1000
const char*_tmp3E4;return(_tmp3E4="destructor",_tag_dyneither(_tmp3E4,sizeof(char),11));}_LLB9: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp1BC=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1BC->tag != 18)goto _LLBB;}_LLBA: {
# 1001
const char*_tmp3E5;return(_tmp3E5="no_check_memory_usage",_tag_dyneither(_tmp3E5,sizeof(char),22));}_LLBB: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1BD=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1BD->tag != 19)goto _LLBD;else{_tmp1BE=_tmp1BD->f1;if(_tmp1BE != Cyc_Absyn_Printf_ft)goto _LLBD;_tmp1BF=_tmp1BD->f2;_tmp1C0=_tmp1BD->f3;}}_LLBC: {
# 1002
const char*_tmp3EA;void*_tmp3E9[2];struct Cyc_Int_pa_PrintArg_struct _tmp3E8;struct Cyc_Int_pa_PrintArg_struct _tmp3E7;return(struct _dyneither_ptr)((_tmp3E7.tag=1,((_tmp3E7.f1=(unsigned int)_tmp1C0,((_tmp3E8.tag=1,((_tmp3E8.f1=(unsigned int)_tmp1BF,((_tmp3E9[0]=& _tmp3E8,((_tmp3E9[1]=& _tmp3E7,Cyc_aprintf(((_tmp3EA="format(printf,%u,%u)",_tag_dyneither(_tmp3EA,sizeof(char),21))),_tag_dyneither(_tmp3E9,sizeof(void*),2))))))))))))));}_LLBD: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1C1=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1C1->tag != 19)goto _LLBF;else{_tmp1C2=_tmp1C1->f1;if(_tmp1C2 != Cyc_Absyn_Scanf_ft)goto _LLBF;_tmp1C3=_tmp1C1->f2;_tmp1C4=_tmp1C1->f3;}}_LLBE: {
# 1003
const char*_tmp3EF;void*_tmp3EE[2];struct Cyc_Int_pa_PrintArg_struct _tmp3ED;struct Cyc_Int_pa_PrintArg_struct _tmp3EC;return(struct _dyneither_ptr)((_tmp3EC.tag=1,((_tmp3EC.f1=(unsigned int)_tmp1C4,((_tmp3ED.tag=1,((_tmp3ED.f1=(unsigned int)_tmp1C3,((_tmp3EE[0]=& _tmp3ED,((_tmp3EE[1]=& _tmp3EC,Cyc_aprintf(((_tmp3EF="format(scanf,%u,%u)",_tag_dyneither(_tmp3EF,sizeof(char),20))),_tag_dyneither(_tmp3EE,sizeof(void*),2))))))))))))));}_LLBF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1C5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1C5->tag != 20)goto _LLC1;else{_tmp1C6=_tmp1C5->f1;}}_LLC0: {
# 1004
const char*_tmp3F3;void*_tmp3F2[1];struct Cyc_Int_pa_PrintArg_struct _tmp3F1;return(struct _dyneither_ptr)((_tmp3F1.tag=1,((_tmp3F1.f1=(unsigned long)_tmp1C6,((_tmp3F2[0]=& _tmp3F1,Cyc_aprintf(((_tmp3F3="initializes(%d)",_tag_dyneither(_tmp3F3,sizeof(char),16))),_tag_dyneither(_tmp3F2,sizeof(void*),1))))))));}_LLC1: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1C7=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1C7->tag != 21)goto _LLC3;else{_tmp1C8=_tmp1C7->f1;}}_LLC2: {
# 1005
const char*_tmp3F7;void*_tmp3F6[1];struct Cyc_Int_pa_PrintArg_struct _tmp3F5;return(struct _dyneither_ptr)((_tmp3F5.tag=1,((_tmp3F5.f1=(unsigned long)_tmp1C8,((_tmp3F6[0]=& _tmp3F5,Cyc_aprintf(((_tmp3F7="noliveunique(%d)",_tag_dyneither(_tmp3F7,sizeof(char),17))),_tag_dyneither(_tmp3F6,sizeof(void*),1))))))));}_LLC3: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1C9=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1C9->tag != 22)goto _LLC5;else{_tmp1CA=_tmp1C9->f1;}}_LLC4: {
# 1006
const char*_tmp3FB;void*_tmp3FA[1];struct Cyc_Int_pa_PrintArg_struct _tmp3F9;return(struct _dyneither_ptr)((_tmp3F9.tag=1,((_tmp3F9.f1=(unsigned long)_tmp1CA,((_tmp3FA[0]=& _tmp3F9,Cyc_aprintf(((_tmp3FB="noconsume(%d)",_tag_dyneither(_tmp3FB,sizeof(char),14))),_tag_dyneither(_tmp3FA,sizeof(void*),1))))))));}_LLC5: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp1CB=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1CB->tag != 23)goto _LLC7;}_LLC6: {
# 1007
const char*_tmp3FC;return(_tmp3FC="pure",_tag_dyneither(_tmp3FC,sizeof(char),5));}_LLC7: {struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp1CC=(struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmp1A6;if(_tmp1CC->tag != 24)goto _LL94;else{_tmp1CD=_tmp1CC->f1;}}_LLC8: {
# 1008
const char*_tmp400;void*_tmp3FF[1];struct Cyc_String_pa_PrintArg_struct _tmp3FE;return(struct _dyneither_ptr)((_tmp3FE.tag=0,((_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1CD),((_tmp3FF[0]=& _tmp3FE,Cyc_aprintf(((_tmp400="__mode__(\"%s\")",_tag_dyneither(_tmp400,sizeof(char),15))),_tag_dyneither(_tmp3FF,sizeof(void*),1))))))));}_LL94:;}
# 1013
int Cyc_Absyn_fntype_att(void*a){
# 1014
void*_tmp200=a;int _tmp202;_LLCD: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp201=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp200;if(_tmp201->tag != 0)goto _LLCF;else{_tmp202=_tmp201->f1;}}_LLCE:
# 1015
 goto _LLD0;_LLCF: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp203=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp200;if(_tmp203->tag != 3)goto _LLD1;}_LLD0:
# 1016
 goto _LLD2;_LLD1: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp204=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp200;if(_tmp204->tag != 1)goto _LLD3;}_LLD2:
# 1017
 goto _LLD4;_LLD3: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp205=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp200;if(_tmp205->tag != 2)goto _LLD5;}_LLD4:
# 1018
 goto _LLD6;_LLD5: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp206=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp200;if(_tmp206->tag != 4)goto _LLD7;}_LLD6:
# 1019
 goto _LLD8;_LLD7: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp207=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp200;if(_tmp207->tag != 23)goto _LLD9;}_LLD8:
# 1020
 goto _LLDA;_LLD9: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp208=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp200;if(_tmp208->tag != 19)goto _LLDB;}_LLDA:
# 1021
 goto _LLDC;_LLDB: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp209=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp200;if(_tmp209->tag != 5)goto _LLDD;}_LLDC:
# 1022
 goto _LLDE;_LLDD: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp20A=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp200;if(_tmp20A->tag != 21)goto _LLDF;}_LLDE:
# 1023
 goto _LLE0;_LLDF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp20B=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp200;if(_tmp20B->tag != 20)goto _LLE1;}_LLE0:
# 1024
 goto _LLE2;_LLE1: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp20C=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp200;if(_tmp20C->tag != 22)goto _LLE3;}_LLE2:
# 1025
 return 1;_LLE3:;_LLE4:
# 1026
 return 0;_LLCC:;}static char _tmp20D[3]="f0";
# 1031
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp20D,_tmp20D,_tmp20D + 3};
# 1032
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
# 1033
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(int i);static void _tmp40E(unsigned int*fsz,unsigned int*_tmp40D,unsigned int*_tmp40C,struct _dyneither_ptr***_tmp40A){for(*_tmp40D=0;*_tmp40D < *_tmp40C;(*_tmp40D)++){struct Cyc_Int_pa_PrintArg_struct _tmp408;void*_tmp407[1];const char*_tmp406;struct _dyneither_ptr*_tmp405;(*_tmp40A)[*_tmp40D]=
# 1039
*_tmp40D < *fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)*_tmp40D)):((_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405[0]=(struct _dyneither_ptr)((_tmp408.tag=1,((_tmp408.f1=(unsigned long)((int)*_tmp40D),((_tmp407[0]=& _tmp408,Cyc_aprintf(((_tmp406="f%d",_tag_dyneither(_tmp406,sizeof(char),4))),_tag_dyneither(_tmp407,sizeof(void*),1)))))))),_tmp405))));}}
# 1034
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
# 1035
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
# 1036
if(i >= fsz){
# 1037
unsigned int _tmp40D;unsigned int _tmp40C;struct _dyneither_ptr _tmp40B;struct _dyneither_ptr**_tmp40A;unsigned int _tmp409;Cyc_Absyn_field_names=(
# 1038
(_tmp409=(unsigned int)(i + 1),((_tmp40A=(struct _dyneither_ptr**)_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp409)),((_tmp40B=_tag_dyneither(_tmp40A,sizeof(struct _dyneither_ptr*),_tmp409),((((_tmp40C=_tmp409,_tmp40E(& fsz,& _tmp40D,& _tmp40C,& _tmp40A))),_tmp40B))))))));}
# 1040
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1043
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){
# 1044
union Cyc_Absyn_AggrInfoU _tmp216=info;struct _tuple2 _tmp217;enum Cyc_Absyn_AggrKind _tmp218;struct _tuple0*_tmp219;struct Cyc_Absyn_Aggrdecl**_tmp21A;struct Cyc_Absyn_Aggrdecl*_tmp21B;struct Cyc_Absyn_Aggrdecl _tmp21C;enum Cyc_Absyn_AggrKind _tmp21D;struct _tuple0*_tmp21E;_LLE6: if((_tmp216.UnknownAggr).tag != 1)goto _LLE8;_tmp217=(struct _tuple2)(_tmp216.UnknownAggr).val;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;_LLE7: {
# 1045
struct _tuple11 _tmp40F;return(_tmp40F.f1=_tmp218,((_tmp40F.f2=_tmp219,_tmp40F)));}_LLE8: if((_tmp216.KnownAggr).tag != 2)goto _LLE5;_tmp21A=(struct Cyc_Absyn_Aggrdecl**)(_tmp216.KnownAggr).val;_tmp21B=*_tmp21A;_tmp21C=*_tmp21B;_tmp21D=_tmp21C.kind;_tmp21E=_tmp21C.name;_LLE9: {
# 1046
struct _tuple11 _tmp410;return(_tmp410.f1=_tmp21D,((_tmp410.f2=_tmp21E,_tmp410)));}_LLE5:;}
# 1049
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){
# 1050
union Cyc_Absyn_AggrInfoU _tmp221=info;struct _tuple2 _tmp222;struct Cyc_Absyn_Aggrdecl**_tmp223;struct Cyc_Absyn_Aggrdecl*_tmp224;_LLEB: if((_tmp221.UnknownAggr).tag != 1)goto _LLED;_tmp222=(struct _tuple2)(_tmp221.UnknownAggr).val;_LLEC: {
# 1051
const char*_tmp413;void*_tmp412;(_tmp412=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp413="unchecked aggrdecl",_tag_dyneither(_tmp413,sizeof(char),19))),_tag_dyneither(_tmp412,sizeof(void*),0)));}_LLED: if((_tmp221.KnownAggr).tag != 2)goto _LLEA;_tmp223=(struct Cyc_Absyn_Aggrdecl**)(_tmp221.KnownAggr).val;_tmp224=*_tmp223;_LLEE:
# 1052
 return _tmp224;_LLEA:;}
# 1055
int Cyc_Absyn_is_union_type(void*t){
# 1056
void*_tmp227=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp229;struct Cyc_Absyn_AggrInfo _tmp22B;union Cyc_Absyn_AggrInfoU _tmp22C;_LLF0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp227;if(_tmp228->tag != 12)goto _LLF2;else{_tmp229=_tmp228->f1;if(_tmp229 != Cyc_Absyn_UnionA)goto _LLF2;}}_LLF1:
# 1057
 return 1;_LLF2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp227;if(_tmp22A->tag != 11)goto _LLF4;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22B.aggr_info;}}_LLF3:
# 1058
 return(Cyc_Absyn_aggr_kinded_name(_tmp22C)).f1 == Cyc_Absyn_UnionA;_LLF4:;_LLF5:
# 1059
 return 0;_LLEF:;}
# 1062
int Cyc_Absyn_is_nontagged_union_type(void*t){
# 1063
void*_tmp22D=Cyc_Tcutil_compress(t);enum Cyc_Absyn_AggrKind _tmp22F;struct Cyc_Absyn_AggrInfo _tmp231;union Cyc_Absyn_AggrInfoU _tmp232;_LLF7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp22D;if(_tmp22E->tag != 12)goto _LLF9;else{_tmp22F=_tmp22E->f1;if(_tmp22F != Cyc_Absyn_UnionA)goto _LLF9;}}_LLF8:
# 1064
 return 1;_LLF9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp22D;if(_tmp230->tag != 11)goto _LLFB;else{_tmp231=_tmp230->f1;_tmp232=_tmp231.aggr_info;}}_LLFA: {
# 1066
union Cyc_Absyn_AggrInfoU _tmp233=_tmp232;struct Cyc_Absyn_Aggrdecl**_tmp234;struct Cyc_Absyn_Aggrdecl*_tmp235;struct _tuple2 _tmp236;enum Cyc_Absyn_AggrKind _tmp237;struct Cyc_Core_Opt*_tmp238;struct _tuple2 _tmp239;enum Cyc_Absyn_AggrKind _tmp23A;struct Cyc_Core_Opt*_tmp23B;struct Cyc_Core_Opt _tmp23C;int _tmp23D;_LLFE: if((_tmp233.KnownAggr).tag != 2)goto _LL100;_tmp234=(struct Cyc_Absyn_Aggrdecl**)(_tmp233.KnownAggr).val;_tmp235=*_tmp234;_LLFF:
# 1068
 return _tmp235->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp235->impl))->tagged == 0;_LL100: if((_tmp233.UnknownAggr).tag != 1)goto _LL102;_tmp236=(struct _tuple2)(_tmp233.UnknownAggr).val;_tmp237=_tmp236.f1;_tmp238=_tmp236.f3;if(_tmp238 != 0)goto _LL102;_LL101:
# 1069
 return _tmp237 == Cyc_Absyn_UnionA;_LL102: if((_tmp233.UnknownAggr).tag != 1)goto _LLFD;_tmp239=(struct _tuple2)(_tmp233.UnknownAggr).val;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f3;if(_tmp23B == 0)goto _LLFD;_tmp23C=*_tmp23B;_tmp23D=(int)_tmp23C.v;_LL103:
# 1070
 return _tmp23A == Cyc_Absyn_UnionA  && !_tmp23D;_LLFD:;}_LLFB:;_LLFC:
# 1072
 return 0;_LLF6:;}
# 1075
int Cyc_Absyn_is_aggr_type(void*t){
# 1076
void*_tmp23E=Cyc_Tcutil_compress(t);_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23E;if(_tmp23F->tag != 3)goto _LL107;}_LL106:
# 1077
 goto _LL108;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23E;if(_tmp240->tag != 4)goto _LL109;}_LL108:
# 1078
 goto _LL10A;_LL109: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23E;if(_tmp241->tag != 10)goto _LL10B;}_LL10A:
# 1079
 goto _LL10C;_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23E;if(_tmp242->tag != 11)goto _LL10D;}_LL10C:
# 1080
 goto _LL10E;_LL10D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp243=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23E;if(_tmp243->tag != 12)goto _LL10F;}_LL10E:
# 1081
 return 1;_LL10F:;_LL110:
# 1082
 return 0;_LL104:;}
# 1086
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={14};
# 1087
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={15};
# 1096 "absyn.cyc"
int Cyc_Absyn_porting_c_code=0;
# 1097
int Cyc_Absyn_no_regions=0;
