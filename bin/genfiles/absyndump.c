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
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 480
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
# 481
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
# 482
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 488
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 507
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 533
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 540
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 558
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 719 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 914
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 924
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 932
void*Cyc_Absyn_compress_kb(void*);
# 937
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 967
extern void*Cyc_Absyn_bounds_one;
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1151
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1157
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 79
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
# 80
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
# 81
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
# 82
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
# 83
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
# 84
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
# 85
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 87
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 27 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 35 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 46
static int Cyc_Absyndump_expand_typedefs;
# 50
static int Cyc_Absyndump_qvar_to_Cids;
# 53
static int Cyc_Absyndump_add_cyc_prefix;
# 56
static int Cyc_Absyndump_generate_line_directives;
# 60
static int Cyc_Absyndump_to_VC;
# 62
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
# 63
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
# 64
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
# 65
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
# 66
Cyc_Absyndump_to_VC=fs->to_VC;
# 67
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 77 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 80
void Cyc_Absyndump_dumptyp(void*);
# 81
void Cyc_Absyndump_dumpntyp(void*t);
# 82
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
# 83
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
# 84
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
# 85
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
# 86
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int);
# 87
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
# 88
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a);
# 89
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
# 90
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
# 91
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 95
typedef struct _dyneither_ptr Cyc_Absyndump_dump_string_t;
# 96
void Cyc_Absyndump_dumploc(unsigned int);
# 99
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 101
void Cyc_Absyndump_ignore(void*x){return;}
# 103
static unsigned int Cyc_Absyndump_pos=0;
# 104
static char Cyc_Absyndump_prev_char='x';
# 106
int Cyc_Absyndump_need_space_before(){
# 107
switch(Cyc_Absyndump_prev_char){case '{': _LL0:
# 108
 goto _LL1;case '}': _LL1:
# 109
 goto _LL2;case '(': _LL2:
# 110
 goto _LL3;case ')': _LL3:
# 111
 goto _LL4;case '[': _LL4:
# 112
 goto _LL5;case ']': _LL5:
# 113
 goto _LL6;case ';': _LL6:
# 114
 goto _LL7;case ',': _LL7:
# 115
 goto _LL8;case '=': _LL8:
# 116
 goto _LL9;case '?': _LL9:
# 117
 goto _LLA;case '!': _LLA:
# 118
 goto _LLB;case ' ': _LLB:
# 119
 goto _LLC;case '\n': _LLC:
# 120
 goto _LLD;case '*': _LLD:
# 121
 return 0;default: _LLE:
# 122
 return 1;}}
# 126
void Cyc_Absyndump_dump(struct _dyneither_ptr s){
# 127
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 133
if(Cyc_Absyndump_need_space_before())
# 134
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
# 135
if(sz >= 1){
# 136
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - 1)));
# 137
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 141
void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
# 142
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 144
if(sz >= 1){
# 145
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
# 146
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 149
void Cyc_Absyndump_dump_char(int c){
# 151
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
# 152
Cyc_Absyndump_prev_char=(char)c;}static char _tmpA[1]="";
# 155
void Cyc_Absyndump_dumploc(unsigned int loc){
# 156
static struct _dyneither_ptr last_file={_tmpA,_tmpA,_tmpA + 1};
# 157
static unsigned int last_line=0;
# 158
if(loc == 0)return;
# 159
if(!Cyc_Absyndump_generate_line_directives)return;{
# 161
struct _dyneither_ptr _tmp1;unsigned int _tmp2;struct _tuple13 _tmp0=Cyc_Lex_xlate_pos(loc);_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;
# 162
if((char*)_tmp1.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmp1.curr == (char*)last_file.curr  && _tmp2 == last_line)return;
# 163
if((char*)_tmp1.curr == (char*)last_file.curr){
# 164
const char*_tmp414;void*_tmp413[1];struct Cyc_Int_pa_PrintArg_struct _tmp412;(_tmp412.tag=1,((_tmp412.f1=(unsigned long)((int)_tmp2),((_tmp413[0]=& _tmp412,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp414="\n# %d\n",_tag_dyneither(_tmp414,sizeof(char),7))),_tag_dyneither(_tmp413,sizeof(void*),1)))))));}else{
# 166
const char*_tmp419;void*_tmp418[2];struct Cyc_Int_pa_PrintArg_struct _tmp417;struct Cyc_String_pa_PrintArg_struct _tmp416;(_tmp416.tag=0,((_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1),((_tmp417.tag=1,((_tmp417.f1=(unsigned long)((int)_tmp2),((_tmp418[0]=& _tmp417,((_tmp418[1]=& _tmp416,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp419="\n# %d %s\n",_tag_dyneither(_tmp419,sizeof(char),10))),_tag_dyneither(_tmp418,sizeof(void*),2)))))))))))));}
# 168
last_file=_tmp1;
# 169
last_line=_tmp2;};}
# 174
void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
# 175
Cyc_Absyndump_dump(*s);}
# 178
void Cyc_Absyndump_dump_semi(){
# 179
Cyc_Absyndump_dump_char((int)';');}
# 182
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
# 183
if(l == 0)
# 184
return;
# 185
for(0;l->tl != 0;l=l->tl){
# 186
f((void*)l->hd);
# 187
Cyc_Absyndump_dump_nospace(sep);}
# 189
f((void*)l->hd);}
# 191
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
# 192
if(l == 0)
# 193
return;
# 194
for(0;l->tl != 0;l=l->tl){
# 195
f(env,(void*)l->hd);
# 196
Cyc_Absyndump_dump_nospace(sep);}
# 198
f(env,(void*)l->hd);}
# 200
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 202
Cyc_Absyndump_dump_nospace(start);
# 203
Cyc_Absyndump_dump_sep(f,l,sep);
# 204
Cyc_Absyndump_dump_nospace(end);}
# 206
void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 208
Cyc_Absyndump_dump_nospace(start);
# 209
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
# 210
Cyc_Absyndump_dump_nospace(end);}
# 212
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 214
if(l != 0)
# 215
Cyc_Absyndump_group(f,l,start,end,sep);}
# 220
void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
# 221
struct Cyc_List_List*_tmpB=0;
# 222
struct _dyneither_ptr**prefix=0;
# 223
{union Cyc_Absyn_Nmspace _tmpC=(*v).f1;int _tmpD;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;_LL11: if((_tmpC.Loc_n).tag != 4)goto _LL13;_tmpD=(int)(_tmpC.Loc_n).val;_LL12:
# 224
 _tmpE=0;goto _LL14;_LL13: if((_tmpC.Rel_n).tag != 1)goto _LL15;_tmpE=(struct Cyc_List_List*)(_tmpC.Rel_n).val;_LL14:
# 226
 _tmpB=_tmpE;
# 227
goto _LL10;_LL15: if((_tmpC.C_n).tag != 3)goto _LL17;_tmpF=(struct Cyc_List_List*)(_tmpC.C_n).val;_LL16:
# 232
 Cyc_Absyndump_dump_char((int)' ');
# 233
goto _LL10;_LL17: if((_tmpC.Abs_n).tag != 2)goto _LL10;_tmp10=(struct Cyc_List_List*)(_tmpC.Abs_n).val;_LL18:
# 235
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
# 236
prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
# 237
_tmpB=_tmp10;
# 238
goto _LL10;_LL10:;}
# 240
if(prefix != 0){
# 241
Cyc_Absyndump_dump_str(*prefix);
# 242
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
# 243
const char*_tmp41A;Cyc_Absyndump_dump_nospace(((_tmp41A="::",_tag_dyneither(_tmp41A,sizeof(char),3))));}}
# 245
if(_tmpB != 0){
# 246
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpB->hd));
# 247
for(_tmpB=_tmpB->tl;_tmpB != 0;_tmpB=_tmpB->tl){
# 250
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
# 251
const char*_tmp41B;Cyc_Absyndump_dump_nospace(((_tmp41B="::",_tag_dyneither(_tmp41B,sizeof(char),3))));}
# 252
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpB->hd));}
# 254
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp41C;Cyc_Absyndump_dump_nospace(((_tmp41C="_",_tag_dyneither(_tmp41C,sizeof(char),2))));}else{
# 255
const char*_tmp41D;Cyc_Absyndump_dump_nospace(((_tmp41D="::",_tag_dyneither(_tmp41D,sizeof(char),3))));}
# 256
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 258
if(prefix != 0)
# 259
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 261
Cyc_Absyndump_dump_str((*v).f2);}}}
# 265
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
# 266
if(tq.q_restrict){const char*_tmp41E;Cyc_Absyndump_dump(((_tmp41E="restrict",_tag_dyneither(_tmp41E,sizeof(char),9))));}
# 267
if(tq.q_volatile){const char*_tmp41F;Cyc_Absyndump_dump(((_tmp41F="volatile",_tag_dyneither(_tmp41F,sizeof(char),9))));}
# 268
if(tq.print_const){const char*_tmp420;Cyc_Absyndump_dump(((_tmp420="const",_tag_dyneither(_tmp420,sizeof(char),6))));}}
# 271
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
# 272
switch(sc){case Cyc_Absyn_Static: _LL19:
# 273
{const char*_tmp421;Cyc_Absyndump_dump(((_tmp421="static",_tag_dyneither(_tmp421,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
# 274
 return;case Cyc_Absyn_Extern: _LL1B:
# 275
{const char*_tmp422;Cyc_Absyndump_dump(((_tmp422="extern",_tag_dyneither(_tmp422,sizeof(char),7))));}return;case Cyc_Absyn_ExternC: _LL1C:
# 276
{const char*_tmp423;Cyc_Absyndump_dump(((_tmp423="extern \"C\"",_tag_dyneither(_tmp423,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:
# 277
{const char*_tmp424;Cyc_Absyndump_dump(((_tmp424="abstract",_tag_dyneither(_tmp424,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:
# 278
{const char*_tmp425;Cyc_Absyndump_dump(((_tmp425="register",_tag_dyneither(_tmp425,sizeof(char),9))));}return;}}
# 282
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
# 283
struct Cyc_Absyn_Kind _tmp1E;enum Cyc_Absyn_KindQual _tmp1F;enum Cyc_Absyn_AliasQual _tmp20;struct Cyc_Absyn_Kind*_tmp1D=ka;_tmp1E=*_tmp1D;_tmp1F=_tmp1E.kind;_tmp20=_tmp1E.aliasqual;
# 284
switch(_tmp1F){case Cyc_Absyn_AnyKind: _LL20:
# 286
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL22:
# 287
{const char*_tmp426;Cyc_Absyndump_dump(((_tmp426="A",_tag_dyneither(_tmp426,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL23:
# 288
{const char*_tmp427;Cyc_Absyndump_dump(((_tmp427="UA",_tag_dyneither(_tmp427,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:
# 289
{const char*_tmp428;Cyc_Absyndump_dump(((_tmp428="TA",_tag_dyneither(_tmp428,sizeof(char),3))));}return;}case Cyc_Absyn_MemKind: _LL21:
# 292
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL27:
# 293
{const char*_tmp429;Cyc_Absyndump_dump(((_tmp429="M",_tag_dyneither(_tmp429,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:
# 294
{const char*_tmp42A;Cyc_Absyndump_dump(((_tmp42A="UM",_tag_dyneither(_tmp42A,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL29:
# 295
{const char*_tmp42B;Cyc_Absyndump_dump(((_tmp42B="TM",_tag_dyneither(_tmp42B,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26:
# 298
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL2C:
# 299
{const char*_tmp42C;Cyc_Absyndump_dump(((_tmp42C="B",_tag_dyneither(_tmp42C,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:
# 300
{const char*_tmp42D;Cyc_Absyndump_dump(((_tmp42D="UB",_tag_dyneither(_tmp42D,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:
# 301
{const char*_tmp42E;Cyc_Absyndump_dump(((_tmp42E="TB",_tag_dyneither(_tmp42E,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind: _LL2B:
# 304
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL31:
# 305
{const char*_tmp42F;Cyc_Absyndump_dump(((_tmp42F="R",_tag_dyneither(_tmp42F,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL32:
# 306
{const char*_tmp430;Cyc_Absyndump_dump(((_tmp430="UR",_tag_dyneither(_tmp430,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:
# 307
{const char*_tmp431;Cyc_Absyndump_dump(((_tmp431="TR",_tag_dyneither(_tmp431,sizeof(char),3))));}return;}case Cyc_Absyn_EffKind: _LL30:
# 309
{const char*_tmp432;Cyc_Absyndump_dump(((_tmp432="E",_tag_dyneither(_tmp432,sizeof(char),2))));}return;case Cyc_Absyn_IntKind: _LL35:
# 310
{const char*_tmp433;Cyc_Absyndump_dump(((_tmp433="I",_tag_dyneither(_tmp433,sizeof(char),2))));}return;}}
# 314
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
# 315
switch(k){case Cyc_Absyn_StructA: _LL37:
# 316
{const char*_tmp434;Cyc_Absyndump_dump(((_tmp434="struct ",_tag_dyneither(_tmp434,sizeof(char),8))));}return;case Cyc_Absyn_UnionA: _LL38:
# 317
{const char*_tmp435;Cyc_Absyndump_dump(((_tmp435="union ",_tag_dyneither(_tmp435,sizeof(char),7))));}return;}}
# 321
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
# 322
const char*_tmp438;const char*_tmp437;const char*_tmp436;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((_tmp436="<",_tag_dyneither(_tmp436,sizeof(char),2))),((_tmp437=">",_tag_dyneither(_tmp437,sizeof(char),2))),((_tmp438=",",_tag_dyneither(_tmp438,sizeof(char),2))));}
# 324
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
# 325
struct _dyneither_ptr n=*tv->name;
# 326
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
# 327
{const char*_tmp439;Cyc_Absyndump_dump(((_tmp439="`TTT",_tag_dyneither(_tmp439,sizeof(char),5))));}
# 328
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 330
Cyc_Absyndump_dump(n);}}
# 332
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
# 333
Cyc_Absyndump_dumptvar(tv);{
# 334
void*_tmp35=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp38;struct Cyc_Absyn_Kind*_tmp3A;struct Cyc_Absyn_Kind _tmp3B;enum Cyc_Absyn_KindQual _tmp3C;enum Cyc_Absyn_AliasQual _tmp3D;struct Cyc_Absyn_Kind*_tmp3F;_LL3B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp36=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp36->tag != 1)goto _LL3D;}_LL3C:
# 335
 goto _LL3A;_LL3D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp37=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp37->tag != 2)goto _LL3F;else{_tmp38=_tmp37->f2;}}_LL3E:
# 336
 goto _LL3A;_LL3F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp39->tag != 0)goto _LL41;else{_tmp3A=_tmp39->f1;_tmp3B=*_tmp3A;_tmp3C=_tmp3B.kind;if(_tmp3C != Cyc_Absyn_BoxKind)goto _LL41;_tmp3D=_tmp3B.aliasqual;if(_tmp3D != Cyc_Absyn_Aliasable)goto _LL41;}}_LL40:
# 337
 goto _LL3A;_LL41: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp3E->tag != 0)goto _LL3A;else{_tmp3F=_tmp3E->f1;}}_LL42:
# 338
{const char*_tmp43A;Cyc_Absyndump_dump(((_tmp43A="::",_tag_dyneither(_tmp43A,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp3F);goto _LL3A;_LL3A:;};}
# 341
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
# 342
const char*_tmp43D;const char*_tmp43C;const char*_tmp43B;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp43B="<",_tag_dyneither(_tmp43B,sizeof(char),2))),((_tmp43C=">",_tag_dyneither(_tmp43C,sizeof(char),2))),((_tmp43D=",",_tag_dyneither(_tmp43D,sizeof(char),2))));}
# 344
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
# 345
const char*_tmp440;const char*_tmp43F;const char*_tmp43E;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((_tmp43E="<",_tag_dyneither(_tmp43E,sizeof(char),2))),((_tmp43F=">",_tag_dyneither(_tmp43F,sizeof(char),2))),((_tmp440=",",_tag_dyneither(_tmp440,sizeof(char),2))));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 348
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
# 349
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 351
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
# 352
const char*_tmp443;const char*_tmp442;const char*_tmp441;((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,((_tmp441="(",_tag_dyneither(_tmp441,sizeof(char),2))),((_tmp442=")",_tag_dyneither(_tmp442,sizeof(char),2))),((_tmp443=",",_tag_dyneither(_tmp443,sizeof(char),2))));}
# 355
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
# 356
for(0;atts != 0;atts=atts->tl){
# 357
void*_tmp4A=(void*)atts->hd;_LL44: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp4B=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp4A;if(_tmp4B->tag != 1)goto _LL46;}_LL45:
# 358
{const char*_tmp444;Cyc_Absyndump_dump(((_tmp444="_stdcall",_tag_dyneither(_tmp444,sizeof(char),9))));}return;_LL46: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp4C=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp4A;if(_tmp4C->tag != 2)goto _LL48;}_LL47:
# 359
{const char*_tmp445;Cyc_Absyndump_dump(((_tmp445="_cdecl",_tag_dyneither(_tmp445,sizeof(char),7))));}return;_LL48: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp4D=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp4A;if(_tmp4D->tag != 3)goto _LL4A;}_LL49:
# 360
{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="_fastcall",_tag_dyneither(_tmp446,sizeof(char),10))));}return;_LL4A:;_LL4B:
# 361
 goto _LL43;_LL43:;}}
# 365
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 367
int hasatt=0;
# 368
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
# 369
void*_tmp51=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL4D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp52=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp51;if(_tmp52->tag != 1)goto _LL4F;}_LL4E:
# 370
 goto _LL50;_LL4F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp53=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp51;if(_tmp53->tag != 2)goto _LL51;}_LL50:
# 371
 goto _LL52;_LL51: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp54=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp51;if(_tmp54->tag != 3)goto _LL53;}_LL52:
# 372
 goto _LL4C;_LL53:;_LL54:
# 373
 hasatt=1;goto _LL4C;_LL4C:;}}
# 375
if(!hasatt)
# 376
return;
# 377
{const char*_tmp447;Cyc_Absyndump_dump(((_tmp447="__declspec(",_tag_dyneither(_tmp447,sizeof(char),12))));}
# 378
for(0;atts != 0;atts=atts->tl){
# 379
void*_tmp56=(void*)atts->hd;_LL56: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp57=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp56;if(_tmp57->tag != 1)goto _LL58;}_LL57:
# 380
 goto _LL59;_LL58: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp58=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp56;if(_tmp58->tag != 2)goto _LL5A;}_LL59:
# 381
 goto _LL5B;_LL5A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp59=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp56;if(_tmp59->tag != 3)goto _LL5C;}_LL5B:
# 382
 goto _LL55;_LL5C:;_LL5D:
# 383
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL55;_LL55:;}
# 385
Cyc_Absyndump_dump_char((int)')');}
# 388
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
# 389
if(atts == 0)return;
# 390
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL5E:
# 392
{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448=" __attribute__((",_tag_dyneither(_tmp448,sizeof(char),17))));}
# 393
for(0;atts != 0;atts=atts->tl){
# 394
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
# 395
if(atts->tl != 0){const char*_tmp449;Cyc_Absyndump_dump(((_tmp449=",",_tag_dyneither(_tmp449,sizeof(char),2))));}}
# 397
{const char*_tmp44A;Cyc_Absyndump_dump(((_tmp44A=")) ",_tag_dyneither(_tmp44A,sizeof(char),4))));}
# 398
return;case Cyc_Cyclone_Vc_c: _LL5F:
# 400
 Cyc_Absyndump_dump_noncallconv(atts);
# 401
return;}}
# 405
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
# 406
if(tms == 0)return 0;{
# 407
void*_tmp5D=(void*)tms->hd;_LL62: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp5E=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp5D;if(_tmp5E->tag != 2)goto _LL64;}_LL63:
# 408
 return 1;_LL64:;_LL65:
# 409
 return 0;_LL61:;};}
# 413
static void Cyc_Absyndump_dumprgn(void*t){
# 414
void*_tmp5F=Cyc_Tcutil_compress(t);_LL67: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp60=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp5F;if(_tmp60->tag != 20)goto _LL69;}_LL68:
# 415
{const char*_tmp44B;Cyc_Absyndump_dump(((_tmp44B="`H",_tag_dyneither(_tmp44B,sizeof(char),3))));}goto _LL66;_LL69:;_LL6A:
# 416
 Cyc_Absyndump_dumpntyp(t);goto _LL66;_LL66:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 420
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
# 421
struct Cyc_List_List*rgions=0;
# 422
struct Cyc_List_List*effects=0;
# 424
{void*_tmp62=Cyc_Tcutil_compress(t);void*_tmp64;struct Cyc_List_List*_tmp66;_LL6C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp63=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp62;if(_tmp63->tag != 23)goto _LL6E;else{_tmp64=(void*)_tmp63->f1;}}_LL6D:
# 425
{struct Cyc_List_List*_tmp44C;rgions=((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C->hd=_tmp64,((_tmp44C->tl=rgions,_tmp44C))))));}goto _LL6B;_LL6E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp62;if(_tmp65->tag != 24)goto _LL70;else{_tmp66=_tmp65->f1;}}_LL6F:
# 427
 for(0;_tmp66 != 0;_tmp66=_tmp66->tl){
# 428
struct Cyc_List_List*_tmp69;struct Cyc_List_List*_tmp6A;struct _tuple15 _tmp68=Cyc_Absyndump_effects_split((void*)_tmp66->hd);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
# 429
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp69,rgions);
# 430
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6A,effects);}
# 432
goto _LL6B;_LL70:;_LL71:
# 433
{struct Cyc_List_List*_tmp44D;effects=((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->hd=t,((_tmp44D->tl=effects,_tmp44D))))));}goto _LL6B;_LL6B:;}{
# 435
struct _tuple15 _tmp44E;return(_tmp44E.f1=rgions,((_tmp44E.f2=effects,_tmp44E)));};}
# 438
static void Cyc_Absyndump_dumpeff(void*t){
# 439
struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp6F;struct _tuple15 _tmp6D=Cyc_Absyndump_effects_split(t);_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
# 440
_tmp6E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E);
# 441
_tmp6F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F);
# 442
if(_tmp6F != 0){
# 443
{struct Cyc_List_List*_tmp70=_tmp6F;for(0;_tmp70 != 0;_tmp70=_tmp70->tl){
# 444
Cyc_Absyndump_dumpntyp((void*)_tmp70->hd);
# 445
if(_tmp70->tl != 0)
# 446
Cyc_Absyndump_dump_char((int)'+');}}
# 448
if(_tmp6E != 0)Cyc_Absyndump_dump_char((int)'+');}
# 450
if(_tmp6E != 0  || _tmp6F == 0){
# 451
Cyc_Absyndump_dump_char((int)'{');
# 452
for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){
# 453
Cyc_Absyndump_dumprgn((void*)_tmp6E->hd);
# 454
if(_tmp6E->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 456
Cyc_Absyndump_dump_char((int)'}');}}
# 460
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
# 461
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
# 462
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 465
void Cyc_Absyndump_dumpntyp(void*t){
# 466
void*_tmp71=t;struct Cyc_Absyn_Tvar*_tmp77;struct Cyc_Core_Opt*_tmp79;void*_tmp7A;int _tmp7B;struct Cyc_Core_Opt*_tmp7D;void*_tmp7E;int _tmp7F;struct Cyc_Absyn_DatatypeInfo _tmp81;union Cyc_Absyn_DatatypeInfoU _tmp82;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_DatatypeFieldInfo _tmp85;union Cyc_Absyn_DatatypeFieldInfoU _tmp86;struct Cyc_List_List*_tmp87;enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8C;enum Cyc_Absyn_Size_of _tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of _tmp90;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;enum Cyc_Absyn_Sign _tmp95;enum Cyc_Absyn_Size_of _tmp96;enum Cyc_Absyn_Sign _tmp98;enum Cyc_Absyn_Size_of _tmp99;enum Cyc_Absyn_Sign _tmp9B;enum Cyc_Absyn_Size_of _tmp9C;enum Cyc_Absyn_Sign _tmp9E;enum Cyc_Absyn_Size_of _tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Size_of _tmpA2;enum Cyc_Absyn_Sign _tmpA4;enum Cyc_Absyn_Size_of _tmpA5;enum Cyc_Absyn_Sign _tmpA7;enum Cyc_Absyn_Size_of _tmpA8;enum Cyc_Absyn_Sign _tmpAA;enum Cyc_Absyn_Size_of _tmpAB;enum Cyc_Absyn_Sign _tmpAD;enum Cyc_Absyn_Size_of _tmpAE;enum Cyc_Absyn_Sign _tmpB0;enum Cyc_Absyn_Size_of _tmpB1;enum Cyc_Absyn_Sign _tmpB3;enum Cyc_Absyn_Size_of _tmpB4;int _tmpB6;struct Cyc_List_List*_tmpB8;struct Cyc_Absyn_AggrInfo _tmpBA;union Cyc_Absyn_AggrInfoU _tmpBB;struct Cyc_List_List*_tmpBC;enum Cyc_Absyn_AggrKind _tmpBE;struct Cyc_List_List*_tmpBF;struct _tuple0*_tmpC1;struct Cyc_List_List*_tmpC3;struct _tuple0*_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Exp*_tmpC8;void*_tmpCA;void*_tmpCC;void*_tmpCD;void*_tmpCF;struct Cyc_Absyn_TypeDecl*_tmpD3;struct Cyc_Absyn_TypeDecl _tmpD4;void*_tmpD5;struct Cyc_Absyn_Aggrdecl*_tmpD7;struct Cyc_Absyn_TypeDecl*_tmpD9;struct Cyc_Absyn_TypeDecl _tmpDA;void*_tmpDB;struct Cyc_Absyn_Enumdecl*_tmpDD;struct Cyc_Absyn_TypeDecl*_tmpDF;struct Cyc_Absyn_TypeDecl _tmpE0;void*_tmpE1;struct Cyc_Absyn_Datatypedecl*_tmpE3;void*_tmpE6;void*_tmpE8;struct Cyc_List_List*_tmpEA;_LL73: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp72=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp71;if(_tmp72->tag != 8)goto _LL75;}_LL74:
# 468
 goto _LL76;_LL75: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp71;if(_tmp73->tag != 9)goto _LL77;}_LL76:
# 469
 goto _LL78;_LL77: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71;if(_tmp74->tag != 5)goto _LL79;}_LL78:
# 470
 return;_LL79: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp71;if(_tmp75->tag != 0)goto _LL7B;}_LL7A:
# 471
{const char*_tmp44F;Cyc_Absyndump_dump(((_tmp44F="void",_tag_dyneither(_tmp44F,sizeof(char),5))));}return;_LL7B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp71;if(_tmp76->tag != 2)goto _LL7D;else{_tmp77=_tmp76->f1;}}_LL7C:
# 472
 Cyc_Absyndump_dumptvar(_tmp77);return;_LL7D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71;if(_tmp78->tag != 1)goto _LL7F;else{_tmp79=_tmp78->f1;_tmp7A=(void*)_tmp78->f2;if(_tmp7A != 0)goto _LL7F;_tmp7B=_tmp78->f3;}}_LL7E:
# 474
{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450="%",_tag_dyneither(_tmp450,sizeof(char),2))));}
# 475
if(_tmp79 == 0){const char*_tmp451;Cyc_Absyndump_dump(((_tmp451="?",_tag_dyneither(_tmp451,sizeof(char),2))));}else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp79->v);}
# 476
{const char*_tmp455;void*_tmp454[1];struct Cyc_Int_pa_PrintArg_struct _tmp453;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp453.tag=1,((_tmp453.f1=(unsigned long)_tmp7B,((_tmp454[0]=& _tmp453,Cyc_aprintf(((_tmp455="(%d)",_tag_dyneither(_tmp455,sizeof(char),5))),_tag_dyneither(_tmp454,sizeof(void*),1)))))))));}return;_LL7F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71;if(_tmp7C->tag != 1)goto _LL81;else{_tmp7D=_tmp7C->f1;_tmp7E=(void*)_tmp7C->f2;_tmp7F=_tmp7C->f3;}}_LL80:
# 477
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp7E));return;_LL81: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp71;if(_tmp80->tag != 3)goto _LL83;else{_tmp81=_tmp80->f1;_tmp82=_tmp81.datatype_info;_tmp83=_tmp81.targs;}}_LL82:
# 479
{union Cyc_Absyn_DatatypeInfoU _tmpF1=_tmp82;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF2;struct _tuple0*_tmpF3;int _tmpF4;struct Cyc_Absyn_Datatypedecl**_tmpF5;struct Cyc_Absyn_Datatypedecl*_tmpF6;struct Cyc_Absyn_Datatypedecl _tmpF7;struct _tuple0*_tmpF8;int _tmpF9;_LLCC: if((_tmpF1.UnknownDatatype).tag != 1)goto _LLCE;_tmpF2=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpF1.UnknownDatatype).val;_tmpF3=_tmpF2.name;_tmpF4=_tmpF2.is_extensible;_LLCD:
# 480
 _tmpF8=_tmpF3;_tmpF9=_tmpF4;goto _LLCF;_LLCE: if((_tmpF1.KnownDatatype).tag != 2)goto _LLCB;_tmpF5=(struct Cyc_Absyn_Datatypedecl**)(_tmpF1.KnownDatatype).val;_tmpF6=*_tmpF5;_tmpF7=*_tmpF6;_tmpF8=_tmpF7.name;_tmpF9=_tmpF7.is_extensible;_LLCF:
# 482
 if(_tmpF9){const char*_tmp456;Cyc_Absyndump_dump(((_tmp456="@extensible ",_tag_dyneither(_tmp456,sizeof(char),13))));}
# 483
{const char*_tmp457;Cyc_Absyndump_dump(((_tmp457="datatype ",_tag_dyneither(_tmp457,sizeof(char),10))));}
# 484
Cyc_Absyndump_dumpqvar(_tmpF8);
# 485
Cyc_Absyndump_dumptps(_tmp83);
# 486
goto _LLCB;_LLCB:;}
# 488
goto _LL72;_LL83: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp84=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp71;if(_tmp84->tag != 4)goto _LL85;else{_tmp85=_tmp84->f1;_tmp86=_tmp85.field_info;_tmp87=_tmp85.targs;}}_LL84:
# 490
{union Cyc_Absyn_DatatypeFieldInfoU _tmpFC=_tmp86;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpFD;struct _tuple0*_tmpFE;struct _tuple0*_tmpFF;int _tmp100;struct _tuple1 _tmp101;struct Cyc_Absyn_Datatypedecl*_tmp102;struct Cyc_Absyn_Datatypedecl _tmp103;struct _tuple0*_tmp104;int _tmp105;struct Cyc_Absyn_Datatypefield*_tmp106;struct Cyc_Absyn_Datatypefield _tmp107;struct _tuple0*_tmp108;_LLD1: if((_tmpFC.UnknownDatatypefield).tag != 1)goto _LLD3;_tmpFD=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpFC.UnknownDatatypefield).val;_tmpFE=_tmpFD.datatype_name;_tmpFF=_tmpFD.field_name;_tmp100=_tmpFD.is_extensible;_LLD2:
# 492
 _tmp104=_tmpFE;_tmp105=_tmp100;_tmp108=_tmpFF;goto _LLD4;_LLD3: if((_tmpFC.KnownDatatypefield).tag != 2)goto _LLD0;_tmp101=(struct _tuple1)(_tmpFC.KnownDatatypefield).val;_tmp102=_tmp101.f1;_tmp103=*_tmp102;_tmp104=_tmp103.name;_tmp105=_tmp103.is_extensible;_tmp106=_tmp101.f2;_tmp107=*_tmp106;_tmp108=_tmp107.name;_LLD4:
# 495
 if(_tmp105){const char*_tmp458;Cyc_Absyndump_dump(((_tmp458="@extensible ",_tag_dyneither(_tmp458,sizeof(char),13))));}
# 496
{const char*_tmp459;Cyc_Absyndump_dump(((_tmp459="datatype ",_tag_dyneither(_tmp459,sizeof(char),10))));}
# 497
Cyc_Absyndump_dumpqvar(_tmp104);
# 498
{const char*_tmp45A;Cyc_Absyndump_dump(((_tmp45A=".",_tag_dyneither(_tmp45A,sizeof(char),2))));}
# 499
Cyc_Absyndump_dumpqvar(_tmp108);
# 500
Cyc_Absyndump_dumptps(_tmp87);
# 501
goto _LLD0;_LLD0:;}
# 503
goto _LL72;_LL85: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp88->tag != 6)goto _LL87;else{_tmp89=_tmp88->f1;if(_tmp89 != Cyc_Absyn_None)goto _LL87;_tmp8A=_tmp88->f2;if(_tmp8A != Cyc_Absyn_Int_sz)goto _LL87;}}_LL86:
# 504
 goto _LL88;_LL87: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp8B->tag != 6)goto _LL89;else{_tmp8C=_tmp8B->f1;if(_tmp8C != Cyc_Absyn_Signed)goto _LL89;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Int_sz)goto _LL89;}}_LL88:
# 505
{const char*_tmp45B;Cyc_Absyndump_dump(((_tmp45B="int",_tag_dyneither(_tmp45B,sizeof(char),4))));}return;_LL89: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp8E->tag != 6)goto _LL8B;else{_tmp8F=_tmp8E->f1;if(_tmp8F != Cyc_Absyn_None)goto _LL8B;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A:
# 506
 goto _LL8C;_LL8B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp91->tag != 6)goto _LL8D;else{_tmp92=_tmp91->f1;if(_tmp92 != Cyc_Absyn_Signed)goto _LL8D;_tmp93=_tmp91->f2;if(_tmp93 != Cyc_Absyn_Long_sz)goto _LL8D;}}_LL8C:
# 507
{const char*_tmp45C;Cyc_Absyndump_dump(((_tmp45C="long",_tag_dyneither(_tmp45C,sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp94->tag != 6)goto _LL8F;else{_tmp95=_tmp94->f1;if(_tmp95 != Cyc_Absyn_None)goto _LL8F;_tmp96=_tmp94->f2;if(_tmp96 != Cyc_Absyn_Char_sz)goto _LL8F;}}_LL8E:
# 508
{const char*_tmp45D;Cyc_Absyndump_dump(((_tmp45D="char",_tag_dyneither(_tmp45D,sizeof(char),5))));}return;_LL8F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp97->tag != 6)goto _LL91;else{_tmp98=_tmp97->f1;if(_tmp98 != Cyc_Absyn_Signed)goto _LL91;_tmp99=_tmp97->f2;if(_tmp99 != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:
# 509
{const char*_tmp45E;Cyc_Absyndump_dump(((_tmp45E="signed char",_tag_dyneither(_tmp45E,sizeof(char),12))));}return;_LL91: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp9A->tag != 6)goto _LL93;else{_tmp9B=_tmp9A->f1;if(_tmp9B != Cyc_Absyn_Unsigned)goto _LL93;_tmp9C=_tmp9A->f2;if(_tmp9C != Cyc_Absyn_Char_sz)goto _LL93;}}_LL92:
# 510
{const char*_tmp45F;Cyc_Absyndump_dump(((_tmp45F="unsigned char",_tag_dyneither(_tmp45F,sizeof(char),14))));}return;_LL93: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmp9D->tag != 6)goto _LL95;else{_tmp9E=_tmp9D->f1;if(_tmp9E != Cyc_Absyn_None)goto _LL95;_tmp9F=_tmp9D->f2;if(_tmp9F != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94:
# 511
 goto _LL96;_LL95: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpA0->tag != 6)goto _LL97;else{_tmpA1=_tmpA0->f1;if(_tmpA1 != Cyc_Absyn_Signed)goto _LL97;_tmpA2=_tmpA0->f2;if(_tmpA2 != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:
# 512
{const char*_tmp460;Cyc_Absyndump_dump(((_tmp460="short",_tag_dyneither(_tmp460,sizeof(char),6))));}return;_LL97: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpA3->tag != 6)goto _LL99;else{_tmpA4=_tmpA3->f1;if(_tmpA4 != Cyc_Absyn_Unsigned)goto _LL99;_tmpA5=_tmpA3->f2;if(_tmpA5 != Cyc_Absyn_Short_sz)goto _LL99;}}_LL98:
# 513
{const char*_tmp461;Cyc_Absyndump_dump(((_tmp461="unsigned short",_tag_dyneither(_tmp461,sizeof(char),15))));}return;_LL99: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpA6->tag != 6)goto _LL9B;else{_tmpA7=_tmpA6->f1;if(_tmpA7 != Cyc_Absyn_Unsigned)goto _LL9B;_tmpA8=_tmpA6->f2;if(_tmpA8 != Cyc_Absyn_Int_sz)goto _LL9B;}}_LL9A:
# 514
{const char*_tmp462;Cyc_Absyndump_dump(((_tmp462="unsigned int",_tag_dyneither(_tmp462,sizeof(char),13))));}return;_LL9B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpA9->tag != 6)goto _LL9D;else{_tmpAA=_tmpA9->f1;if(_tmpAA != Cyc_Absyn_Unsigned)goto _LL9D;_tmpAB=_tmpA9->f2;if(_tmpAB != Cyc_Absyn_Long_sz)goto _LL9D;}}_LL9C:
# 515
{const char*_tmp463;Cyc_Absyndump_dump(((_tmp463="unsigned long",_tag_dyneither(_tmp463,sizeof(char),14))));}return;_LL9D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpAC->tag != 6)goto _LL9F;else{_tmpAD=_tmpAC->f1;if(_tmpAD != Cyc_Absyn_None)goto _LL9F;_tmpAE=_tmpAC->f2;if(_tmpAE != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E:
# 516
 goto _LLA0;_LL9F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpAF->tag != 6)goto _LLA1;else{_tmpB0=_tmpAF->f1;if(_tmpB0 != Cyc_Absyn_Signed)goto _LLA1;_tmpB1=_tmpAF->f2;if(_tmpB1 != Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0:
# 518
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLD5:
# 519
{const char*_tmp464;Cyc_Absyndump_dump(((_tmp464="long long",_tag_dyneither(_tmp464,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLD6:
# 520
{const char*_tmp465;Cyc_Absyndump_dump(((_tmp465="__int64",_tag_dyneither(_tmp465,sizeof(char),8))));}return;}_LLA1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71;if(_tmpB2->tag != 6)goto _LLA3;else{_tmpB3=_tmpB2->f1;if(_tmpB3 != Cyc_Absyn_Unsigned)goto _LLA3;_tmpB4=_tmpB2->f2;if(_tmpB4 != Cyc_Absyn_LongLong_sz)goto _LLA3;}}_LLA2:
# 523
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LLD8:
# 524
{const char*_tmp466;Cyc_Absyndump_dump(((_tmp466="unsigned __int64",_tag_dyneither(_tmp466,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLD9:
# 525
{const char*_tmp467;Cyc_Absyndump_dump(((_tmp467="unsigned long long",_tag_dyneither(_tmp467,sizeof(char),19))));}return;}_LLA3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp71;if(_tmpB5->tag != 7)goto _LLA5;else{_tmpB6=_tmpB5->f1;}}_LLA4:
# 528
 if(_tmpB6 == 0){
# 529
const char*_tmp468;Cyc_Absyndump_dump(((_tmp468="float",_tag_dyneither(_tmp468,sizeof(char),6))));}else{
# 530
if(_tmpB6 == 1){
# 531
const char*_tmp469;Cyc_Absyndump_dump(((_tmp469="double",_tag_dyneither(_tmp469,sizeof(char),7))));}else{
# 533
const char*_tmp46A;Cyc_Absyndump_dump(((_tmp46A="long double",_tag_dyneither(_tmp46A,sizeof(char),12))));}}
# 534
return;_LLA5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB7=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp71;if(_tmpB7->tag != 10)goto _LLA7;else{_tmpB8=_tmpB7->f1;}}_LLA6:
# 535
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpB8);return;_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp71;if(_tmpB9->tag != 11)goto _LLA9;else{_tmpBA=_tmpB9->f1;_tmpBB=_tmpBA.aggr_info;_tmpBC=_tmpBA.targs;}}_LLA8: {
# 537
enum Cyc_Absyn_AggrKind _tmp11D;struct _tuple0*_tmp11E;struct _tuple10 _tmp11C=Cyc_Absyn_aggr_kinded_name(_tmpBB);_tmp11D=_tmp11C.f1;_tmp11E=_tmp11C.f2;
# 538
Cyc_Absyndump_dumpaggr_kind(_tmp11D);Cyc_Absyndump_dumpqvar(_tmp11E);Cyc_Absyndump_dumptps(_tmpBC);
# 539
return;}_LLA9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpBD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp71;if(_tmpBD->tag != 12)goto _LLAB;else{_tmpBE=_tmpBD->f1;_tmpBF=_tmpBD->f2;}}_LLAA:
# 541
 Cyc_Absyndump_dumpaggr_kind(_tmpBE);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpBF);Cyc_Absyndump_dump_char((int)'}');return;_LLAB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpC0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp71;if(_tmpC0->tag != 13)goto _LLAD;else{_tmpC1=_tmpC0->f1;}}_LLAC:
# 542
{const char*_tmp46B;Cyc_Absyndump_dump(((_tmp46B="enum ",_tag_dyneither(_tmp46B,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpC1);return;_LLAD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpC2=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp71;if(_tmpC2->tag != 14)goto _LLAF;else{_tmpC3=_tmpC2->f1;}}_LLAE:
# 543
{const char*_tmp46C;Cyc_Absyndump_dump(((_tmp46C="enum {",_tag_dyneither(_tmp46C,sizeof(char),7))));}Cyc_Absyndump_dumpenumfields(_tmpC3);{const char*_tmp46D;Cyc_Absyndump_dump(((_tmp46D="}",_tag_dyneither(_tmp46D,sizeof(char),2))));}return;_LLAF: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpC4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp71;if(_tmpC4->tag != 17)goto _LLB1;else{_tmpC5=_tmpC4->f1;_tmpC6=_tmpC4->f2;}}_LLB0:
# 544
(Cyc_Absyndump_dumpqvar(_tmpC5),Cyc_Absyndump_dumptps(_tmpC6));return;_LLB1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpC7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp71;if(_tmpC7->tag != 18)goto _LLB3;else{_tmpC8=_tmpC7->f1;}}_LLB2:
# 545
{const char*_tmp46E;Cyc_Absyndump_dump(((_tmp46E="valueof_t(",_tag_dyneither(_tmp46E,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpC8);{const char*_tmp46F;Cyc_Absyndump_dump(((_tmp46F=")",_tag_dyneither(_tmp46F,sizeof(char),2))));}return;_LLB3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpC9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp71;if(_tmpC9->tag != 15)goto _LLB5;else{_tmpCA=(void*)_tmpC9->f1;}}_LLB4:
# 547
{const char*_tmp470;Cyc_Absyndump_dump(((_tmp470="region_t<",_tag_dyneither(_tmp470,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpCA);{const char*_tmp471;Cyc_Absyndump_dump(((_tmp471=">",_tag_dyneither(_tmp471,sizeof(char),2))));}return;_LLB5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp71;if(_tmpCB->tag != 16)goto _LLB7;else{_tmpCC=(void*)_tmpCB->f1;_tmpCD=(void*)_tmpCB->f2;}}_LLB6:
# 548
{const char*_tmp472;Cyc_Absyndump_dump(((_tmp472="dynregion_t<",_tag_dyneither(_tmp472,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpCC);
# 549
{const char*_tmp473;Cyc_Absyndump_dump(((_tmp473=",",_tag_dyneither(_tmp473,sizeof(char),2))));}Cyc_Absyndump_dumprgn(_tmpCD);{const char*_tmp474;Cyc_Absyndump_dump(((_tmp474=">",_tag_dyneither(_tmp474,sizeof(char),2))));}return;_LLB7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpCE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp71;if(_tmpCE->tag != 19)goto _LLB9;else{_tmpCF=(void*)_tmpCE->f1;}}_LLB8:
# 550
{const char*_tmp475;Cyc_Absyndump_dump(((_tmp475="tag_t<",_tag_dyneither(_tmp475,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(_tmpCF);{const char*_tmp476;Cyc_Absyndump_dump(((_tmp476=">",_tag_dyneither(_tmp476,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpD0=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp71;if(_tmpD0->tag != 21)goto _LLBB;}_LLBA:
# 551
{const char*_tmp477;Cyc_Absyndump_dump(((_tmp477="`U",_tag_dyneither(_tmp477,sizeof(char),3))));}goto _LL72;_LLBB: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpD1=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp71;if(_tmpD1->tag != 22)goto _LLBD;}_LLBC:
# 552
{const char*_tmp478;Cyc_Absyndump_dump(((_tmp478="`RC",_tag_dyneither(_tmp478,sizeof(char),4))));}goto _LL72;_LLBD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71;if(_tmpD2->tag != 26)goto _LLBF;else{_tmpD3=_tmpD2->f1;_tmpD4=*_tmpD3;_tmpD5=_tmpD4.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpD6=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpD5;if(_tmpD6->tag != 0)goto _LLBF;else{_tmpD7=_tmpD6->f1;}};}}_LLBE:
# 554
 Cyc_Absyndump_dump_aggrdecl(_tmpD7);return;_LLBF: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71;if(_tmpD8->tag != 26)goto _LLC1;else{_tmpD9=_tmpD8->f1;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpDC=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpDB;if(_tmpDC->tag != 1)goto _LLC1;else{_tmpDD=_tmpDC->f1;}};}}_LLC0:
# 556
 Cyc_Absyndump_dump_enumdecl(_tmpDD);return;_LLC1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpDE=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp71;if(_tmpDE->tag != 26)goto _LLC3;else{_tmpDF=_tmpDE->f1;_tmpE0=*_tmpDF;_tmpE1=_tmpE0.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpE2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpE1;if(_tmpE2->tag != 2)goto _LLC3;else{_tmpE3=_tmpE2->f1;}};}}_LLC2:
# 558
 Cyc_Absyndump_dump_datatypedecl(_tmpE3);return;_LLC3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpE4=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp71;if(_tmpE4->tag != 20)goto _LLC5;}_LLC4:
# 559
{const char*_tmp479;Cyc_Absyndump_dump(((_tmp479="`H",_tag_dyneither(_tmp479,sizeof(char),3))));}goto _LL72;_LLC5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE5=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp71;if(_tmpE5->tag != 23)goto _LLC7;else{_tmpE6=(void*)_tmpE5->f1;}}_LLC6:
# 560
{const char*_tmp47A;Cyc_Absyndump_dump(((_tmp47A="{",_tag_dyneither(_tmp47A,sizeof(char),2))));}Cyc_Absyndump_dumptyp(_tmpE6);{const char*_tmp47B;Cyc_Absyndump_dump(((_tmp47B="}",_tag_dyneither(_tmp47B,sizeof(char),2))));}goto _LL72;_LLC7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp71;if(_tmpE7->tag != 25)goto _LLC9;else{_tmpE8=(void*)_tmpE7->f1;}}_LLC8:
# 561
{const char*_tmp47C;Cyc_Absyndump_dump(((_tmp47C="regions(",_tag_dyneither(_tmp47C,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmpE8);{const char*_tmp47D;Cyc_Absyndump_dump(((_tmp47D=")",_tag_dyneither(_tmp47D,sizeof(char),2))));}goto _LL72;_LLC9: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp71;if(_tmpE9->tag != 24)goto _LL72;else{_tmpEA=_tmpE9->f1;}}_LLCA:
# 563
 for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){
# 564
Cyc_Absyndump_dumptyp((void*)_tmpEA->hd);
# 565
if(_tmpEA->tl != 0){const char*_tmp47E;Cyc_Absyndump_dump(((_tmp47E="+",_tag_dyneither(_tmp47E,sizeof(char),2))));}}_LL72:;}
# 570
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
# 571
if(vo != 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo);}
# 573
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
# 574
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 576
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
# 577
struct _tuple16 _tmp134;void*_tmp135;void*_tmp136;struct _tuple16*_tmp133=cmp;_tmp134=*_tmp133;_tmp135=_tmp134.f1;_tmp136=_tmp134.f2;
# 578
Cyc_Absyndump_dumpeff(_tmp135);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp136);}
# 580
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
# 581
const char*_tmp47F;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,((_tmp47F=",",_tag_dyneither(_tmp47F,sizeof(char),2))));}
# 583
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 586
Cyc_Absyndump_dump_char((int)'(');
# 587
for(0;args != 0;args=args->tl){
# 588
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
# 589
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 591
if(c_varargs){
# 592
const char*_tmp480;Cyc_Absyndump_dump(((_tmp480="...",_tag_dyneither(_tmp480,sizeof(char),4))));}else{
# 593
if(cyc_varargs != 0){
# 594
struct _tuple8*_tmp481;struct _tuple8*_tmp139=(_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481->f1=cyc_varargs->name,((_tmp481->f2=cyc_varargs->tq,((_tmp481->f3=cyc_varargs->type,_tmp481)))))));
# 595
{const char*_tmp482;Cyc_Absyndump_dump(((_tmp482="...",_tag_dyneither(_tmp482,sizeof(char),4))));}
# 596
if(cyc_varargs->inject){const char*_tmp483;Cyc_Absyndump_dump(((_tmp483=" inject ",_tag_dyneither(_tmp483,sizeof(char),9))));}
# 597
Cyc_Absyndump_dumpfunarg(_tmp139);}}
# 599
if(effopt != 0){
# 600
Cyc_Absyndump_dump_semi();
# 601
Cyc_Absyndump_dumpeff((void*)effopt);}
# 603
if(rgn_po != 0){
# 604
Cyc_Absyndump_dump_char((int)':');
# 605
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 607
Cyc_Absyndump_dump_char((int)')');}
# 610
void Cyc_Absyndump_dumptyp(void*t){
# 611
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 614
void Cyc_Absyndump_dumpdesignator(void*d){
# 615
void*_tmp13D=d;struct Cyc_Absyn_Exp*_tmp13F;struct _dyneither_ptr*_tmp141;_LLDC: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp13E=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp13D;if(_tmp13E->tag != 0)goto _LLDE;else{_tmp13F=_tmp13E->f1;}}_LLDD:
# 616
{const char*_tmp484;Cyc_Absyndump_dump(((_tmp484=".[",_tag_dyneither(_tmp484,sizeof(char),3))));}Cyc_Absyndump_dumpexp(_tmp13F);Cyc_Absyndump_dump_char((int)']');goto _LLDB;_LLDE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp140=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp13D;if(_tmp140->tag != 1)goto _LLDB;else{_tmp141=_tmp140->f1;}}_LLDF:
# 617
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp141);goto _LLDB;_LLDB:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 621
void Cyc_Absyndump_dumpde(struct _tuple17*de){
# 622
{const char*_tmp487;const char*_tmp486;const char*_tmp485;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp485="",_tag_dyneither(_tmp485,sizeof(char),1))),((_tmp486="=",_tag_dyneither(_tmp486,sizeof(char),2))),((_tmp487="=",_tag_dyneither(_tmp487,sizeof(char),2))));}
# 623
Cyc_Absyndump_dumpexp((*de).f2);}
# 626
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
# 627
const char*_tmp48A;const char*_tmp489;const char*_tmp488;((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp488="",_tag_dyneither(_tmp488,sizeof(char),1))),((_tmp489="",_tag_dyneither(_tmp489,sizeof(char),1))),((_tmp48A=",",_tag_dyneither(_tmp48A,sizeof(char),2))));}
# 630
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
# 631
int myprec=Cyc_Absynpp_exp_prec(e);
# 632
if(inprec >= myprec){
# 633
const char*_tmp48B;Cyc_Absyndump_dump_nospace(((_tmp48B="(",_tag_dyneither(_tmp48B,sizeof(char),2))));}
# 634
{void*_tmp14A=e->r;union Cyc_Absyn_Cnst _tmp14C;struct _tuple3 _tmp14D;enum Cyc_Absyn_Sign _tmp14E;char _tmp14F;union Cyc_Absyn_Cnst _tmp151;struct _dyneither_ptr _tmp152;union Cyc_Absyn_Cnst _tmp154;struct _tuple4 _tmp155;enum Cyc_Absyn_Sign _tmp156;short _tmp157;union Cyc_Absyn_Cnst _tmp159;struct _tuple5 _tmp15A;enum Cyc_Absyn_Sign _tmp15B;int _tmp15C;union Cyc_Absyn_Cnst _tmp15E;struct _tuple5 _tmp15F;enum Cyc_Absyn_Sign _tmp160;int _tmp161;union Cyc_Absyn_Cnst _tmp163;struct _tuple5 _tmp164;enum Cyc_Absyn_Sign _tmp165;int _tmp166;union Cyc_Absyn_Cnst _tmp168;struct _tuple6 _tmp169;enum Cyc_Absyn_Sign _tmp16A;long long _tmp16B;union Cyc_Absyn_Cnst _tmp16D;struct _tuple7 _tmp16E;struct _dyneither_ptr _tmp16F;union Cyc_Absyn_Cnst _tmp171;int _tmp172;union Cyc_Absyn_Cnst _tmp174;struct _dyneither_ptr _tmp175;union Cyc_Absyn_Cnst _tmp177;struct _dyneither_ptr _tmp178;struct _tuple0*_tmp17A;enum Cyc_Absyn_Primop _tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Core_Opt*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp183;enum Cyc_Absyn_Incrementor _tmp184;struct Cyc_Absyn_Exp*_tmp186;enum Cyc_Absyn_Incrementor _tmp187;struct Cyc_Absyn_Exp*_tmp189;enum Cyc_Absyn_Incrementor _tmp18A;struct Cyc_Absyn_Exp*_tmp18C;enum Cyc_Absyn_Incrementor _tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;void*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AB;void*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;void*_tmp1B1;int _tmp1B3;struct _dyneither_ptr _tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;struct _dyneither_ptr*_tmp1B7;void*_tmp1B9;void*_tmp1BA;struct _dyneither_ptr*_tmp1BC;void*_tmp1BE;void*_tmp1BF;unsigned int _tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C5;struct _dyneither_ptr*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C8;struct _dyneither_ptr*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_List_List*_tmp1CE;struct _tuple8*_tmp1D0;struct Cyc_List_List*_tmp1D1;struct Cyc_List_List*_tmp1D3;struct Cyc_Absyn_Vardecl*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;struct _tuple0*_tmp1D9;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DF;struct Cyc_Absyn_Datatypefield*_tmp1E0;struct _tuple0*_tmp1E2;struct _tuple0*_tmp1E4;struct Cyc_Absyn_MallocInfo _tmp1E6;int _tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;void**_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Core_Opt*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_Absyn_Stmt*_tmp1F2;_LLE1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp14B->tag != 0)goto _LLE3;else{_tmp14C=_tmp14B->f1;if((_tmp14C.Char_c).tag != 2)goto _LLE3;_tmp14D=(struct _tuple3)(_tmp14C.Char_c).val;_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;}}_LLE2:
# 636
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp14F));Cyc_Absyndump_dump_char((int)'\'');
# 637
goto _LLE0;_LLE3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp150->tag != 0)goto _LLE5;else{_tmp151=_tmp150->f1;if((_tmp151.Wchar_c).tag != 3)goto _LLE5;_tmp152=(struct _dyneither_ptr)(_tmp151.Wchar_c).val;}}_LLE4:
# 638
{const char*_tmp48F;void*_tmp48E[1];struct Cyc_String_pa_PrintArg_struct _tmp48D;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp48D.tag=0,((_tmp48D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp152),((_tmp48E[0]=& _tmp48D,Cyc_aprintf(((_tmp48F="L'%s'",_tag_dyneither(_tmp48F,sizeof(char),6))),_tag_dyneither(_tmp48E,sizeof(void*),1)))))))));}goto _LLE0;_LLE5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp153->tag != 0)goto _LLE7;else{_tmp154=_tmp153->f1;if((_tmp154.Short_c).tag != 4)goto _LLE7;_tmp155=(struct _tuple4)(_tmp154.Short_c).val;_tmp156=_tmp155.f1;_tmp157=_tmp155.f2;}}_LLE6:
# 639
{const char*_tmp493;void*_tmp492[1];struct Cyc_Int_pa_PrintArg_struct _tmp491;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp491.tag=1,((_tmp491.f1=(unsigned long)((int)_tmp157),((_tmp492[0]=& _tmp491,Cyc_aprintf(((_tmp493="%d",_tag_dyneither(_tmp493,sizeof(char),3))),_tag_dyneither(_tmp492,sizeof(void*),1)))))))));}goto _LLE0;_LLE7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp158->tag != 0)goto _LLE9;else{_tmp159=_tmp158->f1;if((_tmp159.Int_c).tag != 5)goto _LLE9;_tmp15A=(struct _tuple5)(_tmp159.Int_c).val;_tmp15B=_tmp15A.f1;if(_tmp15B != Cyc_Absyn_None)goto _LLE9;_tmp15C=_tmp15A.f2;}}_LLE8:
# 640
 _tmp161=_tmp15C;goto _LLEA;_LLE9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp15D->tag != 0)goto _LLEB;else{_tmp15E=_tmp15D->f1;if((_tmp15E.Int_c).tag != 5)goto _LLEB;_tmp15F=(struct _tuple5)(_tmp15E.Int_c).val;_tmp160=_tmp15F.f1;if(_tmp160 != Cyc_Absyn_Signed)goto _LLEB;_tmp161=_tmp15F.f2;}}_LLEA:
# 641
{const char*_tmp497;void*_tmp496[1];struct Cyc_Int_pa_PrintArg_struct _tmp495;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp495.tag=1,((_tmp495.f1=(unsigned long)_tmp161,((_tmp496[0]=& _tmp495,Cyc_aprintf(((_tmp497="%d",_tag_dyneither(_tmp497,sizeof(char),3))),_tag_dyneither(_tmp496,sizeof(void*),1)))))))));}goto _LLE0;_LLEB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp162->tag != 0)goto _LLED;else{_tmp163=_tmp162->f1;if((_tmp163.Int_c).tag != 5)goto _LLED;_tmp164=(struct _tuple5)(_tmp163.Int_c).val;_tmp165=_tmp164.f1;if(_tmp165 != Cyc_Absyn_Unsigned)goto _LLED;_tmp166=_tmp164.f2;}}_LLEC:
# 642
{const char*_tmp49B;void*_tmp49A[1];struct Cyc_Int_pa_PrintArg_struct _tmp499;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp499.tag=1,((_tmp499.f1=(unsigned int)_tmp166,((_tmp49A[0]=& _tmp499,Cyc_aprintf(((_tmp49B="%u",_tag_dyneither(_tmp49B,sizeof(char),3))),_tag_dyneither(_tmp49A,sizeof(void*),1)))))))));}goto _LLE0;_LLED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp167->tag != 0)goto _LLEF;else{_tmp168=_tmp167->f1;if((_tmp168.LongLong_c).tag != 6)goto _LLEF;_tmp169=(struct _tuple6)(_tmp168.LongLong_c).val;_tmp16A=_tmp169.f1;_tmp16B=_tmp169.f2;}}_LLEE:
# 644
{const char*_tmp49C;Cyc_Absyndump_dump(((_tmp49C="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp49C,sizeof(char),27))));}goto _LLE0;_LLEF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp16C->tag != 0)goto _LLF1;else{_tmp16D=_tmp16C->f1;if((_tmp16D.Float_c).tag != 7)goto _LLF1;_tmp16E=(struct _tuple7)(_tmp16D.Float_c).val;_tmp16F=_tmp16E.f1;}}_LLF0:
# 645
 Cyc_Absyndump_dump(_tmp16F);goto _LLE0;_LLF1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp170->tag != 0)goto _LLF3;else{_tmp171=_tmp170->f1;if((_tmp171.Null_c).tag != 1)goto _LLF3;_tmp172=(int)(_tmp171.Null_c).val;}}_LLF2:
# 646
{const char*_tmp49D;Cyc_Absyndump_dump(((_tmp49D="NULL",_tag_dyneither(_tmp49D,sizeof(char),5))));}goto _LLE0;_LLF3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp173->tag != 0)goto _LLF5;else{_tmp174=_tmp173->f1;if((_tmp174.String_c).tag != 8)goto _LLF5;_tmp175=(struct _dyneither_ptr)(_tmp174.String_c).val;}}_LLF4:
# 648
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp175));Cyc_Absyndump_dump_char((int)'"');
# 649
goto _LLE0;_LLF5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp176->tag != 0)goto _LLF7;else{_tmp177=_tmp176->f1;if((_tmp177.Wstring_c).tag != 9)goto _LLF7;_tmp178=(struct _dyneither_ptr)(_tmp177.Wstring_c).val;}}_LLF6:
# 651
{const char*_tmp49E;Cyc_Absyndump_dump(((_tmp49E="L\"",_tag_dyneither(_tmp49E,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(_tmp178);Cyc_Absyndump_dump_char((int)'"');
# 652
goto _LLE0;_LLF7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp179->tag != 1)goto _LLF9;else{_tmp17A=_tmp179->f1;}}_LLF8:
# 654
 Cyc_Absyndump_dumpqvar(_tmp17A);goto _LLE0;_LLF9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp17B->tag != 2)goto _LLFB;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLFA: {
# 657
struct _dyneither_ptr _tmp202=Cyc_Absynpp_prim2str(_tmp17C);
# 658
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp17D)){case 1: _LL14B:
# 660
 if(_tmp17C == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){
# 661
{const char*_tmp49F;Cyc_Absyndump_dump(((_tmp49F="numelts(",_tag_dyneither(_tmp49F,sizeof(char),9))));}
# 662
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd);{
# 663
const char*_tmp4A0;Cyc_Absyndump_dump(((_tmp4A0=")",_tag_dyneither(_tmp4A0,sizeof(char),2))));};}else{
# 665
Cyc_Absyndump_dump(_tmp202);
# 666
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd);}
# 668
break;case 2: _LL14C:
# 670
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd);
# 671
Cyc_Absyndump_dump(_tmp202);
# 672
Cyc_Absyndump_dump_char((int)' ');
# 673
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp17D->tl))->hd);
# 674
break;default: _LL14D: {
# 676
struct Cyc_Core_Failure_exn_struct _tmp4A6;const char*_tmp4A5;struct Cyc_Core_Failure_exn_struct*_tmp4A4;(int)_throw((void*)((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=((_tmp4A6.tag=Cyc_Core_Failure,((_tmp4A6.f1=((_tmp4A5="Absyndump -- Bad number of arguments to primop",_tag_dyneither(_tmp4A5,sizeof(char),47))),_tmp4A6)))),_tmp4A4)))));}}
# 678
goto _LLE0;}_LLFB: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp17E->tag != 3)goto _LLFD;else{_tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;_tmp181=_tmp17E->f3;}}_LLFC:
# 681
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17F);
# 682
if(_tmp180 != 0)
# 683
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp180->v));
# 684
{const char*_tmp4A7;Cyc_Absyndump_dump_nospace(((_tmp4A7="=",_tag_dyneither(_tmp4A7,sizeof(char),2))));}
# 685
Cyc_Absyndump_dumpexp_prec(myprec,_tmp181);
# 686
goto _LLE0;_LLFD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp182->tag != 4)goto _LLFF;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;if(_tmp184 != Cyc_Absyn_PreInc)goto _LLFF;}}_LLFE:
# 688
{const char*_tmp4A8;Cyc_Absyndump_dump(((_tmp4A8="++",_tag_dyneither(_tmp4A8,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp183);goto _LLE0;_LLFF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp185->tag != 4)goto _LL101;else{_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;if(_tmp187 != Cyc_Absyn_PreDec)goto _LL101;}}_LL100:
# 689
{const char*_tmp4A9;Cyc_Absyndump_dump(((_tmp4A9="--",_tag_dyneither(_tmp4A9,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp186);goto _LLE0;_LL101: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp188->tag != 4)goto _LL103;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f2;if(_tmp18A != Cyc_Absyn_PostInc)goto _LL103;}}_LL102:
# 690
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp189);{const char*_tmp4AA;Cyc_Absyndump_dump(((_tmp4AA="++",_tag_dyneither(_tmp4AA,sizeof(char),3))));}goto _LLE0;_LL103: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp18B->tag != 4)goto _LL105;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;if(_tmp18D != Cyc_Absyn_PostDec)goto _LL105;}}_LL104:
# 691
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp18C);{const char*_tmp4AB;Cyc_Absyndump_dump(((_tmp4AB="--",_tag_dyneither(_tmp4AB,sizeof(char),3))));}goto _LLE0;_LL105: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp18E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp18E->tag != 5)goto _LL107;else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;_tmp191=_tmp18E->f3;}}_LL106:
# 694
 Cyc_Absyndump_dumploc(e->loc);
# 695
Cyc_Absyndump_dumpexp_prec(myprec,_tmp18F);
# 696
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp190);
# 697
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp191);
# 698
goto _LLE0;_LL107: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp192=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp192->tag != 6)goto _LL109;else{_tmp193=_tmp192->f1;_tmp194=_tmp192->f2;}}_LL108:
# 701
 Cyc_Absyndump_dumploc(e->loc);
# 702
Cyc_Absyndump_dumpexp_prec(myprec,_tmp193);{const char*_tmp4AC;Cyc_Absyndump_dump(((_tmp4AC=" && ",_tag_dyneither(_tmp4AC,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp194);goto _LLE0;_LL109: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp195->tag != 7)goto _LL10B;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LL10A:
# 705
 Cyc_Absyndump_dumploc(e->loc);
# 706
Cyc_Absyndump_dumpexp_prec(myprec,_tmp196);{const char*_tmp4AD;Cyc_Absyndump_dump(((_tmp4AD=" || ",_tag_dyneither(_tmp4AD,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp197);goto _LLE0;_LL10B: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp198->tag != 8)goto _LL10D;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LL10C:
# 711
 Cyc_Absyndump_dumploc(e->loc);
# 712
Cyc_Absyndump_dump_char((int)'(');
# 713
Cyc_Absyndump_dumpexp_prec(myprec,_tmp199);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp19A);
# 714
Cyc_Absyndump_dump_char((int)')');
# 715
goto _LLE0;_LL10D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp19B->tag != 9)goto _LL10F;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL10E:
# 718
 Cyc_Absyndump_dumploc(e->loc);
# 719
Cyc_Absyndump_dumpexp_prec(myprec,_tmp19C);
# 720
{const char*_tmp4AE;Cyc_Absyndump_dump_nospace(((_tmp4AE="(",_tag_dyneither(_tmp4AE,sizeof(char),2))));}{
# 724
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
# 725
Cyc_Absyndump_generate_line_directives=
# 726
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp19C->loc == 0);
# 727
Cyc_Absyndump_dumpexps_prec(20,_tmp19D);
# 728
{const char*_tmp4AF;Cyc_Absyndump_dump_nospace(((_tmp4AF=")",_tag_dyneither(_tmp4AF,sizeof(char),2))));}
# 729
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
# 730
goto _LLE0;};_LL10F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp19E->tag != 10)goto _LL111;else{_tmp19F=_tmp19E->f1;}}_LL110:
# 733
 Cyc_Absyndump_dumploc(e->loc);
# 734
{const char*_tmp4B0;Cyc_Absyndump_dump(((_tmp4B0="throw",_tag_dyneither(_tmp4B0,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp19F);goto _LLE0;_LL111: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1A0->tag != 11)goto _LL113;else{_tmp1A1=_tmp1A0->f1;}}_LL112:
# 736
 _tmp1A3=_tmp1A1;goto _LL114;_LL113: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1A2->tag != 12)goto _LL115;else{_tmp1A3=_tmp1A2->f1;}}_LL114:
# 737
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp1A3);goto _LLE0;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1A4->tag != 13)goto _LL117;else{_tmp1A5=(void*)_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LL116:
# 740
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp1A5);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp1A6);
# 741
goto _LLE0;_LL117: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1A7->tag != 14)goto _LL119;else{_tmp1A8=_tmp1A7->f1;}}_LL118:
# 744
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp1A8);goto _LLE0;_LL119: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1A9->tag != 15)goto _LL11B;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}}_LL11A:
# 747
{const char*_tmp4B1;Cyc_Absyndump_dump(((_tmp4B1="new ",_tag_dyneither(_tmp4B1,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp1AB);goto _LLE0;_LL11B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1AC->tag != 16)goto _LL11D;else{_tmp1AD=(void*)_tmp1AC->f1;}}_LL11C:
# 750
{const char*_tmp4B2;Cyc_Absyndump_dump(((_tmp4B2="sizeof(",_tag_dyneither(_tmp4B2,sizeof(char),8))));}Cyc_Absyndump_dumptyp(_tmp1AD);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL11D: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1AE->tag != 17)goto _LL11F;else{_tmp1AF=_tmp1AE->f1;}}_LL11E:
# 753
{const char*_tmp4B3;Cyc_Absyndump_dump(((_tmp4B3="sizeof(",_tag_dyneither(_tmp4B3,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp1AF);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL11F: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1B0=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1B0->tag != 37)goto _LL121;else{_tmp1B1=(void*)_tmp1B0->f1;}}_LL120:
# 756
{const char*_tmp4B4;Cyc_Absyndump_dump(((_tmp4B4="valueof(",_tag_dyneither(_tmp4B4,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp1B1);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL121: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp1B2=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1B2->tag != 38)goto _LL123;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B2->f2;}}_LL122:
# 759
{const char*_tmp4B5;Cyc_Absyndump_dump(((_tmp4B5="__asm__",_tag_dyneither(_tmp4B5,sizeof(char),8))));}
# 760
if(_tmp1B3){const char*_tmp4B6;Cyc_Absyndump_dump(((_tmp4B6=" volatile ",_tag_dyneither(_tmp4B6,sizeof(char),11))));}
# 761
Cyc_Absyndump_dump_char((int)'(');
# 762
Cyc_Absyndump_dump_nospace(_tmp1B4);
# 763
Cyc_Absyndump_dump_char((int)')');
# 764
goto _LLE0;_LL123: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1B5=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1B5->tag != 36)goto _LL125;else{_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}_LL124:
# 767
{const char*_tmp4B7;Cyc_Absyndump_dump(((_tmp4B7="tagcheck(",_tag_dyneither(_tmp4B7,sizeof(char),10))));}Cyc_Absyndump_dumpexp(_tmp1B6);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp1B7);
# 768
Cyc_Absyndump_dump_char((int)')');
# 769
goto _LLE0;_LL125: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1B8=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1B8->tag != 18)goto _LL127;else{_tmp1B9=(void*)_tmp1B8->f1;_tmp1BA=(void*)_tmp1B8->f2;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1BB=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1BA;if(_tmp1BB->tag != 0)goto _LL127;else{_tmp1BC=_tmp1BB->f1;}};}}_LL126:
# 772
{const char*_tmp4B8;Cyc_Absyndump_dump(((_tmp4B8="offsetof(",_tag_dyneither(_tmp4B8,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp1B9);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp1BC);
# 773
Cyc_Absyndump_dump_char((int)')');
# 774
goto _LLE0;_LL127: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1BD->tag != 18)goto _LL129;else{_tmp1BE=(void*)_tmp1BD->f1;_tmp1BF=(void*)_tmp1BD->f2;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1C0=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1BF;if(_tmp1C0->tag != 1)goto _LL129;else{_tmp1C1=_tmp1C0->f1;}};}}_LL128:
# 777
{const char*_tmp4B9;Cyc_Absyndump_dump(((_tmp4B9="offsetof(",_tag_dyneither(_tmp4B9,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp1BE);Cyc_Absyndump_dump_char((int)',');{const char*_tmp4BD;void*_tmp4BC[1];struct Cyc_Int_pa_PrintArg_struct _tmp4BB;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp4BB.tag=1,((_tmp4BB.f1=(unsigned long)((int)_tmp1C1),((_tmp4BC[0]=& _tmp4BB,Cyc_aprintf(((_tmp4BD="%d",_tag_dyneither(_tmp4BD,sizeof(char),3))),_tag_dyneither(_tmp4BC,sizeof(void*),1)))))))));}
# 778
Cyc_Absyndump_dump_char((int)')');
# 779
goto _LLE0;_LL129: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1C2->tag != 19)goto _LL12B;else{_tmp1C3=_tmp1C2->f1;}}_LL12A:
# 782
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp1C3);goto _LLE0;_LL12B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1C4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1C4->tag != 20)goto _LL12D;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;}}_LL12C:
# 785
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp1C5);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp1C6);
# 786
goto _LLE0;_LL12D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1C7->tag != 21)goto _LL12F;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL12E:
# 789
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp1C8);{const char*_tmp4BE;Cyc_Absyndump_dump_nospace(((_tmp4BE="->",_tag_dyneither(_tmp4BE,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp1C9);
# 790
goto _LLE0;_LL12F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1CA->tag != 22)goto _LL131;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;}}_LL130:
# 793
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp1CB);
# 794
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp1CC);Cyc_Absyndump_dump_char((int)']');goto _LLE0;_LL131: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1CD->tag != 23)goto _LL133;else{_tmp1CE=_tmp1CD->f1;}}_LL132:
# 797
{const char*_tmp4BF;Cyc_Absyndump_dump(((_tmp4BF="$(",_tag_dyneither(_tmp4BF,sizeof(char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp1CE);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL133: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp1CF=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1CF->tag != 24)goto _LL135;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1CF->f2;}}_LL134:
# 800
 Cyc_Absyndump_dump_char((int)'(');
# 801
Cyc_Absyndump_dumptyp((*_tmp1D0).f3);
# 802
Cyc_Absyndump_dump_char((int)')');
# 803
{const char*_tmp4C2;const char*_tmp4C1;const char*_tmp4C0;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1D1,((_tmp4C0="{",_tag_dyneither(_tmp4C0,sizeof(char),2))),((_tmp4C1="}",_tag_dyneither(_tmp4C1,sizeof(char),2))),((_tmp4C2=",",_tag_dyneither(_tmp4C2,sizeof(char),2))));}
# 804
goto _LLE0;_LL135: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1D2=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1D2->tag != 25)goto _LL137;else{_tmp1D3=_tmp1D2->f1;}}_LL136:
# 807
{const char*_tmp4C5;const char*_tmp4C4;const char*_tmp4C3;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1D3,((_tmp4C3="{",_tag_dyneither(_tmp4C3,sizeof(char),2))),((_tmp4C4="}",_tag_dyneither(_tmp4C4,sizeof(char),2))),((_tmp4C5=",",_tag_dyneither(_tmp4C5,sizeof(char),2))));}
# 808
goto _LLE0;_LL137: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1D4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1D4->tag != 26)goto _LL139;else{_tmp1D5=_tmp1D4->f1;_tmp1D6=_tmp1D4->f2;_tmp1D7=_tmp1D4->f3;}}_LL138:
# 811
{const char*_tmp4C6;Cyc_Absyndump_dump(((_tmp4C6="new {for",_tag_dyneither(_tmp4C6,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1D5->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp1D6);
# 812
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp1D7);Cyc_Absyndump_dump_char((int)'}');
# 813
goto _LLE0;_LL139: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1D8->tag != 27)goto _LL13B;else{_tmp1D9=_tmp1D8->f1;_tmp1DA=_tmp1D8->f2;_tmp1DB=_tmp1D8->f3;}}_LL13A:
# 816
 Cyc_Absyndump_dumpqvar(_tmp1D9);
# 817
Cyc_Absyndump_dump_char((int)'{');
# 818
if(_tmp1DA != 0)
# 819
Cyc_Absyndump_dumptps(_tmp1DA);
# 820
{const char*_tmp4C9;const char*_tmp4C8;const char*_tmp4C7;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1DB,((_tmp4C7="",_tag_dyneither(_tmp4C7,sizeof(char),1))),((_tmp4C8="}",_tag_dyneither(_tmp4C8,sizeof(char),2))),((_tmp4C9=",",_tag_dyneither(_tmp4C9,sizeof(char),2))));}
# 821
goto _LLE0;_LL13B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1DC=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1DC->tag != 28)goto _LL13D;else{_tmp1DD=_tmp1DC->f2;}}_LL13C:
# 824
{const char*_tmp4CC;const char*_tmp4CB;const char*_tmp4CA;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1DD,((_tmp4CA="{",_tag_dyneither(_tmp4CA,sizeof(char),2))),((_tmp4CB="}",_tag_dyneither(_tmp4CB,sizeof(char),2))),((_tmp4CC=",",_tag_dyneither(_tmp4CC,sizeof(char),2))));}
# 825
goto _LLE0;_LL13D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1DE=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1DE->tag != 29)goto _LL13F;else{_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DE->f3;}}_LL13E:
# 828
 Cyc_Absyndump_dumpqvar(_tmp1E0->name);
# 829
if(_tmp1DF != 0){const char*_tmp4CF;const char*_tmp4CE;const char*_tmp4CD;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp1DF,((_tmp4CD="(",_tag_dyneither(_tmp4CD,sizeof(char),2))),((_tmp4CE=")",_tag_dyneither(_tmp4CE,sizeof(char),2))),((_tmp4CF=",",_tag_dyneither(_tmp4CF,sizeof(char),2))));}
# 830
goto _LLE0;_LL13F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1E1=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1E1->tag != 30)goto _LL141;else{_tmp1E2=_tmp1E1->f1;}}_LL140:
# 832
 Cyc_Absyndump_dumpqvar(_tmp1E2);goto _LLE0;_LL141: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1E3=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1E3->tag != 31)goto _LL143;else{_tmp1E4=_tmp1E3->f1;}}_LL142:
# 833
 Cyc_Absyndump_dumpqvar(_tmp1E4);goto _LLE0;_LL143: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1E5=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1E5->tag != 32)goto _LL145;else{_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E6.is_calloc;_tmp1E8=_tmp1E6.rgn;_tmp1E9=_tmp1E6.elt_type;_tmp1EA=_tmp1E6.num_elts;}}_LL144:
# 836
 Cyc_Absyndump_dumploc(_tmp1EA->loc);
# 837
if(_tmp1E7){
# 838
if(_tmp1E8 != 0){
# 839
{const char*_tmp4D0;Cyc_Absyndump_dump(((_tmp4D0="rcalloc(",_tag_dyneither(_tmp4D0,sizeof(char),9))));}
# 840
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1E8);{const char*_tmp4D1;Cyc_Absyndump_dump(((_tmp4D1=",",_tag_dyneither(_tmp4D1,sizeof(char),2))));};}else{
# 842
const char*_tmp4D2;Cyc_Absyndump_dump(((_tmp4D2="calloc",_tag_dyneither(_tmp4D2,sizeof(char),7))));}
# 844
Cyc_Absyndump_dumpexp(_tmp1EA);
# 845
{const char*_tmp4D3;Cyc_Absyndump_dump(((_tmp4D3=",",_tag_dyneither(_tmp4D3,sizeof(char),2))));}
# 846
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp1E9)),0));{
# 847
const char*_tmp4D4;Cyc_Absyndump_dump(((_tmp4D4=")",_tag_dyneither(_tmp4D4,sizeof(char),2))));};}else{
# 849
if(_tmp1E8 != 0){
# 850
{const char*_tmp4D5;Cyc_Absyndump_dump(((_tmp4D5="rmalloc(",_tag_dyneither(_tmp4D5,sizeof(char),9))));}
# 851
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1E8);{const char*_tmp4D6;Cyc_Absyndump_dump(((_tmp4D6=",",_tag_dyneither(_tmp4D6,sizeof(char),2))));};}else{
# 853
const char*_tmp4D7;Cyc_Absyndump_dump(((_tmp4D7="malloc(",_tag_dyneither(_tmp4D7,sizeof(char),8))));}
# 856
if(_tmp1E9 != 0)
# 857
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp1E9,0),_tmp1EA,0));else{
# 859
Cyc_Absyndump_dumpexp(_tmp1EA);}{
# 860
const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8=")",_tag_dyneither(_tmp4D8,sizeof(char),2))));};}
# 862
goto _LLE0;_LL145: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1EB=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1EB->tag != 33)goto _LL147;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EB->f2;}}_LL146:
# 865
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp1EC);
# 866
{const char*_tmp4D9;Cyc_Absyndump_dump_nospace(((_tmp4D9=":=:",_tag_dyneither(_tmp4D9,sizeof(char),4))));}
# 867
Cyc_Absyndump_dumpexp_prec(myprec,_tmp1ED);
# 868
goto _LLE0;_LL147: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp1EE=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1EE->tag != 34)goto _LL149;else{_tmp1EF=_tmp1EE->f1;_tmp1F0=_tmp1EE->f2;}}_LL148:
# 872
{const char*_tmp4DC;const char*_tmp4DB;const char*_tmp4DA;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1F0,((_tmp4DA="{",_tag_dyneither(_tmp4DA,sizeof(char),2))),((_tmp4DB="}",_tag_dyneither(_tmp4DB,sizeof(char),2))),((_tmp4DC=",",_tag_dyneither(_tmp4DC,sizeof(char),2))));}
# 873
goto _LLE0;_LL149: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp1F1=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp14A;if(_tmp1F1->tag != 35)goto _LLE0;else{_tmp1F2=_tmp1F1->f1;}}_LL14A:
# 876
{const char*_tmp4DD;Cyc_Absyndump_dump_nospace(((_tmp4DD="({",_tag_dyneither(_tmp4DD,sizeof(char),3))));}
# 877
Cyc_Absyndump_dumpstmt(_tmp1F2,1);
# 878
{const char*_tmp4DE;Cyc_Absyndump_dump_nospace(((_tmp4DE="})",_tag_dyneither(_tmp4DE,sizeof(char),3))));}
# 879
goto _LLE0;_LLE0:;}
# 881
if(inprec >= myprec)
# 882
Cyc_Absyndump_dump_char((int)')');}
# 885
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
# 886
Cyc_Absyndump_dumpexp_prec(0,e);}
# 889
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
# 890
for(0;scs != 0;scs=scs->tl){
# 891
struct Cyc_Absyn_Switch_clause*_tmp23F=(struct Cyc_Absyn_Switch_clause*)scs->hd;
# 892
if(_tmp23F->where_clause == 0  && (_tmp23F->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
# 893
const char*_tmp4DF;Cyc_Absyndump_dump(((_tmp4DF="default:",_tag_dyneither(_tmp4DF,sizeof(char),9))));}else{
# 895
{const char*_tmp4E0;Cyc_Absyndump_dump(((_tmp4E0="case",_tag_dyneither(_tmp4E0,sizeof(char),5))));}
# 896
Cyc_Absyndump_dumppat(_tmp23F->pattern);
# 897
if(_tmp23F->where_clause != 0){
# 898
{const char*_tmp4E1;Cyc_Absyndump_dump(((_tmp4E1="&&",_tag_dyneither(_tmp4E1,sizeof(char),3))));}
# 899
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp23F->where_clause));}{
# 901
const char*_tmp4E2;Cyc_Absyndump_dump_nospace(((_tmp4E2=":",_tag_dyneither(_tmp4E2,sizeof(char),2))));};}
# 903
Cyc_Absyndump_dumpstmt(_tmp23F->body,0);}}
# 907
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
# 908
Cyc_Absyndump_dumploc(s->loc);{
# 909
void*_tmp244=s->r;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Stmt*_tmp249;struct Cyc_Absyn_Stmt*_tmp24A;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Absyn_Stmt*_tmp251;struct Cyc_Absyn_Stmt*_tmp252;struct _tuple9 _tmp254;struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Stmt*_tmp256;struct _dyneither_ptr*_tmp25A;struct Cyc_Absyn_Exp*_tmp25C;struct _tuple9 _tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct _tuple9 _tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Stmt*_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_Absyn_Decl*_tmp266;struct Cyc_Absyn_Stmt*_tmp267;struct _dyneither_ptr*_tmp269;struct Cyc_Absyn_Stmt*_tmp26A;struct Cyc_Absyn_Stmt*_tmp26C;struct _tuple9 _tmp26D;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp272;struct Cyc_Absyn_Stmt*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_Absyn_Exp*_tmp277;_LL150: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp245=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp245->tag != 0)goto _LL152;}_LL151:
# 910
 Cyc_Absyndump_dump_semi();goto _LL14F;_LL152: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp246=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp246->tag != 1)goto _LL154;else{_tmp247=_tmp246->f1;}}_LL153:
# 911
 Cyc_Absyndump_dumpexp(_tmp247);Cyc_Absyndump_dump_semi();goto _LL14F;_LL154: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp248=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp248->tag != 2)goto _LL156;else{_tmp249=_tmp248->f1;_tmp24A=_tmp248->f2;}}_LL155:
# 916
 if(Cyc_Absynpp_is_declaration(_tmp249)){
# 917
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp249,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 919
Cyc_Absyndump_dumpstmt(_tmp249,0);}
# 920
if(Cyc_Absynpp_is_declaration(_tmp24A)){
# 921
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
# 922
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp24A,expstmt);
# 923
Cyc_Absyndump_dump_char((int)'}');
# 924
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 926
Cyc_Absyndump_dumpstmt(_tmp24A,expstmt);}
# 927
goto _LL14F;_LL156: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp24B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp24B->tag != 3)goto _LL158;else{_tmp24C=_tmp24B->f1;if(_tmp24C != 0)goto _LL158;}}_LL157:
# 928
{const char*_tmp4E3;Cyc_Absyndump_dump(((_tmp4E3="return;",_tag_dyneither(_tmp4E3,sizeof(char),8))));}goto _LL14F;_LL158: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp24D=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp24D->tag != 3)goto _LL15A;else{_tmp24E=_tmp24D->f1;}}_LL159:
# 929
{const char*_tmp4E4;Cyc_Absyndump_dump(((_tmp4E4="return",_tag_dyneither(_tmp4E4,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp24E));Cyc_Absyndump_dump_semi();goto _LL14F;_LL15A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp24F=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp24F->tag != 4)goto _LL15C;else{_tmp250=_tmp24F->f1;_tmp251=_tmp24F->f2;_tmp252=_tmp24F->f3;}}_LL15B:
# 931
{const char*_tmp4E5;Cyc_Absyndump_dump(((_tmp4E5="if(",_tag_dyneither(_tmp4E5,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp250);
# 932
{void*_tmp27B=_tmp251->r;_LL177: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp27C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp27B;if(_tmp27C->tag != 2)goto _LL179;}_LL178:
# 933
 goto _LL17A;_LL179: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp27D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp27B;if(_tmp27D->tag != 12)goto _LL17B;}_LL17A:
# 934
 goto _LL17C;_LL17B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp27E=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp27B;if(_tmp27E->tag != 4)goto _LL17D;}_LL17C:
# 935
 goto _LL17E;_LL17D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp27F=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp27B;if(_tmp27F->tag != 13)goto _LL17F;}_LL17E:
# 936
{const char*_tmp4E6;Cyc_Absyndump_dump_nospace(((_tmp4E6="){",_tag_dyneither(_tmp4E6,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp251,0);Cyc_Absyndump_dump_char((int)'}');goto _LL176;_LL17F:;_LL180:
# 937
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp251,0);_LL176:;}
# 939
{void*_tmp281=_tmp252->r;_LL182: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp282=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp281;if(_tmp282->tag != 0)goto _LL184;}_LL183:
# 940
 goto _LL181;_LL184:;_LL185:
# 941
{const char*_tmp4E7;Cyc_Absyndump_dump(((_tmp4E7="else{",_tag_dyneither(_tmp4E7,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp252,0);Cyc_Absyndump_dump_char((int)'}');goto _LL181;_LL181:;}
# 943
goto _LL14F;_LL15C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp253=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp253->tag != 5)goto _LL15E;else{_tmp254=_tmp253->f1;_tmp255=_tmp254.f1;_tmp256=_tmp253->f2;}}_LL15D:
# 945
{const char*_tmp4E8;Cyc_Absyndump_dump(((_tmp4E8="while(",_tag_dyneither(_tmp4E8,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp255);{const char*_tmp4E9;Cyc_Absyndump_dump_nospace(((_tmp4E9="){",_tag_dyneither(_tmp4E9,sizeof(char),3))));}
# 946
Cyc_Absyndump_dumpstmt(_tmp256,0);
# 947
Cyc_Absyndump_dump_char((int)'}');
# 948
goto _LL14F;_LL15E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp257=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp257->tag != 6)goto _LL160;}_LL15F:
# 949
{const char*_tmp4EA;Cyc_Absyndump_dump(((_tmp4EA="break;",_tag_dyneither(_tmp4EA,sizeof(char),7))));}goto _LL14F;_LL160: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp258=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp258->tag != 7)goto _LL162;}_LL161:
# 950
{const char*_tmp4EB;Cyc_Absyndump_dump(((_tmp4EB="continue;",_tag_dyneither(_tmp4EB,sizeof(char),10))));}goto _LL14F;_LL162: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp259=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp259->tag != 8)goto _LL164;else{_tmp25A=_tmp259->f1;}}_LL163:
# 951
{const char*_tmp4EC;Cyc_Absyndump_dump(((_tmp4EC="goto",_tag_dyneither(_tmp4EC,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp25A);Cyc_Absyndump_dump_semi();goto _LL14F;_LL164: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp25B=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp25B->tag != 9)goto _LL166;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;_tmp25E=_tmp25D.f1;_tmp25F=_tmp25B->f3;_tmp260=_tmp25F.f1;_tmp261=_tmp25B->f4;}}_LL165:
# 954
{const char*_tmp4ED;Cyc_Absyndump_dump(((_tmp4ED="for(",_tag_dyneither(_tmp4ED,sizeof(char),5))));}Cyc_Absyndump_dumpexp(_tmp25C);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp25E);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp260);
# 955
{const char*_tmp4EE;Cyc_Absyndump_dump_nospace(((_tmp4EE="){",_tag_dyneither(_tmp4EE,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp261,0);Cyc_Absyndump_dump_char((int)'}');
# 956
goto _LL14F;_LL166: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp262=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp262->tag != 10)goto _LL168;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}_LL167:
# 958
{const char*_tmp4EF;Cyc_Absyndump_dump(((_tmp4EF="switch(",_tag_dyneither(_tmp4EF,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp263);{const char*_tmp4F0;Cyc_Absyndump_dump_nospace(((_tmp4F0="){",_tag_dyneither(_tmp4F0,sizeof(char),3))));}
# 959
Cyc_Absyndump_dumpswitchclauses(_tmp264);
# 960
Cyc_Absyndump_dump_char((int)'}');
# 961
goto _LL14F;_LL168: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp265=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp265->tag != 12)goto _LL16A;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}_LL169:
# 963
 Cyc_Absyndump_dumpdecl(_tmp266);Cyc_Absyndump_dumpstmt(_tmp267,expstmt);goto _LL14F;_LL16A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp268=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp268->tag != 13)goto _LL16C;else{_tmp269=_tmp268->f1;_tmp26A=_tmp268->f2;}}_LL16B:
# 969
 if(Cyc_Absynpp_is_declaration(_tmp26A)){
# 970
Cyc_Absyndump_dump_str(_tmp269);
# 971
if(expstmt){const char*_tmp4F1;Cyc_Absyndump_dump_nospace(((_tmp4F1=": ({",_tag_dyneither(_tmp4F1,sizeof(char),5))));}else{const char*_tmp4F2;Cyc_Absyndump_dump_nospace(((_tmp4F2=": {",_tag_dyneither(_tmp4F2,sizeof(char),4))));}
# 972
Cyc_Absyndump_dumpstmt(_tmp26A,expstmt);
# 973
if(expstmt){const char*_tmp4F3;Cyc_Absyndump_dump_nospace(((_tmp4F3="});}",_tag_dyneither(_tmp4F3,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 975
Cyc_Absyndump_dump_str(_tmp269);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp26A,expstmt);}
# 977
goto _LL14F;_LL16C: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp26B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp26B->tag != 14)goto _LL16E;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26B->f2;_tmp26E=_tmp26D.f1;}}_LL16D:
# 980
{const char*_tmp4F4;Cyc_Absyndump_dump(((_tmp4F4="do{",_tag_dyneither(_tmp4F4,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp26C,0);
# 981
{const char*_tmp4F5;Cyc_Absyndump_dump_nospace(((_tmp4F5="}while(",_tag_dyneither(_tmp4F5,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp26E);{const char*_tmp4F6;Cyc_Absyndump_dump_nospace(((_tmp4F6=");",_tag_dyneither(_tmp4F6,sizeof(char),3))));}
# 982
goto _LL14F;_LL16E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp26F=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp26F->tag != 11)goto _LL170;else{_tmp270=_tmp26F->f1;if(_tmp270 != 0)goto _LL170;}}_LL16F:
# 984
{const char*_tmp4F7;Cyc_Absyndump_dump(((_tmp4F7="fallthru;",_tag_dyneither(_tmp4F7,sizeof(char),10))));}goto _LL14F;_LL170: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp271=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp271->tag != 11)goto _LL172;else{_tmp272=_tmp271->f1;}}_LL171:
# 986
{const char*_tmp4F8;Cyc_Absyndump_dump(((_tmp4F8="fallthru(",_tag_dyneither(_tmp4F8,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(20,_tmp272);{const char*_tmp4F9;Cyc_Absyndump_dump_nospace(((_tmp4F9=");",_tag_dyneither(_tmp4F9,sizeof(char),3))));}goto _LL14F;_LL172: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp273=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp273->tag != 15)goto _LL174;else{_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;}}_LL173:
# 989
{const char*_tmp4FA;Cyc_Absyndump_dump(((_tmp4FA="try",_tag_dyneither(_tmp4FA,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp274,0);
# 990
{const char*_tmp4FB;Cyc_Absyndump_dump(((_tmp4FB="catch{",_tag_dyneither(_tmp4FB,sizeof(char),7))));}
# 991
Cyc_Absyndump_dumpswitchclauses(_tmp275);Cyc_Absyndump_dump_char((int)'}');
# 992
goto _LL14F;_LL174: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp276=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp244;if(_tmp276->tag != 16)goto _LL14F;else{_tmp277=_tmp276->f1;}}_LL175:
# 995
{const char*_tmp4FC;Cyc_Absyndump_dump(((_tmp4FC="reset_region(",_tag_dyneither(_tmp4FC,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp277);{const char*_tmp4FD;Cyc_Absyndump_dump(((_tmp4FD=");",_tag_dyneither(_tmp4FD,sizeof(char),3))));}
# 996
goto _LL14F;_LL14F:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1001
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
# 1002
{const char*_tmp500;const char*_tmp4FF;const char*_tmp4FE;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp4FE="",_tag_dyneither(_tmp4FE,sizeof(char),1))),((_tmp4FF="=",_tag_dyneither(_tmp4FF,sizeof(char),2))),((_tmp500="=",_tag_dyneither(_tmp500,sizeof(char),2))));}
# 1003
Cyc_Absyndump_dumppat((*dp).f2);}
# 1006
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
# 1007
void*_tmp29D=p->r;enum Cyc_Absyn_Sign _tmp2A1;int _tmp2A2;enum Cyc_Absyn_Sign _tmp2A4;int _tmp2A5;enum Cyc_Absyn_Sign _tmp2A7;int _tmp2A8;char _tmp2AA;struct _dyneither_ptr _tmp2AC;struct Cyc_Absyn_Vardecl*_tmp2AE;struct Cyc_Absyn_Pat*_tmp2AF;struct Cyc_Absyn_Pat _tmp2B0;void*_tmp2B1;struct Cyc_Absyn_Vardecl*_tmp2B4;struct Cyc_Absyn_Pat*_tmp2B5;struct Cyc_List_List*_tmp2B7;int _tmp2B8;struct Cyc_Absyn_Pat*_tmp2BA;struct Cyc_Absyn_Vardecl*_tmp2BC;struct Cyc_Absyn_Pat*_tmp2BD;struct Cyc_Absyn_Pat _tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C2;struct Cyc_Absyn_Pat*_tmp2C3;struct Cyc_Absyn_Tvar*_tmp2C5;struct Cyc_Absyn_Vardecl*_tmp2C6;struct _tuple0*_tmp2C8;struct _tuple0*_tmp2CA;struct Cyc_List_List*_tmp2CB;int _tmp2CC;struct Cyc_Absyn_AggrInfo*_tmp2CE;struct Cyc_Absyn_AggrInfo _tmp2CF;union Cyc_Absyn_AggrInfoU _tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_List_List*_tmp2D2;int _tmp2D3;struct Cyc_Absyn_AggrInfo*_tmp2D5;struct Cyc_List_List*_tmp2D6;struct Cyc_List_List*_tmp2D7;int _tmp2D8;struct Cyc_Absyn_Datatypefield*_tmp2DA;struct Cyc_List_List*_tmp2DB;int _tmp2DC;struct Cyc_Absyn_Enumfield*_tmp2DE;struct Cyc_Absyn_Enumfield*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2E2;_LL187: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp29E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp29E->tag != 0)goto _LL189;}_LL188:
# 1008
 Cyc_Absyndump_dump_char((int)'_');goto _LL186;_LL189: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp29F=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp29F->tag != 8)goto _LL18B;}_LL18A:
# 1009
{const char*_tmp501;Cyc_Absyndump_dump(((_tmp501="NULL",_tag_dyneither(_tmp501,sizeof(char),5))));}goto _LL186;_LL18B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2A0=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2A0->tag != 9)goto _LL18D;else{_tmp2A1=_tmp2A0->f1;if(_tmp2A1 != Cyc_Absyn_None)goto _LL18D;_tmp2A2=_tmp2A0->f2;}}_LL18C:
# 1010
 _tmp2A5=_tmp2A2;goto _LL18E;_LL18D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2A3=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2A3->tag != 9)goto _LL18F;else{_tmp2A4=_tmp2A3->f1;if(_tmp2A4 != Cyc_Absyn_Signed)goto _LL18F;_tmp2A5=_tmp2A3->f2;}}_LL18E:
# 1011
{const char*_tmp505;void*_tmp504[1];struct Cyc_Int_pa_PrintArg_struct _tmp503;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp503.tag=1,((_tmp503.f1=(unsigned long)_tmp2A5,((_tmp504[0]=& _tmp503,Cyc_aprintf(((_tmp505="%d",_tag_dyneither(_tmp505,sizeof(char),3))),_tag_dyneither(_tmp504,sizeof(void*),1)))))))));}goto _LL186;_LL18F: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp2A6=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2A6->tag != 9)goto _LL191;else{_tmp2A7=_tmp2A6->f1;if(_tmp2A7 != Cyc_Absyn_Unsigned)goto _LL191;_tmp2A8=_tmp2A6->f2;}}_LL190:
# 1012
{const char*_tmp509;void*_tmp508[1];struct Cyc_Int_pa_PrintArg_struct _tmp507;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp507.tag=1,((_tmp507.f1=(unsigned int)_tmp2A8,((_tmp508[0]=& _tmp507,Cyc_aprintf(((_tmp509="%u",_tag_dyneither(_tmp509,sizeof(char),3))),_tag_dyneither(_tmp508,sizeof(void*),1)))))))));}goto _LL186;_LL191: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp2A9=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2A9->tag != 10)goto _LL193;else{_tmp2AA=_tmp2A9->f1;}}_LL192:
# 1014
{const char*_tmp50A;Cyc_Absyndump_dump(((_tmp50A="'",_tag_dyneither(_tmp50A,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp2AA));{const char*_tmp50B;Cyc_Absyndump_dump_nospace(((_tmp50B="'",_tag_dyneither(_tmp50B,sizeof(char),2))));}goto _LL186;_LL193: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp2AB=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2AB->tag != 11)goto _LL195;else{_tmp2AC=_tmp2AB->f1;}}_LL194:
# 1015
 Cyc_Absyndump_dump(_tmp2AC);goto _LL186;_LL195: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2AD=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2AD->tag != 1)goto _LL197;else{_tmp2AE=_tmp2AD->f1;_tmp2AF=_tmp2AD->f2;_tmp2B0=*_tmp2AF;_tmp2B1=_tmp2B0.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2B2=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2B1;if(_tmp2B2->tag != 0)goto _LL197;};}}_LL196:
# 1016
 Cyc_Absyndump_dumpqvar(_tmp2AE->name);goto _LL186;_LL197: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp2B3=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2B3->tag != 1)goto _LL199;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B3->f2;}}_LL198:
# 1017
 Cyc_Absyndump_dumpqvar(_tmp2B4->name);{const char*_tmp50C;Cyc_Absyndump_dump(((_tmp50C=" as ",_tag_dyneither(_tmp50C,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp2B5);goto _LL186;_LL199: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp2B6=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2B6->tag != 4)goto _LL19B;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B6->f2;}}_LL19A: {
# 1019
const char*_tmp50E;const char*_tmp50D;struct _dyneither_ptr term=_tmp2B8?(_tmp50E=", ...)",_tag_dyneither(_tmp50E,sizeof(char),7)):((_tmp50D=")",_tag_dyneither(_tmp50D,sizeof(char),2)));
# 1020
{const char*_tmp510;const char*_tmp50F;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2B7,((_tmp50F="$(",_tag_dyneither(_tmp50F,sizeof(char),3))),term,((_tmp510=",",_tag_dyneither(_tmp510,sizeof(char),2))));}goto _LL186;}_LL19B: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp2B9=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2B9->tag != 5)goto _LL19D;else{_tmp2BA=_tmp2B9->f1;}}_LL19C:
# 1021
{const char*_tmp511;Cyc_Absyndump_dump(((_tmp511="&",_tag_dyneither(_tmp511,sizeof(char),2))));}Cyc_Absyndump_dumppat(_tmp2BA);goto _LL186;_LL19D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2BB=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2BB->tag != 2)goto _LL19F;else{_tmp2BC=_tmp2BB->f1;_tmp2BD=_tmp2BB->f2;_tmp2BE=*_tmp2BD;_tmp2BF=_tmp2BE.r;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp2C0=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp2BF;if(_tmp2C0->tag != 0)goto _LL19F;};}}_LL19E:
# 1023
{const char*_tmp512;Cyc_Absyndump_dump(((_tmp512="*",_tag_dyneither(_tmp512,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp2BC->name);goto _LL186;_LL19F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp2C1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2C1->tag != 2)goto _LL1A1;else{_tmp2C2=_tmp2C1->f1;_tmp2C3=_tmp2C1->f2;}}_LL1A0:
# 1025
{const char*_tmp513;Cyc_Absyndump_dump(((_tmp513="*",_tag_dyneither(_tmp513,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp2C2->name);{const char*_tmp514;Cyc_Absyndump_dump(((_tmp514=" as ",_tag_dyneither(_tmp514,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp2C3);goto _LL186;_LL1A1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp2C4=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2C4->tag != 3)goto _LL1A3;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;}}_LL1A2:
# 1027
 Cyc_Absyndump_dumpqvar(_tmp2C6->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp2C5);Cyc_Absyndump_dump_char((int)'>');goto _LL186;_LL1A3: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp2C7=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2C7->tag != 14)goto _LL1A5;else{_tmp2C8=_tmp2C7->f1;}}_LL1A4:
# 1028
 Cyc_Absyndump_dumpqvar(_tmp2C8);goto _LL186;_LL1A5: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp2C9=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2C9->tag != 15)goto _LL1A7;else{_tmp2CA=_tmp2C9->f1;_tmp2CB=_tmp2C9->f2;_tmp2CC=_tmp2C9->f3;}}_LL1A6: {
# 1030
const char*_tmp516;const char*_tmp515;struct _dyneither_ptr term=_tmp2CC?(_tmp516=", ...)",_tag_dyneither(_tmp516,sizeof(char),7)):((_tmp515=")",_tag_dyneither(_tmp515,sizeof(char),2)));
# 1031
Cyc_Absyndump_dumpqvar(_tmp2CA);{const char*_tmp518;const char*_tmp517;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2CB,((_tmp517="(",_tag_dyneither(_tmp517,sizeof(char),2))),term,((_tmp518=",",_tag_dyneither(_tmp518,sizeof(char),2))));}goto _LL186;}_LL1A7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2CD=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2CD->tag != 6)goto _LL1A9;else{_tmp2CE=_tmp2CD->f1;if(_tmp2CE == 0)goto _LL1A9;_tmp2CF=*_tmp2CE;_tmp2D0=_tmp2CF.aggr_info;_tmp2D1=_tmp2CD->f2;_tmp2D2=_tmp2CD->f3;_tmp2D3=_tmp2CD->f4;}}_LL1A8: {
# 1033
struct _tuple0*_tmp2FA;struct _tuple10 _tmp2F9=Cyc_Absyn_aggr_kinded_name(_tmp2D0);_tmp2FA=_tmp2F9.f2;{
# 1034
const char*_tmp51A;const char*_tmp519;struct _dyneither_ptr term=_tmp2D3?(_tmp51A=", ...)",_tag_dyneither(_tmp51A,sizeof(char),7)):((_tmp519=")",_tag_dyneither(_tmp519,sizeof(char),2)));
# 1035
Cyc_Absyndump_dumpqvar(_tmp2FA);Cyc_Absyndump_dump_char((int)'{');
# 1036
{const char*_tmp51D;const char*_tmp51C;const char*_tmp51B;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp2D1,((_tmp51B="[",_tag_dyneither(_tmp51B,sizeof(char),2))),((_tmp51C="]",_tag_dyneither(_tmp51C,sizeof(char),2))),((_tmp51D=",",_tag_dyneither(_tmp51D,sizeof(char),2))));}
# 1037
{const char*_tmp51F;const char*_tmp51E;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp2D2,((_tmp51E="",_tag_dyneither(_tmp51E,sizeof(char),1))),term,((_tmp51F=",",_tag_dyneither(_tmp51F,sizeof(char),2))));}
# 1038
goto _LL186;};}_LL1A9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp2D4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2D4->tag != 6)goto _LL1AB;else{_tmp2D5=_tmp2D4->f1;if(_tmp2D5 != 0)goto _LL1AB;_tmp2D6=_tmp2D4->f2;_tmp2D7=_tmp2D4->f3;_tmp2D8=_tmp2D4->f4;}}_LL1AA: {
# 1040
const char*_tmp521;const char*_tmp520;struct _dyneither_ptr term=_tmp2D8?(_tmp521=", ...)",_tag_dyneither(_tmp521,sizeof(char),7)):((_tmp520=")",_tag_dyneither(_tmp520,sizeof(char),2)));
# 1041
Cyc_Absyndump_dump_char((int)'{');
# 1042
{const char*_tmp524;const char*_tmp523;const char*_tmp522;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp2D6,((_tmp522="[",_tag_dyneither(_tmp522,sizeof(char),2))),((_tmp523="]",_tag_dyneither(_tmp523,sizeof(char),2))),((_tmp524=",",_tag_dyneither(_tmp524,sizeof(char),2))));}
# 1043
{const char*_tmp526;const char*_tmp525;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp2D7,((_tmp525="",_tag_dyneither(_tmp525,sizeof(char),1))),term,((_tmp526=",",_tag_dyneither(_tmp526,sizeof(char),2))));}
# 1044
goto _LL186;}_LL1AB: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp2D9=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2D9->tag != 7)goto _LL1AD;else{_tmp2DA=_tmp2D9->f2;_tmp2DB=_tmp2D9->f3;_tmp2DC=_tmp2D9->f4;}}_LL1AC: {
# 1046
const char*_tmp528;const char*_tmp527;struct _dyneither_ptr term=_tmp2DC?(_tmp528=", ...)",_tag_dyneither(_tmp528,sizeof(char),7)):((_tmp527=")",_tag_dyneither(_tmp527,sizeof(char),2)));
# 1047
Cyc_Absyndump_dumpqvar(_tmp2DA->name);
# 1048
if(_tmp2DB != 0){const char*_tmp52A;const char*_tmp529;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2DB,((_tmp529="(",_tag_dyneither(_tmp529,sizeof(char),2))),term,((_tmp52A=",",_tag_dyneither(_tmp52A,sizeof(char),2))));}
# 1049
goto _LL186;}_LL1AD: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp2DD=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2DD->tag != 12)goto _LL1AF;else{_tmp2DE=_tmp2DD->f2;}}_LL1AE:
# 1050
 _tmp2E0=_tmp2DE;goto _LL1B0;_LL1AF: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp2DF=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2DF->tag != 13)goto _LL1B1;else{_tmp2E0=_tmp2DF->f2;}}_LL1B0:
# 1051
 Cyc_Absyndump_dumpqvar(_tmp2E0->name);goto _LL186;_LL1B1: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp2E1=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp29D;if(_tmp2E1->tag != 16)goto _LL186;else{_tmp2E2=_tmp2E1->f1;}}_LL1B2:
# 1052
 Cyc_Absyndump_dumpexp(_tmp2E2);goto _LL186;_LL186:;}
# 1056
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
# 1057
Cyc_Absyndump_dumpqvar(ef->name);
# 1058
if(ef->typs != 0)
# 1059
Cyc_Absyndump_dumpargs(ef->typs);}
# 1061
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
# 1062
const char*_tmp52B;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,((_tmp52B=",",_tag_dyneither(_tmp52B,sizeof(char),2))));}
# 1064
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
# 1065
Cyc_Absyndump_dumpqvar(ef->name);
# 1066
if(ef->tag != 0){
# 1067
{const char*_tmp52C;Cyc_Absyndump_dump(((_tmp52C=" = ",_tag_dyneither(_tmp52C,sizeof(char),4))));}
# 1068
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1071
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
# 1072
const char*_tmp52D;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((_tmp52D=",",_tag_dyneither(_tmp52D,sizeof(char),2))));}
# 1075
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
# 1076
for(0;fields != 0;fields=fields->tl){
# 1077
struct Cyc_Absyn_Aggrfield _tmp311;struct _dyneither_ptr*_tmp312;struct Cyc_Absyn_Tqual _tmp313;void*_tmp314;struct Cyc_Absyn_Exp*_tmp315;struct Cyc_List_List*_tmp316;struct Cyc_Absyn_Aggrfield*_tmp310=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp311=*_tmp310;_tmp312=_tmp311.name;_tmp313=_tmp311.tq;_tmp314=_tmp311.type;_tmp315=_tmp311.width;_tmp316=_tmp311.attributes;
# 1079
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1B3:
# 1081
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp313,_tmp314,Cyc_Absyndump_dump_str,_tmp312);
# 1082
Cyc_Absyndump_dumpatts(_tmp316);
# 1083
break;case Cyc_Cyclone_Vc_c: _LL1B4:
# 1085
 Cyc_Absyndump_dumpatts(_tmp316);
# 1086
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp313,_tmp314,Cyc_Absyndump_dump_str,_tmp312);
# 1087
break;}
# 1090
if(_tmp315 != 0){
# 1091
Cyc_Absyndump_dump_char((int)':');
# 1092
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp315);}
# 1094
Cyc_Absyndump_dump_semi();}}
# 1098
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
# 1099
Cyc_Absyndump_dumpqvar(td->name);
# 1100
Cyc_Absyndump_dumptvars(td->tvs);}
# 1103
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
# 1104
Cyc_Absyndump_dumpatts(fd->attributes);
# 1105
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1107
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
# 1108
{void*_tmp317=(*pr).f1;_LL1B7: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp318=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp317;if(_tmp318->tag != 11)goto _LL1B9;}_LL1B8:
# 1109
 goto _LL1B6;_LL1B9: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp319=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp317;if(_tmp319->tag != 1)goto _LL1BB;}_LL1BA:
# 1110
{const char*_tmp52E;Cyc_Absyndump_dump(((_tmp52E="_stdcall",_tag_dyneither(_tmp52E,sizeof(char),9))));}goto _LL1B6;_LL1BB: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp31A=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp317;if(_tmp31A->tag != 2)goto _LL1BD;}_LL1BC:
# 1111
{const char*_tmp52F;Cyc_Absyndump_dump(((_tmp52F="_cdecl",_tag_dyneither(_tmp52F,sizeof(char),7))));}goto _LL1B6;_LL1BD: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp31B=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp317;if(_tmp31B->tag != 3)goto _LL1BF;}_LL1BE:
# 1112
{const char*_tmp530;Cyc_Absyndump_dump(((_tmp530="_fastcall",_tag_dyneither(_tmp530,sizeof(char),10))));}goto _LL1B6;_LL1BF:;_LL1C0:
# 1113
 goto _LL1B6;_LL1B6:;}
# 1115
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1117
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
# 1118
Cyc_Absyndump_dump_callconv(fd->attributes);
# 1119
Cyc_Absyndump_dumpqvar(fd->name);}
# 1122
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
# 1123
for(0;vds != 0;vds=vds->tl){
# 1124
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
# 1125
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1129
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1131
void*_tmp31F=t;void*_tmp321;struct Cyc_Absyn_PtrInfo _tmp323;void*_tmp324;_LL1C2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp320=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31F;if(_tmp320->tag != 1)goto _LL1C4;else{_tmp321=(void*)_tmp320->f2;}}if(!(_tmp321 != 0))goto _LL1C4;_LL1C3:
# 1132
 return Cyc_Absyndump_is_char_ptr((void*)_tmp321);_LL1C4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp322=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31F;if(_tmp322->tag != 5)goto _LL1C6;else{_tmp323=_tmp322->f1;_tmp324=_tmp323.elt_typ;}}_LL1C5:
# 1134
 L: {
# 1135
void*_tmp325=_tmp324;void*_tmp327;void*_tmp329;enum Cyc_Absyn_Size_of _tmp32B;_LL1C9:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 1)goto _LL1CB;else{_tmp327=(void*)_tmp326->f2;}}if(!(_tmp327 != 0))goto _LL1CB;_LL1CA:
# 1136
 _tmp324=(void*)_tmp327;goto L;_LL1CB:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp325;if(_tmp328->tag != 17)goto _LL1CD;else{_tmp329=(void*)_tmp328->f4;}}if(!(_tmp329 != 0))goto _LL1CD;_LL1CC:
# 1137
 _tmp324=(void*)_tmp329;goto L;_LL1CD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp325;if(_tmp32A->tag != 6)goto _LL1CF;else{_tmp32B=_tmp32A->f2;if(_tmp32B != Cyc_Absyn_Char_sz)goto _LL1CF;}}_LL1CE:
# 1138
 return 1;_LL1CF:;_LL1D0:
# 1139
 return 0;_LL1C8:;}_LL1C6:;_LL1C7:
# 1141
 return 0;_LL1C1:;}
# 1145
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc){
# 1146
struct Cyc_Absyn_Vardecl _tmp32D;enum Cyc_Absyn_Scope _tmp32E;struct _tuple0*_tmp32F;struct Cyc_Absyn_Tqual _tmp330;void*_tmp331;struct Cyc_Absyn_Exp*_tmp332;struct Cyc_List_List*_tmp333;struct Cyc_Absyn_Vardecl*_tmp32C=vd;_tmp32D=*_tmp32C;_tmp32E=_tmp32D.sc;_tmp32F=_tmp32D.name;_tmp330=_tmp32D.tq;_tmp331=_tmp32D.type;_tmp332=_tmp32D.initializer;_tmp333=_tmp32D.attributes;
# 1147
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1D1:
# 1150
 if(_tmp32E == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
# 1151
void*_tmp334=Cyc_Tcutil_compress(_tmp331);_LL1D4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp335=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp334;if(_tmp335->tag != 9)goto _LL1D6;}_LL1D5:
# 1152
 goto _LL1D3;_LL1D6:;_LL1D7:
# 1153
 Cyc_Absyndump_dumpscope(_tmp32E);_LL1D3:;}else{
# 1156
Cyc_Absyndump_dumpscope(_tmp32E);}
# 1157
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp330,_tmp331,Cyc_Absyndump_dumpqvar,_tmp32F);
# 1158
Cyc_Absyndump_dumpatts(_tmp333);
# 1159
break;case Cyc_Cyclone_Vc_c: _LL1D2:
# 1161
 Cyc_Absyndump_dumpatts(_tmp333);
# 1162
Cyc_Absyndump_dumpscope(_tmp32E);{
# 1163
struct _RegionHandle _tmp336=_new_region("temp");struct _RegionHandle*temp=& _tmp336;_push_region(temp);{
# 1164
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp331);
# 1165
struct Cyc_Absyn_Tqual _tmp338;void*_tmp339;struct Cyc_List_List*_tmp33A;struct _tuple11 _tmp337=Cyc_Absynpp_to_tms(temp,_tmp330,_tmp331);_tmp338=_tmp337.f1;_tmp339=_tmp337.f2;_tmp33A=_tmp337.f3;{
# 1167
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
# 1168
{struct Cyc_List_List*tms2=_tmp33A;for(0;tms2 != 0;tms2=tms2->tl){
# 1169
void*_tmp33B=(void*)tms2->hd;struct Cyc_List_List*_tmp33D;_LL1DA: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp33C=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp33B;if(_tmp33C->tag != 5)goto _LL1DC;else{_tmp33D=_tmp33C->f2;}}_LL1DB:
# 1171
 for(0;_tmp33D != 0;_tmp33D=_tmp33D->tl){
# 1172
void*_tmp33E=(void*)_tmp33D->hd;_LL1DF: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp33F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp33E;if(_tmp33F->tag != 1)goto _LL1E1;}_LL1E0:
# 1173
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1DE;_LL1E1: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp340=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp33E;if(_tmp340->tag != 2)goto _LL1E3;}_LL1E2:
# 1174
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL1DE;_LL1E3: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp341=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp33E;if(_tmp341->tag != 3)goto _LL1E5;}_LL1E4:
# 1175
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL1DE;_LL1E5:;_LL1E6:
# 1176
 goto _LL1DE;_LL1DE:;}
# 1178
goto _LL1D9;_LL1DC:;_LL1DD:
# 1179
 goto _LL1D9;_LL1D9:;}}
# 1181
Cyc_Absyndump_dumptq(_tmp338);
# 1182
Cyc_Absyndump_dumpntyp(_tmp339);{
# 1183
struct _tuple19 _tmp531;struct _tuple19 _tmp342=(_tmp531.f1=call_conv,((_tmp531.f2=_tmp32F,_tmp531)));
# 1184
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp33A),Cyc_Absyndump_dump_callconv_qvar,& _tmp342);};};}
# 1186
_npop_handler(0);break;
# 1163
;_pop_region(temp);};}
# 1189
if(_tmp332 != 0){
# 1190
Cyc_Absyndump_dump_char((int)'=');
# 1191
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp332);}
# 1193
Cyc_Absyndump_dump_semi();}
# 1196
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
# 1197
Cyc_Absyndump_dumpscope(ad->sc);
# 1198
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
# 1199
const char*_tmp532;Cyc_Absyndump_dump(((_tmp532="@tagged ",_tag_dyneither(_tmp532,sizeof(char),9))));}
# 1200
Cyc_Absyndump_dumpaggr_kind(ad->kind);
# 1201
Cyc_Absyndump_dumpqvar(ad->name);
# 1202
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
# 1203
if(ad->impl == 0)return;else{
# 1205
Cyc_Absyndump_dump_char((int)'{');
# 1206
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
# 1207
const char*_tmp535;const char*_tmp534;const char*_tmp533;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp533="<",_tag_dyneither(_tmp533,sizeof(char),2))),((_tmp534=">",_tag_dyneither(_tmp534,sizeof(char),2))),((_tmp535=",",_tag_dyneither(_tmp535,sizeof(char),2))));}
# 1208
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
# 1209
Cyc_Absyndump_dump_char((int)':');
# 1210
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1212
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
# 1213
{const char*_tmp536;Cyc_Absyndump_dump(((_tmp536="}",_tag_dyneither(_tmp536,sizeof(char),2))));}
# 1214
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1217
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
# 1218
struct Cyc_Absyn_Enumdecl _tmp34A;enum Cyc_Absyn_Scope _tmp34B;struct _tuple0*_tmp34C;struct Cyc_Core_Opt*_tmp34D;struct Cyc_Absyn_Enumdecl*_tmp349=ed;_tmp34A=*_tmp349;_tmp34B=_tmp34A.sc;_tmp34C=_tmp34A.name;_tmp34D=_tmp34A.fields;
# 1219
Cyc_Absyndump_dumpscope(_tmp34B);
# 1220
{const char*_tmp537;Cyc_Absyndump_dump(((_tmp537="enum ",_tag_dyneither(_tmp537,sizeof(char),6))));}
# 1221
Cyc_Absyndump_dumpqvar(_tmp34C);
# 1222
if(_tmp34D != 0){
# 1223
Cyc_Absyndump_dump_char((int)'{');
# 1224
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp34D->v);{
# 1225
const char*_tmp538;Cyc_Absyndump_dump_nospace(((_tmp538="}",_tag_dyneither(_tmp538,sizeof(char),2))));};}}
# 1228
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
# 1229
struct Cyc_Absyn_Datatypedecl _tmp351;enum Cyc_Absyn_Scope _tmp352;struct _tuple0*_tmp353;struct Cyc_List_List*_tmp354;struct Cyc_Core_Opt*_tmp355;int _tmp356;struct Cyc_Absyn_Datatypedecl*_tmp350=dd;_tmp351=*_tmp350;_tmp352=_tmp351.sc;_tmp353=_tmp351.name;_tmp354=_tmp351.tvs;_tmp355=_tmp351.fields;_tmp356=_tmp351.is_extensible;
# 1230
Cyc_Absyndump_dumpscope(_tmp352);
# 1231
if(_tmp356){const char*_tmp539;Cyc_Absyndump_dump(((_tmp539="@extensible ",_tag_dyneither(_tmp539,sizeof(char),13))));}
# 1232
{const char*_tmp53A;Cyc_Absyndump_dump(((_tmp53A="datatype ",_tag_dyneither(_tmp53A,sizeof(char),10))));}
# 1233
Cyc_Absyndump_dumpqvar(_tmp353);
# 1234
Cyc_Absyndump_dumptvars(_tmp354);
# 1235
if(_tmp355 != 0){
# 1236
Cyc_Absyndump_dump_char((int)'{');
# 1237
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp355->v);{
# 1238
const char*_tmp53B;Cyc_Absyndump_dump_nospace(((_tmp53B="}",_tag_dyneither(_tmp53B,sizeof(char),2))));};}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1242
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
# 1243
Cyc_Absyndump_dumploc(d->loc);{
# 1244
void*_tmp35A=d->r;struct Cyc_Absyn_Vardecl*_tmp35C;struct Cyc_Absyn_Fndecl*_tmp35E;struct Cyc_Absyn_Aggrdecl*_tmp360;struct Cyc_Absyn_Datatypedecl*_tmp362;struct Cyc_Absyn_Enumdecl*_tmp364;struct Cyc_Absyn_Pat*_tmp366;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Tvar*_tmp36B;struct Cyc_Absyn_Vardecl*_tmp36C;int _tmp36D;struct Cyc_Absyn_Exp*_tmp36E;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Tvar*_tmp371;struct Cyc_Absyn_Vardecl*_tmp372;struct Cyc_Absyn_Typedefdecl*_tmp374;struct _dyneither_ptr*_tmp376;struct Cyc_List_List*_tmp377;struct _tuple0*_tmp379;struct Cyc_List_List*_tmp37A;struct Cyc_List_List*_tmp37C;struct Cyc_List_List*_tmp37E;struct Cyc_List_List*_tmp37F;_LL1E8: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp35B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp35B->tag != 0)goto _LL1EA;else{_tmp35C=_tmp35B->f1;}}_LL1E9:
# 1246
 Cyc_Absyndump_dumpvardecl(_tmp35C,d->loc);
# 1247
goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp35D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp35D->tag != 1)goto _LL1EC;else{_tmp35E=_tmp35D->f1;}}_LL1EB:
# 1250
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL208:
# 1251
 Cyc_Absyndump_dumpatts(_tmp35E->attributes);break;case Cyc_Cyclone_Gcc_c: _LL209:
# 1252
 break;}
# 1254
if(_tmp35E->is_inline){
# 1255
enum Cyc_Cyclone_C_Compilers _tmp382=Cyc_Cyclone_c_compiler;_LL20C: if(_tmp382 != Cyc_Cyclone_Vc_c)goto _LL20E;_LL20D:
# 1256
{const char*_tmp53C;Cyc_Absyndump_dump(((_tmp53C="__inline",_tag_dyneither(_tmp53C,sizeof(char),9))));}goto _LL20B;_LL20E:;_LL20F:
# 1257
{const char*_tmp53D;Cyc_Absyndump_dump(((_tmp53D="inline",_tag_dyneither(_tmp53D,sizeof(char),7))));}goto _LL20B;_LL20B:;}
# 1259
Cyc_Absyndump_dumpscope(_tmp35E->sc);{
# 1260
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp543;struct Cyc_Absyn_FnInfo _tmp542;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp541;void*t=(void*)((_tmp541=_cycalloc(sizeof(*_tmp541)),((_tmp541[0]=((_tmp543.tag=9,((_tmp543.f1=((_tmp542.tvars=_tmp35E->tvs,((_tmp542.effect=_tmp35E->effect,((_tmp542.ret_tqual=_tmp35E->ret_tqual,((_tmp542.ret_typ=_tmp35E->ret_type,((_tmp542.args=
# 1262
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp35E->args),((_tmp542.c_varargs=_tmp35E->c_varargs,((_tmp542.cyc_varargs=_tmp35E->cyc_varargs,((_tmp542.rgn_po=_tmp35E->rgn_po,((_tmp542.attributes=0,_tmp542)))))))))))))))))),_tmp543)))),_tmp541))));
# 1265
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL210:
# 1267
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp35E);
# 1268
break;case Cyc_Cyclone_Vc_c: _LL211:
# 1270
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp35E);
# 1271
break;}
# 1273
Cyc_Absyndump_dump_char((int)'{');
# 1274
Cyc_Absyndump_dumpstmt(_tmp35E->body,0);
# 1275
Cyc_Absyndump_dump_char((int)'}');
# 1276
goto _LL1E7;};_LL1EC: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp35F=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp35F->tag != 6)goto _LL1EE;else{_tmp360=_tmp35F->f1;}}_LL1ED:
# 1278
 Cyc_Absyndump_dump_aggrdecl(_tmp360);Cyc_Absyndump_dump_semi();
# 1279
goto _LL1E7;_LL1EE: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp361=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp361->tag != 7)goto _LL1F0;else{_tmp362=_tmp361->f1;}}_LL1EF:
# 1281
 Cyc_Absyndump_dump_datatypedecl(_tmp362);Cyc_Absyndump_dump_semi();
# 1282
goto _LL1E7;_LL1F0: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp363=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp363->tag != 8)goto _LL1F2;else{_tmp364=_tmp363->f1;}}_LL1F1:
# 1284
 Cyc_Absyndump_dump_enumdecl(_tmp364);Cyc_Absyndump_dump_semi();
# 1285
goto _LL1E7;_LL1F2: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp365=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp365->tag != 2)goto _LL1F4;else{_tmp366=_tmp365->f1;_tmp367=_tmp365->f3;}}_LL1F3:
# 1287
{const char*_tmp544;Cyc_Absyndump_dump(((_tmp544="let",_tag_dyneither(_tmp544,sizeof(char),4))));}
# 1288
Cyc_Absyndump_dumppat(_tmp366);
# 1289
Cyc_Absyndump_dump_char((int)'=');
# 1290
Cyc_Absyndump_dumpexp(_tmp367);
# 1291
Cyc_Absyndump_dump_semi();
# 1292
goto _LL1E7;_LL1F4: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp368=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp368->tag != 3)goto _LL1F6;else{_tmp369=_tmp368->f1;}}_LL1F5:
# 1294
{const char*_tmp545;Cyc_Absyndump_dump(((_tmp545="let ",_tag_dyneither(_tmp545,sizeof(char),5))));}
# 1295
Cyc_Absyndump_dumpids(_tmp369);
# 1296
Cyc_Absyndump_dump_semi();
# 1297
goto _LL1E7;_LL1F6: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp36A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp36A->tag != 4)goto _LL1F8;else{_tmp36B=_tmp36A->f1;_tmp36C=_tmp36A->f2;_tmp36D=_tmp36A->f3;_tmp36E=_tmp36A->f4;}}_LL1F7:
# 1299
{const char*_tmp546;Cyc_Absyndump_dump(((_tmp546="region",_tag_dyneither(_tmp546,sizeof(char),7))));}
# 1300
{const char*_tmp547;Cyc_Absyndump_dump(((_tmp547="<",_tag_dyneither(_tmp547,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp36B);{const char*_tmp548;Cyc_Absyndump_dump(((_tmp548="> ",_tag_dyneither(_tmp548,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp36C->name);
# 1301
if(_tmp36D){const char*_tmp549;Cyc_Absyndump_dump(((_tmp549="@resetable",_tag_dyneither(_tmp549,sizeof(char),11))));}
# 1302
if(_tmp36E != 0){
# 1303
{const char*_tmp54A;Cyc_Absyndump_dump(((_tmp54A=" = open(",_tag_dyneither(_tmp54A,sizeof(char),9))));}
# 1304
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp36E);{
# 1305
const char*_tmp54B;Cyc_Absyndump_dump(((_tmp54B=")",_tag_dyneither(_tmp54B,sizeof(char),2))));};}
# 1307
Cyc_Absyndump_dump_semi();
# 1308
goto _LL1E7;_LL1F8: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp36F=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp36F->tag != 5)goto _LL1FA;else{_tmp370=_tmp36F->f1;_tmp371=_tmp36F->f2;_tmp372=_tmp36F->f3;}}_LL1F9:
# 1310
{const char*_tmp54C;Cyc_Absyndump_dump(((_tmp54C="alias ",_tag_dyneither(_tmp54C,sizeof(char),7))));}
# 1311
{const char*_tmp54D;Cyc_Absyndump_dump(((_tmp54D="<",_tag_dyneither(_tmp54D,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp371);{const char*_tmp54E;Cyc_Absyndump_dump(((_tmp54E=">",_tag_dyneither(_tmp54E,sizeof(char),2))));}
# 1312
Cyc_Absyndump_dumpqvar(_tmp372->name);{const char*_tmp54F;Cyc_Absyndump_dump(((_tmp54F=" = ",_tag_dyneither(_tmp54F,sizeof(char),4))));}
# 1313
Cyc_Absyndump_dumpexp(_tmp370);
# 1314
Cyc_Absyndump_dump_semi();
# 1315
goto _LL1E7;_LL1FA: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp373=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp373->tag != 9)goto _LL1FC;else{_tmp374=_tmp373->f1;}}_LL1FB:
# 1321
{const char*_tmp550;Cyc_Absyndump_dump(((_tmp550="typedef",_tag_dyneither(_tmp550,sizeof(char),8))));}{
# 1322
void*t;
# 1323
if(_tmp374->defn == 0)
# 1324
t=Cyc_Absyn_new_evar(_tmp374->kind,0);else{
# 1326
t=(void*)_check_null(_tmp374->defn);}
# 1327
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp374->tq,t,Cyc_Absyndump_dumptypedefname,_tmp374);
# 1328
Cyc_Absyndump_dumpatts(_tmp374->atts);
# 1329
Cyc_Absyndump_dump_semi();
# 1331
goto _LL1E7;};_LL1FC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp375=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp375->tag != 10)goto _LL1FE;else{_tmp376=_tmp375->f1;_tmp377=_tmp375->f2;}}_LL1FD:
# 1333
{const char*_tmp551;Cyc_Absyndump_dump(((_tmp551="namespace ",_tag_dyneither(_tmp551,sizeof(char),11))));}
# 1334
Cyc_Absyndump_dump_str(_tmp376);
# 1335
Cyc_Absyndump_dump_char((int)'{');
# 1336
for(0;_tmp377 != 0;_tmp377=_tmp377->tl){
# 1337
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp377->hd);}
# 1338
Cyc_Absyndump_dump_char((int)'}');
# 1339
goto _LL1E7;_LL1FE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp378=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp378->tag != 11)goto _LL200;else{_tmp379=_tmp378->f1;_tmp37A=_tmp378->f2;}}_LL1FF:
# 1341
{const char*_tmp552;Cyc_Absyndump_dump(((_tmp552="using ",_tag_dyneither(_tmp552,sizeof(char),7))));}
# 1342
Cyc_Absyndump_dumpqvar(_tmp379);
# 1343
Cyc_Absyndump_dump_char((int)'{');
# 1344
for(0;_tmp37A != 0;_tmp37A=_tmp37A->tl){
# 1345
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp37A->hd);}
# 1346
Cyc_Absyndump_dump_char((int)'}');
# 1347
goto _LL1E7;_LL200: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp37B=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp37B->tag != 12)goto _LL202;else{_tmp37C=_tmp37B->f1;}}_LL201:
# 1349
{const char*_tmp553;Cyc_Absyndump_dump(((_tmp553="extern \"C\" {",_tag_dyneither(_tmp553,sizeof(char),13))));}
# 1350
for(0;_tmp37C != 0;_tmp37C=_tmp37C->tl){
# 1351
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp37C->hd);}
# 1352
Cyc_Absyndump_dump_char((int)'}');
# 1353
goto _LL1E7;_LL202: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp37D=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp37D->tag != 13)goto _LL204;else{_tmp37E=_tmp37D->f1;_tmp37F=_tmp37D->f2;}}_LL203:
# 1355
{const char*_tmp554;Cyc_Absyndump_dump(((_tmp554="extern \"C include\" {",_tag_dyneither(_tmp554,sizeof(char),21))));}
# 1356
for(0;_tmp37E != 0;_tmp37E=_tmp37E->tl){
# 1357
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp37E->hd);}
# 1358
Cyc_Absyndump_dump_char((int)'}');
# 1359
if(_tmp37F != 0){
# 1360
{const char*_tmp555;Cyc_Absyndump_dump(((_tmp555=" export {",_tag_dyneither(_tmp555,sizeof(char),10))));}
# 1361
for(0;_tmp37F != 0;_tmp37F=_tmp37F->tl){
# 1362
struct _tuple0*_tmp39B;struct _tuple20 _tmp39A=*((struct _tuple20*)_tmp37F->hd);_tmp39B=_tmp39A.f2;
# 1363
Cyc_Absyndump_dumpqvar(_tmp39B);
# 1364
if(_tmp37F->tl != 0)Cyc_Absyndump_dump_char((int)',');}{
# 1366
const char*_tmp556;Cyc_Absyndump_dump(((_tmp556="}",_tag_dyneither(_tmp556,sizeof(char),2))));};}
# 1368
goto _LL1E7;_LL204: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp380=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp380->tag != 14)goto _LL206;}_LL205:
# 1370
{const char*_tmp557;Cyc_Absyndump_dump(((_tmp557=" __cyclone_port_on__; ",_tag_dyneither(_tmp557,sizeof(char),23))));}
# 1371
goto _LL1E7;_LL206: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp381=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp35A;if(_tmp381->tag != 15)goto _LL1E7;}_LL207:
# 1373
{const char*_tmp558;Cyc_Absyndump_dump(((_tmp558=" __cyclone_port_off__; ",_tag_dyneither(_tmp558,sizeof(char),24))));}
# 1374
goto _LL1E7;_LL1E7:;};}
# 1378
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
# 1379
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
# 1380
if(pr.f1 != 1  || !pr.f2){
# 1381
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1385
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1386
if(tms == 0){
# 1387
f(a);
# 1388
return;}{
# 1390
void*_tmp39F=(void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp3A1;void*_tmp3A2;union Cyc_Absyn_Constraint*_tmp3A3;union Cyc_Absyn_Constraint*_tmp3A4;union Cyc_Absyn_Constraint*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A6;_LL214: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp3A0=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp39F;if(_tmp3A0->tag != 2)goto _LL216;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A1.rgn;_tmp3A3=_tmp3A1.nullable;_tmp3A4=_tmp3A1.bounds;_tmp3A5=_tmp3A1.zero_term;_tmp3A6=_tmp3A0->f2;}}_LL215:
# 1394
{void*_tmp3A7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3A4);struct Cyc_Absyn_Exp*_tmp3AA;_LL219: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3A8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3A7;if(_tmp3A8->tag != 0)goto _LL21B;}_LL21A:
# 1395
 Cyc_Absyndump_dump_char((int)'?');goto _LL218;_LL21B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3A9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3A7;if(_tmp3A9->tag != 1)goto _LL218;else{_tmp3AA=_tmp3A9->f1;}}_LL21C:
# 1397
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp3A3)?'*':'@'));
# 1398
Cyc_Absyndump_dump_upperbound(_tmp3AA);
# 1399
goto _LL218;_LL218:;}
# 1401
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3A5)){const char*_tmp559;Cyc_Absyndump_dump(((_tmp559="@zeroterm",_tag_dyneither(_tmp559,sizeof(char),10))));}
# 1402
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3A5)){const char*_tmp55A;Cyc_Absyndump_dump(((_tmp55A="@nozeroterm",_tag_dyneither(_tmp55A,sizeof(char),12))));}
# 1403
{void*_tmp3AD=Cyc_Tcutil_compress(_tmp3A2);struct Cyc_Absyn_Tvar*_tmp3B2;void*_tmp3B4;_LL21E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp3AE=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3AD;if(_tmp3AE->tag != 20)goto _LL220;}_LL21F:
# 1404
 if(!Cyc_Absyndump_qvar_to_Cids){const char*_tmp55B;Cyc_Absyndump_dump(((_tmp55B="`H",_tag_dyneither(_tmp55B,sizeof(char),3))));}goto _LL21D;_LL220: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3AF=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3AD;if(_tmp3AF->tag != 21)goto _LL222;}_LL221:
# 1405
{const char*_tmp55C;Cyc_Absyndump_dump(((_tmp55C="`U",_tag_dyneither(_tmp55C,sizeof(char),3))));}goto _LL21D;_LL222: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3B0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3AD;if(_tmp3B0->tag != 22)goto _LL224;}_LL223:
# 1406
{const char*_tmp55D;Cyc_Absyndump_dump(((_tmp55D="`RC",_tag_dyneither(_tmp55D,sizeof(char),4))));}goto _LL21D;_LL224: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3B1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3AD;if(_tmp3B1->tag != 2)goto _LL226;else{_tmp3B2=_tmp3B1->f1;}}_LL225:
# 1407
 Cyc_Absyndump_dumptvar(_tmp3B2);goto _LL21D;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AD;if(_tmp3B3->tag != 1)goto _LL228;else{_tmp3B4=(void*)_tmp3B3->f2;if(_tmp3B4 != 0)goto _LL228;}}_LL227:
# 1408
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp3A2));goto _LL21D;_LL228: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3B5=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3AD;if(_tmp3B5->tag != 17)goto _LL22A;}_LL229:
# 1409
{const char*_tmp55E;Cyc_Absyndump_dump(((_tmp55E="@region(",_tag_dyneither(_tmp55E,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp3A2);{const char*_tmp55F;Cyc_Absyndump_dump(((_tmp55F=")",_tag_dyneither(_tmp55F,sizeof(char),2))));}goto _LL21D;_LL22A:;_LL22B: {
# 1410
const char*_tmp562;void*_tmp561;(_tmp561=0,Cyc_Tcutil_impos(((_tmp562="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp562,sizeof(char),37))),_tag_dyneither(_tmp561,sizeof(void*),0)));}_LL21D:;}
# 1412
Cyc_Absyndump_dumptq(_tmp3A6);
# 1413
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
# 1414
return;_LL216:;_LL217: {
# 1417
int next_is_pointer=0;
# 1418
if(tms->tl != 0){
# 1419
void*_tmp3BD=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL22D: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp3BE=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp3BD;if(_tmp3BE->tag != 2)goto _LL22F;}_LL22E:
# 1420
 next_is_pointer=1;goto _LL22C;_LL22F:;_LL230:
# 1421
 goto _LL22C;_LL22C:;}
# 1423
if(next_is_pointer)
# 1424
Cyc_Absyndump_dump_char((int)'(');
# 1425
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
# 1426
if(next_is_pointer)
# 1427
Cyc_Absyndump_dump_char((int)')');
# 1428
{void*_tmp3BF=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C3;union Cyc_Absyn_Constraint*_tmp3C4;void*_tmp3C6;struct Cyc_List_List*_tmp3C8;int _tmp3C9;struct Cyc_Absyn_VarargInfo*_tmp3CA;void*_tmp3CB;struct Cyc_List_List*_tmp3CC;void*_tmp3CE;struct Cyc_List_List*_tmp3D0;unsigned int _tmp3D1;struct Cyc_List_List*_tmp3D3;unsigned int _tmp3D4;int _tmp3D5;struct Cyc_List_List*_tmp3D7;_LL232: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp3C0=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3C0->tag != 0)goto _LL234;else{_tmp3C1=_tmp3C0->f1;}}_LL233:
# 1430
{const char*_tmp563;Cyc_Absyndump_dump(((_tmp563="[]",_tag_dyneither(_tmp563,sizeof(char),3))));}
# 1431
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3C1)){const char*_tmp564;Cyc_Absyndump_dump(((_tmp564="@zeroterm",_tag_dyneither(_tmp564,sizeof(char),10))));}
# 1432
goto _LL231;_LL234: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp3C2=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3C2->tag != 1)goto _LL236;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}}_LL235:
# 1434
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp3C3);Cyc_Absyndump_dump_char((int)']');
# 1435
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3C4)){const char*_tmp565;Cyc_Absyndump_dump(((_tmp565="@zeroterm",_tag_dyneither(_tmp565,sizeof(char),10))));}
# 1436
goto _LL231;_LL236: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp3C5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3C5->tag != 3)goto _LL238;else{_tmp3C6=(void*)_tmp3C5->f1;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp3C7=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp3C6;if(_tmp3C7->tag != 1)goto _LL238;else{_tmp3C8=_tmp3C7->f1;_tmp3C9=_tmp3C7->f2;_tmp3CA=_tmp3C7->f3;_tmp3CB=(void*)_tmp3C7->f4;_tmp3CC=_tmp3C7->f5;}};}}_LL237:
# 1438
 Cyc_Absyndump_dumpfunargs(_tmp3C8,_tmp3C9,_tmp3CA,_tmp3CB,_tmp3CC);goto _LL231;_LL238: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp3CD=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3CD->tag != 3)goto _LL23A;else{_tmp3CE=(void*)_tmp3CD->f1;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp3CF=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp3CE;if(_tmp3CF->tag != 0)goto _LL23A;else{_tmp3D0=_tmp3CF->f1;_tmp3D1=_tmp3CF->f2;}};}}_LL239:
# 1440
{const char*_tmp568;const char*_tmp567;const char*_tmp566;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp3D0,((_tmp566="(",_tag_dyneither(_tmp566,sizeof(char),2))),((_tmp567=")",_tag_dyneither(_tmp567,sizeof(char),2))),((_tmp568=",",_tag_dyneither(_tmp568,sizeof(char),2))));}goto _LL231;_LL23A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp3D2=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3D2->tag != 4)goto _LL23C;else{_tmp3D3=_tmp3D2->f1;_tmp3D4=_tmp3D2->f2;_tmp3D5=_tmp3D2->f3;}}_LL23B:
# 1442
 if(_tmp3D5)Cyc_Absyndump_dumpkindedtvars(_tmp3D3);else{Cyc_Absyndump_dumptvars(_tmp3D3);}goto _LL231;_LL23C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp3D6=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3D6->tag != 5)goto _LL23E;else{_tmp3D7=_tmp3D6->f2;}}_LL23D:
# 1443
 Cyc_Absyndump_dumpatts(_tmp3D7);goto _LL231;_LL23E: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp3D8=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp3BF;if(_tmp3D8->tag != 2)goto _LL231;}_LL23F: {
# 1444
const char*_tmp56B;void*_tmp56A;(_tmp56A=0,Cyc_Tcutil_impos(((_tmp56B="dumptms",_tag_dyneither(_tmp56B,sizeof(char),8))),_tag_dyneither(_tmp56A,sizeof(void*),0)));}_LL231:;}
# 1446
return;}_LL213:;};}
# 1450
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
# 1451
int _tmp3E1=Cyc_Absyndump_is_char_ptr(t);
# 1452
struct _RegionHandle _tmp3E2=_new_region("temp");struct _RegionHandle*temp=& _tmp3E2;_push_region(temp);
# 1453
{struct Cyc_Absyn_Tqual _tmp3E4;void*_tmp3E5;struct Cyc_List_List*_tmp3E6;struct _tuple11 _tmp3E3=Cyc_Absynpp_to_tms(temp,tq,t);_tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E3.f2;_tmp3E6=_tmp3E3.f3;
# 1454
Cyc_Absyndump_dumptq(_tmp3E4);
# 1455
Cyc_Absyndump_dumpntyp(_tmp3E5);
# 1456
Cyc_Absyndump_dumptms(_tmp3E1,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3E6),f,a);}
# 1453
;_pop_region(temp);}
# 1460
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1462
*Cyc_Absyndump_dump_file=f;
# 1463
for(0;tdl != 0;tdl=tdl->tl){
# 1464
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
# 1465
const char*_tmp56E;void*_tmp56D;(_tmp56D=0,Cyc_fprintf(f,((_tmp56E="\n",_tag_dyneither(_tmp56E,sizeof(char),2))),_tag_dyneither(_tmp56D,sizeof(void*),0)));};}
# 1468
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
# 1469
void*_tmp3E9=d->r;struct Cyc_Absyn_Vardecl*_tmp3EB;struct Cyc_Absyn_Fndecl*_tmp3ED;struct Cyc_Absyn_Aggrdecl*_tmp3EF;struct Cyc_Absyn_Datatypedecl*_tmp3F1;struct Cyc_Absyn_Enumdecl*_tmp3F3;struct Cyc_Absyn_Typedefdecl*_tmp3F5;struct _dyneither_ptr*_tmp3F7;struct Cyc_List_List*_tmp3F8;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FC;struct Cyc_List_List*_tmp3FE;struct Cyc_List_List*_tmp3FF;_LL241: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp3EA=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3EA->tag != 0)goto _LL243;else{_tmp3EB=_tmp3EA->f1;}}_LL242:
# 1471
 if(_tmp3EB->sc == Cyc_Absyn_Static)return;{
# 1472
struct Cyc_Absyn_Exp*init=_tmp3EB->initializer;
# 1473
_tmp3EB->initializer=0;
# 1474
if(_tmp3EB->sc == Cyc_Absyn_Public){
# 1475
const char*_tmp56F;Cyc_Absyndump_dump(((_tmp56F="extern ",_tag_dyneither(_tmp56F,sizeof(char),8))));}
# 1476
Cyc_Absyndump_dumpvardecl(_tmp3EB,d->loc);
# 1477
{const char*_tmp570;Cyc_Absyndump_dump(((_tmp570="\n",_tag_dyneither(_tmp570,sizeof(char),2))));}
# 1478
_tmp3EB->initializer=init;
# 1479
goto _LL240;};_LL243: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3EC=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3EC->tag != 1)goto _LL245;else{_tmp3ED=_tmp3EC->f1;}}_LL244:
# 1481
 if(_tmp3ED->sc == Cyc_Absyn_Static)return;
# 1482
Cyc_Absyndump_dumpscope(_tmp3ED->sc);{
# 1483
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp576;struct Cyc_Absyn_FnInfo _tmp575;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp574;void*t=(void*)((_tmp574=_cycalloc(sizeof(*_tmp574)),((_tmp574[0]=((_tmp576.tag=9,((_tmp576.f1=((_tmp575.tvars=_tmp3ED->tvs,((_tmp575.effect=_tmp3ED->effect,((_tmp575.ret_tqual=_tmp3ED->ret_tqual,((_tmp575.ret_typ=_tmp3ED->ret_type,((_tmp575.args=
# 1485
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3ED->args),((_tmp575.c_varargs=_tmp3ED->c_varargs,((_tmp575.cyc_varargs=_tmp3ED->cyc_varargs,((_tmp575.rgn_po=_tmp3ED->rgn_po,((_tmp575.attributes=0,_tmp575)))))))))))))))))),_tmp576)))),_tmp574))));
# 1488
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp3ED);
# 1489
{const char*_tmp577;Cyc_Absyndump_dump(((_tmp577=";\n",_tag_dyneither(_tmp577,sizeof(char),3))));}
# 1490
goto _LL240;};_LL245: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp3EE=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3EE->tag != 6)goto _LL247;else{_tmp3EF=_tmp3EE->f1;}}_LL246:
# 1492
 if(_tmp3EF->sc == Cyc_Absyn_Static)return;{
# 1493
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp3EF->impl;
# 1494
if(_tmp3EF->sc == Cyc_Absyn_Abstract)_tmp3EF->impl=0;
# 1495
Cyc_Absyndump_dump_aggrdecl(_tmp3EF);
# 1496
_tmp3EF->impl=impl;
# 1497
{const char*_tmp578;Cyc_Absyndump_dump(((_tmp578=";\n",_tag_dyneither(_tmp578,sizeof(char),3))));}
# 1498
goto _LL240;};_LL247: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp3F0=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3F0->tag != 7)goto _LL249;else{_tmp3F1=_tmp3F0->f1;}}_LL248:
# 1500
 Cyc_Absyndump_dump_datatypedecl(_tmp3F1);
# 1501
{const char*_tmp579;Cyc_Absyndump_dump(((_tmp579=";\n",_tag_dyneither(_tmp579,sizeof(char),3))));}
# 1502
goto _LL240;_LL249: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp3F2=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3F2->tag != 8)goto _LL24B;else{_tmp3F3=_tmp3F2->f1;}}_LL24A:
# 1504
 Cyc_Absyndump_dump_enumdecl(_tmp3F3);
# 1505
{const char*_tmp57A;Cyc_Absyndump_dump(((_tmp57A=";\n",_tag_dyneither(_tmp57A,sizeof(char),3))));}
# 1506
goto _LL240;_LL24B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp3F4=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3F4->tag != 9)goto _LL24D;else{_tmp3F5=_tmp3F4->f1;}}_LL24C:
# 1508
 if(_tmp3F5->defn == 0){
# 1509
Cyc_Absyndump_dumpdecl(d);{
# 1510
const char*_tmp57B;Cyc_Absyndump_dump(((_tmp57B="\n",_tag_dyneither(_tmp57B,sizeof(char),2))));};}
# 1512
goto _LL240;_LL24D: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3F6=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3F6->tag != 10)goto _LL24F;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;}}_LL24E:
# 1514
{const char*_tmp57C;Cyc_Absyndump_dump(((_tmp57C="namespace ",_tag_dyneither(_tmp57C,sizeof(char),11))));}
# 1515
Cyc_Absyndump_dump_str(_tmp3F7);
# 1516
{const char*_tmp57D;Cyc_Absyndump_dump(((_tmp57D="{\n",_tag_dyneither(_tmp57D,sizeof(char),3))));}
# 1517
for(0;_tmp3F8 != 0;_tmp3F8=_tmp3F8->tl){
# 1518
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp3F8->hd);}
# 1519
{const char*_tmp57E;Cyc_Absyndump_dump(((_tmp57E="}\n",_tag_dyneither(_tmp57E,sizeof(char),3))));}
# 1520
goto _LL240;_LL24F: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3F9=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3F9->tag != 11)goto _LL251;else{_tmp3FA=_tmp3F9->f2;}}_LL250:
# 1522
 for(0;_tmp3FA != 0;_tmp3FA=_tmp3FA->tl){
# 1523
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp3FA->hd);}
# 1524
goto _LL240;_LL251: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3FB=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3FB->tag != 12)goto _LL253;else{_tmp3FC=_tmp3FB->f1;}}_LL252:
# 1526
{const char*_tmp57F;Cyc_Absyndump_dump(((_tmp57F="extern \"C\" {",_tag_dyneither(_tmp57F,sizeof(char),13))));}
# 1527
for(0;_tmp3FC != 0;_tmp3FC=_tmp3FC->tl){
# 1528
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp3FC->hd);}
# 1529
{const char*_tmp580;Cyc_Absyndump_dump(((_tmp580="}\n",_tag_dyneither(_tmp580,sizeof(char),3))));}
# 1530
goto _LL240;_LL253: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3FD=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp3E9;if(_tmp3FD->tag != 13)goto _LL255;else{_tmp3FE=_tmp3FD->f1;_tmp3FF=_tmp3FD->f2;}}_LL254:
# 1547 "absyndump.cyc"
 goto _LL240;_LL255:;_LL256: {
# 1548
const char*_tmp583;void*_tmp582;(_tmp582=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp583="bad top-level declaration",_tag_dyneither(_tmp583,sizeof(char),26))),_tag_dyneither(_tmp582,sizeof(void*),0)));}_LL240:;}
# 1552
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
# 1553
*Cyc_Absyndump_dump_file=f;
# 1554
for(0;ds != 0;ds=ds->tl){
# 1555
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
